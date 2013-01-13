//
//  GUIRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUIView.h"

GUIRect::GUIRect() {
    type = GUIType_Rect;
    parent = NULL;
    visible = true;
    posX = 0;
    posY = 0;
    width = 100;
    height = 100;
}

GUIRect::~GUIRect() {
    
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