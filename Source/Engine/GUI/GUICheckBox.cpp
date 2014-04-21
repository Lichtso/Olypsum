//
//  GUICheckBox.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 04.03.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "../Menu/Menu.h"

GUICheckBox::GUICheckBox() {
    paddingX = paddingY = 0;
    width = height = menu.screenView->width*0.022;
    sizeAlignment = GUISizeAlignment::None;
    GUILabel* label = new GUILabel();
    label->text = "√";
    label->fontHeight = menu.screenView->width*0.03;
    label->posY = menu.screenView->width*0.002;
    GUIButton::addChild(label);
}

bool GUICheckBox::addChild(GUIRect* child) {
    return false;
}

void GUICheckBox::updateContent() {
    GUILabel* label = (GUILabel*)children[0];
    if(state == GUIButton::State::Pressed)
        label->visible = true;
    else
        label->visible = false;
    
    GUIButton::updateContent();
}

bool GUICheckBox::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || !enabled || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    state = (state != GUIButton::State::Pressed) ? GUIButton::State::Pressed : GUIButton::State::Highlighted;
    updateContent();
    if(onClick)
        onClick(this);
    return true;
}

bool GUICheckBox::handleMouseUp(int mouseX, int mouseY) {
    if(!visible || !enabled || state == GUIButton::State::Pressed) return false;
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) {
        state = GUIButton::State::Released;
        updateContent();
    }
    return false;
}

void GUICheckBox::handleMouseMove(int mouseX, int mouseY) {
    if(!visible || !enabled || state == GUIButton::State::Pressed) return;
    GUIButton::State prevState = state;
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) {
        state = GUIButton::State::Released;
    }else
        state = GUIButton::State::Highlighted;
    if(prevState != state) updateContent();
}