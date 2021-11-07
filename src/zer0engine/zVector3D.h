#ifndef ZER0_VECTOR3D_H
#define ZER0_VECTOR3D_H

#include <cmath>

namespace zer0{
	/**
	 * 3D vector
	 */
	class Vector3D
	{
		public:
			//constructors
			Vector3D() : x(0.f), y(0.f), z(0.f){}
			Vector3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z){}
			Vector3D(const float * v) : x(v[0]), y(v[1]), z(v[2]){}
			Vector3D(const Vector3D & v) : x(v.x), y(v.y), z(v.z){}

			//destructor
			~Vector3D(){}

			//initializers
			void loadZero(){x = 0.f; y = 0.f; z = 0.f;}
			void loadOne(){x = 1.f; y = 1.f; z = 1.f;}
			void set(float new_x, float new_y, float new_z)
			{x = new_x; y = new_y; z = new_z;}

			// cross product
			static Vector3D cross(const Vector3D & a, const Vector3D & b){
				return Vector3D(
					a.y * b.z - a.z * b.y,
					a.z * b.x - a.x * b.z,
					a.x * b.y - a.y * b.x
				);
			}

			//calculations
			void normalize();
			Vector3D getNormalized() const;

			float getLength() const
			{return sqrt(x*x + y*y + z*z);}

			float squaredLength() const
			{return x*x + y*y + z*z;}
			
			//applying signumfunction to every coordinate
			Vector3D getSign()const;

			//overloaded operators
			Vector3D operator+(const Vector3D & v) const
			{return Vector3D(x + v.x, y + v.y, z + v.z);}

			Vector3D operator-(const Vector3D & v) const
			{return Vector3D(x - v.x, y - v.y, z - v.z);}

			Vector3D operator*(const float rhs) const
			{return Vector3D(x*rhs, y*rhs, z*rhs);	}

			Vector3D operator*(const Vector3D & v) const
			{Vector3D r(v.x*x, v.y*y, v.z*z); return r;}
			
			Vector3D operator/(const float rhs) const
			{return (rhs==0) ? Vector3D(0.0f, 0.0f, 0.f) : Vector3D(x / rhs, y / rhs, z / rhs); }

			//allow operations like: 3*v
			friend Vector3D operator*(float scale, const Vector3D & v)
			{return Vector3D(v.x * scale, v.y * scale, scale * v.z);}
			bool operator==(const Vector3D & v) const
			{return (x == v.x && y == v.y && z == v.z);}

			bool operator!=(const Vector3D & v) const
			{return (x != v.x || y != v.y || z != v.z);}

			void operator+=(const Vector3D & v)
			{x+=v.x; y+=v.y; z+=v.z;}

			void operator-=(const Vector3D & v)
			{x-=v.x; y-=v.y; z-=v.z;}

			void operator*=(const float s)
			{x*=s; y*=s; z*=s;}

			void operator/=(const float s)
			{x/=s; y/=s;}

			Vector3D operator-() const {return Vector3D(-x, -y, -z);}
			Vector3D operator+() const {return *this;}

			//cast to pointer
			operator float* () {return (float*) this;}
			operator const float* () const {return (const float*) this;}

			//members
			float x, y, z;
	};
	// vector containing all zeros
	extern const Vector3D Vector3D_zer0;
};
#endif
