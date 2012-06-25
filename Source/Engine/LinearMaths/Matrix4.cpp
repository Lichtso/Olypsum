//
//  Matrix4.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Matrix4.h"

Matrix4::Matrix4() {
    
}

Matrix4::Matrix4(Matrix4 const &mat) {
    x = mat.x;
    y = mat.y;
    z = mat.z;
    pos = mat.pos;
}

Matrix4::Matrix4(btTransform const &mat) {
    btScalar matData[16];
    mat.getOpenGLMatrix(matData);
    x = Vector3(matData[0], matData[1], matData[2], matData[3]);
    y = Vector3(matData[4], matData[5], matData[6], matData[7]);
    z = Vector3(matData[8], matData[9], matData[10], matData[11]);
    pos = Vector3(matData[12], matData[13], matData[14], matData[15]);
}

Matrix4::Matrix4(float matData[16]) {
    x = Vector3(matData[0], matData[1], matData[2], matData[3]);
    y = Vector3(matData[4], matData[5], matData[6], matData[7]);
    z = Vector3(matData[8], matData[9], matData[10], matData[11]);
    pos = Vector3(matData[12], matData[13], matData[14], matData[15]);
}

void Matrix4::print() {
    printf("Matrix4:\n");
    printf("x: %f, %f, %f, %f\n", x.x, x.y, x.z, x.w);
    printf("y: %f, %f, %f, %f\n", y.x, y.y, y.z, y.w);
    printf("z: %f, %f, %f, %f\n", z.x, z.y, z.z, z.w);
    printf("pos: %f, %f, %f, %f\n", pos.x, pos.y, pos.z, pos.w);
}

btTransform Matrix4::getMatrix() {
    float matDataA[16];
    getOpenGLMatrix4(matDataA);
    btTransform mat;
    btScalar matDataB[16];
    for(unsigned char i = 0; i < 16; i ++)
        matDataB[i] = (btScalar)matDataA[i];
    mat.setFromOpenGLMatrix(matDataB);
    return mat;
}

void Matrix4::getOpenGLMatrix3(float matData[9]) {
    matData[0] = x.x;
    matData[1] = y.x;
    matData[2] = z.x;
    matData[3] = x.y;
    matData[4] = y.y;
    matData[5] = z.y;
    matData[6] = x.z;
    matData[7] = y.z;
    matData[8] = z.z;
}

void Matrix4::getOpenGLMatrix4(float matData[16]) {
    matData[0] = x.x;
    matData[1] = y.x;
    matData[2] = z.x;
    matData[3] = pos.x;
    matData[4] = x.y;
    matData[5] = y.y;
    matData[6] = z.y;
    matData[7] = pos.y;
    matData[8] = x.z;
    matData[9] = y.z;
    matData[10] = z.z;
    matData[11] = pos.z;
    matData[12] = x.w;
    matData[13] = y.w;
    matData[14] = z.w;
    matData[15] = pos.w;
}

void Matrix4::setMatrix3(const Matrix4& mat) {
    x = Vector3(mat.x.x, mat.x.y, mat.x.z, x.w);
    y = Vector3(mat.y.x, mat.y.y, mat.y.z, y.w);
    z = Vector3(mat.z.x, mat.z.y, mat.z.z, z.w);
}

void Matrix4::setIdentity() {
    x = Vector3(1, 0, 0, 0);
    y = Vector3(0, 1, 0, 0);
    z = Vector3(0, 0, 1, 0);
    pos = Vector3(0, 0, 0, 1);
}

void Matrix4::setZero() {
    x = Vector3(0, 0, 0, 0);
    y = Vector3(0, 0, 0, 0);
    z = Vector3(0, 0, 0, 0);
    pos = Vector3(0, 0, 0, 0);
}

Matrix4 Matrix4::getTransposed() {
    Matrix4 b;
    b.x.x = x.x;
    b.x.y = y.x;
    b.x.z = z.x;
    b.x.w = pos.x;
    b.y.x = x.y;
    b.y.y = y.y;
    b.y.z = z.y;
    b.y.w = pos.y;
    b.z.x = x.z;
    b.z.y = y.z;
    b.z.z = z.z;
    b.z.w = pos.z;
    b.pos.x = x.w;
    b.pos.y = y.w;
    b.pos.z = z.w;
    b.pos.w = pos.w;
    return b;
}

Matrix4 Matrix4::getInverse() {
    Matrix4 a = getTransposed();
    a.x.w = a.y.w = a.z.w = 0;
    a.pos = Vector3(0, 0, 0);
    a.pos = (pos*-1.0)*a;
    return a;
}

Matrix4& Matrix4::normalize() {
    x.normalize();
    y.normalize();
    z.normalize();
    return *this;
}

Matrix4 Matrix4::operator*(const Matrix4& b) {
    Matrix4 a;
    a.x = Vector3(x.x*b.x.x+x.y*b.y.x+x.z*b.z.x+x.w*b.pos.x,
                  x.x*b.x.y+x.y*b.y.y+x.z*b.z.y+x.w*b.pos.y,
                  x.x*b.x.z+x.y*b.y.z+x.z*b.z.z+x.w*b.pos.z,
                  x.x*b.x.w+x.y*b.y.w+x.z*b.z.w+x.w*b.pos.w);
    a.y = Vector3(y.x*b.x.x+y.y*b.y.x+y.z*b.z.x+y.w*b.pos.x,
                  y.x*b.x.y+y.y*b.y.y+y.z*b.z.y+y.w*b.pos.y,
                  y.x*b.x.z+y.y*b.y.z+y.z*b.z.z+y.w*b.pos.z,
                  y.x*b.x.w+y.y*b.y.w+y.z*b.z.w+y.w*b.pos.w);
    a.z = Vector3(z.x*b.x.x+z.y*b.y.x+z.z*b.z.x+z.w*b.pos.x,
                  z.x*b.x.y+z.y*b.y.y+z.z*b.z.y+z.w*b.pos.y,
                  z.x*b.x.z+z.y*b.y.z+z.z*b.z.z+z.w*b.pos.z,
                  z.x*b.x.w+z.y*b.y.w+z.z*b.z.w+z.w*b.pos.w);
    a.pos = Vector3(pos.x*b.x.x+pos.y*b.y.x+pos.z*b.z.x+pos.w*b.pos.x,
                    pos.x*b.x.y+pos.y*b.y.y+pos.z*b.z.y+pos.w*b.pos.y,
                    pos.x*b.x.z+pos.y*b.y.z+pos.z*b.z.z+pos.w*b.pos.z,
                    pos.x*b.x.w+pos.y*b.y.w+pos.z*b.z.w+pos.w*b.pos.w);
    return a;
}

Matrix4& Matrix4::operator*=(const Matrix4& b) {
    Matrix4 a;
    a.x = Vector3(x.x*b.x.x+x.y*b.y.x+x.z*b.z.x+x.w*b.pos.x,
                  x.x*b.x.y+x.y*b.y.y+x.z*b.z.y+x.w*b.pos.y,
                  x.x*b.x.z+x.y*b.y.z+x.z*b.z.z+x.w*b.pos.z,
                  x.x*b.x.w+x.y*b.y.w+x.z*b.z.w+x.w*b.pos.w);
    a.y = Vector3(y.x*b.x.x+y.y*b.y.x+y.z*b.z.x+y.w*b.pos.x,
                  y.x*b.x.y+y.y*b.y.y+y.z*b.z.y+y.w*b.pos.y,
                  y.x*b.x.z+y.y*b.y.z+y.z*b.z.z+y.w*b.pos.z,
                  y.x*b.x.w+y.y*b.y.w+y.z*b.z.w+y.w*b.pos.w);
    a.z = Vector3(z.x*b.x.x+z.y*b.y.x+z.z*b.z.x+z.w*b.pos.x,
                  z.x*b.x.y+z.y*b.y.y+z.z*b.z.y+z.w*b.pos.y,
                  z.x*b.x.z+z.y*b.y.z+z.z*b.z.z+z.w*b.pos.z,
                  z.x*b.x.w+z.y*b.y.w+z.z*b.z.w+z.w*b.pos.w);
    a.pos = Vector3(pos.x*b.x.x+pos.y*b.y.x+pos.z*b.z.x+pos.w*b.pos.x,
                    pos.x*b.x.y+pos.y*b.y.y+pos.z*b.z.y+pos.w*b.pos.y,
                    pos.x*b.x.z+pos.y*b.y.z+pos.z*b.z.z+pos.w*b.pos.z,
                    pos.x*b.x.w+pos.y*b.y.w+pos.z*b.z.w+pos.w*b.pos.w);
    *this = a;
    return *this;
}

Matrix4& Matrix4::operator=(const Matrix4& mat) {
    x = mat.x;
    y = mat.y;
    z = mat.z;
    pos = mat.pos;
    return *this;
}

Matrix4& Matrix4::operator*=(const float& factor) {
    x *= factor;
    y *= factor;
    z *= factor;
    pos *= factor;
    return *this;
}

Matrix4& Matrix4::translate(Vector3 vec) {
    Matrix4 b;
    b.setIdentity();
    b.pos = vec;
    b.pos.w = 1.0;
    return (*this *= b);
}

Matrix4& Matrix4::scale(Vector3 vec) {
    Matrix4 b;
    b.setIdentity();
    b.x.x = vec.x;
    b.y.y = vec.y;
    b.z.z = vec.z;
    return (*this *= b);
}

Matrix4& Matrix4::rotateX(float value) {
    float sinValue = sin(value), cosValue = cos(value);
    Matrix4 b;
    b.setIdentity();
    b.y.y = cosValue;
    b.y.z = -sinValue;
    b.z.y = sinValue;
    b.z.z = cosValue;
    return (*this *= b);
}

Matrix4& Matrix4::rotateY(float value) {
    float sinValue = sin(value), cosValue = cos(value);
    Matrix4 b;
    b.setIdentity();
    b.x.x = cosValue;
    b.x.z = sinValue;
    b.z.x = -sinValue;
    b.z.z = cosValue;
    return (*this *= b);
}

Matrix4& Matrix4::rotateZ(float value) {
    float sinValue = sin(value), cosValue = cos(value);
    Matrix4 b;
    b.setIdentity();
    b.x.x = cosValue;
    b.x.y = -sinValue;
    b.y.x = sinValue;
    b.y.y = cosValue;
    return (*this *= b);
}

Matrix4&  Matrix4::rotateV(Vector3 vec, float value) {
    float sinValue = sin(value), cosValue = cos(value), cosInv = 1.0 - cosValue;
    Matrix4 b;
    b.setIdentity();
    x.x = cosValue+vec.x*vec.x*cosInv;
    x.y = vec.x*vec.y*cosInv-vec.z*sinValue;
    x.z = vec.x*vec.z*cosInv+vec.y*sinValue;
    y.x = vec.x*vec.y*cosInv+vec.z*sinValue;
    y.y = cosValue+vec.y*vec.y*cosInv;
    y.z = vec.y*vec.z*cosInv-vec.x*sinValue;
    z.x = vec.x*vec.z*cosInv-vec.y*sinValue;
    z.y = vec.y*vec.z*cosInv+vec.x*sinValue;
    z.z = cosValue+vec.z*vec.z*cosInv;
    return (*this *= b);
}

Matrix4& Matrix4::setDirection(Vector3 dir, Vector3 up) {
    Matrix4 b;
    b.setIdentity();
    b.z = dir*-1.0;
    b.x = (up/b.z).normalize();
    b.y = (b.z/b.x);
    return (*this *= b);
}

Matrix4& Matrix4::perspective(float fovy, float aspect, float n, float f) {
    float a = 1.0/tan(fovy*0.5);
    Matrix4 b;
    b.setIdentity();
    b.x.x = a/aspect;
    b.y.y = a;
    b.z.z = (n+f)/(n-f);
    b.z.w = -1.0;
    b.pos.z = (2.0*n*f)/(n-f);
    b.pos.w = 0.0;
    return (*this *= b);
}

Matrix4& Matrix4::frustum(float w, float h, float n, float f) {
    Matrix4 b;
    b.setIdentity();
    b.x.x = n/w;
    b.y.y = n/h;
    b.z.z = (n+f)/(n-f);
    b.z.w = -1.0;
    b.pos.z = (2.0*n*f)/(n-f);
    b.pos.w = 0.0;
    return (*this *= b);
}

Matrix4& Matrix4::ortho(float w, float h, float n, float f) {
    Matrix4 b;
    b.setIdentity();
    b.x.x = 1.0/w;
    b.y.y = 1.0/h;
    b.z.z = -2.0/(f-n);
    b.pos.z = (-f-n)/(f-n);
    return (*this *= b);
}
