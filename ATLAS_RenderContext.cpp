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

		Edge edges[3] =
		{
			Edge(v1, v2),
			Edge(v2, v3),
			Edge(v3, v1)
		};

		real32 max_length = 0;
		int32 long_edge = 0;
		for (int32 i = 0; i < 3; ++i) {
			real32 length = edges[i].m_End.pos.y - edges[i].m_Start.pos.y;
			if (length > max_length) {
				max_length = length;
				long_edge = i;
			}
		}

		int32 short_edge1 = (long_edge + 1) % 3;
		int32 short_edge2 = (long_edge + 2) % 3;

		DrawSpansBetweenEdges(edges[long_edge], edges[short_edge1]);
		DrawSpansBetweenEdges(edges[long_edge], edges[short_edge2]);
	}
	void RenderContext::DrawSpansBetweenEdges(Edge &long_edge, Edge &short_edge)
	{
		if (long_edge.y_diff == 0.0f || short_edge.y_diff == 0.0f)
			return;
		
		real32 factor1 = (short_edge.m_Start.pos.y - long_edge.m_Start.pos.y) / long_edge.y_diff;
		real32 factorStep1 = 1.0f / long_edge.y_diff;
		real32 factor2 = 0.0f;
		real32 factorStep2 = 1.0f / short_edge.y_diff;

		uint32 y_min = (uint32)ceil(short_edge.m_Start.pos.y);
		uint32 y_max = (uint32)ceil(short_edge.m_End.pos.y);
		for (uint32 y = y_min; y < y_max; ++y) {
			Span span(
				long_edge.m_Start.pos.x + (long_edge.x_diff * factor1),
				long_edge.m_Start.color + (long_edge.color_diff * factor1),
				long_edge.m_Start.uv + (long_edge.uv_diff * factor1),
				short_edge.m_Start.pos.x + (short_edge.x_diff * factor2),
				short_edge.m_Start.color + (short_edge.color_diff * factor2),
				short_edge.m_Start.uv + (short_edge.uv_diff * factor2));

			DrawSpan(span, y);

			factor1 += factorStep1;
			factor2 += factorStep2;
		}
	}
	void RenderContext::DrawSpan(Span &span, uint32 y)
	{
		if (span.x_diff == 0.0f)
			return;

		Color texel;

		for (uint32 x = span.x_min; x < span.x_max; ++x) {
			texel = GetTexel(span.uv);

			DrawPixel((uint32)x, (uint32)y, BlendNormal(span.color, texel));
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

		uint32 x = (uint32)ceil(uv.u * (m_CurrentTexture->width - 1));
		uint32 y = (uint32)ceil(uv.v *( m_CurrentTexture->height - 1));
		uint32 i = (uint32)ceil((x + y * m_CurrentTexture->width) * 4.0f);
		
		if (i > m_CurrentTexture->width * m_CurrentTexture->height * 4.0f) {
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
	
	Edge::Edge(Vertex v1, Vertex v2)
	{
		if (v1.pos.y < v2.pos.y) {
			m_Start = v1;
			m_End = v2;
			y_min = (uint32)ceil(v1.pos.y);
			y_max = (uint32)ceil(v2.pos.y);
			y_diff = v2.pos.y - v1.pos.y;
			x_diff = (v2.pos.x - v1.pos.x);
			color_diff = (v2.color - v1.color);
			uv_diff = (v2.uv - v1.uv);
		}
		else {
			m_Start = v2;
			m_End = v1;
			y_min = (uint32)ceil(v1.pos.y);
			y_max = (uint32)ceil(v2.pos.y);
			y_diff = v1.pos.y - v2.pos.y;
			x_diff = v1.pos.x - v2.pos.x;
			color_diff = v1.color - v2.color;
			uv_diff = v1.uv - v2.uv;
		}
	}
	Span::Span(real32 x1, const Color &color1, UV uv1,
		real32 x2, const Color &color2, UV uv2)
	{
		if (x1 < x2) {
			x_min = (uint32)ceil(x1);
			x_max = (uint32)ceil(x2);
			x_diff = x2 - x1;
			x_step = 1.0f / x_diff;
			color = color1;
			color_step = (color2 - color1) / x_diff;
			uv = uv1;
			uv_step = (uv2 - uv1) / x_diff;
		}
		else {
			x_min = (uint32)ceil(x2);
			x_max = (uint32)ceil(x1);
			x_diff = x1 - x2;
			x_step = 1.0f / x_diff;
			color = color2;
			color_step = (color1 - color2) / x_diff;
			uv = uv2;
			uv_step = (uv1 - uv2) / x_diff;
		}
	}
	void Span::operator++()
	{
		color += color_step;
		uv += uv_step;
	}
}