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
    unsigned int shadowResolution;
    float range;
    LightType type;
    Vector3 color;
    Light();
    ~Light();
    virtual bool calculateShadowmap();
    virtual void use();
};

class DirectionalLight : public Light {
    public:
    Vector3 direction;
    float distance, width, height;
    DirectionalLight();
    bool calculateShadowmap();
    void use();
};

class SpotLight : public Light {
    public:
    Vector3 direction, position;
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

#define maxLightCount 4

class LightManager {
    void setLightsForShader();
    public:
    std::vector<Light*> lights;
    ~LightManager();
    void setLights();
};

extern LightManager lightManager;

#endif