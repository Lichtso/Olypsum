//
//  Frustum3.h
//  Olypsum
//
//  Created by Alexander Meißner on 23.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Plane3.h"

#ifndef Frustum3_h
#define Frustum3_h

class Frustum3 {
    public:
    bool hemisphere;
    Plane3 front, back, left, right, bottom, top;
    Frustum3();
    bool testPointHit(Vector3 pos);
    bool testBsHit(Bs3* bs);
    template <class T> bool testBoxHit(T* box);
};

#endif
