//
//  GUIRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "GUIRect.h"

GUIRect::GUIRect() {
    posX = 0;
    posY = 0;
    width = 100;
    height = 25;
}

void GUIRect::recalculateSize() {
    
}

void GUIRect::getLimSize(GUIClipRect& clipRect) {
    clipRect.minPosX = -width;
    clipRect.minPosY = -height;
    clipRect.maxPosX = width;
    clipRect.maxPosY = height;
    
    if(posX+clipRect.minPosX < -parent->width)
        clipRect.minPosX = -parent->width-posX;
    if(posY+clipRect.minPosY < -parent->height)
        clipRect.minPosY = -parent->height-posY;
    if(posX+clipRect.maxPosX > parent->width)
        clipRect.maxPosX = parent->width-posX;
    if(posY+clipRect.maxPosY > parent->height)
        clipRect.maxPosY = parent->height-posY;
}

void GUIRect::handleMouseDown(Vector3 relativePos) {
    
}

void GUIRect::handleMouseUp(Vector3 relativePos) {
    
}

void GUIRect::handleMouseMove(Vector3 relativePos) {
    
}

void GUIRect::draw(Matrix4& parentTransform) {
    GUIClipRect clipRect;
    getLimSize(clipRect);
    Vector3 minFactor(clipRect.minPosX/width*0.5+0.5, clipRect.minPosY/height*0.5+0.5, 0.0),
            maxFactor(clipRect.maxPosX/width*0.5+0.5, clipRect.maxPosY/height*0.5+0.5, 0.0);
    
    float vertices[] = {
        clipRect.maxPosX, clipRect.minPosY,
        maxFactor.x, maxFactor.y,
        clipRect.maxPosX, clipRect.maxPosY,
        maxFactor.y, minFactor.y,
        clipRect.minPosX, clipRect.maxPosY,
        minFactor.x, minFactor.y,
        clipRect.minPosX, clipRect.minPosY,
        minFactor.x, maxFactor.y
    };
    
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    spriteShaderProgram->use();
    spriteShaderProgram->setAttribute(VERTEX_ATTRIBUTE, 2, 4*sizeof(float), vertices);
    spriteShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, 4*sizeof(float), &vertices[2]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}