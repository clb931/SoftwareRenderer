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
		v1.pos = m_ScreenTransform * v1.pos;
		v2.pos = m_ScreenTransform * v2.pos;
		v3.pos = m_ScreenTransform * v3.pos;
		v1.PerspectiveDivide();
		v2.PerspectiveDivide();
		v3.PerspectiveDivide();

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
		ScanTriangle(&bot2top, &bot2mid);
		ScanTriangle(&bot2top, &mid2top);

		//Edge *pLeft, *pRight;
		//pLeft = bot2mid.x < bot2top.x ? &bot2mid : &bot2top;
		//pRight = bot2mid.x > bot2top.x ? &bot2mid : &bot2top;
		//int32 y_min = max(0, bot2mid.y_min);
		//int32 y_max = min(bot2mid.y_max, m_Height - 1);
		//if (bot2mid.y_diff && bot2top.y_diff) {
		//	for (int32 y = y_min; y < y_max; ++y) {
		//		DrawScanLine(pLeft, pRight, y);
		//		bot2mid.Step();
		//		bot2top.Step();
		//	}
		//}
		//pLeft = mid2top.x < bot2top.x ? &mid2top : &bot2top;
		//pRight = mid2top.x > bot2top.x ? &mid2top : &bot2top;
		//y_min = max(0, mid2top.y_min);
		//y_max = min(mid2top.y_max, m_Height - 1);
		//if (mid2top.y_diff && bot2top.y_diff) {
		//	for (int32 y = y_min; y < y_max; ++y) {
		//		DrawScanLine(pLeft, pRight, y);
		//		mid2top.Step();
		//		bot2top.Step();
		//	}
		//}
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
			}
			else {
				xmin = v2.pos.x;
				xmax = v1.pos.x;
			}

			real32 slope = ydiff / xdiff;
			for (real32 x = xmin; x <= xmax; x += 1.0f) {
				real32 y = v1.pos.y + ((x - v1.pos.x) * slope);
				Color color = v1.color + ((v2.color - v1.color) * ((x - v1.pos.x) / xdiff));
				DrawPixel((uint32)x, (uint32)y, color);
			}
		}
		else {
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
	void RenderContext::DrawPixel(uint32 x, uint32 y, const Color &color) {
		if (x >= m_Width || y >= m_Height) {
			__debugbreak();
			return;
		}

		uint32 *pixel = (uint32 *)(m_FrameBuffer) + x + (y * m_Width);
		*pixel = color.toColor32();
	}
	void RenderContext::Clear(uint8 flags)
	{
		uint32 *pixel = (uint32 *)m_FrameBuffer;
		real32 *depth = (real32 *)m_DepthBuffer;

		for (uint32 i = 0; i < m_Height * m_Width; ++i) {
			if (flags & FRAME_BUFFER)
				pixel[i] = m_ClearColor;
			if (flags & DEPTH_BUFFER)
				depth[i] = INFINITY;
		}
	}

	void RenderContext::ScanTriangle(Edge *pLongEdge, Edge *pShortEdge)
	{
		Edge *pLeft, *pRight;

		pLeft = pShortEdge->x < pLongEdge->x ? pShortEdge : pLongEdge;
		pRight = pShortEdge->x > pLongEdge->x ? pShortEdge : pLongEdge;

		if (!pLeft->y_diff || !pRight->y_diff)
			return;

		uint32 y_min = pShortEdge->y_min;
		uint32 y_max = pShortEdge->y_max;
		if (y_min > m_Height || y_max > m_Height)
			return;

		for (uint32 y = y_min; y < y_max; ++y) {
			DrawScanLine(pLeft, pRight, y);
			pRight->Step();
			pLeft->Step();
		}
	}
	void RenderContext::DrawScanLine(Edge *pLeft, Edge *pRight, uint32 y)
	{
		uint32 x_min = (uint32)ceil(pLeft->x);
		uint32 x_max = (uint32)ceil(pRight->x);

		if (x_min > m_Width || x_max > m_Width)
			return;

		real32 x_prestep = x_min - pLeft->x;
		real32	x_diff = (real32)(x_max - x_min);
		if (!x_diff)
			return;

		real32	depth_step = (pRight->depth - pLeft->depth) / x_diff;
		real32	depth = pLeft->depth + depth_step * x_prestep;
		real32	one_over_z_step = (pRight->one_over_z - pLeft->one_over_z) / x_diff;
		real32	one_over_z = pLeft->one_over_z + one_over_z_step * x_prestep;
		Color	color_step = (pRight->color - pLeft->color) / x_diff;
		Color	color = pLeft->color + color_step * x_prestep;
		UV		uv_step = (pRight->uv - pLeft->uv) / x_diff;
		UV		uv = pLeft->uv + uv_step * x_prestep;
		real32	*pDepthBuffer = (real32 *)m_DepthBuffer + y * m_Width;

		for (uint32 x = x_min; x < x_max; ++x) {
			real32	z = 1.0f / one_over_z;
			Color	pixel = color * z;
			Color	texel = GetTexel(uv.u * z, uv.v * z);

			// need to fix the depth buffer
			// values arent between 0.0f and 1.0f
			if (m_DepthBuffer) {
				if (depth < *(pDepthBuffer + x)) {
					*(pDepthBuffer + x) = depth;
					DrawPixel(x, y, BlendNormal(pixel, texel));
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
	Color RenderContext::GetTexel(real32 x, real32 y)
	{
		if (!m_CurrentTexture)
			return Color::WHITE;

		int32 xx = (int32)(x * (m_CurrentTexture->width - 1) + 0.5f);
		int32 yy = (int32)(y * (m_CurrentTexture->height - 1) + 0.5f);
		uint32 i = (xx + yy * m_CurrentTexture->width) * 4;

		if (i > m_CurrentTexture->width * m_CurrentTexture->height * 4) {
			OutputDebugStringA("Texture coord out of range\n"); // need to fix this bug...
			//__debugbreak();									// tex coords go out of
			return Color::WHITE;								// range when part of 
		}														// polygon is off screen

		return Color(
			m_CurrentTexture->data[i + 0] / 255.0f,
			m_CurrentTexture->data[i + 1] / 255.0f,
			m_CurrentTexture->data[i + 2] / 255.0f,
			m_CurrentTexture->data[i + 3] / 255.0f);

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
		y_min = (int32)ceil(bot.pos.y);
		y_max = (int32)ceil(top.pos.y);

		real32 y_prestep = y_min - bot.pos.y;

		y_diff = top.pos.y - bot.pos.y;
		real32	x_diff = top.pos.x - bot.pos.x;

		x_step = x_diff / y_diff;
		x = bot.pos.x + x_step * y_prestep;
		real32 x_prestep = x - bot.pos.x;

		depth_step = (top.pos.z - bot.pos.z) / y_diff;
		depth = bot.pos.z + depth_step * y_prestep;

		one_over_z_step = ((1.0f / top.pos.w) - (1.0f / bot.pos.w)) / y_diff;
		one_over_z = (1.0f / bot.pos.w) + one_over_z_step * y_prestep;
		
		color_step = (top.color - bot.color) / y_diff;
		color = bot.color + color_step * y_prestep;

		uv_step = (top.uv - bot.uv) / y_diff;
		uv = bot.uv + uv_step * y_prestep;
	}
	void Edge::Step()
	{
		x += x_step;
		depth += depth_step;
		color += color_step;
		uv += uv_step;
		one_over_z += one_over_z_step;
	}
}