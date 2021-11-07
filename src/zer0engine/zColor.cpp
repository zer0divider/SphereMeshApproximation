#include "zColor.h"

using namespace zer0;

const Color Color::RED(1,0,0,1);
const Color Color::GREEN(0,1,0,1);
const Color Color::BLUE(0,0,1,1);
const Color Color::CYAN(0,1,1,1);
const Color Color::MAGENTA(1,0,1,1);
const Color Color::YELLOW(1,1,0,1);
const Color Color::WHITE(1,1,1,1);
const Color Color::BLACK(0,0,0,1);
const Color Color::GREY(0.5,0.5,0.5,1);

void Color::set(unsigned int rgba, bool msb_red)
{
	if(msb_red)
	{
		//R
		r = ((rgba & 0xFF000000)>>24)/255.f;

		//G
		g = ((rgba & 0x00FF0000)>>16)/255.f;

		//B
		b = ((rgba & 0x0000FF00)>>8)/255.f;

		//A
		a = (rgba & 0x000000FF)/255.f;
	}
	else
	{
		//R
		r = (rgba & 0x000000FF)/255.f;

		//G
		g = ((rgba & 0x0000FF00)>>8)/255.f;

		//B
		b = ((rgba & 0x00FF0000)>>16)/255.f;

		//A
		a = ((rgba & 0xFF000000)>>24)/255.f;
	}
	
}

void Color::setFromByte(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
{
	r = (_r)/255.f;
	g = (_g)/255.f;
	b = (_b)/255.f;
	a = (_a)/255.f;
}

unsigned int Color::getHex(bool msb_red) const
{
	unsigned int rgba = 0x00000000;
	//calculate rounded integer-values
	if(msb_red)
	{
		rgba = (static_cast<unsigned int>(a*255.f + 0.5f)) | (static_cast<unsigned int>(b*255.f + 0.5f)<<8) |
				(static_cast<unsigned int>(g*255.f + 0.5f)<<16) | (static_cast<unsigned int>(r*255.f + 0.5f)<<24);
	}
	else
	{
		rgba = (static_cast<unsigned int>(r*255.f + 0.5f)) | (static_cast<unsigned int>(g*255.f + 0.5f)<<8) |
				(static_cast<unsigned int>(b*255.f + 0.5f)<<16) | (static_cast<unsigned int>(a*255.f + 0.5f)<<24);
	}
	
	return rgba;
}

void Color::darken(float d)
{
	r *= 1-d;
	g *= 1-d;
	b *= 1-d;
}

void Color::brighten(float _b)
{
	r += (1-r)*_b;
	g += (1-g)*_b;
	b += (1-b)*_b;
}

void Color::desaturate(float s)
{
	//calculate grey
	float grey = (r + g + b)/3.f;
	r += s*(grey-r);
	g += s*(grey-g);
	b += s*(grey-b);
}

void Color::saturate(float s)
{
	float l = static_cast<float>(sqrt(static_cast<double>(r*r+g*g+b*b)));
	float new_r = r/l; 
	float new_g = g/l; 
	float new_b = b/l; 
	r += s*(new_r-r);
	g += s*(new_g-r);
	b += s*(new_b-r);
}
