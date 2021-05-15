#include "stdafx.h"
#include "ATLAS_Vertex.h"


namespace ATLAS
{
	Vertex::Vertex(Vector3f position, Vector2f uv, Vector3f color)
	{
		this->pos = position;
		this->norm = Vector3f();
		this->uv = uv;
		this->color = color;
	}

	Vertex::Vertex(Vector3f position, Vector3f normal, Vector2f uv, Vector3f color)
	{
		this->pos = position;
		this->norm = normal;
		this->uv = uv;
		this->color = color;
	}
}