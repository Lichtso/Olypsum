//
//  Matrix4.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Vector3.h"

#ifndef Matrix4_h
#define Matrix4_h

class Matrix4 {
    public:
    Vector3 x, y, z, pos;
    Matrix4();
    Matrix4(Matrix4 const &mat);
    Matrix4(btTransform const &mat);
    Matrix4(float matData[16]);
    void print();
    btTransform getMatrix();
    void getOpenGLMatrix3(float matData[9]);
    void getOpenGLMatrix4(float matData[16]);
    void setMatrix3(const Matrix4&);
    void setIdentity();
    void setZero();
    Matrix4 getTransposed();
    Matrix4 getInverse();
    Matrix4& normalize();
    Matrix4 operator*(const Matrix4&);
    Matrix4& operator*=(const Matrix4&);
    Matrix4& operator=(const Matrix4&);
    Matrix4& operator*=(const float& factor);
    Matrix4& translate(Vector3 vec);
    Matrix4& scale(Vector3 vec);
    Matrix4& rotateX(float x);
    Matrix4& rotateY(float y);
    Matrix4& rotateZ(float z);
    Matrix4& lookAt(Vector3 pos, Vector3 at, Vector3 up, float zoomDist);
    Matrix4& perspective(float fovy, float aspect, float near, float far);
    Matrix4& frustum(float width, float height, float near, float far);
    Matrix4& ortho(float width, float height, float near, float far);
};

#endif
