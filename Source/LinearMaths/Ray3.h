//
//  Ray3.h
//  Olypsum
//
//  Created by Alexander Meißner on 24.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "BoundingVolumes.h"

#ifndef Ray3_h
#define Ray3_h

class Ray3 {
    public:
    Vector3 origin, direction;
    Ray3();
    Ray3(Vector3 origin, Vector3 direction);
    void set(Vector3 origin, Vector3 direction);
};

#endif
