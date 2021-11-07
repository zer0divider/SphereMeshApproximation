#include "zMath.h"

bool zer0::equals(float a, float b, float epsilon)
{
	return (fabs(a-b) <= epsilon);
}

int zer0::max(int a, int b)
{
	if(a > b)
		return a;

	return b;
}

int zer0::min(int a, int b)
{
	if(a < b)
		return a;

	return b;
}

float zer0::max(float a, float b)
{
	if(a > b)
		return a;

	return b;
}

float zer0::min(float a, float b)
{
	if(a < b)
		return a;

	return b;
}


void zer0::limit(int & a, int max, int min)
{
	if(a < min)
		a = min;

	if(a > max)
		a = max;
}

void zer0::limit(float & a, float max, float min)
{
	if(a < min)
		a = min;

	if(a > max)
		a = max;
}

float zer0::round(float a)
{
	if(a >= 0)
		return (float)((int)(a+0.5f));
	
	return (float)((int)(a-0.5f));
}

float zer0::sign(float a)
{
	if(a > 0.f)
		return 1.f;

	if(a < 0.f)
		return -1.f;

	return 0.f;
}

int zer0::sign(int a)
{
	if(a > 0)
		return 1;

	if(a < 0)
		return -1;

	return 0;
}

float zer0::rad(float deg)
{
	return static_cast<float>(M_PI)*(deg/180.f);
}

float zer0::deg(float rad)
{
	return 180.f*(rad/static_cast<float>(M_PI));
}

float zer0::quadricInterpolate(float start, float end, float t)
{
	if(t <= 0.5f){
		t = 2*t*t;
	}
	else{
		t = t-1;
		t = 1.0f - 2*t*t;
	}
	return start + t*(end-start);
}

float zer0::linearInterpolate(float start, float end, float t)
{
	return start + t*(end-start);
}

double zer0::random()
{
	return rand()/static_cast<double>(RAND_MAX);
}

int zer0::randomRangei(int min, int max)
{
	return rand()%(max-min + 1) + min;
}

float zer0::randomRange(float min, float max)
{
	return random()*(max-min) + min;
}

int zer0::randomBuckets(const float * bucket_freq, int num_buckets, const float *sum)
{
	float s = 0.f;
	int i;
	if(sum == NULL){// sum not given -> calculate
		for(i = 0; i < num_buckets; i++){
			s += bucket_freq[i];
		}
	}
	else{ // sum given use given value
		s = *sum;
	}
	float r = random()*s;
	float offset = 0.f;

	for(i = 0; i < num_buckets-1; i++){
		if(offset + bucket_freq[i] > r){
			return i;
		}
		offset += bucket_freq[i];
	}
	return i;
}

void zer0::selectionSort(float * array, int size)
{
	for(int i = 0; i < size-1; i++){
		int min = i;
		for(int j = i+1; j < size; j++){
			if(array[j] < array[min])
				min = j;
		}

		float t = array[i];
		array[i]= array[min];
		array[min] = t;
	}
}

void zer0::selectionSort(int * array, int size)
{
	for(int i = 0; i < size-1; i++){
		int min = i;
		for(int j = i+1; j < size; j++){
			if(array[j] < array[min])
				min = j;
		}

		float t = array[i];
		array[i]= array[min];
		array[min] = t;
	}
}
