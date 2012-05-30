//
//  GUIButton.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIButton.h"

GUIButton::GUIButton() {
    type = GUIType_Button;
    texture = 0;
    onClick = NULL;
    paddingX = 10;
    paddingY = 5;
    sizeAlignment = GUISizeAlignment_All;
    buttonType = GUIButtonTypeNormal;
    state = GUIButtonStateNormal;
    roundedCorners = (GUICorners) (GUITopLeftCorner | GUITopRightCorner | GUIBottomLeftCorner | GUIBottomRightCorner);
}

GUIButton::~GUIButton() {
    if(texture)
        glDeleteTextures(1, &texture);
}

void GUIButton::updateContent() {
    if(sizeAlignment & GUISizeAlignment_Width)
        width = 12+paddingX;
    
    if(sizeAlignment & GUISizeAlignment_Height)
        height = 12+paddingY;
    
    for(unsigned int i = 0; i < children.size(); i ++) {
        if(children[i]->type == GUIType_Label) {
            GUILabel* label = (GUILabel*)children[i];
            switch(state) {
                case GUIButtonStateDisabled:
                    label->color.r = label->color.g = label->color.b = 60;
                break;
                case GUIButtonStateNormal:
                case GUIButtonStateHighlighted:
                    switch(buttonType) {
                        case GUIButtonTypeNormal:
                        case GUIButtonTypeLockable:
                            label->color.r = label->color.g = label->color.b = 60;
                        break;
                        case GUIButtonTypeDelete:
                        case GUIButtonTypeAdd:
                        case GUIButtonTypeEdit:
                            label->color.r = label->color.g = label->color.b = 240;
                        break;
                    }
                break;
                case GUIButtonStatePressed:
                    switch(buttonType) {
                        case GUIButtonTypeNormal:
                            label->color.r = 30;
                            label->color.g = 110;
                            label->color.b = 200;
                        break;
                        case GUIButtonTypeDelete:
                        case GUIButtonTypeAdd:
                        case GUIButtonTypeEdit:
                        case GUIButtonTypeLockable:
                            label->color.r = label->color.g = label->color.b = 240;
                        break;
                    }
                break;
            }
        }
        children[i]->updateContent();
        
        if(sizeAlignment & GUISizeAlignment_Width) {
            if(children[i]->posX+children[i]->width+paddingX > width)
                width = children[i]->posX+children[i]->width+paddingX;
            if(children[i]->posX-children[i]->width-paddingX < -width)
                width = children[i]->width-children[i]->posX+paddingX;
        }
        
        if(sizeAlignment & GUISizeAlignment_Height) {
            if(children[i]->posY+children[i]->height+paddingY > height)
                height = children[i]->posY+children[i]->height+paddingY;
            if(children[i]->posY-children[i]->height-paddingY < -height)
                height = children[i]->height-children[i]->posY+paddingY;
        }
    }
    
    GUIRoundedRect roundedRect;
    roundedRect.texture = &texture;
    roundedRect.width = width;
    roundedRect.height = height;
    roundedRect.roundedCorners = roundedCorners;
    roundedRect.cornerRadius = 12;
    if(state >= GUIButtonStatePressed) roundedRect.innerShadow = 6;
    
    switch(state) {
        case GUIButtonStateDisabled:
            roundedRect.topColor.r = roundedRect.topColor.g = roundedRect.topColor.b = 200;
            roundedRect.bottomColor.r = roundedRect.bottomColor.g = roundedRect.bottomColor.b = 100;
            break;
        case GUIButtonStateNormal:
            switch(buttonType) {
                case GUIButtonTypeNormal:
                case GUIButtonTypeLockable:
                    roundedRect.topColor.r = roundedRect.topColor.g = roundedRect.topColor.b = 240;
                    roundedRect.bottomColor.r = roundedRect.bottomColor.g = roundedRect.bottomColor.b = 180;
                    break;
                case GUIButtonTypeDelete:
                    roundedRect.topColor.r = 240;
                    roundedRect.topColor.g = roundedRect.topColor.b = 160;
                    roundedRect.bottomColor.r = 140;
                    roundedRect.bottomColor.g = roundedRect.bottomColor.b = 0;
                    break;
                case GUIButtonTypeAdd:
                    roundedRect.topColor.g = 240;
                    roundedRect.topColor.r = roundedRect.topColor.b = 140;
                    roundedRect.bottomColor.g = 140;
                    roundedRect.bottomColor.r = roundedRect.bottomColor.b = 0;
                    break;
                case GUIButtonTypeEdit:
                    roundedRect.topColor.r = 140;
                    roundedRect.topColor.g = 200;
                    roundedRect.topColor.b = 240;
                    roundedRect.bottomColor.r = 0;
                    roundedRect.bottomColor.g = 60;
                    roundedRect.bottomColor.b = 160;
                    break;
            }
            break;
        case GUIButtonStateHighlighted:
            switch(buttonType) {
                case GUIButtonTypeNormal:
                case GUIButtonTypeLockable:
                    roundedRect.topColor.r = roundedRect.topColor.g = roundedRect.topColor.b = 250;
                    roundedRect.bottomColor.r = roundedRect.bottomColor.g = roundedRect.bottomColor.b = 210;
                    break;
                case GUIButtonTypeDelete:
                    roundedRect.topColor.r = 250;
                    roundedRect.topColor.g = roundedRect.topColor.b = 190;
                    roundedRect.bottomColor.r = 150;
                    roundedRect.bottomColor.g = roundedRect.bottomColor.b = 30;
                    break;
                case GUIButtonTypeAdd:
                    roundedRect.topColor.g = 250;
                    roundedRect.topColor.r = roundedRect.topColor.b = 170;
                    roundedRect.bottomColor.g = 150;
                    roundedRect.bottomColor.r = roundedRect.bottomColor.b = 30;
                    break;
                case GUIButtonTypeEdit:
                    roundedRect.topColor.r = 160;
                    roundedRect.topColor.g = 220;
                    roundedRect.topColor.b = 240;
                    roundedRect.bottomColor.r = 20;
                    roundedRect.bottomColor.g = 80;
                    roundedRect.bottomColor.b = 180;
                    break;
            }
            break;
        case GUIButtonStatePressed:
            switch(buttonType) {
                case GUIButtonTypeNormal:
                    roundedRect.topColor.r = roundedRect.topColor.g = roundedRect.topColor.b = 180;
                    roundedRect.bottomColor.r = roundedRect.bottomColor.g = roundedRect.bottomColor.b = 240;
                    break;
                case GUIButtonTypeDelete:
                    roundedRect.topColor.r = 140;
                    roundedRect.topColor.g = roundedRect.topColor.b = 0;
                    roundedRect.bottomColor.r = 240;
                    roundedRect.bottomColor.g = roundedRect.bottomColor.b = 160;
                    break;
                case GUIButtonTypeAdd:
                    roundedRect.topColor.g = 140;
                    roundedRect.topColor.r = roundedRect.topColor.b = 0;
                    roundedRect.bottomColor.g = 240;
                    roundedRect.bottomColor.r = roundedRect.bottomColor.b = 140;
                    break;
                case GUIButtonTypeEdit:
                case GUIButtonTypeLockable:
                    roundedRect.topColor.r = 0;
                    roundedRect.topColor.g = 60;
                    roundedRect.topColor.b = 160;
                    roundedRect.bottomColor.r = 140;
                    roundedRect.bottomColor.g = 200;
                    roundedRect.bottomColor.b = 240;
                    break;
            }
            break;
    }
    
    roundedRect.drawInTexture();
}

void GUIButton::draw(Matrix4& parentTransform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    if(!texture) updateContent();
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    Matrix4 transform = parentTransform;
    transform.translate(Vector3(posX, posY, 0.0));
    modelMat = transform;
    
    GUIRoundedRect roundedRect;
    roundedRect.texture = &texture;
    roundedRect.width = width;
    roundedRect.height = height;
    roundedRect.drawOnScreen(false, 0, 0, clipRect);
    
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, clipRect);
}

bool GUIButton::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || state == GUIButtonStateDisabled || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    state = GUIButtonStatePressed;
    updateContent();
    return true;
}

void GUIButton::handleMouseUp(int mouseX, int mouseY) {
    if(!visible || state == GUIButtonStateDisabled) return;
    bool triggerEvent = false;
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height)
        state = GUIButtonStateNormal;
    else{
        if(state == GUIButtonStatePressed)
            triggerEvent = true;
        state = GUIButtonStateHighlighted;
    }
    updateContent();
    if(triggerEvent && onClick)
        onClick(this);
}

void GUIButton::handleMouseMove(int mouseX, int mouseY) {
    if(!visible || state == GUIButtonStateDisabled) return;
    GUIButtonState prevState = state;
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) {
        state = GUIButtonStateNormal;
    }else if(state == GUIButtonStatePressed)
        return;
    else
        state = GUIButtonStateHighlighted;
    if(prevState != state) updateContent();
}