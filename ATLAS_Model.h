#ifndef ATLAS_MODEL_H
#include "ATLAS_Matrix.h"
#include "ATLAS_Texture.h"
#include "ATLAS_Vertex.h"


namespace ATLAS
{
	class Polygon
	{
	public:
		Polygon(uint32 v1 = 0, uint32 v2 = 0, uint32 v3 = 0);

		union {
			uint32 v[3];
			struct {
				uint32 v1;
				uint32 v2;
				uint32 v3;
			};
		};
	};

	class Model
	{
	public:
		Model();
		Model(const char *file_path);
		Model(const char *name, Vertex *vertices, uint32 num_vertices,
			Polygon *polygons, uint32 num_polygons);
		~Model();

		void SetName(char *name);
		char *GetName();

		uint32		m_NumVertices;
		Vertex		*m_Vertices;
		uint32		m_NumPolygons;
		Polygon		*m_Polygons;

	private:
		char m_Name[32];
	};
}


#define ATLAS_MODEL_H
#endif