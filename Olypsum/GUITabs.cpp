//
//  GUITabs.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 03.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUITabs.h"

GUITabs::GUITabs() {
    type = GUITypeTabs;
    selectedIndex = -1;
    deactivatable = true;
    clicked = NULL;
}

void GUITabs::addChild(GUIButton* child) {
    children.push_back(child);
    child->parent = this;
    if(!deactivatable) selectedIndex = 0;
    updateContent();
}

void GUITabs::updateContent() {
    if(children.size() == 0) return;
    
    GUIButton* button;
    int widthAux = 0, heightAux = 0;
    
    for(unsigned int i = 0; i < children.size(); i ++) {
        button = (GUIButton*)children[i];
        button->buttonType = GUIButtonTypeRadio;
        button->autoSize = true;
        button->updateContent();
        if(orientation & GUIOrientationVertical) {
            widthAux = max(widthAux, button->width);
            heightAux += button->height;
        }else{
            widthAux += button->width;
            heightAux = max(heightAux, button->height);
        }
    }
    
    if(orientation & GUIOrientationVertical)
        heightAux -= ceil(children.size() / 2.0) - 1;
    else
        widthAux -= ceil(children.size() / 2.0) - 1;
    
    int posCounter = 0;
    for(unsigned int i = 0; i < children.size(); i ++) {
        button = (GUIButton*)children[i];
        if(button->state != GUIButtonStateDisabled)
            button->state = (i == selectedIndex) ? GUIButtonStatePressed : GUIButtonStateNormal;
        button->roundedCorners = (GUICorners) 0;
        
        if(orientation & GUIOrientationVertical) {
            button->width = widthAux;
            button->posX = 0;
            button->posY = heightAux-posCounter-button->height;
            
            switch((int)orientation) {
                case GUIOrientationLeft:
                    if(i == 0)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopRightCorner);
                    if(i == children.size()-1)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUIBottomRightCorner);
                break;
                case GUIOrientationRight:
                    if(i == 0)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopLeftCorner);
                    if(i == children.size()-1)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUIBottomLeftCorner);
                break;
                case GUIOrientationVertical:
                    if(i == 0)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopLeftCorner | GUITopRightCorner);
                    if(i == children.size()-1)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUIBottomLeftCorner | GUIBottomRightCorner);
                break;
            }
            
            posCounter += button->height*2-1;
        }else{
            button->height = heightAux;
            button->posX = -widthAux+posCounter+button->width;
            button->posY = 0;
            
            switch((int)orientation) {
                case GUIOrientationTop:
                    if(i == 0)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUIBottomLeftCorner);
                    if(i == children.size()-1)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUIBottomRightCorner);
                    break;
                case GUIOrientationBottom:
                    if(i == 0)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopLeftCorner);
                    if(i == children.size()-1)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopRightCorner);
                    break;
                case GUIOrientationHorizontal:
                    if(i == 0)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopLeftCorner | GUIBottomLeftCorner);
                    if(i == children.size()-1)
                        button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopRightCorner | GUIBottomRightCorner);
                    break;
            }
            
            posCounter += button->width*2-1;
        }
        button->autoSize = false;
        button->updateContent();
    }
    
    if(autoSize) {
        width = widthAux;
        height = heightAux;
    }
}

bool GUITabs::handleMouseDown(int mouseXo, int mouseYo) {
    if(mouseXo < -width || mouseXo > width || mouseYo < -height || mouseYo > height) return false;
    GUIButton* button;
    int mouseX, mouseY;
    for(int i = (int)children.size()-1; i >= 0; i --) {
        button = (GUIButton*)children[i];
        mouseX = mouseXo-button->posX;
        mouseY = mouseYo-button->posY;
        if(mouseX < -button->width || mouseX > button->width || mouseY < -button->height || mouseY > button->height || button->state == GUIButtonStateDisabled) continue;
        
        if(selectedIndex != i) {
            button->state = GUIButtonStatePressed;
            selectedIndex = i;
            if(button->clicked)
                button->clicked(button);
            if(clicked)
                clicked(this);
        }else if(deactivatable) {
            button->state = GUIButtonStateHighlighted;
            selectedIndex = -1;
        }else return false;
        
        for(int j = 0; j < children.size(); j ++) {
            button = (GUIButton*)children[j];
            if(i != j && button->state != GUIButtonStateDisabled)
                button->state = GUIButtonStateNormal;
            button->updateContent();
        }
        return true;
    }
    return false;
}

void GUITabs::handleMouseUp(int mouseXo, int mouseYo) {
    
}

void GUITabs::handleMouseMove(int mouseXo, int mouseYo) {
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