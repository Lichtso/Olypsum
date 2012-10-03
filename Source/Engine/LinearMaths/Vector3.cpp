//
//  Vector3.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Vector3.h"
#import "Matrix4.h"

Vector3::Vector3() {
    
}

Vector3::Vector3(Vector3 const &vec) {
    x = vec.x;
    y = vec.y;
    z = vec.z;
    w = vec.w;
}

Vector3::Vector3(btVector3 const &vec) {
    x = vec.getX();
    y = vec.getY();
    z = vec.getZ();
    w = 1.0;
}

Vector3::Vector3(float xB, float yB, float zB) {
    x = xB;
    y = yB;
    z = zB;
    w = 1.0;
}

Vector3::Vector3(float xB, float yB, float zB, float wB) {
    x = xB;
    y = yB;
    z = zB;
    w = wB;
}

std::string Vector3::getString() {
    char buffer[64];
    sprintf(buffer, "(%f, %f, %f)", x, y, z);
    return std::string(buffer);
}

btVector3 Vector3::getBTVector() {
	return btVector3(btScalar(x), btScalar(y), btScalar(z));
}

btQuaternion Vector3::getBTQuaternion(float angle) {
	return btQuaternion(btVector3(btScalar(x), btScalar(y), btScalar(z)), btScalar(angle));
}

float Vector3::getLength() {
	return sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::getNormalized() {
    return Vector3(*this)/getLength();
}

Vector3& Vector3::normalize() {
    float factor = getLength();
    if(factor == 0.0) return *this;
    factor = 1.0/factor;
	x *= factor;
	y *= factor;
	z *= factor;
	return *this;
}

Vector3 Vector3::operator+(const Vector3& b) {
    Vector3 a;
	a.x = x+b.x;
	a.y = y+b.y;
	a.z = z+b.z;
	return a;
}

Vector3 Vector3::operator-(const Vector3& b) {
    Vector3 a;
	a.x = x-b.x;
	a.y = y-b.y;
	a.z = z-b.z;
	return a;
}

float Vector3::operator*(const Vector3& b) {
	return x * b.x + y * b.y + z * b.z;
}

float Vector3::operator%(const Vector3& b) {
    return acos(*this * b);
}

Vector3 Vector3::operator/(const Vector3& b) {
    Vector3 a;
	a.x = y * b.z - z * b.y;
	a.y = z * b.x - x * b.z;
	a.z = x * b.y - y * b.x;
	return a;
}

Vector3 Vector3::operator*(const float& factor) {
    Vector3 a;
	a.x = x * factor;
	a.y = y * factor;
	a.z = z * factor;
	return a;
}

Vector3 Vector3::operator/(const float& b) {
    if(b == 0.0) return Vector3(0, 0, 0);
    float factor = 1.0/b;
    Vector3 a;
	a.x = x * factor;
	a.y = y * factor;
	a.z = z * factor;
	return a;
}

Vector3 Vector3::operator*(const Matrix4& b) {
    Vector3 a;
	a.x = b.x.x * x + b.y.x * y + b.z.x * z + b.pos.x;
	a.y = b.x.y * x + b.y.y * y + b.z.y * z + b.pos.y;
	a.z = b.x.z * x + b.y.z * y + b.z.z * z + b.pos.z;
    a.w = b.x.w * x + b.y.w * y + b.z.w * z + b.pos.w;
	return a;
}

bool Vector3::operator<(const Vector3& b) {
    return (x < b.x && y < b.y && z < b.z);
}

bool Vector3::operator>(const Vector3& b) {
    return (x > b.x && y > b.y && z > b.z);
}

bool Vector3::operator<=(const Vector3& b) {
    return (x <= b.x && y <= b.y && z <= b.z);
}

bool Vector3::operator>=(const Vector3& b) {
    return (x >= b.x && y >= b.y && z >= b.z);
}

bool Vector3::operator==(const Vector3& b) {
    return (x == b.x && y == b.y && z == b.z);
}

bool Vector3::operator!=(const Vector3& b) {
    return (x != b.x || y != b.y || z != b.z);
}

Vector3& Vector3::operator=(const Vector3& b) {
    x = b.x;
	y = b.y;
	z = b.z;
    w = b.w;
	return *this;
}

Vector3& Vector3::operator+=(const Vector3& b) {
    x += b.x;
	y += b.y;
	z += b.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& b) {
    x -= b.x;
	y -= b.y;
	z -= b.z;
	return *this;
}

Vector3& Vector3::operator/=(const Vector3& b) {
    x = y*b.z - z*b.y;
	y = z*b.x - x*b.z;
	z = x*b.y - y*b.x;
	return *this;
}

Vector3& Vector3::operator*=(const float& factor) {
    x *= factor;
    y *= factor;
    z *= factor;
    return *this;
}

Vector3& Vector3::operator/=(const float& b) {
    if(b == 0.0) return *this;
    float factor = 1.0/b;
    x *= factor;
    y *= factor;
    z *= factor;
    return *this;
}

Vector3& Vector3::operator*=(const Matrix4& b) {
	x = b.x.x * x + b.y.x * y + b.z.x * z + b.pos.x;
	y = b.x.y * x + b.y.y * y + b.z.y * z + b.pos.y;
	z = b.x.z * x + b.y.z * y + b.z.z * z + b.pos.z;
    w = b.x.w * x + b.y.w * y + b.z.w * z + b.pos.w;
	return *this;
}
