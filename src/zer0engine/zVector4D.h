#ifndef ZER0_VECTOR4D_H
#define ZER0_VECTOR4D_H

#include "zVector3D.h"

namespace zer0{
	/**
	 * 4D vector
	 */
	class Vector4D
	{
		public:
			//constructors
			Vector4D() : x(0.f), y(0.f), z(0.f), w(0.f){}
			Vector4D(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w){}
			Vector4D(const float * v) : x(v[0]), y(v[1]), z(v[2]), w(v[3]){}
			Vector4D(const Vector4D & v) : x(v.x), y(v.y), z(v.z){}
			Vector4D(const Vector3D & v3, float _w): x(v3.x), y(v3.y), z(v3.z), w(_w){}

			void set(float _x, float _y, float _z, float _w){x = _x; y = _y; z = _z; w = _w;}
			void set(const Vector3D & v3, float _w){x = v3.x; y = v3.y; z = v3.z; w = _w;}

			Vector3D getVector3D()const{return Vector3D(x, y, z);}

		float x, y, z, w;
	};
};
#endif
