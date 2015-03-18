#include "ATLAS_Stdafx.h"
#include "ATLAS_Vertex.h"
using namespace ATLAS;

Vertex::Vertex(Vector4f position, UV uv, Color color)
{
	this->pos = position;
	this->uv = uv;
	this->color = color;
}
