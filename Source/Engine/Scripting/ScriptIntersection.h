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
    ScriptDeclareMethod(Constructor);
    ScriptDeclareMethod(RayCast);
    ScriptDeclareMethod(AABBIntersection);
    ScriptDeclareMethod(SphereIntersection);
    public:
    ScriptIntersection();
};

extern std::unique_ptr<ScriptIntersection> scriptIntersection;

#endif