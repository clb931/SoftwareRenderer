#ifndef ATLAS_VECTOR_H

namespace ATLAS
{
	union Matrix4f;

	union Vector4f
	{
		Vector4f(real32 x = 0.0f, real32 y = 0.0f, real32 z = 0.0f, real32 w = 1.0f);

		Vector4f &operator+=(const Vector4f &rhs);
		Vector4f &operator-=(const Vector4f &rhs);
		Vector4f &operator*=(const Vector4f &rhs);
		Vector4f &operator*=(const Matrix4f &rhs);
		Vector4f &operator&=(const Matrix4f &rhs);
		Vector4f &operator*=(const real32 &rhs);
		Vector4f &operator/=(const real32 &rhs);
		real32 operator^=(const Vector4f &rhs);

		real32 operator~();

		friend Vector4f operator+(Vector4f lhs, const Vector4f &rhs);
		friend Vector4f operator-(Vector4f lhs, const Vector4f &rhs);
		friend Vector4f operator*(Vector4f lhs, const Vector4f &rhs);
		friend Vector4f operator*(Vector4f lhs, const Matrix4f &rhs);
		friend Vector4f operator*(const Matrix4f &lhs, const Vector4f &rhs);
		friend Vector4f operator*(Vector4f lhs, const real32 &rhs);
		friend Vector4f operator/(Vector4f lhs, const real32 &rhs);
		friend real32 operator^(Vector4f lhs, const Vector4f &rhs);

		real32 a[4];
		struct { real32 x, y, z, w; };
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

	union Vector2f
	{
		Vector2f(real32 u = 0.0f, real32 v = 0.0f);

		real32 a[2];
		struct { real32 u, v; };
	};
	typedef Vector2f UV;
}

#define ATLAS_VECTOR_H_
#endif