#include "stdafx.h"
#include "ATLAS_Model.h"


namespace ATLAS
{
	Polygon::Polygon(uint32 v1, uint32 v2, uint32 v3)
	{
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
	}

	void CalculateNormals(Model *model)
	{
		uint32 *edges = new uint32[model->m_NumVertices];

		for (uint32 i = 0; i < model->m_NumPolygons; ++i) {
			Vertex N = Normalize(
				model->m_Vertices[model->m_Polygons[i].v1].pos,
				model->m_Vertices[model->m_Polygons[i].v2].pos,
				model->m_Vertices[model->m_Polygons[i].v3].pos);

			N.pos = Normalize(N.pos);

			edges[model->m_Polygons[i].v1]++;
			edges[model->m_Polygons[i].v2]++;
			edges[model->m_Polygons[i].v3]++;

			model->m_Vertices[model->m_Polygons[i].v1].norm += N.pos;
			model->m_Vertices[model->m_Polygons[i].v2].norm += N.pos;
			model->m_Vertices[model->m_Polygons[i].v3].norm += N.pos;
		}

		for (uint32 i = 0; i < model->m_NumVertices; ++i)
			if (edges > 0)
				model->m_Vertices[i].norm /= (real32)edges[i];

		delete[] edges;
	}

	Model::Model()
	{
		m_Name[0] = '\0';
		m_TransformationMatrix = IdentityMatrix;
		m_NumVertices = 0;
		m_Vertices = nullptr;
		m_NumPolygons = 0;
		m_Polygons = nullptr;
		m_Texture = 0;
	}

	Model::Model(const char *file_path, Texture *texture) : Model()
	{
		char str[256] = "";
		FILE *pFile;
		uint16 chunk_id = 0;
		uint32 chunk_length = 0;
		int64 file_length = 0;

		printf("\nOpenging file...\n");
		fflush(stdout);

		fopen_s(&pFile, file_path, "rb");
		if (pFile) {
			fseek(pFile, 0, SEEK_END);
			file_length = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);

			printf("File Size: %.2fKB\n", file_length / 1024.0f);
			fflush(stdout);

			while (ftell(pFile) < file_length) {
				fread(&chunk_id, sizeof(uint16), 1, pFile);
				fread(&chunk_length, sizeof(uint32), 1, pFile);

				printf("Chunk: %.4X\nLength: %i\n", chunk_id, chunk_length);
				fflush(stdout);

				switch (chunk_id) {
				case 0x4D4D:
				case 0x3D3D:
				case 0x4100:
					break;

				case 0x4000: {
					uint32 i = 0;
					char c = '\0';

					do {
						fread(&c, sizeof(char), 1, pFile);
						m_Name[i++] = c;
					} while (c != '\0' && i < 30);

					printf("\tName: %s\n", m_Name);
					fflush(stdout);
				} break;

				case 0x4110: {
					fread(&m_NumVertices, sizeof(uint16), 1, pFile);

					printf("\tVertices: %i\n", m_NumVertices);
					fflush(stdout);

					m_Vertices = new Vertex[m_NumVertices];
					for (uint32 i = 0; i < m_NumVertices; ++i) {
						fread(&m_Vertices[i].pos.x, sizeof(real32), 1, pFile);
						fread(&m_Vertices[i].pos.y, sizeof(real32), 1, pFile);
						fread(&m_Vertices[i].pos.z, sizeof(real32), 1, pFile);

						m_Vertices[i].color = Color::WHITE;
					}
				} break;

				case 0x4120: {
					uint16 flag = 0;
					fread(&m_NumPolygons, sizeof(uint16), 1, pFile);

					printf("\tFaces: %i\n", m_NumPolygons);
					fflush(stdout);

					m_Polygons = new ATLAS::Polygon[m_NumPolygons];
					for (uint32 i = 0; i < m_NumPolygons; ++i) {
						fread(&m_Polygons[i].v1, sizeof(uint16), 1, pFile);
						fread(&m_Polygons[i].v2, sizeof(uint16), 1, pFile);
						fread(&m_Polygons[i].v3, sizeof(uint16), 1, pFile);
						fread(&flag, sizeof(uint16), 1, pFile);
					}
				} break;

				case 0x4140: {
					uint16 num = 0;
					fread(&num, sizeof(uint16), 1, pFile);

					printf("\tUVs: %i\n", num);
					fflush(stdout);

					for (uint32 i = 0; i < num; ++i) {
						fread(&m_Vertices[i].uv.u, sizeof(real32), 1, pFile);
						fread(&m_Vertices[i].uv.v, sizeof(real32), 1, pFile);
					}
				} break;

				default: {
					fseek(pFile, chunk_length - 6, SEEK_CUR);
				} break;
				}
			}

			m_TransformationMatrix = IdentityMatrix;
			m_Texture = texture;
			CalculateNormals(this);

			printf("Clossing File...\n\n");
			fflush(stdout);

			fclose(pFile);
		}
		else {
			printf("The following error occurred: ");
			printf("File does not exist.\n");
			fflush(stdout);
		}
	}

	Model::Model(Vertex *vertices, uint32 num_vertices,
		ATLAS::Polygon *polygons, uint32 num_polygons,
		Texture *texture)
	{
		m_Name[0] = '\0';
		m_TransformationMatrix = IdentityMatrix;
		m_NumVertices = num_vertices;
		m_Vertices = new Vertex[num_vertices];
		m_NumPolygons = num_polygons;
		m_Polygons = new ATLAS::Polygon[num_polygons];
		m_Texture = texture;

		for (uint32 i = 0; i < max(m_NumPolygons, m_NumVertices); ++i) {
			if (i < m_NumVertices)
				m_Vertices[i] = vertices[i];
			if (i < m_NumPolygons)
				m_Polygons[i] = polygons[i];
		}

		CalculateNormals(this);
	}

	Model::~Model()
	{
		if (m_NumVertices)
			delete[] m_Vertices;
		if (m_NumPolygons)
			delete[] m_Polygons;
	}

	void Model::SetName(char *name)
	{
		uint32 i = 0;

		while (name[i] != '\0' && i < 30) {
			m_Name[i] = name[i++];
		}

		m_Name[i] = '\0';
	}

	char *Model::GetName()
	{
		return m_Name;
	}
}