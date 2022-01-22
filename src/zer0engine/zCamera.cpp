/* Author: Cornelius Marx
 */
#include "zCamera.h"

using namespace zer0;

void Camera::upload()
{
	Matrix4 m, rx, ry;
	m.setTranslation(-_pos);
	rx.setRotationX(_rotation.x);
	ry.setRotationY(_rotation.y);
	SHADER->setViewMatrix(m*rx*ry);
}

void Camera::mouseRotate(int rel_x, int rel_y)
{
	_rotation += Vector2D(rel_y, rel_x)*_rotFactor;
}

void Camera::mouseTranslate(int rel_x, int rel_y)
{
	_pos.x -= rel_x*_translateFactor*_pos.z;
	_pos.y += rel_y*_translateFactor*_pos.z;
}

void Camera::mouseZoom(int wheel)
{
	if(wheel > 0){
		_pos.z /= _zoomFactor;
	}
	else if(wheel < 0){
		_pos.z *= _zoomFactor;
	}
	if(_pos.z < 1.0){
		_pos.z = 1.0;
	}
	else if(_pos.z > 1000.0){
		_pos.z = 1000.0;
	}
}

