//
//  GUIButtonList.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 03.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIButtonList.h"

GUIButtonList::GUIButtonList() {
    orientation = GUIOrientationVertical;
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
        if(orientation & GUIOrientationHorizontal) {
            widthAux = max(widthAux, button->width);
            heightAux += button->height;
        }else{
            widthAux += button->width;
            heightAux = max(heightAux, button->height);
        }
    }
    
    if(orientation & GUIOrientationHorizontal)
        heightAux -= ceil(children.size() / 2.0) - 1;
    else
        widthAux -= ceil(children.size() / 2.0) - 1;
    
    int posCounter = 0;
    for(unsigned int i = 0; i < children.size(); i ++) {
        button = (GUIButton*)children[i];
        if(orientation & GUIOrientationHorizontal) {
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

void GUIButtonList::draw(Matrix4& parentTransform, GUIClipRect* parentClipRect) {
    if(!visible || children.size() == 0) return;
    
    GUIClipRect clipRect;
    getLimSize(parentClipRect, &clipRect);
    if(clipRect.minPosX > clipRect.maxPosX || clipRect.minPosY > clipRect.maxPosY) return;
    
    //GUIRect::draw(parentTransform, parentClipRect);
    
    Matrix4 transform(parentTransform);
    transform.translate(Vector3(posX, posY, 0.0));
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, &clipRect);
}