#ifndef ATLAS_RENDERCONTEXT_H
#include "ATLAS_Texture.h"
#include "ATLAS_Matrix.h"
#include "ATLAS_Vector.h"
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

		DRAW_POINTS = 0,
		DRAW_LINES = 1,
		DRAW_TRIANGLES = 2,
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

		void Resize(uint32 width, uint32 height);

		void DrawTriangle(Vertex v1, Vertex v2, Vertex v3);
		void DrawLine(Vertex v1, Vertex v2);
		void DrawPoint(Vertex v);
		void DrawPixel(uint32 x, uint32 y, const Color &color);
		void Clear(uint8 buffer_flags);

		void SetDrawStyle(AtlasEnum draw_style);
		void SetBlendMode(BlendMode blend_mode);
		void SetTexture(Texture *texture);
		void SetFlag(AtlasFlag flag, bool32 value);
		void SetClearColor(const Color &color);
		void SetPointSize(int32 size);

		AtlasEnum GetDrawStyle();
		BlendMode GetBlendMode();
		Texture *GetTexture();
		bool32 GetFlag(AtlasFlag flag);
		Color GetClearColor();
		int32 GetPointSize();

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
		Matrix4f		m_ScreenTransform;

		AtlasEnum		m_DrawStyle;
		BlendMode		m_BlendMode;
		Texture			*m_Texture;
		uint32			m_Flags;
		Color32			m_ClearColor;
		int32			m_PointSize;
	};
}


#define ATLAS_RENDERCONTEXT_H
#endif