#ifndef ZER0_VECTOR2D_H
#define ZER0_VECTOR2D_H

#include <cmath>

namespace zer0{
	/**
	 * 2D vector
	 */
	class Vector2D
	{
	public:

		//constructors
		Vector2D() : x(0.f), y(0.f){}
		Vector2D(float _x, float _y) : x(_x), y(_y){}
		Vector2D(const float * v) : x(v[0]), y(v[1]){}
		Vector2D(const Vector2D & v) : x(v.x), y(v.y){}

		//destructor
		~Vector2D(){}

		//initializers
		void loadZero(){x = 0.f; y = 0.f;}
		void loadOne(){x = 1.f; y = 1.f;}
		void set(float new_x, float new_y){x = new_x; y = new_y;}

		//calculations
		void normalize(){float len = getLength(); x/=len; y/=len;}
		Vector2D getNormalized() const
		{Vector2D new_v(*this); new_v.normalize(); return new_v;};

		float getLength() const
		{return sqrt(x*x + y*y);}

		float getSquaredLength() const
		{return x*x + y*y;}

		void rotate(float rad); //rotates the vector about a given angle (rad)
		Vector2D getRotated(float rad) const;
		float getRotation() const; //returns the angle of vector to x-axis (rad)

		static float dot(const Vector2D & a, const Vector2D & b){return a.x*b.x + a.y*b.y;}
		static float cross(const Vector2D & a, const Vector2D & b){return a.x * b.y - b.x * a.y;}
		/* angle(rad) between two vectors */
		static float angle(const Vector2D & a, const Vector2D & b){return acos(dot(a.getNormalized(), b.getNormalized()));}
		static float signedAngle(const Vector2D & a, const Vector2D & b){return cross(a,b) < 0.0f ? -angle(a,b) : angle(a,b);}

		//overloaded operators
		Vector2D operator+(const Vector2D & v) const
		{return Vector2D(x + v.x, y + v.y);	}

		Vector2D operator-(const Vector2D & v) const
		{return Vector2D(x - v.x, y - v.y);	}

		Vector2D operator*(const float s) const
		{return Vector2D(x*s, y*s);	}
		
		Vector2D operator*(const Vector2D & v) const
		{return Vector2D(x*v.x, y*v.y);}
		
		Vector2D operator/(const float s) const
		{return Vector2D(x / s, y / s); }

		//allow operations like: 3*v
		friend Vector2D operator*(float scale, const Vector2D & v)
		{return Vector2D(v.x * scale, v.y * scale);}

		bool operator==(const Vector2D & v) const
		{return (x == v.x && y == v.y);}

		bool operator!=(const Vector2D & v) const
		{return (x != v.x || y != v.y);}

		void operator+=(const Vector2D & v)
		{x+=v.x;y+=v.y;}

		void operator-=(const Vector2D & v)
		{x-=v.x;y-=v.y;}

		void operator*=(const float s)
		{x*=s;y*=s;	}

		void operator/=(const float s)
		{x/=s; y/=s;}

		Vector2D operator-() const {return Vector2D(-x, -y);}
		Vector2D operator+() const {return *this;}

		//cast to pointer
		operator float* () const {return (float*) this;}

		//members
		float x, y;
	};

	// vector with all zeros
	extern const Vector2D Vector2D_zer0;
};

#endif
