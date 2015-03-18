#include "ATLAS_Stdafx.h"
#include "ATLAS_RenderContext.h"
#include "ATLAS_Texture.h"
#include <memory>
#include <Windows.h>

real32 zNear = 0.0f;
real32 zFar = 1.0f;
ATLAS::Matrix4f View = ATLAS::IdentityMatrix;

namespace ATLAS
{
	RenderContext::RenderContext(void *buffer, uint32 width, uint32 height, uint32 bpp)
	{
		m_FrameBuffer = buffer;
		m_Width = width;
		m_Height = height;

		m_ScreenTransform = ScreenSpaceMatrix(m_Width, m_Height);
		
		SetClearColor(Color::BLACK);
		Clear(FRAME_BUFFER | DEPTH_BUFFER);

		SetPointSize(1);
	}
	RenderContext::~RenderContext()
	{
	}

	void RenderContext::DrawTriangle(Vertex v1, Vertex v2, Vertex v3)
	{
		v1.pos = (m_ScreenTransform * v1.pos) / v1.pos.w;
		v2.pos = (m_ScreenTransform * v2.pos) / v2.pos.w;
		v3.pos = (m_ScreenTransform * v3.pos) / v3.pos.w;

		if (m_Flags & CULL_FACES)
			if (Normalize(v1.pos, v2.pos, v3.pos).z >= 0)
				return;

		Vertex bottom = v1;
		Vertex middle = v2;
		Vertex top = v3;
		Vertex temp;

		if (top.pos.y < middle.pos.y) {
			temp = top;
			top = middle;
			middle = temp;
		}
		if (middle.pos.y < bottom.pos.y) {
			temp = middle;
			middle = bottom;
			bottom = temp;
		}
		if (top.pos.y < middle.pos.y) {
			temp = top;
			top = middle;
			middle = temp;
		}

		m_Gradients = Gradients(bottom, middle, top);
		Edge bot2top(bottom, top, m_Gradients);
		Edge bot2mid(bottom, middle, m_Gradients);
		Edge mid2top(middle, top, m_Gradients);

		DrawSpansBetweenEdges(bot2top, bot2mid);
		DrawSpansBetweenEdges(bot2top, mid2top);
	}
	void RenderContext::DrawSpansBetweenEdges(Edge &long_edge, Edge &short_edge)
	{
		if (long_edge.y_diff == 0.0f || short_edge.y_diff == 0.0f)
			return;
		
		real32 factor1 = (short_edge.m_Start.pos.y - long_edge.m_Start.pos.y) / long_edge.y_diff;
		real32 factorStep1 = 1.0f / long_edge.y_diff;
		real32 factor2 = 0.0f;
		real32 factorStep2 = 1.0f / short_edge.y_diff;

		Vertex v1;
		Vertex v2;

		uint32 y_min = (uint32)ceil(short_edge.m_Start.pos.y);
		uint32 y_max = (uint32)ceil(short_edge.m_End.pos.y);
		for (uint32 y = y_min; y < y_max; ++y) {
			v1.pos.x = long_edge.m_Start.pos.x + (long_edge.x_diff * factor1);
			v1.uv = long_edge.m_Start.uv + (long_edge.uv_diff * factor1);
			v1.color = long_edge.m_Start.color + (long_edge.color_diff * factor1);
			v2.pos.x = short_edge.m_Start.pos.x + (short_edge.x_diff * factor2);
			v2.uv = short_edge.m_Start.uv + (short_edge.uv_diff * factor2);
			v2.color = short_edge.m_Start.color + (short_edge.color_diff * factor2);
			
			DrawSpan(Span(v1, v2, m_Gradients), y);

			factor1 += factorStep1;
			factor2 += factorStep2;
		}   
	}
	void RenderContext::DrawSpan(Span &span, uint32 y)
	{
		if (span.x_diff == 0.0f)
			return;
		
		Color texel = Color::BLACK;

		for (uint32 x = span.x_min; x < span.x_max; ++x) {
			//texel = GetTexel(span.uv);

			DrawPixel((uint32)x, (uint32)y, BlendAdd(span.color, texel));
			++span;
		}
	}
	
	void RenderContext::DrawLine(Vertex v1, Vertex v2)
	{
		v1.pos = (m_ScreenTransform * v1.pos) / v1.pos.w;
		v2.pos = (m_ScreenTransform * v2.pos) / v2.pos.w;

		real32 xdiff = (v2.pos.x - v1.pos.x);
		real32 ydiff = (v2.pos.y - v1.pos.y);

		if (xdiff == 0.0f && ydiff == 0.0f) {
			DrawPixel((uint32)v1.pos.x, (uint32)v1.pos.y, v1.color);
			return;
		}

		if (fabs(xdiff) > fabs(ydiff)) {
			real32 xmin, xmax;

			if (v1.pos.x < v2.pos.x) {
				xmin = v1.pos.x;
				xmax = v2.pos.x;
			} else {
				xmin = v2.pos.x;
				xmax = v1.pos.x;
			}

			real32 slope = ydiff / xdiff;
			for (real32 x = xmin; x <= xmax; x += 1.0f) {
				real32 y = v1.pos.y + ((x - v1.pos.x) * slope);
				Color color = v1.color + ((v2.color - v1.color) * ((x - v1.pos.x) / xdiff));
				DrawPixel((uint32)x, (uint32)y, color);
			}
		} else {
			real32 ymin, ymax;

			if (v1.pos.y < v2.pos.y) {
				ymin = v1.pos.y;
				ymax = v2.pos.y;
			}
			else {
				ymin = v2.pos.y;
				ymax = v1.pos.y;
			}

			real32 slope = xdiff / ydiff;
			for (real32 y = ymin; y <= ymax; y += 1.0f) {
				real32 x = v1.pos.x + ((y - v1.pos.y) * slope);
				Color color = v1.color + ((v2.color - v1.color) * ((y - v1.pos.y) / ydiff));
				DrawPixel((uint32)x, (uint32)y, color);
			}
		}
	}

	void RenderContext::DrawPoint(Vertex v)
	{		
		v.pos = (m_ScreenTransform * v.pos) / v.pos.w;

		for (int32 y = -m_PointSize; y < m_PointSize; ++y)
			for (int32 x = -m_PointSize; x < m_PointSize; ++x)
				DrawPixel((uint32)v.pos.x + x, (uint32)v.pos.y + y, v.color);
	}
	Color RenderContext::GetTexel(UV uv)
	{
		if (!m_CurrentTexture)
			return Color::WHITE;

		uint32 x = (uint32)(uv.u * (m_CurrentTexture->width - 1) + 0.5f);
		uint32 y = (uint32)(uv.v * (m_CurrentTexture->height - 1) + 0.5f);
		uint32 i = (x + y * m_CurrentTexture->width) * 4;
		
		if (i > m_CurrentTexture->width * m_CurrentTexture->height * 4.0f) {
			OutputDebugStringA("Texture coord out of range\n");
			__debugbreak(); // need to fix this
			return Color::WHITE;
		}
		
		return Color(
			m_CurrentTexture->data[i + 0] / 255.0f,
			m_CurrentTexture->data[i + 1] / 255.0f,
			m_CurrentTexture->data[i + 2] / 255.0f,
			m_CurrentTexture->data[i + 3] / 255.0f);
		
	}
	void RenderContext::DrawPixel(uint32 x, uint32 y, const Color &color) {
		if (x >= m_Width || y >= m_Height) {
			__debugbreak();
			return;
		}

		uint32 *pixel = (uint32 *)m_FrameBuffer;
		pixel[x + (y * m_Width)] = color.toColor32();
	}
	void RenderContext::Clear(uint8 flags)
	{
		if (flags & FRAME_BUFFER){
			uint32 *pixel = (uint32 *)m_FrameBuffer;

			for (uint32 i = 0; i < m_Height * m_Width; ++i) {
				pixel[i] = m_ClearColor;
			}
		}
	}

	void RenderContext::SetCurrentTexture(Texture *texture)
	{
		m_CurrentTexture = texture;
	}
	void RenderContext::SetClearColor(const Color &color)
	{
		m_ClearColor = color.toColor32();
	}
	void RenderContext::SetPointSize(int32 size)
	{
		m_PointSize = size;
	}
	void RenderContext::SetFlag(AtlasFlag flag, bool32 value)
	{
		if (value)
			m_Flags |= flag;
		else
			m_Flags &= ~flag;
	}
	

	Gradients::Gradients(Vertex bot, Vertex mid, Vertex top)
	{
		real32 one_over_dx = 1.0f /
			(((mid.pos.x - top.pos.x) * (bot.pos.y - top.pos.y)) -
			((bot.pos.x - top.pos.x) * (mid.pos.y - top.pos.y)));
		real32 one_over_dy = -one_over_dx;

		Color colors[3] = { bot.color, mid.color, top.color };
		color_x_step = CalculateXStep<Color>(colors, bot, mid, top, one_over_dx);
		color_y_step = CalculateYStep<Color>(colors, bot, mid, top, one_over_dy);
	}
	template<typename T>
	T Gradients::CalculateXStep(T t[], Vertex bot, Vertex mid, Vertex top, real32 one_over_dx)
	{
		return (((t[1] - t[2]) * (bot.pos.y - top.pos.y)) *
			((t[0] - t[2]) * (mid.pos.y - top.pos.y))) * one_over_dx;
	}
	template<typename T>
	T Gradients::CalculateYStep(T t[], Vertex bot, Vertex mid, Vertex top, real32 one_over_dx)
	{
		return (((t[1] - t[2]) * (bot.pos.x - top.pos.x)) *
			((t[0] - t[2]) * (mid.pos.x - top.pos.x))) * one_over_dx;
	}

	Edge::Edge(Vertex bot, Vertex top, Gradients gradients)
	{
		m_Start = bot;
		m_End = top;
		y_min = (uint32)ceil(bot.pos.y);
		y_max = (uint32)ceil(top.pos.y);
		y_diff = top.pos.y - bot.pos.y;
		x_diff = (top.pos.x - bot.pos.x);
		color_diff = (top.color - bot.color);
		uv_diff = (top.uv - bot.uv);
	}
	Span::Span(Vertex v1, Vertex v2, Gradients gradients)
	{
		if (v1.pos.x < v2.pos.x) {
			x_min = (uint32)ceil(v1.pos.x);
			x_max = (uint32)ceil(v2.pos.x);
			x_diff = v2.pos.x - v1.pos.x;
			x_step = 1.0f / x_diff;
			color = v1.color;
			color_step = gradients.color_x_step + gradients.color_y_step;// (v2.color - v1.color) / x_diff;
			uv = v1.uv;
			uv_step = (v2.uv - v1.uv) / x_diff;
		}
		else {
			x_min = (uint32)ceil(v2.pos.x);
			x_max = (uint32)ceil(v1.pos.x);
			x_diff = v1.pos.x - v2.pos.x;
			x_step = 1.0f / x_diff;
			color = v2.color;
			color_step = gradients.color_x_step + gradients.color_y_step;// (v1.color - v2.color) / x_diff;
			uv = v2.uv;
			uv_step = (v1.uv - v2.uv) / x_diff;
		}
	}
	void Span::operator++()
	{
		color += color_step;
		uv += uv_step;
	}
}