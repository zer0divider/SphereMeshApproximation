#ifndef ZER0_MATRIX4_H
#define ZER0_MATRIX4_H

#include "zVector2D.h"
#include "zVector3D.h"
#include "zVector4D.h"
#include "zQuaternion.h"
#include <cmath>
#include <cstring> //needed for memset

namespace zer0{
	class Matrix4
	{
	public:
		//constructors
		Matrix4(){}
		Matrix4 (float e0, float e1, float e2, float e3,
					float e4, float e5, float e6, float e7,
					float e8, float e9, float e10, float e11,
					float e12, float e13, float e14, float e15);

		Matrix4(const float * mat);
		Matrix4(const Matrix4 & mat);

		//destructor
		~Matrix4(){}

		//access data
		Vector4D getRow(int row) const;
		Vector4D getColumn(int col) const;

		//load identity matrix
		void loadIdentity();

		//operators
		Matrix4 operator+(const Matrix4 & m) const;
		Matrix4 operator-(const Matrix4 & m) const;
		Matrix4 operator*(const Matrix4 & m) const;
		Matrix4 operator*(const float m) const;
		Matrix4 operator/(const float m) const;
		void multLeft(const Matrix4 & m);//multiply a matrix from left: this=m*this
		void multRight(const Matrix4 & m);//multiply a matrix from right: this=this*m

		bool operator==(const Matrix4 & m) const;
		bool operator!=(const Matrix4 & m) const;
		void operator+=(const Matrix4 & m);
		void operator-=(const Matrix4 & m);
		void operator*=(const Matrix4 & m);
		void operator*=(const float m);
		void operator/=(const float m);

		Matrix4 operator-() const;
		Matrix4 operator+() const {return (*this);}
		
		operator float* () {return (float*)this;}
		operator const float*() const {return (const float*)this;}
		
		//transform a 4D vector
		Vector4D operator* (const Vector4D & v) const;

		//transform a vector
		Vector4D getTransformedVector(const Vector4D & v) const;
		void transformVector(Vector4D & v) const;

		Vector3D getTransformedVector(const Vector3D & v, float w) const;
		void transformVector(Vector3D & v, float w) const;

		Vector2D getTransformedVector(const Vector2D & v, float z, float w) const;
		void transformVector(Vector2D & v, float z, float w) const;

		//transpose matrix
		void transpose();
		Matrix4 getTranspose() const;

		void set2DTransform(const Vector2D & pos, const Vector2D & scale);
		void set2DTransform(const Vector2D & pos, float uniform_scale);
		void set2DTransform(const Vector2D & pos, float uniform_scale, float rad);
		void set2DTransform(const Vector2D & pos, const Vector2D & scale, float rad);
		void set2DCameraTransform(const Vector2D & pos, float scale, float rad);
		void setInverse2DCameraTransform(const Vector2D & pos, float scale, float rad);
		
		void setTransform(const Vector3D & translation, const Vector3D & scale, const Vector3D & XYZeuler);
		void setTransform(const Vector3D & translation, const Vector3D & scale, const Quaternion & orientation);
		void setTransform(const Vector3D & translation, float uniform_scale, const Vector3D & XYZeuler);
		void setTransform(const Vector3D & translation, float uniform_scale, const Quaternion & orientation);
		void setTranslation(const Vector3D & translation);
		void setScale(const Vector3D & scaleFactor);
		void setScale(const float scaleFactor);//uniform scaling
		void setRotationXYZ(float x, float y, float z);
		void setRotationXYZ(const Vector3D & xyz);
		void setRotationAxis(const float rad, const Vector3D & axis);
		void setRotation(const Quaternion & orientation);
		void setRotationX(const float rad);
		void setRotationY(const float rad);
		void setRotationZ(const float rad);
		void setPerspective(float left, float right, float bottom, float top, float n, float f, Matrix4 * inverse = NULL);
		void setPerspectiveY(float fovy, float aspect, float n, float f, Matrix4 * inverse = NULL);
		void setPerspectiveX(float fovx, float aspect, float n, float f, Matrix4 * inverse = NULL);
		void setOrtho(float left, float right, float bottom, float top, float n, float f, Matrix4 * inverse = NULL);

		//members
		float values[16];
		static const Matrix4 IDENTITY;
	};
}

#endif
