#ifndef ATLAS_RENDERCONTEXT_H
#include "ATLAS_Texture.h"
#include "ATLAS_Vertex.h"

namespace ATLAS
{
	enum AtlasEnum
	{
		AWS_DOUBLE_BUFFER = 0,
		AWS_VISIBLE = 1,
		AWS_DEPTH_BUFFER = 2,
		AWS_FULLSCREEN = 4,
		AWS_BORDERLESS = 8,

		FRAME_BUFFER = 1,
		DEPTH_BUFFER = 2,

		DRAW_POINTS = 1,
		DRAW_LINES = 2,
		DRAW_TRIANGLES = 4,
	};
	enum AtlasFlag
	{
		CULL_FACES = 1
	};

	class Edge
	{
	public:
		Edge(Vertex v1, Vertex v2);

		Vertex m_Start, m_End;
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

	class RenderContext
	{
	public:
		RenderContext(void *buffer, uint32 width, uint32 height, uint32 bpp);
		~RenderContext();

		void DrawScene();
		void DrawTriangle(Vertex v1, Vertex v2, Vertex v3);
		void DrawSpansBetweenEdges(const Edge &long_edge, const Edge &short_edge);
		void DrawSpan(const Span &span, uint32 y);
		void DrawLine(Vertex v1, Vertex v2);
		void DrawPoint(Vertex v);
		Color GetTexel(UV uv);
		void DrawPixel(uint32 x, uint32 y, const Color &color);
		void Clear(uint8 buffer_flags);

		void SetCurrentTexture(Texture *texture);
		void SetClearColor(const Color &color);
		void SetPointSize(int32 size);
		void SetFlag(AtlasFlag flag, bool32 value);

	private:
		void			*m_FrameBuffer;
		uint32			m_Width;
		uint32			m_Height;


		Texture			*m_CurrentTexture;
		Matrix4f		m_ScreenTransform;
		Color32			m_ClearColor;
		int32			m_PointSize;
		uint32			m_Flags;
	};
}

#define ATLAS_RENDERCONTEXT_H
#endif