#ifndef F_MATH_H
#define F_MATH_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h> // for rand()

namespace zer0{

    //returning maximum/minimum of two given values
    int max(int a, int b);
    int min(int a, int b);
    float max(float a, float b);
    float min(float a, float b);

    //limiting given value to upper and lower bound
    void limit(int & a, int max, int min);
    void limit(float & a, float max, float min);

    //round to next integer
    float round(float a);

    //signum: returns 1 if a is positive, -1 if a is negative or else 0
    float sign(float a);
    int sign(int a);

    //converting radiants to degrees and vice versa
    float rad(float deg);
    float deg(float rad);

    // return true if abs(a-b) <= epsilon
    bool equals(float a, float b, float epsilon = 0.000001);

    // interpolating quadraticly from start to end 
    float quadricInterpolate(float start, float end, float t);

    // interpolating linearly from start to end
    float linearInterpolate(float start, float end, float t);

    // random value between 0 and 1
    double random();

    // returns a random value in [min, max]
    int randomRangei(int min, int max);
    float randomRange(float min, float max);

    // selects a random index in [0, num_buckets[ according to frequency
    // if sum != NULL total the value of @sum will be used as sum of all frequencies, otherwise it is calculated in this function
    int randomBuckets(const float * bucket_freq, int num_buckets, const float *sum);

    // selection sort on given array
    void selectionSort(float * array, int size);
    void selectionSort(int * array, int size);
}

#endif
