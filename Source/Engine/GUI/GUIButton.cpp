//
//  GUIButton.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Menu.h"

GUIButton::GUIButton() :sizeAlignment(GUISizeAlignment::All), type(GUIButton::Type::Normal), enabled(true), state(GUIButton::State::Released) {
    paddingX = menu.screenView->width*0.012;
    paddingY = menu.screenView->height*0.01;
}

void GUIButton::updateContent() {
    if(sizeAlignment & GUISizeAlignment::Width)
        width = 12+paddingX;
    
    if(sizeAlignment & GUISizeAlignment::Height)
        height = 12+paddingY;
    
    for(unsigned int i = 0; i < children.size(); i ++) {
        GUILabel* label = dynamic_cast<GUILabel*>(children[i]);
        if(label) {
            if(enabled)
                switch(state) {
                    case GUIButton::State::Released:
                    case GUIButton::State::Highlighted:
                        switch(type) {
                            case GUIButton::Type::Normal:
                            case GUIButton::Type::Lockable:
                                label->color = Color4(0.24);
                            break;
                            case GUIButton::Type::Delete:
                            case GUIButton::Type::Add:
                            case GUIButton::Type::Edit:
                                label->color = Color4(0.94);
                            break;
                        }
                        label->font = TextFont::normalFont();
                    break;
                    case GUIButton::State::Pressed:
                        switch(type) {
                            case GUIButton::Type::Normal:
                                label->color = Color4(0.12, 0.43, 0.78);
                            break;
                            case GUIButton::Type::Delete:
                            case GUIButton::Type::Add:
                            case GUIButton::Type::Edit:
                            case GUIButton::Type::Lockable:
                                label->color = Color4(0.94);
                            break;
                        }
                        label->font = TextFont::italicFont();
                    break;
                }
            else
                label->color = Color4(0.12);
        }
        
        children[i]->updateContent();
        
        if(sizeAlignment & GUISizeAlignment::Width) {
            width = max(width, children[i]->width+paddingX+children[i]->posX);
            width = max(width, children[i]->width+paddingX-children[i]->posX);
        }
        
        if(sizeAlignment & GUISizeAlignment::Height) {
            height = max(height, children[i]->height+paddingY+children[i]->posY);
            height = max(height, children[i]->height+paddingY-children[i]->posY);
        }
    }
    
    content.width = width;
    content.height = height;
    content.cornerRadius = menu.screenView->width*0.014;
    content.innerShadow = (state >= GUIButton::State::Pressed) ? menu.screenView->width*0.01 : 0;
    content.borderColor = Color4(0.5);
    
    if(enabled) {
        switch(state) {
            case GUIButton::State::Released:
                switch(type) {
                    case GUIButton::Type::Normal:
                    case GUIButton::Type::Lockable:
                        content.topColor = Color4(0.94);
                        content.bottomColor = Color4(0.71);
                        break;
                    case GUIButton::Type::Delete:
                        content.topColor = Color4(0.94, 0.63, 0.63);
                        content.bottomColor = Color4(0.55, 0.0, 0.0);
                        break;
                    case GUIButton::Type::Add:
                        content.topColor = Color4(0.55, 0.94, 0.55);
                        content.bottomColor = Color4(0.0, 0.55, 0.0);
                        break;
                    case GUIButton::Type::Edit:
                        content.topColor = Color4(0.55, 0.78, 0.94);
                        content.bottomColor = Color4(0.0, 0.24, 0.63);
                        break;
                }
            break;
            case GUIButton::State::Highlighted:
                switch(type) {
                    case GUIButton::Type::Normal:
                    case GUIButton::Type::Lockable:
                        content.topColor = Color4(0.98);
                        content.bottomColor = Color4(0.82);
                        break;
                    case GUIButton::Type::Delete:
                        content.topColor = Color4(0.98, 0.75, 0.75);
                        content.bottomColor = Color4(0.59, 0.12, 0.12);
                        break;
                    case GUIButton::Type::Add:
                        content.topColor = Color4(0.71, 0.98, 0.71);
                        content.bottomColor = Color4(0.12, 0.59, 0.12);
                        break;
                    case GUIButton::Type::Edit:
                        content.topColor = Color4(0.63, 0.86, 0.94);
                        content.bottomColor = Color4(0.08, 0.31, 0.71);
                        break;
                }
            break;
            case GUIButton::State::Pressed:
                switch(type) {
                    case GUIButton::Type::Normal:
                        content.topColor = Color4(0.7);
                        content.bottomColor = Color4(0.78);
                        break;
                    case GUIButton::Type::Delete:
                        content.topColor = Color4(0.55, 0.0, 0.0);
                        content.bottomColor = Color4(0.94, 0.63, 0.63);
                        break;
                    case GUIButton::Type::Add:
                        content.topColor = Color4(0.0, 0.55, 0.0);
                        content.bottomColor = Color4(0.55, 0.94, 0.55);
                        break;
                    case GUIButton::Type::Edit:
                    case GUIButton::Type::Lockable:
                        content.topColor = Color4(0.0, 0.24, 0.63);
                        content.bottomColor = Color4(0.55, 0.78, 0.94);
                        break;
                }
            break;
        }
    }else{
        content.topColor = Color4(0.78);
        content.bottomColor = Color4(0.4);
    }
    
    content.updateContent();
}

void GUIButton::draw(btVector3 transform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    
    transform += btVector3(posX, posY, 0.0);
    content.drawOnScreen(transform, 0, 0, clipRect);
    
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, clipRect);
}

bool GUIButton::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || !enabled || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    state = GUIButton::State::Pressed;
    updateContent();
    return true;
}

bool GUIButton::handleMouseUp(int mouseX, int mouseY) {
    if(!visible || !enabled) return false;
    bool triggerEvent = false;
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height)
        state = GUIButton::State::Released;
    else{
        if(state == GUIButton::State::Pressed)
            triggerEvent = true;
        state = GUIButton::State::Highlighted;
    }
    updateContent();
    if(triggerEvent && onClick) {
        onClick(this);
        return true;
    }
    return false;
}

void GUIButton::handleMouseMove(int mouseX, int mouseY) {
    if(!visible || !enabled) return;
    GUIButton::State prevState = state;
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) {
        state = GUIButton::State::Released;
    }else if(state == GUIButton::State::Pressed)
        return;
    else
        state = GUIButton::State::Highlighted;
    if(prevState != state) updateContent();
}