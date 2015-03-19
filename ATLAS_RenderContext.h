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
		T CalculateXStep(T t[3], Vertex pVertices[3], real32 one_over_dx);
		template<typename T>
		T CalculateYStep(T t[3], Vertex pVertices[3], real32 one_over_dy);

		real32	a_one_over_z[3];
		real32	d_one_over_z_dx, d_one_over_z_dy;
		UV		a_uv_over_z[3];
		UV		d_uv_over_z_dx, d_uv_over_z_dy;
		Color	a_c_over_z[3];
		Color	d_c_over_z_dx, d_c_over_z_dy;
	};
	struct Edge
	{
		Edge(Vertex bot, Vertex top, Gradients &gradients, int32 i);
		int32 operator++();

		real32 x, x_step;
		int32 y, height;
		real32 one_over_z, one_over_z_step;
		UV uv_over_z, uv_over_z_step;
		Color color_over_z, color_over_z_step;
	};

	class RenderContext
	{
	public:
		RenderContext(void *buffer, uint32 width, uint32 height, uint32 bpp);
		~RenderContext();

		void DrawScene();
		void DrawTriangle(Vertex v1, Vertex v2, Vertex v3);
		void DrawSpansBetweenEdges(Edge &long_edge, Edge &short_edge);
		void DrawScanLine(Edge *left, Edge *right, uint32 y);
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