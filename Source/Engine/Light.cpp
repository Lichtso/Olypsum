//
//  Light.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Light.h"

Light::Light() {
    illuminationActive = shadowActive = false;
    color[0] = color[1] = color[2] = 1.0;
}



std::vector<Light*> lights;