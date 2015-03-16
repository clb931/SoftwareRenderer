#include "ATLAS_Stdafx.h"
#include "ATLAS_RenderEngine.h"
#include <memory>
#include <Windows.h>

real32 zNear = 0.0f;
real32 zFar = 0.0f;
ATLAS::Matrix4f View = ATLAS::IdentityMatrix;

namespace ATLAS
{
	Color::Color(real32 r, real32 g, real32 b, real32 a)
	{
		R = min(1.0f, r);
		G = min(1.0f, g);
		B = min(1.0f, b);
		A = min(1.0f, a);
	}
	Color32 Color::toColor32() const
	{
		return Color32(
			((uint32)(A * 255.0f) << 24) |
			((uint32)(R * 255.0f) << 16) |
			((uint32)(G * 255.0f) << 8) |
			((uint32)(B * 255.0f) << 0));
	}
	Color Color::operator+(const Color &top) const
	{
		return Color(R + top.R, G + top.G, B + top.B, A + top.A);
	}
	Color Color::operator-(const Color &top) const
	{
		return Color(R - top.R, G - top.G, B - top.B, A - top.A);
	}
	Color Color::operator*(const Color &top) const
	{
		return Color(R * top.R, G * top.G, B * top.B, A * top.A);
	}
	Color Color::operator/(const Color &top) const
	{
		if (top.R == 0.0f && top.G == 0.0f && top.B == 0.0f)
			return Color();

		Color c(R / top.R, G / top.G, B / top.B, A / top.A);
		return c;
	}
	Color Color::operator*(real32 f) const
	{
		return Color(R * f, G * f, B * f, A * f);
	}
	Color Color::operator/(real32 f) const
	{
		return Color(R / f, G / f, B / f, A / f);
	}
	Color Color::operator~()
	{
		return Color(1.0f - R, 1.0f - G, 1.0f - B, 1.0f - A);
	}

	Color BlendAdd(Color bottom, Color top)
	{
		return bottom + top;
	}
	Color BlendSubtract(Color bottom, Color top)
	{
		return bottom - top;
	}
	Color BlendMultiply(Color bottom, Color top)
	{
		return bottom * top;
	}
	Color BlendScreen(Color bottom, Color top)
	{
		return ~(~bottom * ~top);
	}
	Color BlendOverlay(Color bottom, Color top)
	{
		Color c1 = (bottom * top) * 2.0f;
		Color c2 = ~((~bottom * ~top) * 2.0f);
		return Color(bottom.R < 0.5f ? c1.R : c2.R,
			bottom.G < 0.5f ? c1.G : c2.G,
			bottom.B < 0.5f ? c1.B : c2.B,
			bottom.A < 0.5f ? c1.A : c2.A);
	}
	Color BlendDodge(Color bottom, Color top)
	{
		return bottom / ~top;
	}
	Color BlendBurn(Color bottom, Color top)
	{
		return ~((~bottom) / top);
	}
	Color BlendDifference(Color bottom, Color top)
	{
		Color c;
		c.R = bottom.R < top.R ? top.R - bottom.R : bottom.R - top.R;
		c.G = bottom.G < top.G ? top.G - bottom.G : bottom.G - top.G;
		c.B = bottom.B < top.B ? top.B - bottom.B : bottom.B - top.B;
		c.A = bottom.A < top.A ? top.A - bottom.A : bottom.A - top.A;
		return c;
	}
	Color BlendDarken(Color bottom, Color top)
	{
		return Color(min(bottom.R, top.R), min(bottom.G, top.G), min(bottom.B, top.B), min(bottom.A, top.A));
	}
	Color BlendLighten(Color bottom, Color top)
	{
		return Color(max(bottom.R, top.R), max(bottom.G, top.G), max(bottom.B, top.B), max(bottom.A, top.A));
	}

	Texture::Texture(const char *file_path) : data(nullptr)
	{
		char str[256] = "";
		FILE *pFile;
		BMP colors;

		OutputDebugStringA("Openging file...\n");
		fopen_s(&pFile, file_path, "rb");
		if (pFile) {
			fseek(pFile, sizeof(short) * 9, SEEK_SET);
			fread(&width, sizeof(int), 1, pFile);
			fread(&height, sizeof(int), 1, pFile);
			fseek(pFile, sizeof(short), SEEK_CUR);
			fread(&bpp, sizeof(short), 1, pFile);
			fseek(pFile, sizeof(int) * 6, SEEK_CUR);
			
			sprintf_s(str, 256, "Dimensions: %i x %i\nBPP: %i\n", width, height, bpp);
			OutputDebugStringA(str);

			if (bpp <= 8)
				for (uint32 a = 0; a < bpp; a++)
					fseek(pFile, sizeof(int), SEEK_CUR);

			data = (uint8 *)malloc(width * height * 4);
			memset(data, 0, width * height * 4);

			sprintf_s(str, 256, "nsize: %.2fKB\n", (width * height) / 1024.0f);
			OutputDebugStringA(str);
			uint32 j = 0;
			for (uint32 i = 0; i < width * height; ++i) {
				fread(&colors, sizeof(BMP), 1, pFile);

				data[j + 0] = colors.r;
				data[j + 1] = colors.g;
				data[j + 2] = colors.b;
				data[j + 3] = 0;

				j += 4;
			}

			OutputDebugStringA("Clossing File...\n\n");
			fclose(pFile);
		}

		perror("The following error occurred");
		OutputDebugStringA("File does not exist.\n");
	}
	Texture::~Texture()
	{
		OutputDebugStringA("Freeing Texture...\n\n");

		if (data)
			free(data);
	}
		
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
	Polygon::Polygon(uint32 v1, uint32 v2, uint32 v3)
	{
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
	}
	Model::Model(Vertex *vertices, uint32 num_vertices,
		Polygon *polygons, uint32 num_polygons,
		UV *uvs, Texture *texture,
		Color *colors, Vertex *normals)
	{
		m_TransformationMatrix = IdentityMatrix;
		m_NumVertices = num_vertices;
		m_Vertices = vertices;
		m_NumPolygons = num_polygons;
		m_Polygons = polygons;
		m_UVs = uvs;
		m_Texture = texture;
		m_Colors = colors;
		m_Normals = new Vector4f[m_NumVertices];

		if (normals) {
			for (uint32 i = 0; i < m_NumVertices; ++i)
				m_Normals[i] = normals[i];
		} else {
			uint32 *edges = new uint32[m_NumVertices];

			for (uint32 i = 0; i < m_NumPolygons; ++i) {
				Vector4f N = Normalize(m_Vertices[m_Polygons[i].v1], 
					m_Vertices[m_Polygons[i].v2], 
					m_Vertices[m_Polygons[i].v3]);

				N = Normalize(N);

				edges[m_Polygons[i].v1]++;
				edges[m_Polygons[i].v2]++;
				edges[m_Polygons[i].v3]++;

				m_Normals[m_Polygons[i].v1] += N;
				m_Normals[m_Polygons[i].v2] += N;
				m_Normals[m_Polygons[i].v3] += N;
			}

			for (uint32 i = 0; i < m_NumVertices; ++i)
				if (edges > 0)
					m_Normals[i] /= (real32)edges[i];

			delete[] edges;
		}
	}
	Model::~Model()
	{
		delete[] m_Normals;
	}

	RenderEngine::RenderEngine(Window *game_window)
	{
		m_FrameBuffer = &game_window->back_buffer;
		if (game_window->style & DEPTH_BUFFER)
			m_DepthBuffer = &game_window->depth_buffer;
		else
			m_DepthBuffer = nullptr;
		
		SetClearColor(BLACK);
		Clear(COLOR_BUFFER | DEPTH_BUFFER);
				
		SetProjection((real32)m_FrameBuffer->width / (real32)m_FrameBuffer->height,
			30.0f, 5.0f, 1000.0f);
		SetView(&View);
		SetPointSize(1);
	}
	RenderEngine::~RenderEngine()
	{
	}
	
	void RenderEngine::DrawModel(Model *model, uint32 style)
	{
		Matrix4f MV = *m_View * model->m_TransformationMatrix;
		real32 halfwidth = m_FrameBuffer->width / 2.0f;
		real32 halfheight = m_FrameBuffer->height / 2.0f;
		real32 halfZ1 = (zFar - zNear) / 2.0f;
		real32 halfZ2 = (zFar + zNear) / 2.0f;
		real32 X = 0.0f, Y = 0.0f;
		Color c1;
		Color c2;
		Color c3;
		UV uv1;
		UV uv2;
		UV uv3;

		for (uint32 i = 0; i < model->m_NumPolygons; ++i) {
			Vertex v1 = model->m_Vertices[model->m_Polygons[i].v1];
			Vertex v2 = model->m_Vertices[model->m_Polygons[i].v2];
			Vertex v3 = model->m_Vertices[model->m_Polygons[i].v3];
			
			v1 *= MV;
			v2 *= MV;
			v3 *= MV;

			if (-v1.z < zNear || -v1.z > zFar || -v2.z < zNear || -v2.z > zFar || -v3.z < zNear || -v3.z > zFar)
				return;
				
			v1 *= m_Projection;
			v2 *= m_Projection;
			v3 *= m_Projection;
				
			v1 /= v1.w;
			v2 /= v2.w;
			v3 /= v3.w;

			v1.x = (halfwidth * v1.x) + (X + halfwidth);
			v1.y = (halfheight * v1.y) + (Y + halfheight);
			v1.z = (halfZ1 * v1.z) + halfZ2;
			v2.x = (halfwidth * v2.x) + (X + halfwidth);
			v2.y = (halfheight * v2.y) + (Y + halfheight);
			v2.z = (halfZ1 * v2.z) + halfZ2;
			v3.x = (halfwidth * v3.x) + (X + halfwidth);
			v3.y = (halfheight * v3.y) + (Y + halfheight);
			v3.z = (halfZ1 * v3.z) + halfZ2;

			if (model->m_Colors)
			{
				c1 = model->m_Colors[model->m_Polygons[i].v1];
				c2 = model->m_Colors[model->m_Polygons[i].v2];
				c3 = model->m_Colors[model->m_Polygons[i].v3];
			}

			if (model->m_Texture) {
				uv1 = model->m_UVs[model->m_Polygons[i].v1];
				uv2 = model->m_UVs[model->m_Polygons[i].v2];
				uv3 = model->m_UVs[model->m_Polygons[i].v3];
			}

			if (style & DRAW_TRIANGLES) {
				DrawTriangle(v1, c1, uv1, 
							 v2, c2, uv2, 
							 v3, c3, uv3,
							 model->m_Texture);
			}
			if (style & DRAW_LINES) {
				DrawLine(v1, c1, v2, c2);
				DrawLine(v2, c2, v3, c3);
				DrawLine(v3, c3, v1, c1);
			}
			if (style & DRAW_POINTS) {
				DrawPoint(v1, c1);
				DrawPoint(v2, c2);
				DrawPoint(v3, c3);
			}
		}
	}
	void RenderEngine::DrawTriangle(
		Vertex v1, const Color &color1, UV uv1,
		Vertex v2, const Color &color2, UV uv2,
		Vertex v3, const Color &color3, UV uv3,
		Texture *texture)
	{
		if (m_Flags & CULL_FACES)
			if (Normalize(v1, v2, v3).z <= 0)
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
	void RenderEngine::DrawSpansBetweenEdges(const Edge &long_edge, const Edge &short_edge, Texture *tex)
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

		for (real32 y = short_edge.m_Start.y; y < short_edge.m_End.y; y += 1.0f) {
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
	void RenderEngine::DrawSpan(const Span &span, real32 y, Texture *texture)
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

		for (real32 x = span.m_Start; x < span.m_End; x += 1.0f) {
			if (texture) {
				uint32 u = min((span.m_UV1.u + (udiff * factor)) * texture->width, 1440 - 1);
				uint32 v = min((span.m_UV1.v + (vdiff * factor)) * texture->height, 900 - 1);
				uint32 uv = (u + (v * texture->width)) * 4.0f;

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
	void RenderEngine::DrawLine(Vertex v1, const Color &color1, Vertex v2, const Color &color2)
	{
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

	void RenderEngine::DrawPoint(Vertex v, const Color &color)
	{
		for (int32 y = -m_PointSize; y < m_PointSize; ++y)
			for (int32 x = -m_PointSize; x < m_PointSize; ++x)
				DrawPixel((uint32)v.x + x, (uint32)v.y + y, color);
	}
	void RenderEngine::DrawPixel(uint32 x, uint32 y, const Color &color) {
		if (x >= m_FrameBuffer->width || y >= m_FrameBuffer->height)
			return;

		uint32 *pixel = (uint32 *)m_FrameBuffer->pixels;
		pixel[x + (y * m_FrameBuffer->width)] = color.toColor32();
	}
	void RenderEngine::Clear(uint8 flags)
	{
		int32 j = 0;
		int32 m = 0;
		if (flags & COLOR_BUFFER){
			uint32 *pixel = (uint32 *)m_FrameBuffer->pixels;

			for (uint32 i = 0; i < m_FrameBuffer->height * m_FrameBuffer->width; ++i) {
				pixel[i] = m_ClearColor;
			}
		}
		if (flags & DEPTH_BUFFER && m_DepthBuffer) {
			real32 *pixel = (real32 *)m_DepthBuffer->pixels;

			for (uint32 i = 0; i < m_DepthBuffer->height * m_DepthBuffer->width; ++i)
				pixel[i] = 1.0f;
		}
	}

	INTERN Matrix4f SetPerspective(real32 left, real32 right, real32 bottom, real32 top,real32 zNear, real32 zFar)
	{
		Matrix4f Perspective = IdentityMatrix;
		Perspective.a[0] = 2.0f * zNear / (right - left);
		Perspective.a[2] = (right + left) / (right - left);
		Perspective.a[5] = 2.0f * zNear / (top - bottom);
		Perspective.a[6] = (top + bottom) / (top - bottom);
		Perspective.a[10] = -(zFar + zNear) / (zFar - zNear);
		Perspective.a[11] = -(2.0f * zFar * zNear) / (zFar - zNear);
		Perspective.a[14] = -1.0f;
		Perspective.a[15] = 0.0f;

		return Perspective;
	}
	INTERN Matrix4f SetOrthographic(real32 left, real32 right, real32 bottom, real32 top, real32 zNear, real32 zFar)
	{
		Matrix4f Perspective = IdentityMatrix;
		Perspective.a[0] = 2.0f / (right - left);
		Perspective.a[3] = -(right + left) / (right - left);
		Perspective.a[5] = 2.0f / (top - bottom);
		Perspective.a[7] = -(top + bottom) / (top - bottom);
		Perspective.a[10] = -2.0f / (zFar - zNear);
		Perspective.a[11] = -(zFar + zNear) / (zFar - zNear);

		return Perspective;
	}
	void RenderEngine::SetProjection(real32 aspect_ration, real32 FOV, real32 z_near, real32 z_far)
	{
		const real32 DEG2RAD = PI / 180.0f;

		zNear = z_near;
		zFar = z_far;

		real32 tangent = tanf(FOV / 2 * DEG2RAD);
		real32 height = z_near * tangent;
		real32 width = height * aspect_ration;

		if (m_Flags & ORTHOGRAPHIC_PROJECTION)
			m_Projection = SetOrthographic(-width, width, -height, height, z_near, z_far);
		else
			m_Projection = SetPerspective(-width, width, -height, height, z_near, z_far);
	}
	void RenderEngine::SetView(Matrix4f *view)
	{
		m_View = view;
	}
	void RenderEngine::SetClearColor(const Color &color)
	{
		m_ClearColor = color.toColor32();
	}
	void RenderEngine::SetPointSize(int32 size)
	{
		m_PointSize = size;
	}
	void RenderEngine::SetFlag(AtlasEnum flag, bool32 value)
	{
		if (value)
			m_Flags |= flag;
		else
			m_Flags &= ~flag;
	}
}