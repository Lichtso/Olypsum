//
//  GUIRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "../Scripting/ScriptManager.h"

GUIRect::GUIRect() :scriptInstance(NULL), parent(NULL), visible(true), posX(0), posY(0), width(100), height(100) {
    
}

GUIRect::~GUIRect() {
    if(scriptInstance)
        JSValueUnprotect(scriptManager->mainScript->context, scriptInstance);
}

GUIRect* GUIRect::getRootParent() {
    GUIRect* node = parent;
    while(node->parent)
        node = node->parent;
    return node;
}

bool GUIRect::isFocused() {
    GUIScreenView* screenView = dynamic_cast<GUIScreenView*>(getRootParent());
    if(!screenView) return false;
    return (screenView->focus == this);
}

void GUIRect::setFocused(bool active) {
    GUIScreenView* screenView = dynamic_cast<GUIScreenView*>(getRootParent());
    if(!screenView) return;
    if(active) {
        if(screenView->focus)
            screenView->focus->setFocused(false);
        screenView->focus = this;
    }else
        screenView->focus = NULL;
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
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    return true;
}