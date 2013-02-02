//
//  Matrix4.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Utilities.h"

#ifndef Matrix4_h
#define Matrix4_h

//! A extension of the btTransform
/*!
 This class supports additional functions for the graphics
 */
class Matrix4 {
    public:
    union {
        struct {
            btVector3 x, y, z, w;
        }; //!< The transformation split up in 4 btVector3
        btVector3 rows[4]; //!< The transformation split up in btVector3[4]
        btScalar values[16]; //!< The transformation split up in 16 btScalar
    };
    Matrix4();
    Matrix4(Matrix4 const &mat);
    Matrix4(btTransform const &mat);
    Matrix4(btScalar matData[16]);
    //! Returns the colum of this Matrix4 at index
    btVector3 getColum(unsigned char index);
    //! Converts this Matrix4 into a btTransform
    btTransform getBTMatrix();
    //! Converts this Matrix4 into a btMatrix3x3 (the w component is ignored)
    btMatrix3x3 getNormalMatrix();
    //! Converts this Matrix4 into a float[16]
    void getOpenGLMatrix(float* matData) const;
    //! Resets the transformation
    Matrix4& setIdentity();
    //! Mirrors the values at the diagonal
    Matrix4 getTransposed() const;
    //! Returns the inverse of this Matrix4
    Matrix4 getInverse();
    //! Returns a Matrix4 without scalation
    Matrix4& normalize();
    //! Returns the product of this Matrix4 and another
    Matrix4 operator*(const Matrix4&);
    //! Multiplies this Matrix4 with another
    Matrix4& operator*=(const Matrix4&);
    //! Overwrites the transformation with another Matrix4
    Matrix4& operator=(const Matrix4&);
    //! Returns the product of this Matrix4 and a btVector3
    btVector3 operator()(const btVector3&);
    //! Applies a scale to the transformation
    Matrix4& scale(btVector3 vec);
    //! Applies a rotation to the transformation
    Matrix4& rotate(btVector3 vec, btScalar angle);
    //! Applies a translation to the transformation
    Matrix4& translate(btVector3 vec);
    //! Applies a shift and a scale to the transformation by 0.5 to get a shadow map for example
    Matrix4& makeTextureMat();
    /*! Applies a perspective projection to the transformation
     @param fovy The field of view
     @param aspect Width / Height of the view
     @param near The distance of the near plane
     @param far The distance of the near far
     */
    Matrix4& perspective(btScalar fovy, btScalar aspect, btScalar near, btScalar far);
    /*! Applies a perspective projection to the transformation
     @param width Width of the view
     @param height Height of the view
     @param near The distance of the near plane
     @param far The distance of the near far
     */
    Matrix4& frustum(btScalar width, btScalar height, btScalar near, btScalar far);
    /*! Applies a orthogonal projection to the transformation
     @param width Width of the view
     @param height Height of the view
     @param near The distance of the near plane
     @param far The distance of the near far
     */
    Matrix4& ortho(btScalar width, btScalar height, btScalar near, btScalar far);
};

//! Converts a Matrix4 into a std::string
std::string stringOf(Matrix4& mat);

#endif
