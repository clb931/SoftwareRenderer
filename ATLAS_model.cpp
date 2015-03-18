#include "ATLAS_Stdafx.h"
#include "ATLAS_Model.h"
#include "ATLAS_Texture.h"
#include <Windows.h>
using namespace ATLAS;

ATLAS::Polygon::Polygon(uint32 v1, uint32 v2, uint32 v3)
{
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;
}
void CalculateNormals(Model *model)
{
	model->m_Normals = new Vector4f[model->m_NumVertices];
	uint32 *edges = new uint32[model->m_NumVertices];

	for (uint32 i = 0; i < model->m_NumPolygons; ++i) {
		Vector4f N = Normalize(model->m_Vertices[model->m_Polygons[i].v1],
			model->m_Vertices[model->m_Polygons[i].v2],
			model->m_Vertices[model->m_Polygons[i].v3]);

		N = Normalize(N);

		edges[model->m_Polygons[i].v1]++;
		edges[model->m_Polygons[i].v2]++;
		edges[model->m_Polygons[i].v3]++;

		model->m_Normals[model->m_Polygons[i].v1] += N;
		model->m_Normals[model->m_Polygons[i].v2] += N;
		model->m_Normals[model->m_Polygons[i].v3] += N;
	}

	for (uint32 i = 0; i < model->m_NumVertices; ++i)
		if (edges > 0)
			model->m_Normals[i] /= (real32)edges[i];

	delete[] edges;
}
Model::Model(const char *file_path, Texture *texture)
{
	char str[256] = "";
	FILE *pFile;
	uint16 chunk_id = 0;
	uint32 chunk_length = 0;
	int64 file_length = 0;

	OutputDebugStringA("\nOpenging file...\n");
	fopen_s(&pFile, file_path, "rb");
	if (pFile) {
		fseek(pFile, 0, SEEK_END);
		file_length = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		sprintf_s(str, 256, "File Size: %.2fKB\n", file_length / 1024.0f);
		OutputDebugStringA(str);

		while (ftell(pFile) < file_length) {
			fread(&chunk_id, sizeof(uint16), 1, pFile);
			fread(&chunk_length, sizeof(uint32), 1, pFile);

			sprintf_s(str, 256, "Chunk: %.4X\nLength: %i\n", chunk_id, chunk_length);
			OutputDebugStringA(str);

			switch (chunk_id) {
			case 0x4D4D:
			case 0x3D3D:
			case 0x4100:
				break;

			case 0x4000: {
				char name[32] = "\t";
				uint32 i = 1;
				char c = '\0';

				do {
					fread(&c, sizeof(char), 1, pFile);
					name[i++] = c;
				} while (c != '\0' && i < 30);

				name[i++] = '\n';
				OutputDebugStringA(name);
			} break;

			case 0x4110: {
				fread(&m_NumVertices, sizeof(uint16), 1, pFile);

				sprintf_s(str, 256, "\tVertices: %i\n", m_NumVertices);
				OutputDebugStringA(str);

				m_Vertices = new Vertex[m_NumVertices];
				for (uint32 i = 0; i < m_NumVertices; ++i) {
					fread(&m_Vertices[i].x, sizeof(real32), 1, pFile);
					fread(&m_Vertices[i].y, sizeof(real32), 1, pFile);
					fread(&m_Vertices[i].z, sizeof(real32), 1, pFile);
				}
			} break;

			case 0x4120: {
				uint16 flag = 0;
				fread(&m_NumPolygons, sizeof(uint16), 1, pFile);

				sprintf_s(str, 256, "\tFaces: %i\n", m_NumPolygons);
				OutputDebugStringA(str);

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

				sprintf_s(str, 256, "\tUVs: %i\n", num);
				OutputDebugStringA(str);

				m_UVs = new UV[num];
				for (uint32 i = 0; i < num; ++i) {
					fread(&m_UVs[i].u, sizeof(real32), 1, pFile);
					fread(&m_UVs[i].v, sizeof(real32), 1, pFile);
				}
			} break;

			default: {
				fseek(pFile, chunk_length - 6, SEEK_CUR);
			} break;
			}
		}

		m_TransformationMatrix = IdentityMatrix;
		m_Texture = texture;
		m_Colors = nullptr;
		CalculateNormals(this);

		OutputDebugStringA("Clossing File...\n\n");
		fclose(pFile);
	}
	else {
		perror("The following error occurred");
		OutputDebugStringA("File does not exist.\n");
	}
}
Model::Model(Vertex *vertices, uint32 num_vertices,
	ATLAS::Polygon *polygons, uint32 num_polygons,
	UV *uvs, Texture *texture,
	Color *colors, Vertex *normals)
{
	m_TransformationMatrix = IdentityMatrix;
	m_NumVertices = num_vertices;
	m_Vertices = new Vertex[num_vertices];
	m_NumPolygons = num_polygons;
	m_Polygons = new ATLAS::Polygon[num_polygons];
	m_UVs = new UV[num_vertices];
	m_Texture = texture;
	m_Colors = colors;

	for (uint32 i = 0; i < max(m_NumPolygons, m_NumVertices); ++i) {
		if (i < m_NumVertices) {
			m_Vertices[i] = vertices[i];
			m_UVs[i] = uvs[i];

			if (normals)
				m_Normals[i] = normals[i];
		}
		if (i < m_NumPolygons)
			m_Polygons[i] = polygons[i];
	}

	if (!normals)
		CalculateNormals(this);
}
Model::~Model()
{
	delete[] m_Vertices;
	delete[] m_Polygons;
	delete[] m_UVs;
	delete[] m_Normals;
}