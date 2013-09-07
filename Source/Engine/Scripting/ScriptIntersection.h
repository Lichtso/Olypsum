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
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void RayCast(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AABBIntersection(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SphereIntersection(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptIntersection();
};

extern ScriptIntersection scriptIntersection;

#endif