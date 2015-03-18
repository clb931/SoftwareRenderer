#ifndef ATLAS_RENDERCONTEXT_H
#include "ATLAS_Texture.h"

namespace ATLAS
{
	enum AtlasEnum
	{
		AWS_DOUBLE_BUFFER = 0,
		AWS_VISIBLE = 1,
		AWS_DEPTH_BUFFER = 2,
		AWS_FULLSCREEN = 4,
		AWS_BORDERLESS = 8,

		COLOR_BUFFER = 1,
		DEPTH_BUFFER = 2,

		DRAW_POINTS = 1,
		DRAW_LINES = 2,
		DRAW_TRIANGLES = 4,

		//Flags
		ORTHOGRAPHIC_PROJECTION = 1,
		CULL_FACES = 4
	};

	class Span
	{
	public:
		Span(real32 x1, const Color &color1, UV uv1,
			real32 x2, const Color &color, UV uv2);

		real32 m_Start, m_End;
		real32 m_Depth1, m_Depth2;
		Color m_Color1, m_Color2;
		UV m_UV1, m_UV2;
	};
	class Edge
	{
	public:
		Edge(Vertex v1, const Color &color1, UV uv1,
			Vertex v2, const Color &color2,	UV uv2);

		Color m_Color1, m_Color2;
		Vertex m_Start, m_End;
		UV m_UV1, m_UV2;
	};

	class RenderContext
	{
	public:
		RenderContext(void *buffer, uint32 width, uint32 height, uint32 bpp);
		~RenderContext();

		void DrawScene();
		void DrawTriangle(Vertex v1, const Color &color1, UV uv1,
						  Vertex v2, const Color &color2, UV uv2,
						  Vertex v3, const Color &color3, UV uv3,
						  Texture *tex);
		void DrawSpansBetweenEdges(const Edge &long_line,
								   const Edge &shortLine,
								   Texture *tex);
		void DrawSpan(const Span &span, uint32 y, Texture *texture);
		void DrawLine(Vertex v1, const Color &color1,
					  Vertex v2, const Color &color2);
		void DrawPoint(Vertex v, const Color &color);
		void DrawPixel(uint32 x, uint32 y, const Color &color);
		void Clear(uint8 buffer_flags);

		void SetClearColor(const Color &color);
		void SetPointSize(int32 size);
		void SetFlag(AtlasEnum flag, bool32 value);

	private:
		void			*m_FrameBuffer;
		uint32			m_Width;
		uint32			m_Height;

		Color32			m_ClearColor;
		int32			m_PointSize;
		uint32			m_Flags;
	};
}

#define ATLAS_RENDERCONTEXT_H
#endif