//
//  GUITextField.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 13.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUITextField.h"

GUITextField::GUITextField() {
    type = GUIType_TextField;
    texture = 0;
    highlighted = false;
    enabled = true;
    onFocus = onChange = onBlur = NULL;
    label = new GUILabel();
    label->autoSize = false;
    label->textAlign = GUITextAlign_Left;
    height = 5 + (label->fontHeight >> 1);
}

GUITextField::~GUITextField() {
    if(texture)
        glDeleteTextures(1, &texture);
    delete label;
}

bool GUITextField::isFirstResponder() {
    GUIScreenView* screenView = (GUIScreenView*)getRootParent();
    if(!screenView || screenView->type != GUIType_ScreenView) return false;
    return (screenView->firstResponder == this);
}

void GUITextField::updateContent() {
    if(texture) glDeleteTextures(1, &texture);
    
    label->width = width-10;
    label->height = (label->fontHeight >> 1);
    
    GUIRoundedRect roundedRect;
    roundedRect.texture = &texture;
    roundedRect.width = width;
    roundedRect.height = height;
    roundedRect.cornerRadius = 8;
    if(isFirstResponder()) {
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
    
    label->draw(modelMat, clipRect);
}

bool GUITextField::handleMouseDown(int mouseX, int mouseY) {
    GUIScreenView* screenView = (GUIScreenView*)getRootParent();
    
    if(!visible || !enabled || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) {
        if(!screenView || screenView->firstResponder != this) return false;
        updateFirstResponderStatus();
        screenView->firstResponder = NULL;
        updateContent();
        return false;
    }
    
    if(!screenView) return true;
    if(screenView->firstResponder != this) {
        if(screenView->firstResponder)
            updateFirstResponderStatus();
        screenView->firstResponder = this;
    }else{
        
    }
    
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
    if(key->sym == SDLK_BACKSPACE) {
        if(label->text.size() == 0) return true;
        label->text = label->text.substr(0, label->text.size()-1);
        label->updateContent();
    }else if(key->sym == SDLK_TAB || key->sym == SDLK_ESCAPE) {
        GUIScreenView* screenView = (GUIScreenView*)getRootParent();
        if(!screenView || screenView->firstResponder != this) return false;
        updateFirstResponderStatus();
        screenView->firstResponder = NULL;
        updateContent();
    }else if((key->unicode & 0xFF00) == 0 && (key->unicode & 0x00FF) > 0) {
        char str[] = { (char)(key->unicode & 0xFF), 0 };
        label->text += str;
        label->updateContent();
    }
    return true;
}

bool GUITextField::handleKeyUp(SDL_keysym* key) {
    
    return true;
}