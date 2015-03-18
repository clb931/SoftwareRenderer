#ifndef ATLAS_RENDERCONTEXT_H
#include "ATLAS_Vertex.h"

namespace ATLAS
{
	class Texture;
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

	struct Gradients
	{
		Gradients(){}
		Gradients(Vertex bot, Vertex mid, Vertex top);

		template<typename T>
		T CalculateXStep(T t[3], Vertex bot, Vertex mid, Vertex top, real32 one_over_dx);
		template<typename T>
		T CalculateYStep(T t[3], Vertex bot, Vertex mid, Vertex top, real32 one_over_dy);

		Color color_x_step;
		Color color_y_step;
	};
	struct Edge
	{
		Edge(Vertex bot, Vertex top, Gradients gradients);

		Vertex m_Start, m_End;

		uint32	y_min;
		uint32	y_max;

		real32	x_diff;
		real32	y_diff;
		Color	color_diff;
		UV		uv_diff;
	};
	struct Span
	{
		Span(Vertex v1, Vertex v2, Gradients gradients);
		void operator++();

		uint32	x_min;
		uint32	x_max;

		real32	x_diff;
		real32	x_step;
		Color	color;
		Color	color_step;
		UV		uv;
		UV		uv_step;
	};

	class RenderContext
	{
	public:
		RenderContext(void *buffer, uint32 width, uint32 height, uint32 bpp);
		~RenderContext();

		void DrawScene();
		void DrawTriangle(Vertex v1, Vertex v2, Vertex v3);
		void DrawSpansBetweenEdges(Edge &long_edge, Edge &short_edge);
		void DrawSpan(Span &span, uint32 y);
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
		Gradients		m_Gradients;
		Matrix4f		m_ScreenTransform;
		Color32			m_ClearColor;
		int32			m_PointSize;
		uint32			m_Flags;
	};
}

#define ATLAS_RENDERCONTEXT_H
#endif