//
//  GUICheckBox.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 04.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUICheckBox.h"

GUICheckBox::GUICheckBox() {
    paddingX = paddingY = 0;
    width = height = currentScreenView->width*0.022;
    sizeAlignment = GUISizeAlignment_None;
    GUILabel* label = new GUILabel();
    label->text = "√";
    label->parent = this;
    label->fontHeight = currentScreenView->width*0.03;
    label->posY = currentScreenView->width*0.002;
    children.push_back(label);
}

void GUICheckBox::addChild(GUIRect* child) {
    
}

void GUICheckBox::updateContent() {
    GUILabel* label = (GUILabel*)children[0];
    if(state == GUIButtonStatePressed)
        label->visible = true;
    else
        label->visible = false;
    
    GUIButton::updateContent();
}

bool GUICheckBox::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || state == GUIButtonStateDisabled || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    state = (state != GUIButtonStatePressed) ? GUIButtonStatePressed : GUIButtonStateHighlighted;
    updateContent();
    if(onClick)
        onClick(this);
    return true;
}

bool GUICheckBox::handleMouseUp(int mouseX, int mouseY) {
    if(!visible || state == GUIButtonStateDisabled || state == GUIButtonStatePressed) return false;
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) {
        state = GUIButtonStateNormal;
        updateContent();
    }
    return false;
}

void GUICheckBox::handleMouseMove(int mouseX, int mouseY) {
    if(!visible || state == GUIButtonStateDisabled || state == GUIButtonStatePressed) return;
    GUIButtonState prevState = state;
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) {
        state = GUIButtonStateNormal;
    }else
        state = GUIButtonStateHighlighted;
    if(prevState != state) updateContent();
}