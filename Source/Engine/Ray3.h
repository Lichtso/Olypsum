//
//  Ray3.h
//  Olypsum
//
//  Created by Alexander Meißner on 24.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "XMLUtilities.h"

#ifndef Ray3_h
#define Ray3_h

class BaseObject;

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
    //! Performs a hit test against physics world and returns the neares hit point, normal and object on Success else NULL
    unsigned int hitTestNearest(short filterMask, BaseObject*& object, btVector3& point, btVector3& normal);
    //! Performs a hit test against physics world and returns the count of hits on Success else 0
    unsigned int hitTestAll(short filterMask, std::vector<BaseObject*>& objects, std::vector<btVector3>& points, std::vector<btVector3>& normals);
};

#endif
