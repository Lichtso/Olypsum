//
//  Frustum3.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 23.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Frustum3.h"

Frustum3::Frustum3() {
    hemisphere = false;
}

bool Frustum3::testBsHit(Bs3* bs) {
    if(hemisphere)
        return bs->center*front.normal-front.distance >= bs->radius;
    if(bs->center*front.normal-front.distance < bs->radius) return false;
    if(bs->center*back.normal-back.distance < bs->radius) return false;
    if(bs->center*left.normal-left.distance < bs->radius) return false;
    if(bs->center*right.normal-right.distance < bs->radius) return false;
    if(bs->center*bottom.normal-bottom.distance < bs->radius) return false;
    if(bs->center*top.normal-top.distance < bs->radius) return false;
    return true;
}

bool Frustum3::testAabbHit(Aabb3* aabb) {
    if(hemisphere)
        return front.testAabbHitCount(aabb) < 8;
    if(front.testAabbHitCount(aabb) == 8) return false;
    if(back.testAabbHitCount(aabb) == 8) return false;
    if(left.testAabbHitCount(aabb) == 8) return false;
    if(right.testAabbHitCount(aabb) == 8) return false;
    if(bottom.testAabbHitCount(aabb) == 8) return false;
    if(top.testAabbHitCount(aabb) == 8) return false;
    return true;
}