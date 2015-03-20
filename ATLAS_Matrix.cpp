#include "ATLAS_Stdafx.h"

namespace ATLAS
{
	Matrix4f operator+(const Matrix4f &lhs, const Matrix4f &rhs)
	{
		Matrix4f result =
		{
			lhs.a2[0][0] + rhs.a2[0][0], lhs.a2[0][1] + rhs.a2[0][1], lhs.a2[0][2] + rhs.a2[0][2], lhs.a2[0][3] + rhs.a2[0][3],
			lhs.a2[1][0] + rhs.a2[1][0], lhs.a2[1][1] + rhs.a2[1][1], lhs.a2[1][2] + rhs.a2[1][2], lhs.a2[1][3] + rhs.a2[1][3],
			lhs.a2[2][0] + rhs.a2[2][0], lhs.a2[2][1] + rhs.a2[2][1], lhs.a2[2][2] + rhs.a2[2][2], lhs.a2[2][3] + rhs.a2[2][3],
			lhs.a2[3][0] + rhs.a2[3][0], lhs.a2[3][1] + rhs.a2[3][1], lhs.a2[3][2] + rhs.a2[3][2], lhs.a2[3][3] + rhs.a2[3][3]
		};
		return result;
	}
	Matrix4f operator-(const Matrix4f &lhs, const Matrix4f &rhs)
	{
		Matrix4f result =
		{
			lhs.a2[0][0] - rhs.a2[0][0], lhs.a2[0][1] - rhs.a2[0][1], lhs.a2[0][2] - rhs.a2[0][2], lhs.a2[0][3] - rhs.a2[0][3],
			lhs.a2[1][0] - rhs.a2[1][0], lhs.a2[1][1] - rhs.a2[1][1], lhs.a2[1][2] - rhs.a2[1][2], lhs.a2[1][3] - rhs.a2[1][3],
			lhs.a2[2][0] - rhs.a2[2][0], lhs.a2[2][1] - rhs.a2[2][1], lhs.a2[2][2] - rhs.a2[2][2], lhs.a2[2][3] - rhs.a2[2][3],
			lhs.a2[3][0] - rhs.a2[3][0], lhs.a2[3][1] - rhs.a2[3][1], lhs.a2[3][2] - rhs.a2[3][2], lhs.a2[3][3] - rhs.a2[3][3]
		};
		return result;
	}
	Matrix4f operator*(const Matrix4f &lhs, const Matrix4f &rhs)
	{
		Matrix4f result =
		{
/*[0][0]*/	lhs.a2[0][0] * rhs.a2[0][0] + lhs.a2[0][1] * rhs.a2[1][0] + lhs.a2[0][2] * rhs.a2[2][0] + lhs.a2[0][3] * rhs.a2[3][0],
/*[0][1]*/	lhs.a2[0][0] * rhs.a2[0][1] + lhs.a2[0][1] * rhs.a2[1][1] + lhs.a2[0][2] * rhs.a2[2][1] + lhs.a2[0][3] * rhs.a2[3][1],
/*[0][2]*/	lhs.a2[0][0] * rhs.a2[0][2] + lhs.a2[0][1] * rhs.a2[1][2] + lhs.a2[0][2] * rhs.a2[2][2] + lhs.a2[0][3] * rhs.a2[3][2],
/*[0][3]*/	lhs.a2[0][0] * rhs.a2[0][3] + lhs.a2[0][1] * rhs.a2[1][3] + lhs.a2[0][2] * rhs.a2[2][3] + lhs.a2[0][3] * rhs.a2[3][3],

/*[1][0]*/	lhs.a2[1][0] * rhs.a2[0][0] + lhs.a2[1][1] * rhs.a2[1][0] + lhs.a2[1][2] * rhs.a2[2][0] + lhs.a2[1][3] * rhs.a2[3][0],
/*[1][1]*/	lhs.a2[1][0] * rhs.a2[0][1] + lhs.a2[1][1] * rhs.a2[1][1] + lhs.a2[1][2] * rhs.a2[2][1] + lhs.a2[1][3] * rhs.a2[3][1],
/*[1][2]*/	lhs.a2[1][0] * rhs.a2[0][2] + lhs.a2[1][1] * rhs.a2[1][2] + lhs.a2[1][2] * rhs.a2[2][2] + lhs.a2[1][3] * rhs.a2[3][2],
/*[1][3]*/	lhs.a2[1][0] * rhs.a2[0][3] + lhs.a2[1][1] * rhs.a2[1][3] + lhs.a2[1][2] * rhs.a2[2][3] + lhs.a2[1][3] * rhs.a2[3][3],

/*[2][0]*/	lhs.a2[2][0] * rhs.a2[0][0] + lhs.a2[2][1] * rhs.a2[1][0] + lhs.a2[2][2] * rhs.a2[2][0] + lhs.a2[2][3] * rhs.a2[3][0],
/*[2][1]*/	lhs.a2[2][0] * rhs.a2[0][1] + lhs.a2[2][1] * rhs.a2[1][1] + lhs.a2[2][2] * rhs.a2[2][1] + lhs.a2[2][3] * rhs.a2[3][1],
/*[2][2]*/	lhs.a2[2][0] * rhs.a2[0][2] + lhs.a2[2][1] * rhs.a2[1][2] + lhs.a2[2][2] * rhs.a2[2][2] + lhs.a2[2][3] * rhs.a2[3][2],
/*[2][3]*/	lhs.a2[2][0] * rhs.a2[0][3] + lhs.a2[2][1] * rhs.a2[1][3] + lhs.a2[2][2] * rhs.a2[2][3] + lhs.a2[2][3] * rhs.a2[3][3],

/*[3][0]*/	lhs.a2[3][0] * rhs.a2[0][0] + lhs.a2[3][1] * rhs.a2[1][0] + lhs.a2[3][2] * rhs.a2[2][0] + lhs.a2[3][3] * rhs.a2[3][0],
/*[3][1]*/	lhs.a2[3][0] * rhs.a2[0][1] + lhs.a2[3][1] * rhs.a2[1][1] + lhs.a2[3][2] * rhs.a2[2][1] + lhs.a2[3][3] * rhs.a2[3][1],
/*[3][2]*/	lhs.a2[3][0] * rhs.a2[0][2] + lhs.a2[3][1] * rhs.a2[1][2] + lhs.a2[3][2] * rhs.a2[2][2] + lhs.a2[3][3] * rhs.a2[3][2],
/*[3][3]*/	lhs.a2[3][0] * rhs.a2[0][3] + lhs.a2[3][1] * rhs.a2[1][3] + lhs.a2[3][2] * rhs.a2[2][3] + lhs.a2[3][3] * rhs.a2[3][3]
		};
		return result;
	}
	Matrix4f operator*(const Matrix4f &lhs, const real32 &rhs)
	{
		Matrix4f result =
		{
			lhs.a2[0][0] * rhs, lhs.a2[0][1] * rhs, lhs.a2[0][2] * rhs, lhs.a2[0][3] * rhs,
			lhs.a2[1][0] * rhs, lhs.a2[1][1] * rhs, lhs.a2[1][2] * rhs, lhs.a2[1][3] * rhs,
			lhs.a2[2][0] * rhs, lhs.a2[2][1] * rhs, lhs.a2[2][2] * rhs, lhs.a2[2][3] * rhs,
			lhs.a2[3][0] * rhs, lhs.a2[3][1] * rhs, lhs.a2[3][2] * rhs, lhs.a2[3][3] * rhs
		};
		return result;
	}
	Matrix4f operator/(const Matrix4f &lhs, const real32 &rhs)
	{
		Matrix4f result =
		{
			lhs.a2[0][0] / rhs, lhs.a2[0][1] / rhs, lhs.a2[0][2] / rhs, lhs.a2[0][3] / rhs,
			lhs.a2[1][0] / rhs, lhs.a2[1][1] / rhs, lhs.a2[1][2] / rhs, lhs.a2[1][3] / rhs,
			lhs.a2[2][0] / rhs, lhs.a2[2][1] / rhs, lhs.a2[2][2] / rhs, lhs.a2[2][3] / rhs,
			lhs.a2[3][0] / rhs, lhs.a2[3][1] / rhs, lhs.a2[3][2] / rhs, lhs.a2[3][3] / rhs
		};
		return result;
	}
	
	const Matrix4f IdentityMatrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	Matrix4f InvertMatrix(const Matrix4f &m)
	{
		Matrix4f mat = 
		{
			m.a2[0][0], m.a2[1][0], m.a2[2][0], m.a2[3][0],
			m.a2[0][1], m.a2[1][1], m.a2[2][1], m.a2[3][1],
			m.a2[0][2], m.a2[1][2], m.a2[2][2], m.a2[3][2],
			m.a2[0][3], m.a2[1][3], m.a2[2][3], m.a2[3][3],
		};

		return mat;
	}
	Matrix4f ScaleMatrix(real32 x, real32 y, real32 z)
	{
		Matrix4f mat = IdentityMatrix;
		mat.a2[0][0] = x;
		mat.a2[1][1] = y;
		mat.a2[2][2] = z;

		return mat;
	}
	Matrix4f TranslationMatrix(real32 x, real32 y, real32 z)
	{
		Matrix4f mat = IdentityMatrix;
		mat.a2[0][3] = x;
		mat.a2[1][3] = y;
		mat.a2[2][3] = z;

		return mat;
	}
	Matrix4f RotationMatrix(real32 x, real32 y, real32 z)
	{
		real32 cos_theta;
		real32 sin_theta;
		Matrix4f mat = IdentityMatrix;
		
		if (x)
		{
			x /= 100.0f;
			cos_theta = cosf(x);
			sin_theta = sinf(x);

			Matrix4f matX = IdentityMatrix;
			matX.a2[1][1] = cos_theta;
			matX.a2[1][2] = -sin_theta;
			matX.a2[2][1] = sin_theta;
			matX.a2[2][2] = cos_theta;

			mat = mat * matX;
		}

		if (y)
		{
			y /= 100.0f;
			cos_theta = cosf(y);
			sin_theta = sinf(y);

			Matrix4f matY = IdentityMatrix;
			matY.a2[0][0] = cos_theta;
			matY.a2[0][2] = sin_theta;
			matY.a2[2][0] = -sin_theta;
			matY.a2[2][2] = cos_theta;

			mat = mat * matY;
		}

		if (z)
		{
			z /= 100.0f;
			cos_theta = cosf(z);
			sin_theta = sinf(z);

			Matrix4f matZ = IdentityMatrix;
			matZ.a2[0][0] = cos_theta;
			matZ.a2[0][1] = -sin_theta;
			matZ.a2[1][0] = sin_theta;
			matZ.a2[1][1] = cos_theta;

			mat = mat * matZ;
		}

		return mat;
	}
	Matrix4f PerspectiveMatrix(real32 aspect_ratio, real32 FOV, real32 z_near, real32 z_far)
	{
		real32 tan_half_fov = tanf(ToRadians(0.5f * FOV));
		real32 z_range = z_near = z_far;


		Matrix4f Projection =
		{
			1.0f / (tan_half_fov * aspect_ratio), 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f / tan_half_fov, 0.0f, 0.0f,
			0.0f, 0.0f, (-z_near - z_far) / z_range, 2.0f * z_far * z_near / z_range,
			0.0f, 0.0f, 1.0f, 0.0f
		};

		return Projection;
	}
	Matrix4f OrthograohicMatrix(real32 aspect_ratio, real32 FOV, real32 z_near, real32 z_far)
	{
		real32 tan_half_fov = tanf(ToRadians(0.5f * FOV));
		real32 height = z_near * tan_half_fov;
		real32 width = height * aspect_ratio;
		real32 left = -width, right = width;
		real32 bottom = -height, top = height;
		real32 depth = z_far - z_near;

		Matrix4f Projection =
		{
			2.0f / width, 0.0f, 0.0f, -(right + left) / width,
			0.0f, 2.0f / height, 0.0f, -(top + bottom) / height,
			0.0f, 0.0f, -2.0f / depth, -(z_far + z_near) / depth,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		return Projection;
	}
	Matrix4f ScreenSpaceMatrix(uint32 width, uint32 height, real32 z_near, real32 z_far)
	{
		real32 half_width = (real32)width / 2.0f;
		real32 half_height = (real32)height / 2.0f;

		real32 half_zminus = (z_far - z_near) / 2.0f;
		real32 half_zplus = (z_far + z_near) / 2.0f;

		const Matrix4f mat =
		{
			-half_width, 0.0f, 0.0f, half_width,
			0.0f, -half_height, 0.0f, half_height,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		return mat;
	}

	string ToString(Matrix4f mat)
	{
		char str[256] = "";
		sprintf_s(str, 256, "\n"
			"[%+7.1f][%+7.1f][%+7.1f][%+7.1f]\n"
			"[%+7.1f][%+7.1f][%+7.1f][%+7.1f]\n"
			"[%+7.1f][%+7.1f][%+7.1f][%+7.1f]\n"
			"[%+7.1f][%+7.1f][%+7.1f][%+7.1f]\n\n",
			mat.a2[0][0], mat.a2[0][1], mat.a2[0][2], mat.a2[0][3],
			mat.a2[1][0], mat.a2[1][1], mat.a2[1][2], mat.a2[1][3],
			mat.a2[2][0], mat.a2[2][1], mat.a2[2][2], mat.a2[2][3],
			mat.a2[3][0], mat.a2[3][1], mat.a2[3][2], mat.a2[3][3]);
		return str;
	}
}