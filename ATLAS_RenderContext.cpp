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
	Span::Span(
		real32 x1, const Color &color1, UV uv1,
		real32 x2, const Color &color2, UV uv2)
	{
		if (x1 < x2) {
			m_Color1 = color1;
			m_Start = x1;
			m_UV1 = uv1;
			m_Color2 = color2;
			m_End = x2;
			m_UV2 = uv2;
		}
		else {
			m_Color1 = color2;
			m_Start = x2;
			m_UV1 = uv2;
			m_Color2 = color1;
			m_End = x1;
			m_UV2 = uv1;
		}
	}
	Edge::Edge(
		Vertex v1, const Color &color1, UV uv1,
		Vertex v2, const Color &color2,	UV uv2)
	{
		if (v1.y < v2.y) {
			m_Color1 = color1;
			m_Start = v1;
			m_UV1 = uv1;
			m_Color2 = color2;
			m_End = v2;
			m_UV2 = uv2;
		} else {
			m_Color1 = color2;
			m_Start = v2;
			m_UV1 = uv2;
			m_Color2 = color1;
			m_End = v1;
			m_UV2 = uv1;
		}
	}

	RenderContext::RenderContext(void *buffer, uint32 width, uint32 height, uint32 bpp)
	{
		m_FrameBuffer = buffer;
		m_Width = width;
		m_Height = height;
		
		SetClearColor(Color::BLACK);
		Clear(COLOR_BUFFER | DEPTH_BUFFER);

		SetPointSize(1);
	}
	RenderContext::~RenderContext()
	{
	}

	void RenderContext::DrawTriangle(
		Vertex v1, const Color &color1, UV uv1,
		Vertex v2, const Color &color2, UV uv2,
		Vertex v3, const Color &color3, UV uv3,
		Texture *texture)
	{
		Matrix4f sst = ScreenSpaceMatrix(m_Width, m_Height);
		v1 = sst * v1 / v1.w;
		v2 = sst * v2 / v2.w;
		v3 = sst * v3 / v3.w;

		if (m_Flags & CULL_FACES)
			if (Normalize(v1, v2, v3).z >= 0)
				return;

		Edge edges[3] =
		{
			Edge(v1, color1, uv1, v2, color2, uv2),
			Edge(v2, color2, uv2, v3, color3, uv3),
			Edge(v3, color3, uv3, v1, color1, uv1)
		};

		real32 max_length = 0;
		int32 long_edge = 0;
		for (int32 i = 0; i < 3; ++i) {
			real32 length = edges[i].m_End.y - edges[i].m_Start.y;
			if (length > max_length) {
				max_length = length;
				long_edge = i;
			}
		}

		int32 short_edge1 = (long_edge + 1) % 3;
		int32 short_edge2 = (long_edge + 2) % 3;

		DrawSpansBetweenEdges(edges[long_edge], edges[short_edge1], texture);
		DrawSpansBetweenEdges(edges[long_edge], edges[short_edge2], texture);
	}
	void RenderContext::DrawSpansBetweenEdges(const Edge &long_edge, const Edge &short_edge, Texture *tex)
	{
		real32 ydiff1 = long_edge.m_End.y - long_edge.m_Start.y;
		if (ydiff1 == 0.0f)
			return;

		real32 ydiff2 = short_edge.m_End.y - short_edge.m_Start.y;
		if (ydiff2 == 0.0f)
			return;

		real32 xdiff1 = long_edge.m_End.x - long_edge.m_Start.x;
		real32 xdiff2 = short_edge.m_End.x - short_edge.m_Start.x;
		Color cdiff1 = long_edge.m_Color2 - long_edge.m_Color1;
		Color cdiff2 = short_edge.m_Color2 - short_edge.m_Color1;

		real32 factor1 = (short_edge.m_Start.y - long_edge.m_Start.y) / ydiff1;
		real32 factorStep1 = 1.0f / ydiff1;
		real32 factor2 = 0.0f;
		real32 factorStep2 = 1.0f / ydiff2;

		real32 vdiff1 = long_edge.m_UV2.v - long_edge.m_UV1.v;
		real32 vdiff2 = short_edge.m_UV2.v - short_edge.m_UV1.v;
		real32 udiff1 = long_edge.m_UV2.u - long_edge.m_UV1.u;
		real32 udiff2 = short_edge.m_UV2.u - short_edge.m_UV1.u;

		uint32 y_min = (uint32)ceil(short_edge.m_Start.y);
		uint32 y_max = (uint32)ceil(short_edge.m_End.y);
		for (uint32 y = y_min; y < y_max; ++y) {
			Span span(
				long_edge.m_Start.x + (xdiff1 * factor1),
				long_edge.m_Color1 + (cdiff1 * factor1),
				UV(long_edge.m_UV1.u + (udiff1 * factor1), long_edge.m_UV1.v + (vdiff1 * factor1)),
				short_edge.m_Start.x + (xdiff2 * factor2),
				short_edge.m_Color1 + (cdiff2 * factor2),
				UV(short_edge.m_UV1.u + (udiff2 * factor2), short_edge.m_UV1.v + (vdiff2 * factor2))
			);

			DrawSpan(span, y, tex);

			factor1 += factorStep1;
			factor2 += factorStep2;
		}
	}
	void RenderContext::DrawSpan(const Span &span, uint32 y, Texture *texture)
	{
		real32 xdiff = span.m_End - span.m_Start;
		if (xdiff == 0)
			return;

		Color cdiff = span.m_Color2 - span.m_Color1;
		Color texel;

		real32 factor = 0.0f;
		real32 factorStep = 1.0f / xdiff;

		real32 udiff = span.m_UV2.u - span.m_UV1.u;
		real32 vdiff = span.m_UV2.v - span.m_UV1.v;

		uint32 x_min = (uint32)ceil(span.m_Start);
		uint32 x_max = (uint32)ceil(span.m_End);
		for (uint32 x = x_min; x < x_max; ++x) {
			if (texture) {
				uint32 u = (uint32)min((span.m_UV1.u + (udiff * factor)) * texture->width, 1440 - 1);
				uint32 v = (uint32)min((span.m_UV1.v + (vdiff * factor)) * texture->height, 900 - 1);
				uint32 uv = (uint32)((u + (v * texture->width)) * 4.0f);

				texel = Color(
					texture->data[uv + 0] / 255.0f,
					texture->data[uv + 1] / 255.0f,
					texture->data[uv + 2] / 255.0f,
					texture->data[uv + 3] / 255.0f);
			}

			DrawPixel((uint32)x, (uint32)y, texel * (span.m_Color1 + (cdiff * factor)));
			factor += factorStep;
		}
	}
	
	void RenderContext::DrawLine(Vertex v1, const Color &color1, Vertex v2, const Color &color2)
	{
		Matrix4f sst = ScreenSpaceMatrix(m_Width, m_Height);
		v1 *= sst;
		v2 *= sst;

		v1 /= v1.w;
		v2 /= v2.w;

		real32 xdiff = (v2.x - v1.x);
		real32 ydiff = (v2.y - v1.y);

		if (xdiff == 0.0f && ydiff == 0.0f) {
			DrawPixel((uint32)v1.x, (uint32)v1.y, color1);
			return;
		}

		if (fabs(xdiff) > fabs(ydiff)) {
			real32 xmin, xmax;

			if (v1.x < v2.x) {
				xmin = v1.x;
				xmax = v2.x;
			} else {
				xmin = v2.x;
				xmax = v1.x;
			}

			real32 slope = ydiff / xdiff;
			for (real32 x = xmin; x <= xmax; x += 1.0f) {
				real32 y = v1.y + ((x - v1.x) * slope);
				Color color = color1 + ((color2 - color1) * ((x - v1.x) / xdiff));
				DrawPixel((uint32)x, (uint32)y, color);
			}
		} else {
			real32 ymin, ymax;

			if (v1.y < v2.y) {
				ymin = v1.y;
				ymax = v2.y;
			}
			else {
				ymin = v2.y;
				ymax = v1.y;
			}

			real32 slope = xdiff / ydiff;
			for (real32 y = ymin; y <= ymax; y += 1.0f) {
				real32 x = v1.x + ((y - v1.y) * slope);
				Color color = color1 + ((color2 - color1) * ((y - v1.y) / ydiff));
				DrawPixel((uint32)x, (uint32)y, color);
			}
		}
	}

	void RenderContext::DrawPoint(Vertex v, const Color &color)
	{
		Matrix4f sst = ScreenSpaceMatrix(m_Width, m_Height);
		v *= sst;

		v /= v.w;

		for (int32 y = -m_PointSize; y < m_PointSize; ++y)
			for (int32 x = -m_PointSize; x < m_PointSize; ++x)
				DrawPixel((uint32)v.x + x, (uint32)v.y + y, color);
	}
	void RenderContext::DrawPixel(uint32 x, uint32 y, const Color &color) {
		if (x >= m_Width || y >= m_Height)
			return;

		uint32 *pixel = (uint32 *)m_FrameBuffer;
		pixel[x + (y * m_Width)] = color.toColor32();
	}
	void RenderContext::Clear(uint8 flags)
	{
		if (flags & COLOR_BUFFER){
			uint32 *pixel = (uint32 *)m_FrameBuffer;

			for (uint32 i = 0; i < m_Height * m_Width; ++i) {
				pixel[i] = m_ClearColor;
			}
		}
	}

	void RenderContext::SetClearColor(const Color &color)
	{
		m_ClearColor = color.toColor32();
	}
	void RenderContext::SetPointSize(int32 size)
	{
		m_PointSize = size;
	}
	void RenderContext::SetFlag(AtlasEnum flag, bool32 value)
	{
		if (value)
			m_Flags |= flag;
		else
			m_Flags &= ~flag;
	}
}