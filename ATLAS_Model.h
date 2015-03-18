#ifndef ATLAS_MODEL_H
#include "ATLAS_Texture.h"

namespace ATLAS
{
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
}

#define ATLAS_MODEL_H
#endif