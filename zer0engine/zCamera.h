/* Author: Cornelius Marx
 */
#ifndef ZER0_CAMERA_H
#define ZER0_CAMERA_H

#include "zVector2D.h"
#include "zShader.h"

namespace zer0{

// standard 3D camera
class Camera
{
public:
	Camera(): Camera(0.1, 1.5){};
	Camera(float rotate_factor, float zoom_factor):
		_rotFactor(rotate_factor), _zoomFactor(zoom_factor), _rotation(0,0), _pos(0,0,2){}
	
	void mouseRotate(int rel_x, int rel_y);

	void mouseZoom(int wheel);
	
	/* calculate matrix and upload to view matrix in shader */
	void upload();
private:
	float _rotFactor, _zoomFactor;
	Vector2D _rotation;	
	Vector3D _pos;
};

}

#endif
