//
//  Ray3.h
//  Olypsum
//
//  Created by Alexander Meißner on 24.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef Ray3_h
#define Ray3_h

#include "XMLUtilities.h"

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
    //! Returns the distance from the origin to the hit point of the plane defined by the two parameters
    float getDistToPlane(btVector3 normal, float dist);
    //! Returns the hit point of the plane defined by the two parameters
    btVector3 getHitPointPlane(btVector3 normal, float dist);
    //! Performs a hit test against physics world and returns the neares hit point, normal and object on Success else NULL
    unsigned int hitTestNearest(short filterMask, BaseObject*& object, btVector3& point, btVector3& normal);
    //! Performs a hit test against physics world and returns the count of hits on Success else 0
    unsigned int hitTestAll(short filterMask, std::vector<BaseObject*>& objects, std::vector<btVector3>& points, std::vector<btVector3>& normals);
};

#endif
