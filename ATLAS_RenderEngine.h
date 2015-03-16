#ifndef ATLAS_RENDERENGINE_H
#include "ATLAS_Vector.h"
#include "ATLAS_Matrix.h"

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
	
	struct ScreenBuffer
	{
		uint32		width;
		uint32		height;
		uint32		size;
		uint32		bytes_per_pixel;
		uint32		pitch;
		void		*pixels;
	};
	struct Window
	{
		int32				width;
		int32				height;
		int32				style;
		ATLAS::ScreenBuffer	back_buffer;
		ATLAS::ScreenBuffer depth_buffer;
	};

	typedef uint32 Color32;
	class Color
	{
	public:
		Color(real32 r = 1.0f, real32 g = 1.0f, real32 b = 1.0f, real32 a = 0.0f);

		Color32 toColor32() const;
		Color operator+(const Color &top) const;
		Color operator-(const Color &top) const;
		Color operator*(const Color &top) const;
		Color operator/(const Color &top) const;
		Color operator*(real32 f) const;
		Color operator/(real32 f) const;
		Color operator~();

		real32 R, G, B, A;

#define RED		ATLAS::Color(1.0f, 0.0f, 0.0f, 0.1f)
#define GREEN	ATLAS::Color(0.0f, 1.0f, 0.0f, 0.1f)
#define BLUE	ATLAS::Color(0.0f, 0.0f, 1.0f, 0.1f)
#define BLACK	ATLAS::Color(0.0f, 0.0f, 0.0f, 0.1f)
#define CYAN	ATLAS::Color(0.0f, 1.0f, 1.0f, 0.1f)
#define MAGENTA ATLAS::Color(1.0f, 0.0f, 1.0f, 0.1f)
#define YELLOW	ATLAS::Color(1.0f, 1.0f, 0.0f, 0.1f)
#define WHITE	ATLAS::Color(1.0f, 1.0f, 1.0f, 0.1f)
	};

	Color BlendAdd(Color bottom, Color top);
	Color BlendSubtract(Color bottom, Color top);
	Color BlendMultiply(Color bottom, Color top);
	Color BlendScreen(Color bottom, Color top);
	Color BlendOverlay(Color bottom, Color top);
	Color BlendDodge(Color bottom, Color top);
	Color BlendBurn(Color bottom, Color top);
	Color BlendDifference(Color bottom, Color top);
	Color BlendDarken(Color bottom, Color top);
	Color BlendLighten(Color bottom, Color top);

	class Texture
	{
	public:
		Texture(const char *file_path);
		~Texture();

		uint8 *data;
		uint32 width;
		uint32 height;
		uint32 bpp;
		uint32 id;

	private:
		struct BMP{ uint8 b, g, r; };
	};

	typedef Vector4f Vertex;
	class Span
	{
	public:
		Span(real32 x1, const Color &color1, UV uv1,
			real32 x2, const Color &color, UV uv2);

		real32 m_Start, m_End;
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
	class Polygon
	{
	public:
		Polygon(uint32 v1 = 0, uint32 v2 = 0, uint32 v3 = 0);

		uint32 v1;
		uint32 v2;
		uint32 v3;
	};
	class Model
	{
	public:
		Model(const char *file_path, Texture *texture = nullptr);
		Model(Vertex *vertices, uint32 num_vertices,
			  Polygon *polygons, uint32 num_polygons,
			  UV *uvs = nullptr, Texture *texture = nullptr,
			  Color *colors = nullptr, Vector4f *normals = nullptr);
		~Model();

		Matrix4f m_TransformationMatrix;
		uint32	m_NumVertices;
		Vertex	*m_Vertices;
		uint32	m_NumPolygons;
		Polygon *m_Polygons;
		UV		*m_UVs;
		Texture *m_Texture;
		Color	*m_Colors;
		Vertex	*m_Normals;
	};

	class RenderEngine
	{
	public:
		RenderEngine(Window *game_window);
		~RenderEngine();

		void DrawScene();
		void DrawModel(Model *model, uint32);
		void DrawTriangle(Vertex v1, const Color &color1, UV uv1,
						  Vertex v2, const Color &color2, UV uv2,
						  Vertex v3, const Color &color3, UV uv3,
						  Texture *tex);
		void DrawSpansBetweenEdges(const Edge &long_line,
								   const Edge &shortLine,
								   Texture *tex);
		void DrawSpan(const Span &span, real32 y, Texture *texture);
		void DrawLine(Vertex v1, const Color &color1,
					  Vertex v2, const Color &color2);
		void DrawPoint(Vertex v, const Color &color);
		void DrawPixel(uint32 x, uint32 y, const Color &color);
		void Clear(uint8 buffer_flags);

		void SetProjection(real32 aspect_ration, real32 FOV,
						   real32 z_near, real32 z_far);
		void SetView(Matrix4f *view);
		void SetClearColor(const Color &color);
		void SetPointSize(int32 size);
		void SetFlag(AtlasEnum flag, bool32 value);

	private:
		ScreenBuffer	*m_FrameBuffer;
		ScreenBuffer	*m_DepthBuffer;

		Matrix4f		*m_View;
		Matrix4f		m_Projection;
		Color32			m_ClearColor;
		int32			m_PointSize;
		uint32			m_Flags;
	};
}

#define ATLAS_RENDERENGINE_H
#endif