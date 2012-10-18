//
//  Matrix4.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Matrix4.h"

Matrix4::Matrix4() {
    
}

Matrix4::Matrix4(Matrix4 const &mat) {
    x = mat.x;
    y = mat.y;
    z = mat.z;
    w = mat.w;
}

Matrix4::Matrix4(btTransform const &mat) {
    btMatrix3x3 basis = mat.getBasis();
    x = basis.getRow(0);
    y = basis.getRow(1);
    z = basis.getRow(2);
    w = mat.getOrigin();
}

Matrix4::Matrix4(float matData[16]) {
    x = btVector3(matData[0], matData[1], matData[2]);
    x.setW(matData[3]);
    y = btVector3(matData[4], matData[5], matData[6]);
    y.setW(matData[7]);
    z = btVector3(matData[8], matData[9], matData[10]);
    z.setW(matData[11]);
    w = btVector3(matData[12], matData[13], matData[14]);
    w.setW(matData[15]);
}

std::string Matrix4::getString() {
    char buffer[64];
    btScalar values[16];
    getOpenGLMatrix(values);
    std::string str = "(";
    for(unsigned char i = 0; i < 16; i ++) {
        if(i % 4 > 0) str += ", ";
        sprintf(buffer, "%f", values[i]);
        str += buffer;
        if(i % 4 == 3 && i < 15) str += ",\n ";
    }
    return str+")";
}

btMatrix3x3 Matrix4::getNormalMatrix() {
    return btMatrix3x3(x.normalized(), y.normalized(), z.normalized());
}

btTransform Matrix4::getBTMatrix() {
    float matDataA[16];
    getOpenGLMatrix(matDataA);
    btTransform mat;
    btScalar matDataB[16];
    for(unsigned char i = 0; i < 16; i ++)
        matDataB[i] = (btScalar)matDataA[i];
    mat.setFromOpenGLMatrix(matDataB);
    return mat;
}

void Matrix4::getOpenGLMatrix(float* matData) const {
    matData[0] = x.x();
    matData[1] = y.x();
    matData[2] = z.x();
    matData[3] = w.x();
    matData[4] = x.y();
    matData[5] = y.y();
    matData[6] = z.y();
    matData[7] = w.y();
    matData[8] = x.z();
    matData[9] = y.z();
    matData[10] = z.z();
    matData[11] = w.z();
    matData[12] = x.w();
    matData[13] = y.w();
    matData[14] = z.w();
    matData[15] = w.w();
}

Matrix4& Matrix4::setIdentity() {
    x = btVector3(1, 0, 0);
    y = btVector3(0, 1, 0);
    z = btVector3(0, 0, 1);
    w = btVector3(0, 0, 0);
    w.setW(1);
    return *this;
}

Matrix4 Matrix4::getTransposed() {
    Matrix4 b;
    b.x.setX(x.x());
    b.x.setY(y.x());
    b.x.setZ(z.x());
    b.x.setW(w.x());
    b.y.setX(x.y());
    b.y.setY(y.y());
    b.y.setZ(z.y());
    b.y.setW(w.y());
    b.z.setX(x.z());
    b.z.setY(y.z());
    b.z.setZ(z.z());
    b.z.setW(w.z());
    b.w.setX(x.w());
    b.w.setY(y.w());
    b.w.setZ(z.w());
    b.w.setX(w.w());
    return b;
}

Matrix4 Matrix4::operator*(const Matrix4& b) {
    Matrix4 a;
    a.x.setX(x.x()*b.x.x()+x.y()*b.y.x()+x.z()*b.z.x()+x.w()*b.w.x());
    a.x.setY(x.x()*b.x.y()+x.y()*b.y.y()+x.z()*b.z.y()+x.w()*b.w.y());
    a.x.setZ(x.x()*b.x.z()+x.y()*b.y.z()+x.z()*b.z.z()+x.w()*b.w.z());
    a.x.setW(x.x()*b.x.w()+x.y()*b.y.w()+x.z()*b.z.w()+x.w()*b.w.w());
    a.y.setX(y.x()*b.x.x()+y.y()*b.y.x()+y.z()*b.z.x()+y.w()*b.w.x());
    a.y.setY(y.x()*b.x.y()+y.y()*b.y.y()+y.z()*b.z.y()+y.w()*b.w.y());
    a.y.setZ(y.x()*b.x.z()+y.y()*b.y.z()+y.z()*b.z.z()+y.w()*b.w.z());
    a.y.setW(y.x()*b.x.w()+y.y()*b.y.w()+y.z()*b.z.w()+y.w()*b.w.w());
    a.z.setX(z.x()*b.x.x()+z.y()*b.y.x()+z.z()*b.z.x()+z.w()*b.w.x());
    a.z.setY(z.x()*b.x.y()+z.y()*b.y.y()+z.z()*b.z.y()+z.w()*b.w.y());
    a.z.setZ(z.x()*b.x.z()+z.y()*b.y.z()+z.z()*b.z.z()+z.w()*b.w.z());
    a.z.setW(z.x()*b.x.w()+z.y()*b.y.w()+z.z()*b.z.w()+z.w()*b.w.w());
    a.w.setX(w.x()*b.x.x()+w.y()*b.y.x()+w.z()*b.z.x()+w.w()*b.w.x());
    a.w.setY(w.x()*b.x.y()+w.y()*b.y.y()+w.z()*b.z.y()+w.w()*b.w.y());
    a.w.setZ(w.x()*b.x.z()+w.y()*b.y.z()+w.z()*b.z.z()+w.w()*b.w.z());
    a.w.setW(w.x()*b.x.w()+w.y()*b.y.w()+w.z()*b.z.w()+w.w()*b.w.w());
    return a;
}

Matrix4& Matrix4::operator*=(const Matrix4& b) {
    Matrix4 a;
    a.x.setX(x.x()*b.x.x()+x.y()*b.y.x()+x.z()*b.z.x()+x.w()*b.w.x());
    a.x.setY(x.x()*b.x.y()+x.y()*b.y.y()+x.z()*b.z.y()+x.w()*b.w.y());
    a.x.setZ(x.x()*b.x.z()+x.y()*b.y.z()+x.z()*b.z.z()+x.w()*b.w.z());
    a.x.setW(x.x()*b.x.w()+x.y()*b.y.w()+x.z()*b.z.w()+x.w()*b.w.w());
    a.y.setX(y.x()*b.x.x()+y.y()*b.y.x()+y.z()*b.z.x()+y.w()*b.w.x());
    a.y.setY(y.x()*b.x.y()+y.y()*b.y.y()+y.z()*b.z.y()+y.w()*b.w.y());
    a.y.setZ(y.x()*b.x.z()+y.y()*b.y.z()+y.z()*b.z.z()+y.w()*b.w.z());
    a.y.setW(y.x()*b.x.w()+y.y()*b.y.w()+y.z()*b.z.w()+y.w()*b.w.w());
    a.z.setX(z.x()*b.x.x()+z.y()*b.y.x()+z.z()*b.z.x()+z.w()*b.w.x());
    a.z.setY(z.x()*b.x.y()+z.y()*b.y.y()+z.z()*b.z.y()+z.w()*b.w.y());
    a.z.setZ(z.x()*b.x.z()+z.y()*b.y.z()+z.z()*b.z.z()+z.w()*b.w.z());
    a.z.setW(z.x()*b.x.w()+z.y()*b.y.w()+z.z()*b.z.w()+z.w()*b.w.w());
    a.w.setX(w.x()*b.x.x()+w.y()*b.y.x()+w.z()*b.z.x()+w.w()*b.w.x());
    a.w.setY(w.x()*b.x.y()+w.y()*b.y.y()+w.z()*b.z.y()+w.w()*b.w.y());
    a.w.setZ(w.x()*b.x.z()+w.y()*b.y.z()+w.z()*b.z.z()+w.w()*b.w.z());
    a.w.setW(w.x()*b.x.w()+w.y()*b.y.w()+w.z()*b.z.w()+w.w()*b.w.w());
    *this = a;
    return *this;
}

Matrix4& Matrix4::operator=(const Matrix4& mat) {
    x = mat.x;
    y = mat.y;
    z = mat.z;
    w = mat.w;
    return *this;
}

Matrix4& Matrix4::scale(btVector3 vec) {
    Matrix4 a;
    a.setIdentity();
    a.x.setX(vec.x());
    a.y.setY(vec.y());
    a.z.setZ(vec.z());
    *this = a;
    return *this;
}

Matrix4& Matrix4::makeTextureMat() {
    Matrix4 a;
    a.setIdentity();
    a.x.setX(0.5);
    a.y.setY(0.5);
    a.w = btVector3(0.5, 0.5, 0.0);
    *this = a;
    return *this;
}

Matrix4& Matrix4::perspective(float fovy, float aspect, float n, float f) {
    float a = 1.0/tan(fovy*0.5);
    Matrix4 b;
    b.setIdentity();
    b.x.setX(a/aspect);
    b.y.setY(a);
    b.z.setZ((n+f)/(n-f));
    b.z.setW((2.0*n*f)/(n-f));
    b.w.setZ(-1.0);
    b.w.setW(0.0);
    return (*this *= b);
}

Matrix4& Matrix4::frustum(float w, float h, float n, float f) {
    Matrix4 b;
    b.setIdentity();
    b.x.setX(n/w);
    b.y.setY(n/h);
    b.z.setZ((n+f)/(n-f));
    b.z.setW((2.0*n*f)/(n-f));
    b.w.setZ(-1.0);
    b.w.setW(0.0);
    return (*this *= b);
}

Matrix4& Matrix4::ortho(float w, float h, float n, float f) {
    Matrix4 b;
    b.setIdentity();
    b.x.setX(1.0/w);
    b.y.setY(1.0/h);
    b.z.setZ(2.0/(n-f));
    b.z.setW((f+n)/(n-f));
    return (*this *= b);
}