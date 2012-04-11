//
//  GUIRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "GUIView.h"

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

bool GUIRect::isFirstResponder() {
    GUIScreenView* screenView = (GUIScreenView*)getRootParent();
    if(!screenView || screenView->type != GUIType_ScreenView) return false;
    return (screenView->firstResponder == this);
}

void GUIRect::removeFirstResponderStatus() {
    
}

bool GUIRect::getLimSize(GUIClipRect& clipRect, GUIClipRect& parentClipRect) {
    return clipRect.getLimSize(posX, posY, width, height, parentClipRect);
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
    spriteShaderProgram->setAttribute(POSITION_ATTRIBUTE, 2, 2*sizeof(float), vertices);
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