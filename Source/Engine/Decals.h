//
//  Decals.h
//  Olypsum
//
//  Created by Alexander Meißner on 11.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "ShaderProgram.h"

#ifndef Decals_h
#define Decals_h

class Decal {
    public:
    btTransform transformation;
    std::shared_ptr<Texture> diffuse, heightMap;
    float life;
};

#endif