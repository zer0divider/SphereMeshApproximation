#include "zVector2D.h"
using namespace zer0;

const Vector2D zer0::Vector2D_zer0(0,0);

void Vector2D::rotate(float rad)
{
	float sin_rad = sin(rad);
	float cos_rad = cos(rad);
	set(x*cos_rad - y*sin_rad, x*sin_rad + y*cos_rad);
}

Vector2D Vector2D::getRotated(float rad) const
{
	float sin_rad = sin(rad);
	float cos_rad = cos(rad);
	return Vector2D(x*cos_rad - y*sin_rad, x*sin_rad + y*cos_rad);
}

float Vector2D::getRotation() const
{
	if(x == 0.f){
		if(y < 0.0f)
		{
			return M_PI*1.5;
		}
		else{
			return M_PI*0.5;
		}
	}
	else if(y == 0.f){
		if(x < 0.0f)
		{
			return M_PI;
		}
		else{
			return 0.0;
		}
	}
	else{
		if(x > 0 && y > 0)
		{
			return atan(y/x);
		}
		else if(x < 0 && y > 0){
			return -atan(x/y) + M_PI*0.5;
		}
		else if(x < 0 && y < 0){
			return atan(y/x) + M_PI;
		}
		else if(x > 0 && y < 0){
			return -atan(x/y) + M_PI*1.5;
		}
	}
	return 0.f;
}
