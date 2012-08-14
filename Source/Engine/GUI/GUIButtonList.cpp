//
//  GUIButtonList.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 03.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUIButtonList.h"

GUIButtonList::GUIButtonList() {
    type = GUIType_ButtonList;
    orientation = GUIOrientation_Vertical;
    sizeAlignment = GUISizeAlignment_All;
}

void GUIButtonList::addChild(GUIButton* child) {
    children.push_back(child);
    child->parent = this;
    updateContent();
}

void GUIButtonList::updateContent() {
    if(children.size() == 0) return;
    
    GUIButton* button;
    int widthAux = 0, heightAux = 0;
    
    for(unsigned int i = 0; i < children.size(); i ++) {
        button = (GUIButton*)children[i];
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
        if(orientation & GUIOrientation_Vertical) {
            button->width = width;
            if(!(sizeAlignment & GUISizeAlignment_Height)) button->height = round(height/(float)children.size());
            button->posX = 0;
            button->posY = height-posCounter-button->height;
            button->roundedCorners = (GUICorners) ((i == 0) ? GUITopLeftCorner | GUITopRightCorner : 0);
            if(i == children.size()-1) button->roundedCorners = (GUICorners) (button->roundedCorners | GUIBottomLeftCorner | GUIBottomRightCorner);
            posCounter += button->height*2-1;
        }else{
            if(!(sizeAlignment & GUISizeAlignment_Width)) button->width = round(width/(float)children.size());
            button->height = height;
            button->posX = -width+posCounter+button->width;
            button->posY = 0;
            button->roundedCorners = (GUICorners) ((i == 0) ? GUITopLeftCorner | GUIBottomLeftCorner : 0);
            if(i == children.size()-1) button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopRightCorner | GUIBottomRightCorner);
            posCounter += button->width*2-1;
        }
        button->sizeAlignment = GUISizeAlignment_None;
        button->updateContent();
    }
}