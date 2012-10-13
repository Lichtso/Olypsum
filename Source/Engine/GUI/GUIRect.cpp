//
//  GUIRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "ShaderProgram.h"
#import "GUIView.h"

GUIRect::GUIRect() {
    type = GUIType_Rect;
    parent = NULL;
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

void GUIRect::setFirstResponderStatus() {
    GUIScreenView* screenView = (GUIScreenView*)getRootParent();
    if(!screenView || screenView->type != GUIType_ScreenView) return;
    if(screenView->firstResponder)
        screenView->firstResponder->removeFirstResponderStatus();
    screenView->firstResponder = this;
}

void GUIRect::removeFirstResponderStatus() {
    GUIScreenView* screenView = (GUIScreenView*)getRootParent();
    if(!screenView || screenView->type != GUIType_ScreenView) return;
    screenView->firstResponder = NULL;
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
        (float)clipRect.maxPosX, (float)clipRect.minPosY,
        (float)clipRect.maxPosX, (float)clipRect.maxPosY,
        (float)clipRect.minPosX, (float)clipRect.maxPosY,
        (float)clipRect.minPosX, (float)clipRect.minPosY
    };
    
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    shaderPrograms[spriteSP]->use();
    shaderPrograms[spriteSP]->setAttribute(POSITION_ATTRIBUTE, 2, 2*sizeof(float), vertices);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

bool GUIRect::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    return true;
}

bool GUIRect::handleMouseUp(int mouseX, int mouseY) {
    return false;
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