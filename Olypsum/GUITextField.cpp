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
    delete label;
}

bool GUITextField::isFirstResponder() {
    GUIScreenView* screenView = (GUIScreenView*)getRootParent();
    if(!screenView || screenView->type != GUIType_ScreenView) return false;
    return (screenView->firstResponder == this);
}

void GUITextField::updateContent() {
    if(texture) glDeleteTextures(1, &texture);
    
    label->width = width;
    label->height = label->fontHeight >> 1;
    
    GUIRoundedRect roundedRect;
    roundedRect.texture = &texture;
    roundedRect.width = width;
    roundedRect.height = height;
    roundedRect.cornerRadius = 8;
    roundedRect.drawInTexture();
}

void GUITextField::draw(Matrix4& parentTransform, GUIClipRect* parentClipRect) {
    if(!visible) return;
    if(!texture) updateContent();
    
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    GUIClipRect clipRect;
    getLimSize(parentClipRect, &clipRect);
    
    GUIRoundedRect roundedRect;
    roundedRect.texture = &texture;
    roundedRect.width = width;
    roundedRect.height = height;
    roundedRect.drawOnScreen(clipRect);
    
    label->draw(modelMat, &clipRect);
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
    
    return true;
}

bool GUITextField::handleKeyUp(SDL_keysym* key) {
    
    return true;
}