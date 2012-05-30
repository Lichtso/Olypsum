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
    LightType_Directional = 1,
    LightType_Spot = 2,
    LightType_Positional = 3
};

class Light {
    protected:
    Cam* shadowCam;
    int shadowMap;
    public:
    int glIndex;
    Vector3 direction, upDir;
    float range;
    LightType type;
    Vector3 color;
    Light();
    ~Light();
    virtual bool calculateShadowmap();
    virtual void use();
    float getPriority(Vector3 position);
};

class LightPrioritySorter {
    public:
    Vector3 position;
    bool operator()(Light* a, Light* b);
};

class DirectionalLight : public Light {
    public:
    float distance, width, height;
    DirectionalLight();
    bool calculateShadowmap();
    void use();
};

class SpotLight : public Light {
    public:
    Vector3 position;
    float cutoff;
    SpotLight();
    bool calculateShadowmap();
    void use();
};

class PositionalLight : public Light {
    int shadowMapB;
    public:
    Vector3 position;
    PositionalLight();
    ~PositionalLight();
    bool calculateShadowmap();
    void use();
};

#define maxLightCount 3

class LightManager {
    public:
    std::vector<Light*> lights;
    ~LightManager();
    void setLights(Vector3 position);
    void setAllLightsOff();
};

extern LightManager lightManager;

#endif