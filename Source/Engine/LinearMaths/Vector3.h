//
//  Vector3.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Utilities.h"

#ifndef Vector3_h
#define Vector3_h

class Matrix4;

class Vector3 {
    public:
    float x, y, z, w;
    Vector3();
    Vector3(Vector3 const &vec);
    Vector3(btVector3 const &vec);
    Vector3(float x, float y, float z);
    Vector3(float x, float y, float z, float w);
    std::string getString();
    btVector3 getVector();
    float getLength();
    Vector3 getNormalized();
    Vector3& normalize();
    Vector3 operator+(const Vector3&);
    Vector3 operator-(const Vector3&);
    float operator*(const Vector3&); //Point product
    float operator%(const Vector3&); //Angle between
    Vector3 operator/(const Vector3&); //Cross product
    Vector3 operator*(const float&);
    Vector3 operator/(const float&);
    Vector3 operator*(const Matrix4&); //Vec * Mat
    bool operator<(const Vector3&);
    bool operator>(const Vector3&);
    bool operator<=(const Vector3&);
    bool operator>=(const Vector3&);
    bool operator==(const Vector3&);
    bool operator!=(const Vector3&);
    Vector3& operator=(const Vector3&);
    Vector3& operator+=(const Vector3&);
    Vector3& operator-=(const Vector3&);
    Vector3& operator/=(const Vector3&); //Cross product
    Vector3& operator*=(const float&);
    Vector3& operator/=(const float&);
    Vector3& operator*=(const Matrix4&); //Vec *= Mat
};

inline Vector3 vec3rand(Vector3 min, Vector3 max) {
    return Vector3(frand(min.x, max.x), frand(min.y, max.y), frand(min.z, max.z));
}

#endif
