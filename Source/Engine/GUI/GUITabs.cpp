//
//  GUITabs.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 03.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUITabs.h"

GUITabs::GUITabs() {
    type = GUIType_Tabs;
    selectedIndex = -1;
    deactivatable = true;
    onChange = NULL;
}

void GUITabs::addChild(GUIButton* child) {
    children.push_back(child);
    child->parent = this;
    if(!deactivatable && selectedIndex < 0) selectedIndex = 0;
    updateContent();
}

void GUITabs::updateContent() {
    if(children.size() == 0) return;
    
    GUIButton* button;
    int widthAux = 0, heightAux = 0;
    
    for(unsigned int i = 0; i < children.size(); i ++) {
        button = (GUIButton*)children[i];
        button->buttonType = GUIButtonTypeLockable;
        button->sizeAlignment = GUISizeAlignment_All;
        button->updateContent();
        if(orientation & GUIOrientation_Vertical) {
            widthAux = max(widthAux, button->width);
            heightAux += button->height;
        }else{
            widthAux += button->width;
            heightAux = max(heightAux, button->height);
        }
    }
    
    if(orientation & GUIOrientation_Vertical)
        widthAux -= ceil(children.size() / 2.0) - 1;
    else
        heightAux -= ceil(children.size() / 2.0) - 1;
    
    if(sizeAlignment & GUISizeAlignment_Width) width = widthAux;
    if(sizeAlignment & GUISizeAlignment_Height) height = heightAux;
    
    int posCounter = 0;
    for(unsigned int i = 0; i < children.size(); i ++) {
        button = (GUIButton*)children[i];
        if(button->state != GUIButtonStateDisabled)
            button->state = (i == selectedIndex) ? GUIButtonStatePressed : GUIButtonStateNormal;
        button->roundedCorners = (GUICorners) 0;
        
        if(orientation & GUIOrientation_Vertical) {
            button->width = width;
            if(!(sizeAlignment & GUISizeAlignment_Height)) button->height = floor(height/(float)children.size())+(i%2);
            button->posX = 0;
            button->posY = height-posCounter-button->height;
            
            switch((int)orientation) {
                case GUIOrientation_Left:
                    if(i == 0)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopRightCorner);
                    if(i == children.size()-1)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUIBottomRightCorner);
                break;
                case GUIOrientation_Right:
                    if(i == 0)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopLeftCorner);
                    if(i == children.size()-1)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUIBottomLeftCorner);
                break;
                case GUIOrientation_Vertical:
                    if(i == 0)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopLeftCorner | GUITopRightCorner);
                    if(i == children.size()-1)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUIBottomLeftCorner | GUIBottomRightCorner);
                break;
            }
            
            posCounter += button->height*2-1;
        }else{
            if(!(sizeAlignment & GUISizeAlignment_Width)) button->width = floor(width/(float)children.size())+(i%2);
            button->height = height;
            button->posX = posCounter+button->width-width;
            button->posY = 0;
            
            switch((int)orientation) {
                case GUIOrientation_Top:
                    if(i == 0)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUIBottomLeftCorner);
                    if(i == children.size()-1)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUIBottomRightCorner);
                    break;
                case GUIOrientation_Bottom:
                    if(i == 0)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopLeftCorner);
                    if(i == children.size()-1)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopRightCorner);
                    break;
                case GUIOrientation_Horizontal:
                    if(i == 0)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopLeftCorner | GUIBottomLeftCorner);
                    if(i == children.size()-1)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopRightCorner | GUIBottomRightCorner);
                    break;
            }
            
            posCounter += button->width*2-1;
        }
        button->sizeAlignment = GUISizeAlignment_None;
        button->updateContent();
    }
}

bool GUITabs::handleMouseDown(int mouseXo, int mouseYo) {
    if(!visible || mouseXo < -width || mouseXo > width || mouseYo < -height || mouseYo > height) return false;
    
    GUIButton* button;
    int mouseX, mouseY;
    for(int i = children.size()-1; i >= 0; i --) {
        button = (GUIButton*)children[i];
        mouseX = mouseXo-button->posX;
        mouseY = mouseYo-button->posY;
        if(mouseX < -button->width || mouseX > button->width || mouseY < -button->height || mouseY > button->height || button->state == GUIButtonStateDisabled) continue;
        
        if(selectedIndex != i) {
            button->state = GUIButtonStatePressed;
            selectedIndex = i;
            if(button->onClick)
                button->onClick(button);
            if(onChange)
                onChange(this);
        }else if(deactivatable) {
            button->state = GUIButtonStateHighlighted;
            selectedIndex = -1;
        }else return false;
        
        for(unsigned int j = 0; j < children.size(); j ++) {
            button = (GUIButton*)children[j];
            if(i != j && button->state != GUIButtonStateDisabled)
                button->state = GUIButtonStateNormal;
            button->updateContent();
        }
        return true;
    }
    return false;
}

bool GUITabs::handleMouseUp(int mouseX, int mouseY) {
    return false;
}

void GUITabs::handleMouseMove(int mouseXo, int mouseYo) {
    if(!visible) return;
    
    GUIButton* button;
    int mouseX, mouseY;
    for(int i = (int)children.size()-1; i >= 0; i --) {
        button = (GUIButton*) children[i];
        if(button->state == GUIButtonStateDisabled || button->state == GUIButtonStatePressed) continue;
        mouseX = mouseXo-button->posX;
        mouseY = mouseYo-button->posY;
        
        GUIButtonState prevState = button->state;
        if(mouseX < -button->width || mouseX > button->width || mouseY < -button->height || mouseY > button->height) {
            button->state = GUIButtonStateNormal;
        }else
            button->state = GUIButtonStateHighlighted;
        if(prevState != button->state) button->updateContent();
    }
}