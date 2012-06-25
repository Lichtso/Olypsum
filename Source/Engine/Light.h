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
    Vector3 direction, upDir;
    float range;
    LightType type;
    Vector3 color;
    Light();
    ~Light();
    virtual bool calculateShadowmap();
    virtual void deleteShadowmap();
    virtual void use();
    virtual void selectShaderProgram(bool skeletal);
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
    void selectShaderProgram(bool skeletal);
};

class SpotLight : public Light {
    public:
    Vector3 position;
    float cutoff;
    SpotLight();
    bool calculateShadowmap();
    void deleteShadowmap();
    void use();
    void selectShaderProgram(bool skeletal);
};

class PositionalLight : public Light {
    ColorBuffer* shadowMapB;
    Matrix4 shadowMatB;
    public:
    Vector3 position;
    bool omniDirectional;
    PositionalLight();
    ~PositionalLight();
    bool calculateShadowmap();
    void deleteShadowmap();
    void use();
    void selectShaderProgram(bool skeletal);
};

class LightManager {
    public:
    std::vector<Light*> lights;
    Light* currentShadowLight;
    ~LightManager();
    void calculateShadows(unsigned int maxShadows);
    void useLights();
};

extern LightManager lightManager;

#endif