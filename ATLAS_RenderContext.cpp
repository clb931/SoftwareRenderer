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

		Vertex top = v1;
		Vertex mid = v2;
		Vertex bot = v3;
		Vertex temp;

		if (top.pos.y < mid.pos.y) {
			temp = top;
			top = mid;
			mid = temp;
		}
		if (mid.pos.y < bot.pos.y) {
			temp = mid;
			mid = bot;
			bot = temp;
		}
		if (top.pos.y < mid.pos.y) {
			temp = top;
			top = mid;
			mid = temp;
		}

		m_Gradients = Gradients(bot, mid, top);
		Edge bot2top(bot, top, m_Gradients, 0);
		Edge bot2mid(bot, mid, m_Gradients, 0);
		Edge mid2top(mid, top, m_Gradients, 1);
		real32 xmax1 = bot2top.x + bot2top.height * bot2top.x_step;
		real32 xmax2 = bot2mid.x + bot2mid.height * bot2mid.x_step;
		real32 xmax3 = mid2top.x + mid2top.height * mid2top.x_step;
		Edge *pLeft, *pRight;

		pLeft = bot2mid.x < bot2top.x ? &bot2mid : &bot2top;
		pRight = bot2mid.x > bot2top.x ? &bot2mid : &bot2top;

		uint32 y_min = bot2mid.y;
		uint32 y_max = bot2mid.height + 1;
		for (uint32 y = y_min; y < y_max; ++y)
		{
			DrawScanLine(pLeft, pRight, y);
			++bot2mid;
			++bot2top;
		}

		pLeft = mid2top.x < bot2top.x ? &mid2top : &bot2top;
		pRight = mid2top.x > bot2top.x ? &mid2top : &bot2top;

		y_min = mid2top.y;
		y_max = mid2top.height + 1;
		for (uint32 y = y_min; y < y_max; ++y)
		{
			DrawScanLine(pLeft, pRight, y);
			++mid2top;
			++bot2top;
		}
	}
	void RenderContext::DrawScanLine(Edge *pLeft, Edge *pRight, uint32 y)
	{
		int32 x_min = (int32)ceil(pLeft->x);
		int32 x_max = (int32)ceil(pRight->x);
		real32 x_prestep = x_min - pLeft->x;
		
		real32 one_over_z = pLeft->one_over_z + x_prestep * m_Gradients.d_one_over_z_dx;
		UV uv_over_z = pLeft->uv_over_z + x_prestep * m_Gradients.d_uv_over_z_dx;
		Color color_over_z = pLeft->color_over_z + m_Gradients.d_c_over_z_dx * x_prestep;

		for (uint32 x = x_min; x < x_max; ++x) {
			real32 z = 1.0f / one_over_z;
			UV uv = uv_over_z * z;
			Color color = color_over_z * z;
			//Color texel = GetTexel(uv);
			DrawPixel(x, y, color);

			one_over_z += m_Gradients.d_one_over_z_dx;
			uv_over_z += m_Gradients.d_uv_over_z_dx;
			color_over_z += m_Gradients.d_c_over_z_dx;
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
		Vertex pVertices[3] = { bot, mid, top };
		int32 counter;

		real32 one_over_dx = 1.0f /
			(((pVertices[1].pos.x - pVertices[2].pos.x) *
			(pVertices[0].pos.y - pVertices[2].pos.y)) -
			((pVertices[0].pos.x - pVertices[2].pos.x) *
			(pVertices[1].pos.y - pVertices[2].pos.y)));
		real32 one_over_dy = -one_over_dx;

		for (counter = 0; counter < 3; ++counter) {
			real32 const one_over_z = 1 / pVertices[counter].pos.w;
			a_one_over_z[counter] = one_over_z;
			a_uv_over_z[counter] = pVertices[counter].uv * one_over_z;
			a_c_over_z[counter] = pVertices[counter].color * one_over_z;
		}

		d_one_over_z_dx = CalculateYStep<real32>(a_one_over_z, pVertices, one_over_dx);
		d_one_over_z_dy = CalculateYStep<real32>(a_one_over_z, pVertices, one_over_dy);

		d_uv_over_z_dx = CalculateYStep<UV>(a_uv_over_z, pVertices, one_over_dx);
		d_uv_over_z_dy = CalculateYStep<UV>(a_uv_over_z, pVertices, one_over_dy);

		d_c_over_z_dx = CalculateYStep<Color>(a_c_over_z, pVertices, one_over_dx);
		d_c_over_z_dy = CalculateYStep<Color>(a_c_over_z, pVertices, one_over_dy);
	}	
	template<typename T>
	T Gradients::CalculateXStep(T t[3], Vertex pVertices[3], real32 one_over_dx)
	{
		return ((((t[1] - t[2]) * (pVertices[0].pos.y - pVertices[2].pos.y)) -
			((t[0] - t[2]) * (pVertices[1].pos.y - pVertices[2].pos.y))) * one_over_dx);
	}
	template<typename T>
	T Gradients::CalculateYStep(T t[3], Vertex pVertices[3], real32 one_over_dy)
	{
		return ((((t[1] - t[2]) * (pVertices[0].pos.x - pVertices[2].pos.x)) -
			((t[0] - t[2]) * (pVertices[1].pos.x - pVertices[2].pos.x))) * one_over_dy);
	}
	
	Edge::Edge(Vertex bot, Vertex top, Gradients &gradients, int32 i)
	{
		y = (int32)ceil(bot.pos.y);
		int32 y_end = (int32)ceil(top.pos.y);
		height = y_end - 1;

		real32 y_prestep = y - bot.pos.y;

		real32 real_height = top.pos.y - bot.pos.y;
		real32 real_width = top.pos.x - bot.pos.x;

		x = ((real_width * y_prestep) / real_height) + bot.pos.x;
		x_step = real_width / real_height;
		real32 x_prestep = x - bot.pos.x;

		one_over_z = gradients.a_one_over_z[i] +
			y_prestep * gradients.d_one_over_z_dy + 
			x_prestep * gradients.d_one_over_z_dx;
		one_over_z_step = x_step *
			gradients.d_one_over_z_dx + gradients.d_one_over_z_dy;

		uv_over_z = gradients.a_uv_over_z[i] +
			y_prestep * gradients.d_uv_over_z_dy +
			x_prestep * gradients.d_uv_over_z_dx;
		uv_over_z_step = x_step *
			gradients.d_uv_over_z_dx + gradients.d_uv_over_z_dy;

		color_over_z = gradients.a_c_over_z[i] +
			gradients.d_c_over_z_dy * y_prestep + 
			gradients.d_c_over_z_dx * x_prestep;
		color_over_z_step = gradients.d_c_over_z_dy + 
			gradients.d_c_over_z_dx * x_step;
	}
	int32 Edge::operator++()
	{
		x += x_step;
		y++;
		height--;
		uv_over_z += uv_over_z_step;
		one_over_z += one_over_z_step;
		color_over_z += color_over_z_step;
		return height;
	}
}