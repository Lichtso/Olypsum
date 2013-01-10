//
//  Ray3.h
//  Olypsum
//
//  Created by Alexander Meißner on 24.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include <BulletDynamics/btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftBody.h>

#ifndef Ray3_h
#define Ray3_h

//! A ray with a origin and a direction
class Ray3 {
    public:
    btVector3 origin, //!< The origin of the Ray3
              direction; //!< The direction of the Ray3
    Ray3() { };
    /*! Constructs a new Ray3
     @param origin The origin of the Ray3
     @param direction The direction of the Ray3
     */
    Ray3(btVector3 origin, btVector3 direction);
};

#endif
