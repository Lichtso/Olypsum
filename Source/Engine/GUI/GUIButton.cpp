//
//  GUIButton.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUIButton.h"

GUIButton::GUIButton() {
    type = GUIType_Button;
    texture = 0;
    onClick = NULL;
    paddingX = 10*screenSize[2];
    paddingY = 5*screenSize[2];
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
                    label->color = Color4(0.12);
                break;
                case GUIButtonStateNormal:
                case GUIButtonStateHighlighted:
                    switch(buttonType) {
                        case GUIButtonTypeNormal:
                        case GUIButtonTypeLockable:
                            label->color = Color4(0.24);
                        break;
                        case GUIButtonTypeDelete:
                        case GUIButtonTypeAdd:
                        case GUIButtonTypeEdit:
                            label->color = Color4(0.94);
                        break;
                    }
                break;
                case GUIButtonStatePressed:
                    switch(buttonType) {
                        case GUIButtonTypeNormal:
                            label->color = Color4(0.12, 0.43, 0.78);
                        break;
                        case GUIButtonTypeDelete:
                        case GUIButtonTypeAdd:
                        case GUIButtonTypeEdit:
                        case GUIButtonTypeLockable:
                            label->color = Color4(0.94);
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
    roundedRect.cornerRadius = 10*screenSize[2];
    if(state >= GUIButtonStatePressed) roundedRect.innerShadow = 6*screenSize[2];
    
    switch(state) {
        case GUIButtonStateDisabled:
            roundedRect.topColor = Color4(0.78);
            roundedRect.bottomColor = Color4(0.4);
            break;
        case GUIButtonStateNormal:
            switch(buttonType) {
                case GUIButtonTypeNormal:
                case GUIButtonTypeLockable:
                    roundedRect.topColor = Color4(0.94);
                    roundedRect.bottomColor = Color4(0.71);
                    break;
                case GUIButtonTypeDelete:
                    roundedRect.topColor = Color4(0.94, 0.63, 0.63);
                    roundedRect.bottomColor = Color4(0.55, 0.0, 0.0);
                    break;
                case GUIButtonTypeAdd:
                    roundedRect.topColor = Color4(0.55, 0.94, 0.55);
                    roundedRect.bottomColor = Color4(0.0, 0.55, 0.0);
                    break;
                case GUIButtonTypeEdit:
                    roundedRect.topColor = Color4(0.55, 0.78, 0.94);
                    roundedRect.bottomColor = Color4(0.0, 0.24, 0.63);
                    break;
            }
            break;
        case GUIButtonStateHighlighted:
            switch(buttonType) {
                case GUIButtonTypeNormal:
                case GUIButtonTypeLockable:
                    roundedRect.topColor = Color4(0.98);
                    roundedRect.bottomColor = Color4(0.82);
                    break;
                case GUIButtonTypeDelete:
                    roundedRect.topColor = Color4(0.98, 0.75, 0.75);
                    roundedRect.bottomColor = Color4(0.59, 0.12, 0.12);
                    break;
                case GUIButtonTypeAdd:
                    roundedRect.topColor = Color4(0.71, 0.98, 0.71);
                    roundedRect.bottomColor = Color4(0.12, 0.59, 0.12);
                    break;
                case GUIButtonTypeEdit:
                    roundedRect.topColor = Color4(0.63, 0.86, 0.94);
                    roundedRect.bottomColor = Color4(0.08, 0.31, 0.71);
                    break;
            }
            break;
        case GUIButtonStatePressed:
            switch(buttonType) {
                case GUIButtonTypeNormal:
                    roundedRect.topColor = Color4(0.7);
                    roundedRect.bottomColor = Color4(0.78);
                    break;
                case GUIButtonTypeDelete:
                    roundedRect.topColor = Color4(0.55, 0.0, 0.0);
                    roundedRect.bottomColor = Color4(0.94, 0.63, 0.63);
                    break;
                case GUIButtonTypeAdd:
                    roundedRect.topColor = Color4(0.0, 0.55, 0.0);
                    roundedRect.bottomColor = Color4(0.55, 0.94, 0.55);
                    break;
                case GUIButtonTypeEdit:
                case GUIButtonTypeLockable:
                    roundedRect.topColor = Color4(0.0, 0.24, 0.63);
                    roundedRect.bottomColor = Color4(0.55, 0.78, 0.94);
                    break;
            }
            break;
    }
    
    roundedRect.drawInTexture();
}

void GUIButton::draw(btVector3 transform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    if(!texture) updateContent();
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    
    transform += btVector3(posX, posY, 0.0);
    modelMat.setIdentity();
    modelMat.setOrigin(transform);
    
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

bool GUIButton::handleMouseUp(int mouseX, int mouseY) {
    if(!visible || state == GUIButtonStateDisabled) return false;
    bool triggerEvent = false;
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height)
        state = GUIButtonStateNormal;
    else{
        if(state == GUIButtonStatePressed)
            triggerEvent = true;
        state = GUIButtonStateHighlighted;
    }
    updateContent();
    if(triggerEvent && onClick) {
        onClick(this);
        return true;
    }
    return false;
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