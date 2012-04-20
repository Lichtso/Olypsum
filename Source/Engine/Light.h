//
//  Light.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Cam.h"

#ifndef Light_h
#define Light_h

enum LightType {
    LightType_Directional = 0,
    LightType_Spot = 1,
    LightType_Positional = 2
};

class Light {
    public:
    bool illuminationActive, shadowActive;
    LightType type;
    float color[3];
    //virtual void calculate();
    //virtual void use();
    Light();
};
/*
class DirectionalLight : public Light {
    public:
    Vector3 dir;
    void calculate();
    void use();
    DirectionalLight();
};

class SpotLight : public Light {
    public:
    Vector3 dir, pos;
    float cutoff;
    void calculate();
    void use();
    SpotLight();
};

class PositionalLight : public Light {
    public:
    Vector3 pos;
    void calculate();
    void use();
    PositionalLight();
};*/

extern std::vector<Light*> lights;

#endif