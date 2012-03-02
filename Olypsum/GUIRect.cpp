//
//  GUIRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "GUIRect.h"

GUIRect::GUIRect() {
    visible = true;
    posX = 0;
    posY = 0;
    width = 100;
    height = 100;
}

void GUIRect::getLimSize(GUIClipRect* parentClipRect, GUIClipRect* clipRect) {
    clipRect->minPosX = -width;
    clipRect->minPosY = -height;
    clipRect->maxPosX = width;
    clipRect->maxPosY = height;
    
    if(posX+clipRect->minPosX < parentClipRect->minPosX)
        clipRect->minPosX = parentClipRect->minPosX-posX;
    if(posY+clipRect->minPosY < parentClipRect->minPosY)
        clipRect->minPosY = parentClipRect->minPosY-posY;
    if(posX+clipRect->maxPosX > parentClipRect->maxPosX)
        clipRect->maxPosX = parentClipRect->maxPosX-posX;
    if(posY+clipRect->maxPosY > parentClipRect->maxPosY)
        clipRect->maxPosY = parentClipRect->maxPosY-posY;
}

void GUIRect::updateContent() {
    
}

void GUIRect::draw(Matrix4& parentTransform, GUIClipRect* parentClipRect) {
    GUIClipRect clipRect;
    getLimSize(parentClipRect, &clipRect);
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
    //spriteShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, 4*sizeof(float), &vertices[2]);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void GUIRect::handleMouseDown(int mouseX, int mouseY) {
    
}

void GUIRect::handleMouseUp(int mouseX, int mouseY) {
    
}

void GUIRect::handleMouseMove(int mouseX, int mouseY) {
    
}