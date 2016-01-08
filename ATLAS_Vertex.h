#ifndef ATLAS_VERTEX_H
#include "ATLAS_Color.h"
#include "ATLAS_Vector.h"


namespace ATLAS
{
	class Vertex
	{
	public:
		Vertex(Vector4f position = Vector4f(), UV uv = UV(), Color color = Color::WHITE);
		Vertex(Vector4f position, Vector4f normal, UV uv = UV(), Color color = Color::WHITE);
		
		void PerspectiveDivide();
		Vertex Lerp(const Vertex &vertex, const real32 lerp_amnt);
		bool32 IsInView();

		Vector4f pos;
		Vector4f norm;
		UV uv;
		Color color;
	};
}


#define ATLAS_VERTEX_H
#endif