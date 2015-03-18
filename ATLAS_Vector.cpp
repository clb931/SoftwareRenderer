#include "ATLAS_Stdafx.h"

namespace ATLAS
{
	Vector4f::Vector4f(real32 x, real32 y, real32 z, real32 w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Vector4f &Vector4f::operator+=(const Vector4f &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;

		return *this;
	}
	Vector4f &Vector4f::operator-=(const Vector4f &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;

		return *this;
	}
	Vector4f &Vector4f::operator*=(const Vector4f &rhs)
	{
		real32 xx = x, yy = y, zz = z;

		x = yy * rhs.z - zz * rhs.y;
		y = xx * rhs.z - zz * rhs.x;
		z = xx * rhs.y - yy * rhs.x;
		w = 0.0f;

		return *this;
	}
	Vector4f &Vector4f::operator*=(const Matrix4f &rhs)
	{
		*this = rhs * *this;
		return *this;
	}
	Vector4f &Vector4f::operator&=(const Matrix4f &rhs)
	{
		*this = *this * rhs;
		return *this;
	}
	Vector4f &Vector4f::operator*=(const real32 &rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		w *= rhs;

		return *this;
	}
	Vector4f &Vector4f::operator/=(const real32 &rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		w /= rhs;

		return *this;
	}

	real32 Vector4f::operator~()
	{
		return sqrtf(*this ^ *this);
	}
	
	Vector4f operator+(Vector4f lhs, const Vector4f &rhs)
	{
		return lhs += rhs;
	}
	Vector4f operator-(Vector4f lhs, const Vector4f &rhs)
	{
		return lhs -= rhs;
	}
	Vector4f operator*(Vector4f lhs, const Vector4f &rhs)
	{
		return lhs *= rhs;
	}
	Vector4f operator*(Vector4f lhs, const Matrix4f &rhs)
	{
		return Vector4f(
			lhs.x * rhs.a2[0][0] + lhs.y * rhs.a2[1][0] + lhs.z * rhs.a2[2][0] + lhs.w * rhs.a2[3][0],
			lhs.x * rhs.a2[0][1] + lhs.y * rhs.a2[1][1] + lhs.z * rhs.a2[2][1] + lhs.w * rhs.a2[3][1],
			lhs.x * rhs.a2[0][2] + lhs.y * rhs.a2[1][2] + lhs.z * rhs.a2[2][2] + lhs.w * rhs.a2[3][2],
			lhs.x * rhs.a2[0][3] + lhs.y * rhs.a2[1][3] + lhs.z * rhs.a2[2][3] + lhs.w * rhs.a2[3][3]
		);
	}
	Vector4f operator*(const Matrix4f &lhs, const Vector4f &rhs)
	{
		Vector4f result =
		{
			lhs.a2[0][0] * rhs.x + lhs.a2[0][1] * rhs.y + lhs.a2[0][2] * rhs.z + lhs.a2[0][3] * rhs.w,
			lhs.a2[1][0] * rhs.x + lhs.a2[1][1] * rhs.y + lhs.a2[1][2] * rhs.z + lhs.a2[1][3] * rhs.w,
			lhs.a2[2][0] * rhs.x + lhs.a2[2][1] * rhs.y + lhs.a2[2][2] * rhs.z + lhs.a2[2][3] * rhs.w,
			lhs.a2[3][0] * rhs.x + lhs.a2[3][1] * rhs.y + lhs.a2[3][2] * rhs.z + lhs.a2[3][3] * rhs.w
		};

		return result;
	}
	Vector4f operator*(Vector4f lhs, const real32 &rhs)
	{
		return lhs *= rhs;
	}
	Vector4f operator/(Vector4f lhs, const real32 &rhs)
	{
		return lhs /= rhs;
	}
	real32 operator^(Vector4f lhs, const Vector4f &rhs)
	{
		return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w);
	}

	inline bool operator==(const Vector4f &lhs, const Vector4f &rhs)
	{
		if (lhs.x != rhs.x || lhs.y != rhs.y || lhs.z == rhs.z)
			return false;

		return true;
	}
	inline bool operator!=(const Vector4f &lhs, const Vector4f &rhs)
	{
		return !(lhs == rhs);
	}

	Vector4f Normalize(Vector4f v)
	{
		return v / ~v;
	}
	Vector4f Normalize(Vector4f v1, Vector4f v2, Vector4f v3)
	{
		return (v2 - v1) * (v3 - v1);
	}

	string ToString(Vector4f vec)
	{
		char str[128] = "";
		sprintf_s(str, 128,
			"\n[%+7.1f][%+7.1f][%+7.1f][%+7.1f]\n\n",
			vec.a[0], vec.a[1], vec.a[2], vec.a[3]);
		return str;
	}

	Vector2f::Vector2f(real32 u, real32 v)
	{
		this->u = u;
		this->v = v;
	}

	Vector2f &Vector2f::operator+=(const Vector2f &rhs)
	{
		u += rhs.u;
		v += rhs.v;
		return *this;
	}
	Vector2f &Vector2f::operator-=(const Vector2f &rhs)
	{
		u -= rhs.u;
		v -= rhs.v;
		return *this;
	}
	Vector2f &Vector2f::operator*=(const real32 &rhs)
	{
		u *= rhs;
		v *= rhs;
		return *this;
	}
	Vector2f &Vector2f::operator/=(const real32 &rhs)
	{
		u /= rhs;
		v /= rhs;
		return *this;
	}

	Vector2f operator+(Vector2f lhs, const Vector2f &rhs)
	{
		return lhs += rhs;
	}
	Vector2f operator-(Vector2f lhs, const Vector2f &rhs)
	{
		return lhs -= rhs;
	}
	Vector2f operator*(Vector2f lhs, const real32 &rhs)
	{
		return lhs *= rhs;
	}
	Vector2f operator/(Vector2f lhs, const real32 &rhs)
	{
		return lhs /= rhs;
	}
}