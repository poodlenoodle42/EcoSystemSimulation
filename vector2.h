#ifndef VECTOR2_H
#define VECTOR2_H
#include <tuple>
#include <vector>
#include <math.h>
class Vector2
{
public:
    Vector2();
    Vector2(float x, float y);
    Vector2(std::tuple<float,float> vec);
    const Vector2 operator + (const Vector2 &f)const;
    const Vector2 operator - (const Vector2 &f)const;
    const Vector2 operator * (const Vector2 &f)const;
    const Vector2 operator * (const float &f)const;
    const Vector2 operator / (const Vector2 &f)const;
    bool operator == (const Vector2 &f)const;
    bool operator < (const Vector2 &f)const;
    bool operator > (const Vector2 &f)const;
    void printVector2()const;
    const Vector2 normalized()const;
    void normalize();
    static float distance(Vector2 &from,Vector2 &to);
    static Vector2 makePositive(const Vector2 &in);
    static Vector2 randomVec(const Vector2 &min,const Vector2 &max);
    static bool compareVecInt(const Vector2 &in1, const Vector2 & in2);
    float x;
    float y;
};

#endif // VECTOR2_H
