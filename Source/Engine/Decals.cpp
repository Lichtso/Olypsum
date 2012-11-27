//
//  Decals.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 11.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "WorldManager.h"

GLuint vbo;

void initLightVolumes() {
    float vertices[] = {
        -1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
        1.0, -1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0,
        -1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
        1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0
    };
    glGenBuffers(1, &decalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, decalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}