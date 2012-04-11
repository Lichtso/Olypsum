//
//  GUIButtonList.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 03.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIButtonList.h"

GUIButtonList::GUIButtonList() {
    type = GUIType_ButtonList;
    orientation = GUIOrientation_Vertical;
    autoSize = true;
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
        button->autoSize = true;
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
        heightAux -= ceil(children.size() / 2.0) - 1;
    else
        widthAux -= ceil(children.size() / 2.0) - 1;
    
    int posCounter = 0;
    for(unsigned int i = 0; i < children.size(); i ++) {
        button = (GUIButton*)children[i];
        if(orientation & GUIOrientation_Vertical) {
            button->width = widthAux;
            button->posX = 0;
            button->posY = heightAux-posCounter-button->height;
            button->roundedCorners = (GUICorners) ((i == 0) ? GUITopLeftCorner | GUITopRightCorner : 0);
            if(i == children.size()-1) button->roundedCorners = (GUICorners) (button->roundedCorners | GUIBottomLeftCorner | GUIBottomRightCorner);
            posCounter += button->height*2-1;
        }else{
            button->height = heightAux;
            button->posX = -widthAux+posCounter+button->width;
            button->posY = 0;
            button->roundedCorners = (GUICorners) ((i == 0) ? GUITopLeftCorner | GUIBottomLeftCorner : 0);
            if(i == children.size()-1) button->roundedCorners = (GUICorners) (button->roundedCorners | GUITopRightCorner | GUIBottomRightCorner);
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