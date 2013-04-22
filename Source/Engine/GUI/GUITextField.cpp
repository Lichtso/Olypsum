//
//  GUITextField.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 13.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUITextField.h"
#include "LevelManager.h"
#include "AppMain.h"

GUITextField::GUITextField() {
    highlighted = false;
    enabled = true;
    cursorDrawTick = 0.0;
    cursorIndexX = -1;
    GUILabel* label = new GUILabel();
    addChild(label);
    label->width = 0;
    height = (float)label->fontHeight/1.2;
    GUIFramedView* cursor = new GUIFramedView();
    addChild(cursor);
    cursor->width = 1;
    cursor->visible = false;
    cursor->content.borderColor = Color4(0.0);
    cursor->content.cornerRadius = cursor->content.innerShadow = content.innerShadow = 0;
}

void GUITextField::insertStr(const char* str) {
    cursorDrawTick = 0.0;
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    label->text = label->text.substr(0, cursorIndexX)+str+label->text.substr(cursorIndexX);
    label->updateContent();
    cursorIndexX += strlen(str);
    if(onChange) onChange(this);
}

void GUITextField::removeChar() {
    if(cursorIndexX == 0) return;
    moveCursorLeft();
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    unsigned char len = getNextCharSize(&label->text[cursorIndexX]);
    label->text = label->text.substr(0, cursorIndexX)+label->text.substr(cursorIndexX+len);
    label->updateContent();
    if(onChange) onChange(this);
}

void GUITextField::moveCursorLeft() {
    if(cursorIndexX == 0) return;
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    while(label->text[-- cursorIndexX] >> 7)
        if((label->text[cursorIndexX] & 0xC0) == 0xC0)
            break;
    cursorDrawTick = 0.0;
}

void GUITextField::moveCursorRight() {
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    if(cursorIndexX == label->text.size()) return;
    cursorIndexX += getNextCharSize(&label->text[cursorIndexX]);
    cursorDrawTick = 0.0;
}

void GUITextField::setFocus(bool active) {
    GUIRect::setFocus(active);
    if(active) {
        cursorDrawTick = 0.0;
        cursorIndexX = 0;
        if(onFocus) onFocus(this);
    }else{
        cursorIndexX = -1;
        updateContent();
        if(onBlur) onBlur(this);
    }
    updateContent();
}

void GUITextField::updateContent() {
    content.width = width;
    content.height = height;
    content.cornerRadius = menu.screenView->width*0.014;
    content.borderColor = (highlighted) ? Color4(0.31, 0.51, 1.0) : Color4(0.51);
    content.drawInTexture();
    
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    GUIFramedView* cursor = static_cast<GUIFramedView*>(children[1]);
    label->fontHeight = 1.2*height;
    cursor->height = label->fontHeight >> 1;
    cursor->updateContent();
}

#define GUITextField_CursorDist 8

void GUITextField::draw(btVector3 transform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    
    transform += btVector3(posX, posY, 0.0);
    modelMat.setIdentity();
    modelMat.setOrigin(transform);
    
    content.drawOnScreen(transform, 0, 0, clipRect);
    if(!clipRect.getLimSize(posX, posY, width-GUITextField_CursorDist+1, height, parentClipRect)) return;
    
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    GUIFramedView* cursor = static_cast<GUIFramedView*>(children[1]);
    int cursorPosX, cursorPosY;
    bool cursorActive = getFocus();
    if(cursorActive) {
        if(fmod(cursorDrawTick, 0.25) >= 0.2) {
            if(keyState[SDLK_BACKSPACE])
                removeChar();
            else if(keyState[SDLK_LEFT])
                moveCursorLeft();
            else if(keyState[SDLK_RIGHT])
                moveCursorRight();
        }
        
        cursorDrawTick += profiler.animationFactor;
        if(cursorDrawTick > 0.5) cursorDrawTick = 0.0;
        cursorIndexX = clamp(cursorIndexX, 0, (int)label->text.size());
        label->getPosOfChar(cursorIndexX, 0, cursorPosX, cursorPosY);
        label->posX = -max(width-label->width-GUITextField_CursorDist, cursorPosX-width+GUITextField_CursorDist);
        cursor->posX = label->posX+cursorPosX;
        cursor->visible = (cursorDrawTick <= 0.25);
    }else
        label->posX = label->width-width+GUITextField_CursorDist;
    
    label->draw(transform, clipRect);
    cursor->draw(transform, clipRect);
}

bool GUITextField::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || !enabled || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) {
        setFocus(false);
        return false;
    }else
        setFocus(true);
    
    int cursorPosXa, cursorPosXb, cursorPosY;
    GUILabel* label = static_cast<GUILabel*>(children[0]);
    mouseX -= label->posX;
    label->getPosOfChar(0, 0, cursorPosXa, cursorPosY);
    for(cursorIndexX = 0; cursorIndexX < label->text.size(); cursorPosXa = cursorPosXb) {
        unsigned int len = getNextCharSize(&label->text[cursorIndexX]);
        label->getPosOfChar(cursorIndexX+len, 0, cursorPosXb, cursorPosY);
        if(cursorPosXa-mouseX <= 0 && cursorPosXb-mouseX >= 0) {
            if(mouseX-cursorPosXa > cursorPosXb-mouseX)
                cursorIndexX += len;
            break;
        }
        cursorIndexX += len;
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

bool GUITextField::handleKeyDown(SDL_keysym* key) {
    if(keyState[SDLK_LMETA] || keyState[SDLK_RMETA]) {
        switch(key->sym) {
            case SDLK_c:
                setClipboardText(static_cast<GUILabel*>(children[0])->text.c_str());
                break;
            case SDLK_v: {
                if(!hasClipboardText()) break;
                insertStr(getClipboardText().c_str());
            } break;
            default:
                
                break;
        }
        return true;
    }
    
    switch(key->sym) {
        case SDLK_TAB:
        case SDLK_RETURN:
        case SDLK_ESCAPE:
            setFocus(false);
        break;
        case SDLK_UP:
            cursorIndexX = 0;
            cursorDrawTick = 0.0;
        break;
        case SDLK_DOWN:
            cursorIndexX = static_cast<GUILabel*>(children[0])->text.size();
            cursorDrawTick = 0.0;
        break;
        case SDLK_BACKSPACE:
            removeChar();
        break;
        case SDLK_LEFT:
            moveCursorLeft();
        break;
        case SDLK_RIGHT:
            moveCursorRight();
        break;
        default: {
            char str[] = { (char)(key->unicode & 0xFF), 0, 0, 0 };
            if(key->unicode > 0x07FF) {
                str[0] = 0xE0 | ((key->unicode >> 12) & 0x0F);
                str[1] = 0x80 | ((key->unicode >> 6) & 0x3F);
                str[2] = 0x80 | (key->unicode & 0x3F);
            }else if(key->unicode > 0x007F) {
                str[0] = 0xC0 | ((key->unicode >> 6) & 0x1F);
                str[1] = 0x80 | (key->unicode & 0x3F);
            }
            insertStr(str);
        } break;
    }
    return true;
}