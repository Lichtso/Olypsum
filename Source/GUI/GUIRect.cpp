//
//  GUIRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "GUIRect.h"

GUIRect::GUIRect() {
    type = GUIType_Rect;
    visible = true;
    posX = 0;
    posY = 0;
    width = 100;
    height = 100;
}

GUIRect* GUIRect::getRootParent() {
    GUIRect* node = parent;
    while(node->parent)
        node = node->parent;
    return node;
}

void GUIRect::updateFirstResponderStatus() {
    
}

bool GUIRect::getLimSize(GUIClipRect& clipRect, GUIClipRect& parentClipRect) {
    clipRect.minPosX = -width;
    clipRect.minPosY = -height;
    clipRect.maxPosX = width;
    clipRect.maxPosY = height;
    
    if(parentClipRect.minPosX > posX-width)
        clipRect.minPosX = parentClipRect.minPosX-posX;
    if(parentClipRect.minPosY > posY-height)
        clipRect.minPosY = parentClipRect.minPosY-posY;
    if(parentClipRect.maxPosX < posX+width)
        clipRect.maxPosX = parentClipRect.maxPosX-posX;
    if(parentClipRect.maxPosY < posY+height)
        clipRect.maxPosY = parentClipRect.maxPosY-posY;
    
    return (clipRect.minPosX <= clipRect.maxPosX && clipRect.minPosY <= clipRect.maxPosY);
}

void GUIRect::updateContent() {
    
}

void GUIRect::draw(Matrix4& parentTransform, GUIClipRect& parentClipRect) {
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    
    float vertices[] = {
        clipRect.maxPosX, clipRect.minPosY,
        clipRect.maxPosX, clipRect.maxPosY,
        clipRect.minPosX, clipRect.maxPosY,
        clipRect.minPosX, clipRect.minPosY,
    };
    
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    spriteShaderProgram->use();
    spriteShaderProgram->setAttribute(VERTEX_ATTRIBUTE, 2, 2*sizeof(float), vertices);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

bool GUIRect::handleMouseDown(int mouseX, int mouseY) {
    return false;
}

void GUIRect::handleMouseUp(int mouseX, int mouseY) {
    
}

void GUIRect::handleMouseMove(int mouseX, int mouseY) {
    
}

bool GUIRect::handleMouseWheel(int mouseX, int mouseY, float delta) {
    return false;
}

bool GUIRect::handleKeyDown(SDL_keysym* key) {
    return false;
}

bool GUIRect::handleKeyUp(SDL_keysym* key) {
    return false;
}