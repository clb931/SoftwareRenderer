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
		//if (style & DEPTH_BUFFER)
		m_DepthBuffer = new real32[m_Width * m_Height];

		m_ScreenTransform = ScreenSpaceMatrix(m_Width, m_Height);
		
		SetClearColor(Color::BLACK);
		Clear(FRAME_BUFFER | DEPTH_BUFFER);

		SetPointSize(1);
	}
	RenderContext::~RenderContext()
	{
		if (m_DepthBuffer)
			delete[] m_DepthBuffer;
	}

	void RenderContext::DrawTriangle(Vertex v1, Vertex v2, Vertex v3)
	{
		v1.pos = (m_ScreenTransform * v1.pos) / v1.pos.w;
		v2.pos = (m_ScreenTransform * v2.pos) / v2.pos.w;
		v3.pos = (m_ScreenTransform * v3.pos) / v3.pos.w;

		if (m_Flags & CULL_FACES)
			if (Normalize(v1.pos, v2.pos, v3.pos).z <= 0)
				return;

		Vertex pVertices[3] = { v1, v2, v3 };
		Vertex temp;

		if (pVertices[2].pos.y < pVertices[1].pos.y) {
			temp = pVertices[2];
			pVertices[2] = pVertices[1];
			pVertices[1] = temp;
		}
		if (pVertices[1].pos.y < pVertices[0].pos.y) {
			temp = pVertices[1];
			pVertices[1] = pVertices[0];
			pVertices[0] = temp;
		}
		if (pVertices[2].pos.y < pVertices[1].pos.y) {
			temp = pVertices[2];
			pVertices[2] = pVertices[1];
			pVertices[1] = temp;
		}

		Edge bot2top(pVertices[0], pVertices[2]);
		Edge bot2mid(pVertices[0], pVertices[1]);
		Edge mid2top(pVertices[1], pVertices[2]);
		Edge *pLeft, *pRight;

		pLeft = bot2mid.x < bot2top.x ? &bot2mid : &bot2top;
		pRight = bot2mid.x > bot2top.x ? &bot2mid : &bot2top;

		uint32 y_min = max(0.0f, bot2mid.y_min);
		uint32 y_max = min(bot2mid.y_max, m_Height - 1);
		for (uint32 y = y_min; y < y_max; ++y) {
			DrawScanLine(pLeft, pRight, y);
			++bot2mid;
			++bot2top;
		}

		pLeft = mid2top.x < bot2top.x ? &mid2top : &bot2top;
		pRight = mid2top.x > bot2top.x ? &mid2top : &bot2top;

		y_min = max(0.0f, mid2top.y_min);
		y_max = min(mid2top.y_max, m_Height);
		for (uint32 y = y_min; y < y_max; ++y) {
			DrawScanLine(pLeft, pRight, y);
			++mid2top;
			++bot2top;
		}
	}
	void RenderContext::DrawScanLine(Edge *pLeft, Edge *pRight, uint32 y)
	{
		int32 x_min = max(0.0f, (int32)ceil(pLeft->x));
		int32 x_max = min((int32)ceil(pRight->x), m_Width - 1);;
		real32 x_prestep = x_min - pLeft->x;

		real32	x_diff = x_max - x_min;
		real32	depth_step = (pRight->z - pLeft->z) / x_diff;
		real32	depth = pLeft->z + depth_step * x_prestep;
		real32	one_over_z_step = (pRight->one_over_z - pLeft->one_over_z) / x_diff;
		real32	one_over_z = pLeft->one_over_z + one_over_z_step * x_prestep;
		Color	color_step = (pRight->color - pLeft->color) / x_diff;
		Color	color = pLeft->color + color_step * x_prestep;
		UV		uv_step = (pRight->uv - pLeft->uv) / x_diff;
		UV		uv = pLeft->uv + uv_step * x_prestep;
		real32	*pDepthBuffer = (real32 *)m_DepthBuffer + y * m_Width;

		for (uint32 x = x_min; x < x_max; ++x) {
			Color	pixel = color;
			Color	texel = GetTexel(uv);

			if (m_DepthBuffer) {
				if (depth < *(pDepthBuffer + x)) {
					*(pDepthBuffer + x) = depth;
					DrawPixel(x, y, texel);
				} else {
					printf("");
				}
			} else {
				DrawPixel(x, y, pixel);
			}

			depth += depth_step;
			one_over_z += one_over_z_step;
			color += color_step;
			uv += uv_step;
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
		uint32 *pixel = (uint32 *)m_FrameBuffer;
		uint32 *depth = (uint32 *)m_DepthBuffer;

		for (uint32 i = 0; i < m_Height * m_Width; ++i) {
			if (flags & FRAME_BUFFER)
				pixel[i] = m_ClearColor;
			if (flags & DEPTH_BUFFER)
				depth[i] = 1.0f;
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

	Edge::Edge(Vertex bot, Vertex top)
	{
		y_min = (uint32)ceil(bot.pos.y);
		y_max = (uint32)ceil(top.pos.y);

		real32 y_prestep = y_min - bot.pos.y;

		real32	y_diff = top.pos.y - bot.pos.y;
		real32	x_diff = top.pos.x - bot.pos.x;

		x_step = x_diff / y_diff;
		x = bot.pos.x + x_step * y_prestep;
		real32 x_prestep = x - bot.pos.x;

		z_step = (top.pos.z - bot.pos.z) / y_diff;
		z = bot.pos.z + z_step * y_prestep;

		one_over_z_step = ((1.0f / top.pos.w) - (1.0f / bot.pos.w)) / y_diff;
		one_over_z = (1.0f / bot.pos.w) + one_over_z_step * y_prestep;

		color_step = (top.color - bot.color) / y_diff;
		color = bot.color + color_step * y_prestep;

		uv_step = (top.uv - bot.uv) / y_diff;
		uv = bot.uv + uv_step * y_prestep;
	}
	void Edge::operator++()
	{
		x += x_step;
		z += z_step;
		color += color_step;
		uv += uv_step;
		one_over_z += one_over_z_step;
	}
}