#ifndef ATLAS_MATRIX_H


namespace ATLAS
{
	union Matrix4f
	{
		friend Matrix4f operator+(const Matrix4f &lhs, const Matrix4f &rhs);
		friend Matrix4f operator-(const Matrix4f &lhs, const Matrix4f &rhs);
		friend Matrix4f operator*(const Matrix4f &lhs, const Matrix4f &rhs);
		friend Matrix4f operator*(const Matrix4f &lhs, const real32 &rhs);
		friend Matrix4f operator/(const Matrix4f &lhs, const real32 &rhs);

		real32 a[16];
		real32 a2[4][4];
	};

	Matrix4f operator+(const Matrix4f &lhs, const Matrix4f &rhs);
	Matrix4f operator-(const Matrix4f &lhs, const Matrix4f &rhs);
	Matrix4f operator*(const Matrix4f &lhs, const Matrix4f &rhs);
	Matrix4f operator*(const Matrix4f &lhs, const real32 &rhs);
	Matrix4f operator/(const Matrix4f &lhs, const real32 &rhs);

	extern const Matrix4f IdentityMatrix4f;
	Matrix4f InvertMatrix4f(const Matrix4f &m);
	Matrix4f ScaleMatrix4f(real32 x, real32 y, real32 z);
	Matrix4f TranslationMatrix4f(real32 x = 1, real32 y = 1, real32 z = 1);
	Matrix4f RotationMatrix4f(real32 x = 1, real32 y = 1, real32 z = 1);
	Matrix4f PerspectiveMatrix4f(real32 aspect_ratio, real32 FOV, real32 near_z, real32 far_z);
	Matrix4f OrthograohicMatrix4f(real32 right, real32 left, real32 bottom, real32 top, real32 z_near, real32 z_far);
	Matrix4f ScreenSpaceMatrix4f(uint32 width, uint32 height, real32 z_near = 0.0f, real32 z_far = 1.0f);
	string ToString(Matrix4f mat);

	
	union Matrix3f
	{
		friend Matrix3f operator+(const Matrix3f &lhs, const Matrix3f &rhs);
		friend Matrix3f operator-(const Matrix3f &lhs, const Matrix3f &rhs);
		friend Matrix3f operator*(const Matrix3f &lhs, const Matrix3f &rhs);
		friend Matrix3f operator*(const Matrix3f &lhs, const real32 &rhs);
		friend Matrix3f operator/(const Matrix3f &lhs, const real32 &rhs);

		real32 a[9];
		real32 a2[3][3];
	};

	Matrix3f operator+(const Matrix3f &lhs, const Matrix3f &rhs);
	Matrix3f operator-(const Matrix3f &lhs, const Matrix3f &rhs);
	Matrix3f operator*(const Matrix3f &lhs, const Matrix3f &rhs);
	Matrix3f operator*(const Matrix3f &lhs, const real32 &rhs);
	Matrix3f operator/(const Matrix3f &lhs, const real32 &rhs);

	extern const Matrix3f IdentityMatrix3f;
	Matrix3f InvertMatrix3f(const Matrix3f &m);
	string ToString(Matrix3f mat);
}


#define ATLAS_MATRIX_H
#endif