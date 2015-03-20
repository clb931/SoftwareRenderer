#include "ATLAS_Stdafx.h"
#include "ATLAS_Vertex.h"
using namespace ATLAS;

Vertex::Vertex(Vector4f position, UV uv, Color color)
{
	this->pos = position;
	this->uv = uv;
	this->color = color;
}
void Vertex::PerspectiveDivide()
{
	pos.x /= pos.w;
	pos.y /= pos.w;
	pos.z /= pos.w;

	uv.u /= pos.w;
	uv.v /= pos.w;

	color.R /= pos.w;
	color.G /= pos.w;
	color.B /= pos.w;
}
