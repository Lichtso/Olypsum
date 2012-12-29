//
//  GUITextField.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 13.03.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUITextField.h"
#import "LevelManager.h"
#import "AppMain.h"

GUITextField::GUITextField() {
    type = GUIType_TextField;
    texture = 0;
    highlighted = false;
    enabled = true;
    cursorDrawTick = 0.0;
    cursorIndexX = -1;
    label = new GUILabel();
    label->width = 0;
    height = 5 + (label->fontHeight >> 1);
}

GUITextField::~GUITextField() {
    if(texture)
        glDeleteTextures(1, &texture);
    delete label;
}

void GUITextField::insertStr(const char* str) {
    label->text = label->text.substr(0, cursorIndexX)+str+label->text.substr(cursorIndexX);
    cursorIndexX += strlen(str);
    cursorDrawTick = 0.0;
    label->updateContent();
    if(onChange) onChange(this);
}

void GUITextField::removeChar() {
    if(cursorIndexX == 0) return;
    moveCursorLeft();
    unsigned char len = getNextCharSize(&label->text[cursorIndexX]);
    label->text = label->text.substr(0, cursorIndexX)+label->text.substr(cursorIndexX+len);
    cursorDrawTick = 0.0;
    label->updateContent();
    if(onChange) onChange(this);
}

void GUITextField::moveCursorLeft() {
    if(cursorIndexX == 0) return;
    while(label->text[-- cursorIndexX] >> 7)
        if((label->text[cursorIndexX] & 0xC0) == 0xC0)
            break;
    cursorDrawTick = 0.0;
}

void GUITextField::moveCursorRight() {
    if(cursorIndexX == label->text.size()) return;
    cursorIndexX += getNextCharSize(&label->text[cursorIndexX]);
    cursorDrawTick = 0.0;
}

void GUITextField::setFirstResponderStatus() {
    GUIRect::setFirstResponderStatus();
    cursorDrawTick = 0.0;
    cursorIndexX = 0;
    updateContent();
    if(onFocus) onFocus(this);
}

void GUITextField::removeFirstResponderStatus() {
    GUIRect::removeFirstResponderStatus();
    cursorIndexX = -1;
    updateContent();
    if(onBlur) onBlur(this);
}

void GUITextField::updateContent() {
    if(texture) glDeleteTextures(1, &texture);
    
    GUIRoundedRect roundedRect;
    roundedRect.texture = &texture;
    roundedRect.width = width;
    roundedRect.height = height;
    roundedRect.cornerRadius = 8;
    if(highlighted) {
        roundedRect.borderColor = Color4(0.31, 0.51, 1.0);
    }else{
        roundedRect.borderColor = Color4(0.51);
    }
    roundedRect.drawInTexture();
}

void GUITextField::draw(btVector3 transform, GUIClipRect& parentClipRect) {
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
    
    if(!clipRect.getLimSize(posX, posY, width-7, height, parentClipRect)) return;
    
    int cursorPosX, cursorPosY;
    bool cursorActive = isFirstResponder();
    if(cursorActive) {
        if(fmod(cursorDrawTick, 0.25) >= 0.2) {
            if(keyState[SDLK_BACKSPACE])
                removeChar();
            else if(keyState[SDLK_LEFT])
                moveCursorLeft();
            else if(keyState[SDLK_RIGHT])
                moveCursorRight();
        }
        
        if(cursorIndexX < 0) cursorIndexX = 0;
        else if(cursorIndexX > label->text.size()) cursorIndexX = (int)label->text.size();
        label->getPosOfChar(cursorIndexX, 0, cursorPosX, cursorPosY);
        label->posX = -max(width-label->width-8, cursorPosX-width+8);
        
        cursorDrawTick += animationFactor;
        if(cursorDrawTick > 0.5) cursorDrawTick = 0.0;
    }else
        label->posX = label->width-width+8;
    
    label->draw(transform, clipRect);
    
    if(cursorActive && cursorDrawTick <= 0.25) {
        modelMat.setIdentity();
        modelMat.setOrigin(transform);
        GUIRoundedRect cursor;
        cursor.width = 1;
        cursor.height = label->fontHeight >> 1;
        cursor.texture = NULL;
        cursor.drawOnScreen(false, cursorPosX+label->posX, 0, clipRect);
    }
}

bool GUITextField::handleMouseDown(int mouseX, int mouseY) {
    GUIScreenView* screenView = (GUIScreenView*)getRootParent();
    
    if(!visible || !enabled || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) {
        if(!screenView || screenView->firstResponder != this) return false;
        removeFirstResponderStatus();
        return false;
    }
    
    if(!screenView) return true;
    if(screenView->firstResponder != this)
        setFirstResponderStatus();
    
    mouseX -= label->posX;
    
    int cursorPosXa, cursorPosXb, cursorPosY;
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
                setClipboardText(label->text);
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
            removeFirstResponderStatus();
        break;
        case SDLK_UP:
            cursorIndexX = 0;
            cursorDrawTick = 0.0;
        break;
        case SDLK_DOWN:
            cursorIndexX = (int)label->text.size();
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