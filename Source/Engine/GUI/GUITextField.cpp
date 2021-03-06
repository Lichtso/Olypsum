//
//  GUITextField.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 13.03.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "../Menu/Menu.h"

GUITextField::GUITextField() :highlighted(false), enabled(true), cursorX(0), cursorDrawTick(0.0) {
    GUILabel* label = new GUILabel();
    GUIFramedView::addChild(label);
    label->width = 0;
    height = (float)label->fontHeight/1.2;
    cursor.width = 1;
    cursor.borderColor = Color4(0.0);
    cursor.cornerRadius = 0;
    content.decorationType = GUIDecorationType::Stipple;
    content.cornerRadius = menu.screenView->width*0.014;
    content.edgeGradientCenter = 1.0;
    content.edgeGradientBorder = 0.6;
}

void GUITextField::insertStr(const char* str) {
    cursorDrawTick = 0.0;
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    label->text = label->text.substr(0, cursorX)+str+label->text.substr(cursorX);
    label->updateContent();
    cursorX += strlen(str);
    if(onChange) onChange(this);
}

void GUITextField::removeChar() {
    if(cursorX == 0) return;
    moveCursorLeft();
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    unsigned char len = getNextCharSize(&label->text[cursorX]);
    label->text = label->text.substr(0, cursorX)+label->text.substr(cursorX+len);
    label->updateContent();
    if(onChange) onChange(this);
}

void GUITextField::moveCursorLeft() {
    if(cursorX == 0) return;
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    while(label->text[-- cursorX] >> 7)
        if((label->text[cursorX] & 0xC0) == 0xC0)
            break;
    cursorDrawTick = 0.0;
}

void GUITextField::moveCursorRight() {
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    if(cursorX == label->text.size()) return;
    cursorX += getNextCharSize(&label->text[cursorX]);
    cursorDrawTick = 0.0;
}

void GUITextField::setFocused(bool active) {
    GUIRect::setFocused(active);
    if(active) {
        cursorDrawTick = 0.0;
        cursorX = 0;
    }else{
        cursorX = -1;
        updateContent();
    }
    if(onFocus) onFocus(this);
    updateContent();
}

bool GUITextField::addChild(GUIRect* child) {
    return false;
}

void GUITextField::updateContent() {
    content.width = width;
    content.height = height;
    content.borderColor = (isFocused()) ? Color4(0.31, 0.51, 1.0) : Color4(0.51);
    content.updateContent();
    
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    label->fontHeight = 1.2*height;
    cursor.height = label->fontHeight >> 1;
    cursor.updateContent();
}

#define GUITextField_Padding 8

void GUITextField::draw(const btVector3& parentTransform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    
    btVector3 transform = parentTransform + btVector3(posX, posY, 0.0);
    modelMat.setIdentity();
    modelMat.setOrigin(transform);
    
    content.drawOnScreen(transform, 0, 0, clipRect);
    if(!clipRect.getLimSize(posX, posY, width-GUITextField_Padding+1, height, parentClipRect)) return;
    
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    int cursorPosX, cursorPosY;
    bool cursorActive = isFocused();
    if(cursorActive) {
        if(fmod(cursorDrawTick, 0.25) >= 0.2) {
            if(keyState[SDL_SCANCODE_BACKSPACE])
                removeChar();
            else if(keyState[SDL_SCANCODE_LEFT])
                moveCursorLeft();
            else if(keyState[SDL_SCANCODE_RIGHT])
                moveCursorRight();
        }
        
        cursorDrawTick += profiler.animationFactor;
        if(cursorDrawTick > 0.5) cursorDrawTick = 0.0;
        cursorX = clamp(cursorX, 0U, (unsigned int)label->text.size());
        
        label->getPosOfChar(cursorX, 0, cursorPosX, cursorPosY);
        label->posX = -max(width-label->width-GUITextField_Padding, cursorPosX-width+GUITextField_Padding);
        label->draw(transform, clipRect);
        
        if(cursorDrawTick <= 0.25)
            cursor.drawOnScreen(transform, cursorPosX+label->posX, 0, clipRect);
    }else{
        label->posX = label->width-width+GUITextField_Padding;
        label->draw(transform, clipRect);
    }
}

bool GUITextField::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || !enabled || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) {
        setFocused(false);
        return false;
    }else
        setFocused(true);
    
    int cursorPosXa, cursorPosXb, cursorPosY;
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    mouseX -= label->posX;
    label->getPosOfChar(0, 0, cursorPosXa, cursorPosY);
    for(cursorX = 0; cursorX < label->text.size(); cursorPosXa = cursorPosXb) {
        unsigned int len = getNextCharSize(&label->text[cursorX]);
        label->getPosOfChar(cursorX+len, 0, cursorPosXb, cursorPosY);
        if(cursorPosXa-mouseX <= 0 && cursorPosXb-mouseX >= 0) {
            if(mouseX-cursorPosXa > cursorPosXb-mouseX)
                cursorX += len;
            break;
        }
        cursorX += len;
    }
    
    updateContent();
    return true;
}

void GUITextField::handleMouseMove(int mouseX, int mouseY) {
    if(!visible || !enabled) return;
    
    bool prevHighlighted = highlighted;
    highlighted = mouseX >= -width && mouseX <= width && mouseY >= -height && mouseY <= height;
    if(prevHighlighted != highlighted) updateContent();
}

bool GUITextField::handleKeyDown(SDL_Keycode key, const char* text) {
    if(keyState[SDL_SCANCODE_LGUI] || keyState[SDL_SCANCODE_RGUI]) {
        switch(key) {
            case SDLK_c:
                SDL_SetClipboardText(static_cast<GUILabel*>(children[0])->text.c_str());
                return true;
            case SDLK_v:
                if(SDL_HasClipboardText())
                    insertStr(SDL_GetClipboardText());
                return true;
            default:
                return true;
        }
    }
    
    switch(key) {
        case SDLK_TAB:
        case SDLK_RETURN:
        case SDLK_ESCAPE:
            setFocused(false);
            return true;
        case SDLK_UP:
            cursorX = 0;
            cursorDrawTick = 0.0;
            return true;
        case SDLK_DOWN:
            cursorX = static_cast<GUILabel*>(children[0])->text.size();
            cursorDrawTick = 0.0;
            return true;
        case SDLK_BACKSPACE:
            removeChar();
            return true;
        case SDLK_LEFT:
            moveCursorLeft();
            return true;
        case SDLK_RIGHT:
            moveCursorRight();
            return true;
        default:
            insertStr(text);
            return true;
    }
}

void GUITextField::setCursorX(unsigned int newCursorX) {
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    cursorX = fromUTF8Length(label->text.c_str(), min(newCursorX, (unsigned int)label->text.length()));
}

unsigned int GUITextField::getCursorX() {
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    return toUTF8Length(label->text.c_str(), cursorX);
}