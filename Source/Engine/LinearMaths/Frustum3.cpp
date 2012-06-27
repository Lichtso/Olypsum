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
    if(hemisphere) return front.testPointHit(pos);
    if(!front.testPointHit(pos)) return false;
    if(!back.testPointHit(pos)) return false;
    if(!left.testPointHit(pos)) return false;
    if(!right.testPointHit(pos)) return false;
    if(!bottom.testPointHit(pos)) return false;
    if(!top.testPointHit(pos)) return false;
    return true;
}

bool Frustum3::testBsHit(Bs3* bs) {
    if(hemisphere) return front.testBsHit(bs);
    if(!front.testBsHit(bs)) return false;
    if(!back.testBsHit(bs)) return false;
    if(!left.testBsHit(bs)) return false;
    if(!right.testBsHit(bs)) return false;
    if(!bottom.testBsHit(bs)) return false;
    if(!top.testBsHit(bs)) return false;
    return true;
}

template <class T> bool Frustum3::testBoxHit(T* box) {
    Vector3 vertices[8];
    box->getVertices(vertices);
    if(hemisphere) return front.testPolyhedronInclusiveHit(vertices, 8);
    if(!front.testPolyhedronInclusiveHit(vertices, 8)) return false;
    if(!back.testPolyhedronInclusiveHit(vertices, 8)) return false;
    if(!left.testPolyhedronInclusiveHit(vertices, 8)) return false;
    if(!right.testPolyhedronInclusiveHit(vertices, 8)) return false;
    if(!bottom.testPolyhedronInclusiveHit(vertices, 8)) return false;
    if(!top.testPolyhedronInclusiveHit(vertices, 8)) return false;
    return true;
}

bool Frustum3::testPolyhedronInclusiveHit(Vector3* vertices, unsigned int verticesCount) {
    if(hemisphere) return front.testPolyhedronInclusiveHit(vertices, verticesCount);
    if(!front.testPolyhedronInclusiveHit(vertices, verticesCount)) return false;
    if(!back.testPolyhedronInclusiveHit(vertices, verticesCount)) return false;
    if(!left.testPolyhedronInclusiveHit(vertices, verticesCount)) return false;
    if(!right.testPolyhedronInclusiveHit(vertices, verticesCount)) return false;
    if(!bottom.testPolyhedronInclusiveHit(vertices, verticesCount)) return false;
    if(!top.testPolyhedronInclusiveHit(vertices, verticesCount)) return false;
    return true;
}