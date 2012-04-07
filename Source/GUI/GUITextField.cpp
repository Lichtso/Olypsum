//
//  GUITextField.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 13.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "InputController.h"

GUITextField::GUITextField() {
    type = GUIType_TextField;
    texture = 0;
    highlighted = false;
    enabled = true;
    cursorDrawTick = 0;
    cursorIndexX = -1;
    onFocus = onChange = onBlur = NULL;
    label = new GUILabel();
    label->width = 0;
    height = 5 + (label->fontHeight >> 1);
}

GUITextField::~GUITextField() {
    if(texture)
        glDeleteTextures(1, &texture);
    delete label;
}

void GUITextField::removeChar() {
    if(cursorIndexX == 0) return;
    label->text = label->text.substr(0, cursorIndexX-1)+label->text.substr(cursorIndexX);
    cursorIndexX --;
    cursorDrawTick = 0;
    label->updateContent();
}

void GUITextField::moveCursorLeft() {
    if(cursorIndexX == 0) return;
    cursorIndexX --;
    cursorDrawTick = 0;
}

void GUITextField::moveCursorRight() {
    if(cursorIndexX == label->text.size()) return;
    cursorIndexX ++;
    cursorDrawTick = 0;
}

void GUITextField::removeFirstResponderStatus() {
    cursorDrawTick = 0;
    cursorIndexX = -1;
    updateContent();
}

void GUITextField::updateContent() {
    if(texture) glDeleteTextures(1, &texture);
    
    GUIRoundedRect roundedRect;
    roundedRect.texture = &texture;
    roundedRect.width = width;
    roundedRect.height = height;
    roundedRect.cornerRadius = 8;
    if(highlighted) {
        roundedRect.borderColor.r = 80;
        roundedRect.borderColor.g = 130;
        roundedRect.borderColor.b = 255;
    }else{
        roundedRect.borderColor.r = roundedRect.borderColor.g = roundedRect.borderColor.b = 130;
    }
    roundedRect.drawInTexture();
}

void GUITextField::draw(Matrix4& parentTransform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    if(!texture) updateContent();
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    
    GUIRoundedRect roundedRect;
    roundedRect.texture = &texture;
    roundedRect.width = width;
    roundedRect.height = height;
    roundedRect.drawOnScreen(false, 0, 0, clipRect);
    
    if(!clipRect.getLimSize(posX, posY, width-7, height, parentClipRect)) return;
    
    int cursorPosX, cursorPosY;
    bool cursorActive = isFirstResponder();
    if(cursorActive) {
        if(cursorDrawTick % 10 == 9) {
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
        
        cursorDrawTick ++;
        if(cursorDrawTick > 60)
            cursorDrawTick = 0;
    }else
        label->posX = label->width-width+8;
    
    label->draw(modelMat, clipRect);
    
    if(cursorActive && cursorDrawTick <= 30) {
        modelMat = parentTransform;
        modelMat.translate(Vector3(posX, posY, 0.0));
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
        screenView->firstResponder = NULL;
        removeFirstResponderStatus();
        return false;
    }
    
    if(!screenView) return true;
    if(screenView->firstResponder != this) {
        if(screenView->firstResponder)
            screenView->firstResponder->removeFirstResponderStatus();
        screenView->firstResponder = this;
    }
    
    unsigned int minIndex = 0, maxIndex = (unsigned int)label->text.size();
    int cursorPosX, cursorPosY;
    mouseX -= label->posX;
    while(true) {
        if(maxIndex-minIndex <= 1) {
            int diffMin, diffMax;
            label->getPosOfChar(minIndex, 0, cursorPosX, cursorPosY);
            diffMin = mouseX-cursorPosX;
            label->getPosOfChar(maxIndex, 0, cursorPosX, cursorPosY);
            diffMax = cursorPosX-mouseX;
            cursorIndexX = (diffMin < diffMax) ? minIndex : maxIndex;
            break;
        }
        cursorIndexX = minIndex+((maxIndex-minIndex)>>1);
        label->getPosOfChar(cursorIndexX, 0, cursorPosX, cursorPosY);
        if(mouseX >= cursorPosX) {
            minIndex = cursorIndexX;
        }else{
            maxIndex = cursorIndexX;
        }
    }
    cursorDrawTick = 0;
    updateContent();
    return true;
}

void GUITextField::handleMouseUp(int mouseX, int mouseY) {
    
}

void GUITextField::handleMouseMove(int mouseX, int mouseY) {
    if(!visible || !enabled) return;
    
    bool prevHighlighted = highlighted;
    highlighted = mouseX >= -width && mouseX <= width && mouseY >= -height && mouseY <= height;
    if(prevHighlighted != highlighted) updateContent();
}

bool GUITextField::handleKeyDown(SDL_keysym* key) {
    switch(key->sym) {
        case SDLK_TAB:
        case SDLK_RETURN:
        case SDLK_ESCAPE: {
            GUIScreenView* screenView = (GUIScreenView*)getRootParent();
            if(!screenView || screenView->firstResponder != this) return false;
            screenView->firstResponder = NULL;
            removeFirstResponderStatus();
        } break;
        case SDLK_UP:
            cursorIndexX = 0;
            cursorDrawTick = 0;
        break;
        case SDLK_DOWN:
            cursorIndexX = (int)label->text.size();
            cursorDrawTick = 0;
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
            if((key->unicode & 0xFF00) != 0 || (key->unicode & 0x00FF) == 0) return false;
            char str[] = { (char)(key->unicode & 0xFF), 0 };
            label->text = label->text.substr(0, cursorIndexX)+str+label->text.substr(cursorIndexX);
            cursorIndexX ++;
            cursorDrawTick = 0;
            label->updateContent();
        } break;
    }
    return true;
}

bool GUITextField::handleKeyUp(SDL_keysym* key) {
    
    return true;
}