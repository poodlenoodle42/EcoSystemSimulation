#include "vector2.h"
#include <iostream>
#include <random>
Vector2::Vector2(){
    x = 0;
    y = 0;
}

Vector2::Vector2(float x, float y)
{
    this->x = x;
    this->y = y;
}

Vector2::Vector2(std::tuple<float, float> vec){
    x = std::get<0>(vec);
    y = std::get<1>(vec);
}

void Vector2::printVector2()const{
    std::cout << "Vector2: x = " << x << " , y = " << y << std::endl;
}

const Vector2 Vector2::operator +(const Vector2 &f)const{
    return Vector2(f.x+this->x,f.y+this->y);
}

const Vector2 Vector2::operator -(const Vector2 &f)const{
    return Vector2(this->x-f.x,this->y-f.y);
}

const Vector2 Vector2::operator *(const Vector2 &f)const{
    return Vector2(f.x*this->x,f.y*this->y);
}
const Vector2 Vector2::operator *(const float &f)const{
    return Vector2(f*this->x,f*this->y);
}
const Vector2 Vector2::operator /(const Vector2 &f)const{
    return Vector2(this->x/f.x,this->y/f.y);
}

bool Vector2::operator == (const Vector2 &f)const{
    if(this->x == f.x && this->y == f.y){
        return true;
    }
    else{
        return false;
    }
}

bool Vector2::operator <(const Vector2 &f) const{
    return this->x<f.x && this->y<f.y;
}

bool Vector2::operator>(const Vector2 &f) const{
    return this->x>f.x && this->y>f.y;
}

const Vector2 Vector2::normalized()const{
    float lenght = sqrt(this->x*this->x + this->y*this->y);
    return Vector2(this->x/lenght,this->y/lenght);
}

void Vector2::normalize(){
    float lenght = sqrt(this->x*this->x + this->y*this->y);
    x = x/lenght;
    y =y/lenght;
}

Vector2 Vector2::makePositive(const Vector2 &in){
    Vector2 newVec;
    if(in.x<0){
        newVec.x = in.x * (-1);
    }
    else{
        newVec.x = in.x;
    }
    if(in.y<0){
        newVec.y = in.y * (-1);
    }
    else{
        newVec.y = in.y;
    }
    return newVec;
}

float Vector2::distance(Vector2 &from, Vector2 &to){
    Vector2 delta = to - from;
    delta = Vector2::makePositive(delta);
    float dis = sqrt((delta.x*delta.x)+(delta.y*delta.y));
    return dis;

}

Vector2 Vector2::randomVec(const Vector2 &min, const Vector2 &max){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> x(min.x,max.x);
    std::uniform_real_distribution<float> y(min.y,max.y);
    return Vector2(x(mt),y(mt));
}

bool Vector2::compareVecInt(const Vector2 &in1, const Vector2 &in2){
    return (static_cast<int>(in1.x) == static_cast<int>(in2.x)&&
            static_cast<int>(in1.y) == static_cast<int>(in2.y));

}
