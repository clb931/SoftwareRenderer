#ifndef ATLAS_VERTEX_H
#include "ATLAS_Color.h"
#include "ATLAS_Vector.h"


namespace ATLAS
{
	class Vertex
	{
	public:
		Vertex(Vector3f position = Vector3f(), UV uv = UV(), Color color = Color::WHITE);
		Vertex(Vector3f position, Vector4f normal, UV uv = UV(), Color color = Color::WHITE);
		
		void PerspectiveDivide();
		Vertex Lerp(const Vertex &vertex, const real32 lerp_amnt);
		bool32 IsInView();

		Vector3f pos;
		Vector4f norm;
		UV uv;
		Color color;
	};
}


#define ATLAS_VERTEX_H
#endif