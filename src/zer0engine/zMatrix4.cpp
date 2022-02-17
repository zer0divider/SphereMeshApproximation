#include "zMatrix4.h"

using namespace zer0;

const Matrix4 Matrix4::IDENTITY(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);

Matrix4::Matrix4( float e0, float e1, float e2, float e3,
                        float e4, float e5, float e6, float e7,
                        float e8, float e9, float e10, float e11,
                        float e12, float e13, float e14, float e15)
{
	values[0] = e0;
	values[1] = e1;
	values[2] = e2;
	values[3] = e3;
	values[4] = e4;
	values[5] = e5;
	values[6] = e6;
	values[7] = e7;
	values[8] = e8;
	values[9] = e9;
	values[10] = e10;
	values[11] = e11;
	values[12] = e12;
	values[13] = e13;
	values[14] = e14;
	values[15] = e15;
}

Matrix4::Matrix4(const float * mat)
{
	memcpy(values, mat, sizeof(values));
}

Matrix4::Matrix4(const Matrix4 & mat)
{
    memcpy(values, mat.values, sizeof(values));
}

Vector4D Matrix4::getRow(int position) const
{
    return Vector4D(values[position], values[4 + position], values[8+position], values[12+position]);
}

Vector4D Matrix4::getColumn(int position) const
{
	return Vector4D(values[position*4], values[position*4+1], values[position*4+2], values[position*4+3]);
}

void Matrix4::loadIdentity()
{
	values[0]=1.0f; values[4]=0.0f; values[ 8]=0.0f; values[12]=0.0f;
	values[1]=0.0f; values[5]=1.0f; values[ 9]=0.0f; values[13]=0.0f;
	values[2]=0.0f; values[6]=0.0f; values[10]=1.0f; values[14]=0.0f;
	values[3]=0.0f; values[7]=0.0f; values[11]=0.0f; values[15]=1.0f;
}

void Matrix4::set2DTransform(const Vector2D & pos, float uniform_scale){
	set2DTransform(pos, Vector2D(uniform_scale, uniform_scale));
}

void Matrix4::set2DTransform(const Vector2D & pos, float uniform_scale, float rad){
	set2DTransform(pos, Vector2D(uniform_scale, uniform_scale), rad);
}

void Matrix4::set2DTransform(const Vector2D & pos, const Vector2D & scale, float rad)
{
	float cos_rad = cos(rad);
	float sin_rad = sin(rad);
	values[0] = scale.x*cos_rad;
	values[1] = -sin_rad*scale.y;
	values[2] = 0;
	values[3] = 0;

	values[4] = sin_rad*scale.x;
	values[5] = scale.y*cos_rad;
	values[6] = 0;
	values[7] = 0;

	values[8] = 0;
	values[9] = 0;
	values[10] = 1;
	values[11] = 0;

	values[12] = pos.x;
	values[13] = pos.y;
	values[14] = 0;
	values[15] = 1;
}

void Matrix4::set2DCameraTransform(const Vector2D & pos, float scale, float rad)
{
	float cos_rad = cos(rad);
	float sin_rad = sin(rad);
	Matrix4 transform;
	values[0] = scale*cos_rad;
	values[1] = -scale*sin_rad;
	values[2] = 0;
	values[3] = 0;

	values[4] = scale*sin_rad;
	values[5] = scale*cos_rad;
	values[6] = 0;
	values[7] = 0;

	values[8] = 0;
	values[9] = 0;
	values[10] = 1;
	values[11] = 0;

	values[12] = -(pos.x*cos_rad+pos.y*sin_rad)*scale;
	values[13] = -(pos.y*cos_rad-pos.x*sin_rad)*scale;
	values[14] = 0;
	values[15] = 1;
}

void Matrix4::setInverse2DCameraTransform(const Vector2D & pos, float scale, float rad)
{
	float cos_rad = cos(rad);
	float sin_rad = sin(rad);
	float fraction1 = cos_rad/scale;
	float fraction2 = sin_rad/scale;

	values[0] = fraction1;
	values[1] = fraction2;
	values[2] = 0;
	values[3] = 0;

	values[4] = fraction2;
	values[5] = fraction1;
	values[6] = 0;
	values[7] = 0;

	values[8] = 0;
	values[9] = 0;
	values[10] = 1;
	values[11] = 0;

	values[12] = pos.x;
	values[13] = pos.y;
	values[14] = 0;
	values[15] = 1;
}

void Matrix4::set2DTransform(const Vector2D & pos, const Vector2D & scale)
{
	Matrix4 transform;
	values[0] = scale.x;
	values[1] = 0;
	values[2] = 0;
	values[3] = 0;

	values[4] = 0;
	values[5] = scale.y;
	values[6] = 0;
	values[7] = 0;

	values[8] = 0;
	values[9] = 0;
	values[10] = 1;
	values[11] = 0;

	values[12] = pos.x;
	values[13] = pos.y;
	values[14] = 0;
	values[15] = 1;
}


Matrix4 Matrix4::operator+(const Matrix4 & m) const		//overloaded operators
{
	
	return Matrix4(	values[0]+m.values[0],
						values[1]+m.values[1],
						values[2]+m.values[2],
						values[3]+m.values[3],
						values[4]+m.values[4],
						values[5]+m.values[5],
						values[6]+m.values[6],
						values[7]+m.values[7],
						values[8]+m.values[8],
						values[9]+m.values[9],
						values[10]+m.values[10],
						values[11]+m.values[11],
						values[12]+m.values[12],
						values[13]+m.values[13],
						values[14]+m.values[14],
						values[15]+m.values[15]);
}

Matrix4 Matrix4::operator-(const Matrix4 & m) const		//overloaded operators
{
	return Matrix4(	values[0]-m.values[0],
						values[1]-m.values[1],
						values[2]-m.values[2],
						values[3]-m.values[3],
						values[4]-m.values[4],
						values[5]-m.values[5],
						values[6]-m.values[6],
						values[7]-m.values[7],
						values[8]-m.values[8],
						values[9]-m.values[9],
						values[10]-m.values[10],
						values[11]-m.values[11],
						values[12]-m.values[12],
						values[13]-m.values[13],
						values[14]-m.values[14],
						values[15]-m.values[15]);
}

Matrix4 Matrix4::operator*(const Matrix4 & m) const
{
    //WARNING: Multiplication can be optimized for common cases (e.g. bottom row = {0,0,0,1})
	return Matrix4(	values[0]*m.values[0]+values[4]*m.values[1]+values[8]*m.values[2]+values[12]*m.values[3],
						values[1]*m.values[0]+values[5]*m.values[1]+values[9]*m.values[2]+values[13]*m.values[3],
						values[2]*m.values[0]+values[6]*m.values[1]+values[10]*m.values[2]+values[14]*m.values[3],
						values[3]*m.values[0]+values[7]*m.values[1]+values[11]*m.values[2]+values[15]*m.values[3],
						values[0]*m.values[4]+values[4]*m.values[5]+values[8]*m.values[6]+values[12]*m.values[7],
						values[1]*m.values[4]+values[5]*m.values[5]+values[9]*m.values[6]+values[13]*m.values[7],
						values[2]*m.values[4]+values[6]*m.values[5]+values[10]*m.values[6]+values[14]*m.values[7],
						values[3]*m.values[4]+values[7]*m.values[5]+values[11]*m.values[6]+values[15]*m.values[7],
						values[0]*m.values[8]+values[4]*m.values[9]+values[8]*m.values[10]+values[12]*m.values[11],
						values[1]*m.values[8]+values[5]*m.values[9]+values[9]*m.values[10]+values[13]*m.values[11],
						values[2]*m.values[8]+values[6]*m.values[9]+values[10]*m.values[10]+values[14]*m.values[11],
						values[3]*m.values[8]+values[7]*m.values[9]+values[11]*m.values[10]+values[15]*m.values[11],
						values[0]*m.values[12]+values[4]*m.values[13]+values[8]*m.values[14]+values[12]*m.values[15],
						values[1]*m.values[12]+values[5]*m.values[13]+values[9]*m.values[14]+values[13]*m.values[15],
						values[2]*m.values[12]+values[6]*m.values[13]+values[10]*m.values[14]+values[14]*m.values[15],
						values[3]*m.values[12]+values[7]*m.values[13]+values[11]*m.values[14]+values[15]*m.values[15]);
}

void Matrix4::multLeft(const Matrix4 & m)
{
    (*this) = m * (*this);
}

void Matrix4::multRight(const Matrix4 & m)
{
    (*this) = (*this) * m;
}

Matrix4 Matrix4::operator*(const float s) const
{
	return Matrix4(	values[0]*s,
						values[1]*s,
						values[2]*s,
						values[3]*s,
						values[4]*s,
						values[5]*s,
						values[6]*s,
						values[7]*s,
						values[8]*s,
						values[9]*s,
						values[10]*s,
						values[11]*s,
						values[12]*s,
						values[13]*s,
						values[14]*s,
						values[15]*s);
}

Matrix4 Matrix4::operator/ (const float m) const
{
	if (m==0.0f || m==1.0f)
		return (*this);
		
	float temp=1/m;

	return (*this)*temp;
}

Matrix4 operator*(float scaleFactor, const Matrix4 & m)
{
	return m*scaleFactor;
}

bool Matrix4::operator==(const Matrix4 & m) const
{
	for(int i=0; i<16; i++)
	{
		if(values[i]!=m.values[i])
			return false;
	}
	return true;
}

bool Matrix4::operator!=(const Matrix4 & m) const
{
	return !((*this)==m);
}

void Matrix4::operator+=(const Matrix4 & m)
{
	(*this)=(*this)+m;
}

void Matrix4::operator-=(const Matrix4 & m)
{
	(*this)=(*this)-m;
}

void Matrix4::operator*=(const Matrix4 & m)
{
	(*this)=(*this)*m;
}

void Matrix4::operator*=(const float s)
{
	(*this)=(*this)*s;
}

void Matrix4::operator/=(const float s)
{
	(*this)=(*this)/s;
}

Matrix4 Matrix4::operator-() const
{
	Matrix4 result(*this);

	for(int i=0; i<16; i++)
		result.values[i]=-result.values[i];

	return result;
}

Vector4D Matrix4::operator*(const Vector4D & v) const
{
	if(values[3]==0.0f && values[7]==0.0f && values[11]==0.0f && values[15]==1.0f)
	{
		return Vector4D(values[0]*v.x
					+	values[4]*v.y
					+	values[8]*v.z
					+	values[12]*v.w,

						values[1]*v.x
					+	values[5]*v.y
					+	values[9]*v.z
					+	values[13]*v.w,

						values[2]*v.x
					+	values[6]*v.y
					+	values[10]*v.z
					+	values[14]*v.w,

						v.w);
	}
	
	return Vector4D(	values[0]*v.x
					+	values[4]*v.y
					+	values[8]*v.z
					+	values[12]*v.w,

						values[1]*v.x
					+	values[5]*v.y
					+	values[9]*v.z
					+	values[13]*v.w,

						values[2]*v.x
					+	values[6]*v.y
					+	values[10]*v.z
					+	values[14]*v.w,

						values[3]*v.x
					+	values[7]*v.y
					+	values[11]*v.z
					+	values[15]*v.w);
}

void Matrix4::transpose()
{
	(*this)=getTranspose();
}

Matrix4 Matrix4::getTranspose() const
{
	Matrix4 m;
	m.values[0] = values[0];
	m.values[1] = values[4];
	m.values[2] = values[8];
	m.values[3] = values[12];
	m.values[4] = values[1];
	m.values[5] = values[5];
	m.values[6] = values[9];
	m.values[7] = values[13];
	m.values[8] = values[2];
	m.values[9] = values[6];
	m.values[10] = values[10];
	m.values[11] = values[14];
	m.values[12] = values[3];
	m.values[13] = values[7];
	m.values[14] = values[11];
	m.values[15] = values[15];
	return m;
}

Vector4D Matrix4::getTransformedVector(const Vector4D & v) const
{
	return (*this)*v;
}

void Matrix4::transformVector(Vector4D & v) const
{
	v = (*this)*v;
}

Vector3D Matrix4::getTransformedVector(const Vector3D & v, float w) const
{
	if(w == 0){
		return Vector3D(values[0]*v.x + values[4]*v.y + values[8]*v.z,
						values[1]*v.x + values[5]*v.y + values[9]*v.z,
						values[2]*v.x + values[6]*v.y + values[10]*v.z);
	}
	else if(w == 1){
		return Vector3D(values[0]*v.x + values[4]*v.y + values[8]*v.z + values[12],
						values[1]*v.x + values[5]*v.y + values[9]*v.z + values[13],
						values[2]*v.x + values[6]*v.y + values[10]*v.z + values[14]);
	}
	else{
		return Vector3D(values[0]*v.x + values[4]*v.y + values[8]*v.z + w*values[12],
						values[1]*v.x + values[5]*v.y + values[9]*v.z + w*values[13],
						values[2]*v.x + values[6]*v.y + values[10]*v.z + w*values[14]);
	}
}

void Matrix4::transformVector(Vector3D & v, float w)const{
	v = getTransformedVector(v, w);
}

Vector2D Matrix4::getTransformedVector(const Vector2D & v, float z, float w) const
{
	if(w == 0){
		return Vector2D(values[0]*v.x + values[4]*v.y + values[8]*z,
						values[1]*v.x + values[5]*v.y + values[9]*z);
	}
	else if(w == 1){
		return Vector2D(values[0]*v.x + values[4]*v.y + values[8]*z + values[12],
						values[1]*v.x + values[5]*v.y + values[9]*z + values[13]);
	}
	else{
		return Vector2D(values[0]*v.x + values[4]*v.y + values[8]*z + w*values[12],
						values[1]*v.x + values[5]*v.y + values[9]*z + w*values[13]);
	}
}

void Matrix4::transformVector(Vector2D & v, float z, float w)const{
	v = getTransformedVector(v, z, w);
}

void Matrix4::setTranslation(const Vector3D & translation)
{
	loadIdentity();
	values[12] =translation.x;
	values[13] =translation.y;
	values[14] =translation.z;
}

void Matrix4::setScale(const Vector3D & scaleFactor)
{
	loadIdentity();

	values[0]=scaleFactor.x;
	values[5]=scaleFactor.y;
	values[10]=scaleFactor.z;
}

void Matrix4::setScale(float s)
{
	loadIdentity();

	values[0]=s;
	values[5]=s;
	values[10]=s;
}

void Matrix4::setRotationAxis(const float rad, const Vector3D & axis)
{
	Vector3D u=axis.getNormalized();

	float sinAngle=static_cast<float>(sin(rad));
	float cosAngle=static_cast<float>(cos(rad));
	float oneMinusCosAngle=1.0f-cosAngle;

	loadIdentity();

	values[0]=(u.x)*(u.x) + cosAngle*(1-(u.x)*(u.x));
	values[4]=(u.x)*(u.y)*(oneMinusCosAngle) - sinAngle*u.z;
	values[8]=(u.x)*(u.z)*(oneMinusCosAngle) + sinAngle*u.y;

	values[1]=(u.x)*(u.y)*(oneMinusCosAngle) + sinAngle*u.z;
	values[5]=(u.y)*(u.y) + cosAngle*(1-(u.y)*(u.y));
	values[9]=(u.y)*(u.z)*(oneMinusCosAngle) - sinAngle*u.x;
	
	values[2]=(u.x)*(u.z)*(oneMinusCosAngle) - sinAngle*u.y;
	values[6]=(u.y)*(u.z)*(oneMinusCosAngle) + sinAngle*u.x;
	values[10]=(u.z)*(u.z) + cosAngle*(1-(u.z)*(u.z));
}

void Matrix4::setRotationX(const float rad)
{
	loadIdentity();

	values[5]=(float)cos(rad);
	values[6]=(float)sin(rad);

	values[9]=-values[6];
	values[10]=values[5];
}

void Matrix4::setRotationY(const float rad)
{
	loadIdentity();

	values[0]=(float)cos(rad);
	values[2]=-(float)sin(rad);

	values[8]=-values[2];
	values[10]=values[0];
}

void Matrix4::setRotationZ(const float rad)
{
	loadIdentity();

	values[0]=(float)cos(rad);
	values[1]=(float)sin(rad);

	values[4]=-values[1];
	values[5]=values[0];
}

void Matrix4::setRotationXYZ(float x, float y, float z)
{
	setRotationXYZ(Vector3D(x, y, z));
}


void Matrix4::setRotationXYZ(const Vector3D & v)
{
	float s_x = sin(v.x);
	float s_y = sin(v.y);
	float s_z = sin(v.z);
	float c_x = cos(v.x);
	float c_y = cos(v.y);
	float c_z = cos(v.z);
	values[0] = c_y*c_z;
	values[1] = -c_y*s_z;
	values[2] = s_y;
	values[3] = 0;

	values[4] = c_x*s_z + s_x*s_y*c_z;
	values[5] = c_x*c_z - s_x*s_y*s_z;
	values[6] = -s_x*c_y;
	values[7] = 0;

	values[8] = s_x*s_z - c_x*s_y*c_z;
	values[9] = s_x*c_z + c_x*s_y*s_z;
	values[10] = c_x*c_y;
	values[11] = 0;

	values[12] = 0;
	values[13] = 0;
	values[14] = 0;
	values[15] = 1;
}

void Matrix4::setRotation(const Quaternion & q)
{
	float x_sq = 2*q.x*q.x;
	float y_sq = 2*q.y*q.y;
	float z_sq = 2*q.z*q.z;
	values[0] = 1-y_sq-z_sq;
	values[1] = 2*q.x*q.y + 2*q.w*q.z;
	values[2] = 2*q.x*q.z - 2*q.w*q.y;
	values[3] = 0;

	values[4] = 2*q.x*q.y - 2*q.w*q.z;
	values[5] = 1-x_sq-z_sq;
	values[6] = 2*q.y*q.z + 2*q.w*q.x;
	values[7] = 0;

	values[8] = 2*q.x*q.z + 2*q.w*q.y;
	values[9] = 2*q.y*q.z - 2*q.w*q.x;
	values[10] = 1- x_sq - y_sq;
	values[11] = 0;

	values[12] = 0;
	values[13] = 0;
	values[14] = 0;
	values[15] = 1;
}

void Matrix4::setTransform(const Vector3D & translation, const Vector3D & scale, const Vector3D & XYZeuler)
{
	setRotationXYZ(XYZeuler);
	values[0] *= scale.x;
	values[4] *= scale.y;
	values[8] *= scale.z;

	values[1] *= scale.x;
	values[5] *= scale.y;
	values[9] *= scale.z;

	values[2] *= scale.x;
	values[6] *= scale.y;
	values[10] *= scale.z;

	values[12] = translation.x;
	values[13] = translation.y;
	values[14] = translation.z;
}

void Matrix4::setTransform(const Vector3D & translation, float uniform_scale, const Vector3D & XYZeuler)
{
	setTransform(translation, Vector3D(uniform_scale, uniform_scale, uniform_scale), XYZeuler);
}

void Matrix4::setPerspective(float left, float right, float bottom,
								float top, float n, float f, Matrix4* inverse)
{
	values[0]  = (2*n)/(right-left);
    values[1]  = 0.f;
    values[2]  = 0.f;
    values[3]  = 0.f;
    values[4]  = 0.f;
	values[5]  = (2*n)/(top-bottom);
    values[6]  = 0.f;
    values[7]  = 0.f;
	values[8]  = (right+left)/(right-left);
	values[9]  = (top+bottom)/(top-bottom);
    values[10] =-(f+n)/(f-n);
	values[11] =-1.f;
    values[12] = 0.f;
    values[13] = 0.f;
    values[14] =-(2*f*n)/(f-n);
    values[15] = 0.f;
}

void Matrix4::setPerspectiveY(float fovy, float aspect, float n, float f, Matrix4* inverse)
{
	float deltaZ = n - f;
	float a= 1.f/tan((M_PI * fovy)/(360.f));
	
	values[0] = a/aspect;
	values[1] = 0.f;
	values[2] = 0.f;
	values[3] = 0.f;
	
	values[4] = 0.f;
	values[5] = a;
	values[6] = 0.f;
	values[7] = 0.f;
	
	values[8] = 0.f;	
	values[9] = 0.f;	
	values[10] = (n + f)/deltaZ;	
	values[11] = -1.f;	

	values[12]  = 0.f;
	values[13]  = 0.f;
	values[14]  = (2*n*f)/deltaZ;
	values[15]  = 0.f;
}

void Matrix4::setPerspectiveX(float fovx, float aspect, float n, float f, Matrix4* inverse)
{
    float deltaZ = n - f;
	float a =1.f/tan((M_PI * fovx)/(360.f));
	values[0] = a;
	values[1] = 0.f;
	values[2] = 0.f;
	values[3] = 0.f;

	values[4] = 0;
	values[5] = a/aspect;
	values[6] = 0.f;
	values[7] = 0.f;

	values[8] = 0.f;	
	values[9] = 0.f;	
	values[10] = (n + f)/deltaZ;	
	values[11] = -1.f;	

	values[12]  = 0.f;
	values[13]  = 0.f;
	values[14]  = (2*n*f)/deltaZ;
	values[15]  = 0.f;
}

void Matrix4::setOrtho(	float left, float right, float bottom,
							float top, float n, float f, Matrix4* inverse)
{
	values[0]  = 2.0f/(right-left);
    values[1]  = 0.f;
    values[2]  = 0.f;
    values[3]  = 0.f;
    values[4]  = 0.f;
	values[5]  = 2.0f/(top-bottom);
    values[6]  = 0.f;
	values[7]  = 0.f;
    values[8]  = 0.f;
    values[9]  = 0.f;
    values[10] =-2.f/(f-n);
    values[11] = 0.f;
	values[12] =-(right+left)/(right-left);
	values[13] =-(top+bottom)/(top-bottom);
	values[14] =-(f+n)/(f-n);
    values[15] = 1.f;
}
