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
		CULL_FACES = 1,
		DEPTH_TEST = 2,
	};
	
	struct Edge
	{
		Edge(Vertex bot, Vertex top);
		void Step();

		int32	y_min, y_max;
		real32	y_diff;
		real32	x_step, x;
		real32	depth_step, depth;
		real32	one_over_z_step, one_over_z;
		Color	color_step, color;
		UV		uv_step, uv;
	};

	class RenderContext
	{
	public:
		RenderContext(void *buffer, uint32 width, uint32 height, uint32 bpp);
		~RenderContext();

		void DrawTriangle(Vertex v1, Vertex v2, Vertex v3, AtlasEnum style = DRAW_TRIANGLES);
		void DrawLine(Vertex v1, Vertex v2);
		void DrawPoint(Vertex v);
		void DrawPixel(uint32 x, uint32 y, const Color &color);
		void Clear(uint8 buffer_flags);

		void SetCurrentTexture(Texture *texture);
		void SetBlendMode(BlendMode blend_mode);
		void SetClearColor(const Color &color);
		void SetPointSize(int32 size);
		void SetFlag(AtlasFlag flag, bool32 value);
		bool32 GetFlag(AtlasFlag flag);

	private:
		void FillTriangle(Vertex v1, Vertex v2, Vertex v3);
		bool32 ClipPolygonAxis(std::vector<Vertex> *vertices,
			std::vector<Vertex> *temp, int32 axis);
		void ClipPolygonComponent(std::vector<Vertex> *in_vertices, int32 component_index,
			real32 component_factor, std::vector<Vertex> *out_vertices);
		void ScanTriangle(Edge *pLongEdge, Edge *pRightEdge);
		void FillScanLine(Edge *left, Edge *right, uint32 y);
		Color GetTexel(real32 x, real32 y);

		void			*m_FrameBuffer;
		void			*m_DepthBuffer;
		uint32			m_Width;
		uint32			m_Height;

		BlendMode		m_CurrentBlendMode;
		Texture			*m_CurrentTexture;
		Matrix4f		m_ScreenTransform;
		Color32			m_ClearColor;
		int32			m_PointSize;
		uint32			m_Flags;
	};
}

#define ATLAS_RENDERCONTEXT_H
#endif