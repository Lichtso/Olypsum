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
    ColorBuffer* shadowMap;
    public:
    int glIndex;
    Vector3 direction, upDir;
    float range;
    LightType type;
    Vector3 color;
    Light();
    ~Light();
    virtual bool calculateShadowmap();
    virtual void deleteShadowmap();
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
    void deleteShadowmap();
    void use();
};

class SpotLight : public Light {
    public:
    Vector3 position;
    float cutoff;
    SpotLight();
    bool calculateShadowmap();
    void deleteShadowmap();
    void use();
};

class PositionalLight : public Light {
    ColorBuffer* shadowMapB;
    public:
    Vector3 position;
    bool omniDirectional;
    PositionalLight();
    ~PositionalLight();
    bool calculateShadowmap();
    void deleteShadowmap();
    void use();
};

#define maxLightCount 3

class LightManager {
    public:
    std::vector<Light*> lights;
    ~LightManager();
    void calculateShadows(unsigned int maxShadows);
    void setLights(Vector3 position);
    void setAllLightsOff();
};

extern LightManager lightManager;

#endif