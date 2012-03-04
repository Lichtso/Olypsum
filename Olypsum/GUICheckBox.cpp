//
//  GUICheckBox.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 04.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUICheckBox.h"

GUICheckBox::GUICheckBox() {
    buttonType = GUIButtonTypeLockable;
    paddingX = 4;
    paddingY = 0;
    
    GUILabel* label = new GUILabel();
    label->text = "x";
    label->parent = this;
    label->posY = 2;
    children.push_back(label);
}

GUICheckBox::~GUICheckBox() {
    delete children[0];
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
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height || state == GUIButtonStateDisabled) return false;
    state = (state != GUIButtonStatePressed) ? GUIButtonStatePressed : GUIButtonStateHighlighted;
    updateContent();
    if(clicked)
        clicked(this);
    return true;
}

void GUICheckBox::handleMouseUp(int mouseX, int mouseY) {
    if(state == GUIButtonStateDisabled || state == GUIButtonStatePressed) return;
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height)
        state = GUIButtonStateNormal;
    else return;
    updateContent();
}

void GUICheckBox::handleMouseMove(int mouseX, int mouseY) {
    if(state == GUIButtonStateDisabled || state == GUIButtonStatePressed) return;
    GUIButtonState prevState = state;
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) {
        state = GUIButtonStateNormal;
    }else
        state = GUIButtonStateHighlighted;
    if(prevState != state) updateContent();
}