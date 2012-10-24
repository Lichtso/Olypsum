//
//  Ray3.h
//  Olypsum
//
//  Created by Alexander Meißner on 24.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <BulletDynamics/btBulletDynamicsCommon.h>
#import <BulletSoftBody/btSoftBody.h>
#import "Utilities.h"

#ifndef Ray3_h
#define Ray3_h

class Ray3 {
    public:
    btVector3 origin, direction;
    Ray3();
    Ray3(btVector3 origin, btVector3 direction);
    void set(btVector3 origin, btVector3 direction);
};

class FrustumCullingCallback : public btCollisionWorld::ContactResultCallback {
    public:
    bool inView;
	FrustumCullingCallback() : inView(false) { }
	virtual	btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
        inView = true;
        return 0;
	}
};

inline btVector3 vec3rand(btVector3 min, btVector3 max) {
    return btVector3(frand(min.x(), max.x()), frand(min.y(), max.y()), frand(min.z(), max.z()));
}

#endif
