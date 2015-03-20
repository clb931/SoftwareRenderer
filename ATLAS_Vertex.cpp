#include "ATLAS_Stdafx.h"
#include "ATLAS_Vertex.h"
using namespace ATLAS;

Vertex::Vertex(Vector4f position, UV uv, Color color)
{
	this->pos = position;
	this->norm = Vector4f();
	this->uv = uv;
	this->color = color;
}
Vertex::Vertex(Vector4f position, Vector4f normal, UV uv, Color color)
{
	this->pos = position;
	this->norm = normal;
	this->uv = uv;
	this->color = color;
}

void Vertex::PerspectiveDivide()
{
	pos.x /= pos.w;
	pos.y /= pos.w;
	pos.z /= pos.w;

	norm /= pos.w;

	uv /= pos.w;

	color =  color / pos.w;
}
Vertex Vertex::Lerp(const Vertex &vertex, const real32 lerp_amnt)
{
	return Vertex(lerp(pos, vertex.pos, lerp_amnt),
		lerp(norm, vertex.norm, lerp_amnt),
		lerp(uv, vertex.uv, lerp_amnt),
		lerp(color, vertex.color, lerp_amnt));
}