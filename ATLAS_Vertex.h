#ifndef ATLAS_VERTEX_H

namespace ATLAS
{
	struct Vertex
	{
		Vertex(Vector4f position = Vector4f(), UV uv = UV(), Color color = Color());
		Vertex &operator/=(const real32 &rhs);

		Vector4f pos;
		UV uv;
		Color color;
	};
}

#define ATLAS_VERTEX_H
#endif