#ifndef ATLAS_MATRIX_H

namespace ATLAS
{
	union Vector4f;

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

	extern const Matrix4f IdentityMatrix;
	Matrix4f InvertMatrix(const Matrix4f &m);
	Matrix4f ScaleMatrix(real32 x, real32 y, real32 z);
	Matrix4f TranslationMatrix(real32 x = 1, real32 y = 1, real32 z = 1);
	Matrix4f RotationMatrix(real32 x = 1, real32 y = 1, real32 z = 1);
	Matrix4f PerspectiveMatrix(real32 aspect_ratio, real32 FOV, real32 near_z, real32 far_z);
	Matrix4f OrthograohicMatrix(real32 right, real32 left, real32 bottom, real32 top, real32 z_near, real32 z_far);
	Matrix4f ScreenSpaceMatrix(uint32 width, uint32 height, real32 z_near = 0.0f, real32 z_far = 1.0f);


	string ToString(Matrix4f mat);
}

#define ATLAS_MATRIX_H_
#endif