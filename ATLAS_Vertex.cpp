#include "ATLAS_Stdafx.h"
#include "ATLAS_Vertex.h"
using namespace ATLAS;

Vertex::Vertex(Vector4f position, UV uv, Color color)
{
	this->pos = position;
	this->uv = uv;
	this->color = color;
}
Vertex &Vertex::operator /= (const real32 &rhs)
{
	pos.x /= rhs;
	pos.y /= rhs;
	pos.z /= rhs;

	uv.u /= rhs;
	uv.v /= rhs;

	color.R /= rhs;
	color.G /= rhs;
	color.B /= rhs;

	return *this;
}
