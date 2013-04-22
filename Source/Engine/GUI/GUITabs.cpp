//
//  GUITabs.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 03.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUITabs.h"

GUITabs::GUITabs() {
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
        button->sizeAlignment = GUISizeAlignment::All;
        button->updateContent();
        if(orientation & GUIOrientation::Vertical) {
            widthAux = max(widthAux, button->width*2);
            heightAux += button->height*2-1;
        }else{
            widthAux += button->width*2-1;
            heightAux = max(heightAux, button->height*2);
        }
    }
    
    if(sizeAlignment & GUISizeAlignment::Width) width = ceil(0.5*widthAux)+1;
    if(sizeAlignment & GUISizeAlignment::Height) height = ceil(0.5*heightAux)+1;
    
    int posCounter = 0;
    for(unsigned int i = 0; i < children.size(); i ++) {
        button = (GUIButton*)children[i];
        if(button->state != GUIButtonStateDisabled)
            button->state = (i == selectedIndex) ? GUIButtonStatePressed : GUIButtonStateNormal;
        button->content.roundedCorners = (GUICorner) 0;
        
        if(orientation & GUIOrientation::Vertical) {
            button->width = width;
            if(!(sizeAlignment & GUISizeAlignment::Height)) button->height = floor((float)height/children.size())+(i%2);
            button->posX = 0;
            button->posY = height-posCounter-button->height;
            
            switch((int)orientation) {
                case GUIOrientation::Left:
                    if(i == 0)
                        button->content.roundedCorners = (GUICorner) (button->content.roundedCorners | GUICorner::TopRight);
                    if(i == children.size()-1)
                        button->content.roundedCorners = (GUICorner) (button->content.roundedCorners | GUICorner::BottomRight);
                break;
                case GUIOrientation::Right:
                    if(i == 0)
                        button->content.roundedCorners = (GUICorner) (button->content.roundedCorners | GUICorner::TopLeft);
                    if(i == children.size()-1)
                        button->content.roundedCorners = (GUICorner) (button->content.roundedCorners | GUICorner::BottomLeft);
                break;
                case GUIOrientation::Vertical:
                    if(i == 0)
                        button->content.roundedCorners = (GUICorner) (button->content.roundedCorners | GUICorner::TopLeft | GUICorner::TopRight);
                    if(i == children.size()-1)
                        button->content.roundedCorners = (GUICorner) (button->content.roundedCorners | GUICorner::BottomLeft | GUICorner::BottomRight);
                break;
            }
            
            posCounter += button->height*2-1;
        }else{
            if(!(sizeAlignment & GUISizeAlignment::Width)) button->width = floor(width/(float)children.size())+(i%2);
            button->height = height;
            button->posX = posCounter+button->width-width;
            button->posY = 0;
            
            switch((int)orientation) {
                case GUIOrientation::Top:
                    if(i == 0)
                        button->content.roundedCorners = (GUICorner) (button->content.roundedCorners | GUICorner::BottomLeft);
                    if(i == children.size()-1)
                        button->content.roundedCorners = (GUICorner) (button->content.roundedCorners | GUICorner::BottomRight);
                    break;
                case GUIOrientation::Bottom:
                    if(i == 0)
                        button->content.roundedCorners = (GUICorner) (button->content.roundedCorners | GUICorner::TopLeft);
                    if(i == children.size()-1)
                        button->content.roundedCorners = (GUICorner) (button->content.roundedCorners | GUICorner::TopRight);
                    break;
                case GUIOrientation::Horizontal:
                    if(i == 0)
                        button->content.roundedCorners = (GUICorner) (button->content.roundedCorners | GUICorner::TopLeft | GUICorner::BottomLeft);
                    if(i == children.size()-1)
                        button->content.roundedCorners = (GUICorner) (button->content.roundedCorners | GUICorner::TopRight | GUICorner::BottomRight);
                    break;
            }
            
            posCounter += button->width*2-1;
        }
        button->sizeAlignment = GUISizeAlignment::None;
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