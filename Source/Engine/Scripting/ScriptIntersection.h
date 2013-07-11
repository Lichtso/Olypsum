//
//  ScriptIntersectionTest.h
//  Olypsum
//
//  Created by Alexander Meißner on 03.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptIntersection_h
#define ScriptIntersection_h

#include "ScriptLinks.h"

class ScriptIntersection : public ScriptClass {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> RayCast(const v8::Arguments& args);
    static v8::Handle<v8::Value> AABBIntersection(const v8::Arguments& args);
    static v8::Handle<v8::Value> SphereIntersection(const v8::Arguments& args);
    public:
    ScriptIntersection();
};

extern ScriptIntersection scriptIntersection;

#endif