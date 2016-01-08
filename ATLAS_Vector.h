#ifndef ATLAS_VECTOR_H
#include "ATLAS_Matrix.h"


namespace ATLAS
{
	class Vector4f
	{
	public:
		Vector4f(real32 x = 0.0f, real32 y = 0.0f, real32 z = 0.0f, real32 w = 1.0f);

		Vector4f &operator+=(const Vector4f &rhs);
		Vector4f &operator-=(const Vector4f &rhs);
		Vector4f &operator*=(const Vector4f &rhs);
		Vector4f &operator*=(const Matrix4f &rhs);
		Vector4f &operator&=(const Matrix4f &rhs);
		Vector4f &operator*=(const real32 &rhs);
		Vector4f &operator/=(const real32 &rhs);

		real32 operator~();

		friend Vector4f operator+(Vector4f lhs, const Vector4f &rhs);
		friend Vector4f operator-(Vector4f lhs, const Vector4f &rhs);
		friend Vector4f operator*(Vector4f lhs, const Vector4f &rhs);
		friend Vector4f operator*(Vector4f lhs, const Matrix4f &rhs);
		friend Vector4f operator*(const Matrix4f &lhs, const Vector4f &rhs);
		friend Vector4f operator*(Vector4f lhs, const real32 &rhs);
		friend Vector4f operator/(Vector4f lhs, const real32 &rhs);
		friend real32 operator^(Vector4f lhs, const Vector4f &rhs);

		union
		{
			real32 a[4];
			struct { real32 x, y, z, w; };
		};
	};
	
	Vector4f operator+(Vector4f lhs, const Vector4f &rhs);
	Vector4f operator-(Vector4f lhs, const Vector4f &rhs);
	Vector4f operator*(Vector4f lhs, const Vector4f &rhs);
	Vector4f operator*(Vector4f lhs, const Matrix4f &rhs);
	Vector4f operator*(const Matrix4f &lhs, const Vector4f &rhs);
	Vector4f operator*(Vector4f lhs, const real32 &rhs);
	Vector4f operator/(Vector4f lhs, const real32 &rhs);
	real32 operator^(Vector4f lhs, const Vector4f &rhs);

	inline bool operator==(const Vector4f &lhs, const Vector4f &rhs);
	inline bool operator!=(const Vector4f &lhs, const Vector4f &rhs);

	Vector4f Normalize(Vector4f v);
	Vector4f Normalize(Vector4f v1, Vector4f v2, Vector4f v3);

	string ToString(Vector4f vec);

	class Vector2f
	{
	public:
		Vector2f(real32 u = 0.0f, real32 v = 0.0f);

		Vector2f &operator+=(const Vector2f &rhs);
		Vector2f &operator-=(const Vector2f &rhs);
		Vector2f &operator*=(const Vector2f &rhs);
		Vector2f &operator*=(const real32 &rhs);
		Vector2f &operator/=(const real32 &rhs);

		friend Vector2f operator+(Vector2f lhs, const Vector2f &rhs);
		friend Vector2f operator-(Vector2f lhs, const Vector2f &rhs);
		friend Vector2f operator*(Vector2f lhs, const Vector2f &rhs);
		friend Vector2f operator*(Vector2f lhs, const real32 &rhs);
		friend Vector2f operator/(Vector2f lhs, const real32 &rhs);

		union
		{
			real32 a[2];
			struct { real32 x, y; };
			struct { real32 u, v; };
		};
	};
	typedef Vector2f UV;

	Vector2f operator+(Vector2f lhs, const Vector2f &rhs);
	Vector2f operator-(Vector2f lhs, const Vector2f &rhs);
	Vector2f operator*(Vector2f lhs, const Vector2f &rhs);
	Vector2f operator*(Vector2f lhs, const real32 &rhs);
	Vector4f operator/(Vector4f lhs, const real32 &rhs);
}


#define ATLAS_VECTOR_H
#endif