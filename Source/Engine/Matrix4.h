//
//  Matrix4.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "LinearMaths.h"

#ifndef Matrix4_h
#define Matrix4_h

class Matrix4 {
    public:
    union {
        struct {
            btVector3 x, y, z, w;
        } data;
        btScalar values[16];
    };
    Matrix4();
    Matrix4(Matrix4 const &mat);
    Matrix4(btTransform const &mat);
    Matrix4(btScalar matData[16]);
    std::string getString();
    btTransform getBTMatrix();
    btMatrix3x3 getNormalMatrix();
    void getOpenGLMatrix(float* matData) const;
    Matrix4& setIdentity();
    Matrix4 getTransposed() const;
    Matrix4 getInverse();
    Matrix4& normalize();
    Matrix4 operator*(const Matrix4&);
    Matrix4& operator*=(const Matrix4&);
    Matrix4& operator=(const Matrix4&);
    Matrix4& scale(btVector3 vec);
    Matrix4& makeTextureMat();
    Matrix4& perspective(btScalar fovy, btScalar aspect, btScalar near, btScalar far);
    Matrix4& frustum(btScalar width, btScalar height, btScalar near, btScalar far);
    Matrix4& ortho(btScalar width, btScalar height, btScalar near, btScalar far);
};

#endif
