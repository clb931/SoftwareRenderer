#ifndef ATLAS_VERTEX_H
#include "ATLAS_Color.h"
#include "ATLAS_Vector.h"


namespace ATLAS
{
	class Vertex
	{
	public:
		Vertex(Vector3f position = Vector3f(), Vector2f uv = Vector2f(), Vector3f color = Vector3f());
		Vertex(Vector3f position, Vector3f normal, Vector2f uv = Vector2f(), Vector3f color = Vector3f());
		
		Vector3f pos;
		Vector3f color;
		Vector3f norm;
		Vector2f uv;
	};
}


#define ATLAS_VERTEX_H
#endif