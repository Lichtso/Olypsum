//
//  GUIButtonList.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 03.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUIButtonList.h"

GUIButtonList::GUIButtonList() {
    orientation = GUIOrientation::Vertical;
    sizeAlignment = GUISizeAlignment::All;
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
        button->sizeAlignment = GUISizeAlignment::All;
        button->updateContent();
        if(orientation & GUIOrientation::Vertical) {
            widthAux = max(widthAux, button->width);
            heightAux += button->height;
        }else{
            widthAux += button->width;
            heightAux = max(heightAux, button->height);
        }
    }
    
    if(orientation & GUIOrientation::Vertical)
        widthAux -= ceil(children.size() / 2.0) - 1;
    else
        heightAux -= ceil(children.size() / 2.0) - 1;
    
    if(sizeAlignment & GUISizeAlignment::Width) width = widthAux;
    if(sizeAlignment & GUISizeAlignment::Height) height = heightAux;
    
    int posCounter = 0;
    for(unsigned int i = 0; i < children.size(); i ++) {
        button = (GUIButton*)children[i];
        if(orientation & GUIOrientation::Vertical) {
            button->width = width;
            if(!(sizeAlignment & GUISizeAlignment::Height)) button->height = floor(height/(float)children.size())+(i%2);
            button->posX = 0;
            button->posY = height-posCounter-button->height;
            button->content.roundedCorners = (GUICorner) ((i == 0) ? GUICorner::TopLeft | GUICorner::TopRight : 0);
            if(i == children.size()-1) button->content.roundedCorners = (GUICorner) (button->content.roundedCorners | GUICorner::BottomLeft | GUICorner::BottomRight);
            posCounter += button->height*2-1;
        }else{
            if(!(sizeAlignment & GUISizeAlignment::Width)) button->width = floor(width/(float)children.size())+(i%2);
            button->height = height;
            button->posX = -width+posCounter+button->width;
            button->posY = 0;
            button->content.roundedCorners = (GUICorner) ((i == 0) ? GUICorner::TopLeft | GUICorner::BottomLeft : 0);
            if(i == children.size()-1) button->content.roundedCorners = (GUICorner) (button->content.roundedCorners | GUICorner::TopRight | GUICorner::BottomRight);
            posCounter += button->width*2-1;
        }
        button->sizeAlignment = GUISizeAlignment::None;
        button->updateContent();
    }
}