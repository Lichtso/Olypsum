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

bool Frustum3::testPointHit(Vector3 pos) {
    if(hemisphere) return pos*front.normal-front.distance <= 0.0;
    if(pos*front.normal-front.distance > 0.0) return false;
    if(pos*back.normal-back.distance > 0.0) return false;
    if(pos*left.normal-left.distance > 0.0) return false;
    if(pos*right.normal-right.distance > 0.0) return false;
    if(pos*bottom.normal-bottom.distance > 0.0) return false;
    if(pos*top.normal-top.distance > 0.0) return false;
    return true;
}

bool Frustum3::testBsHit(Bs3* bs) {
    if(hemisphere) return bs->transformation->pos*front.normal-front.distance <= bs->radius;
    if(bs->transformation->pos*front.normal-front.distance > bs->radius) return false;
    if(bs->transformation->pos*back.normal-back.distance > bs->radius) return false;
    if(bs->transformation->pos*left.normal-left.distance > bs->radius) return false;
    if(bs->transformation->pos*right.normal-right.distance > bs->radius) return false;
    if(bs->transformation->pos*bottom.normal-bottom.distance > bs->radius) return false;
    if(bs->transformation->pos*top.normal-top.distance > bs->radius) return false;
    return true;
}

template <class T> bool Frustum3::testBoxHit(T* box) {
    Vector3 vertices[8];
    box->getVertices(vertices);
    if(hemisphere) return front.testPolyhedronHitCount(vertices, 8) < 8;
    if(front.testPolyhedronHitCount(vertices, 8) == 8) return false;
    if(back.testPolyhedronHitCount(vertices, 8) == 8) return false;
    if(left.testPolyhedronHitCount(vertices, 8) == 8) return false;
    if(right.testPolyhedronHitCount(vertices, 8) == 8) return false;
    if(bottom.testPolyhedronHitCount(vertices, 8) == 8) return false;
    if(top.testPolyhedronHitCount(vertices, 8) == 8) return false;
    return true;
}