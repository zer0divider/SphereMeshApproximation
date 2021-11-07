#ifndef Z_RECT_H
#define Z_RECT_H

#include "zVector2D.h"
#include "zMath.h"

namespace zer0
{
class Rect
{
public:
	Rect(float r_x, float r_y, float r_w, float r_h): x(r_x), y(r_y), w(r_w), h(r_h){}
	Rect(const Vector2D & pos, const Vector2D & dim): x(pos.x), y(pos.y), w(dim.x), h(dim.y){}
	Rect(){}
	void set(float r_x, float r_y, float r_w, float r_h){x = r_x; y = r_y; w = r_w; h = r_h;}
	void set(const Vector2D & pos, const Vector2D & dim){x=pos.x; y=pos.y; w=dim.x; h=dim.y;}
	void setPos(const Vector2D & p){x = p.x; y = p.y;}
	void setDim(const Vector2D & d){w = d.x; h = d.y;}
	void toFulldim(){x -= w; y -= h; w*=2; h*=2;}
	void toHalfdim(){w/=2; h/=2;x += w; y += h; }//convert from upper left point center to middle center with half dimensions

	// check whether a given point lies within the rect
	// rects are considered to be half rects
	bool checkPoint(const Vector2D & point)const;
	static bool intersect(const Rect & a, const Rect & b, Rect * intersection_rect = NULL);

	// returns true if given rect lies inside this rect fully
	bool contains(const Rect & r, float epsilon = 0.f)const;

	float x;
	float y;
	float w;//mostly used as half-dimensions but needs to be specified by application
	float h;
};
}

#endif
