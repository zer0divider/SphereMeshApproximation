#include "zQuaternion.h"

using namespace zer0;

void Quaternion::setAxis(float theta, const Vector3D & axis)
{
	float len = axis.getLength();
	float theta_half = theta/2.0;
	w = cos(theta_half);
	float sin_t = sin(theta_half);
	x = axis.x*sin_t/len;
	y = axis.y*sin_t/len;
	z = axis.z*sin_t/len;
}

float Quaternion::getAxis(Vector3D & axis)const{
	float theta_half = acos(w);
	float sin_t = sin(theta_half);
	axis.x = x/sin_t;
	axis.y = y/sin_t;
	axis.z = z/sin_t;
	return 2*theta_half;
}

Quaternion Quaternion::interpolate(const Quaternion & a, const Quaternion & b, float t)
{
	Quaternion q_inter = (1-t)*a + t*b; 
	q_inter.normalize();
	return q_inter;
}

Quaternion Quaternion::sphericalInterpolate(const Quaternion & a, const Quaternion & b, float t)
{
	float q_dot = dot(a, b);
	float theta = acos(q_dot);
	float sin_theta = sqrt(1-q_dot*q_dot);
	Quaternion q_inter = (sin(theta*(1-t))/sin_theta)*a + (sin(theta*t)/sin_theta)*b;
	q_inter.normalize();
	return q_inter;
}
