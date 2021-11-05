#include "zVector3D.h"
using namespace zer0;

const Vector3D zer0::Vector3D_zer0(0, 0, 0);

void Vector3D::normalize()
{
	*this = getNormalized();
}

Vector3D Vector3D::getNormalized() const
{
	return *this/getLength();
}

Vector3D Vector3D::getSign() const 
{
	Vector3D s;
	if(x > 0.f)
		s.x = 1.f;
	else if(x < 0.f)
		s.x = -1.f;
	else
		s.x = 0.f;

	if(y > 0.f)
		s.y = 1.f;
	else if(y < 0.f)
		s.y = -1.f;
	else
		s.y = 0.f;

	if(z > 0.f)
		s.z = 1.f;
	else if(z < 0.f)
		s.z = -1.f;
	else
		s.z = 0.f;

	return s;
}
