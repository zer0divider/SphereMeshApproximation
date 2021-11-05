#ifndef ZER0_QUATERNION_H
#define ZER0_QUATERNION_H

#include "zVector3D.h"
#include "zVector4D.h"

namespace zer0{

	/**
	 * Quaternion
	 */
	class Quaternion
	{
	public:
		/**
		 * Constructors
		 */
		Quaternion() : x(0.f), y(0.f), z(0.f), w(0.f){}
		Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w){}
		Quaternion(const float * v) : x(v[0]), y(v[1]), z(v[2]), w(v[3]){}
		Quaternion(const Vector4D & v) : x(v.x), y(v.y), z(v.z){}
		void set(float _x, float _y, float _z, float _w){x = _x; y = _y; z = _z; w = _w;}

		/**
		 * Conjugate
		 */
		void conjugate(){x = -x; y = -y; z = -z;}

		/**
		 * Squared Length
		 */
		float getSquaredLength()const{return x*x + y*y + z*z + w*w;}

		/**
		 * Length
		 */
		float getLength()const{return sqrt(getSquaredLength());}

		/**
		 * Normalize q
		 */
		void normalize(){float len = getLength(); x/=len; y/=len; z/=len; w/=len;}

		/**
		 * Inverting q, so it represents the negative rotation
		 */
		void invert(){conjugate(); float sq_len = getSquaredLength(); x /= sq_len; y /= sq_len; z /= sq_len; w /= sq_len;}

		/**
		 * Multiply two quaternions together (not commutative)
		 */
		friend Quaternion operator*(const Quaternion & a, const Quaternion & b){
			return Quaternion (	a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
								a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
								a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
								a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w);
		}

		/**
		 * Adding two quaternions together (component-wise)
		 */
		friend Quaternion operator+(const Quaternion & a, const Quaternion & b){
			return Quaternion(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
		}


		/**
		 * Set quaternion to represent a rotation of angle theta about given axis
		 */
		void setAxis(float theta, const Vector3D & axis);

		void setAxis(float theta, float x, float y, float z){
			setAxis(theta, Vector3D(x, y, z));
		}

		/**
		 * Scalar multiplication/dvision
		 */
		friend Quaternion operator*(const Quaternion & a, float s){
			Quaternion result(a.x*s, a.y*s, a.z*s, a.w*s);
			return result;
		}

		friend Quaternion operator*(float s, const Quaternion & a){
			Quaternion result(a.x*s, a.y*s, a.z*s, a.w*s);
			return result;
		}
		friend Quaternion operator/(const Quaternion & a, float s){
			Quaternion result(a.x/s, a.y/s, a.z/s, a.w/s);
			return result;
		}

		/**
		 * Dot product
		 */
		static float dot(const Quaternion & a, const Quaternion & b){
			return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
		}
		
		/**
		 * Get rotation around a single axis represented by this quaternion
		 * @return angle theta
		 */
		float getAxis(Vector3D & axis)const;

		
		/**
		 * Linear interpolation between two quaternions
		 * High performance, but no constant rate
		 */
		static Quaternion interpolate(const Quaternion & a, const Quaternion & b, float t);

		/**
		 * Spherical linear interpolation between two quaternions
		 * Slower, but guarantees constant rotation rate between a and b
		 */
		static Quaternion sphericalInterpolate(const Quaternion & a, const Quaternion & b, float t);

		float x, y, z, w;
	};
};

#endif
