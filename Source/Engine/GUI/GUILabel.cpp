//
//  GUILabel.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUILabel.h"

GUILabel::GUILabel() {
    font = mainFont;
    color = Color4(0.0, 1.0);
    textAlign = GUITextAlign_Middle;
    sizeAlignment = GUISizeAlignment_All;
    fontHeight = currentScreenView->height*0.04;
    height = fontHeight >> 1;
}

GUILabel::~GUILabel() {
    for(unsigned int i = 0; i < lines.size(); i ++)
        glDeleteTextures(1, &lines[i].texture);
    lines.clear();
}

void GUILabel::addSegment(unsigned int& newWidth, unsigned int& newHeight, std::string text) {
    GUILabelLine line;
    if(text.size() > 0) {
        line.text = text;
        line.texture = font->renderStringToTexture(line.text.c_str(), color, true, line.content.width, line.content.height);
        line.scale = (float)fontHeight/line.content.height;
        line.content.width = 0.5*line.scale*line.content.width;
        newWidth = fmax(newWidth, line.content.width);
    }else{
        line.texture = NULL;
        line.scale = 0.0;
        line.content.width = 0;
    }
    line.content.height = 0.5*fontHeight;
    lines.push_back(line);
    newHeight += line.content.height;
}

void GUILabel::addLine(unsigned int& newWidth, unsigned int& newHeight, std::string text) {
    if(sizeAlignment & GUISizeAlignment_Width)
        return addSegment(newWidth, newHeight, text);
    
    while(text.size() > 0) {
        unsigned int length = max(1U, getCharCountThatFitsIn(width, text));
        addSegment(newWidth, newHeight, text.substr(0, length));
        text = text.substr(length);
    }
}

void GUILabel::updateContent() {
    unsigned int newWidth = 0, newHeight = 0;
    
    for(unsigned int i = 0; i < lines.size(); i ++)
        glDeleteTextures(1, &lines[i].texture);
    lines.clear();
    
    unsigned int prevPos = 0, length = text.size();
    for(unsigned int pos = 0; pos < length; pos ++)
        if(text[pos] == '\n') {
            addLine(newWidth, newHeight, text.substr(prevPos, pos-prevPos));
            prevPos = pos+1;
        }
    addLine(newWidth, newHeight, text.substr(prevPos, length-prevPos));
    
    if(sizeAlignment & GUISizeAlignment_Width) width = newWidth;
    if(sizeAlignment & GUISizeAlignment_Height) height = newHeight;
    
    GUILabelLine* line;
    unsigned int renderY = height-(fontHeight>>1);
    for(unsigned int i = 0; i < lines.size(); i ++) {
        line = &lines[i];
        switch(textAlign) {
            case GUITextAlign_Left:
                line->posX = line->content.width-width;
                break;
            case GUITextAlign_Middle:
                line->posX = 0;
                break;
            case GUITextAlign_Right:
                line->posX = width-line->content.width;
                break;
        }
        line->posY = renderY;
        renderY -= fontHeight;
    }
}

void GUILabel::draw(btVector3 transform, GUIClipRect& parentClipRect) {
    if(!visible || text.size() == 0 || fontHeight == 0) return;
    if(lines.size() == 0) updateContent();
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    transform += btVector3(posX, posY, 0.0);
    
    GUILabelLine* line;
    shaderPrograms[spriteSP]->setUniformF("alpha", color.a);
    for(unsigned int i = 0; i < lines.size(); i ++) {
        line = &lines[i];
        
        if(!line->texture) continue;
        modelMat.setIdentity();
        modelMat.setOrigin(transform);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, line->texture);
        line->content.drawOnScreen(transform, line->posX, line->posY, clipRect);
    }
    shaderPrograms[spriteSP]->setUniformF("alpha", 1.0);
}

bool GUILabel::handleMouseDown(int mouseX, int mouseY) {
    return false;
}

void GUILabel::getPosOfChar(unsigned int charIndex, unsigned int lineIndex, int& posX, int& posY) {
    if(lineIndex >= lines.size()) {
        posY = 0;
        switch(textAlign) {
            case GUITextAlign_Left:
                posX = -width;
            return;
            case GUITextAlign_Middle:
                posX = 0;
            return;
            case GUITextAlign_Right:
                posX = width;
            return;
        }
    }
    GUILabelLine* line = &lines[lineIndex];
    posY = line->posY;
    std::string str = line->text.substr(0, charIndex);
    int height;
    TTF_SizeUTF8(font->ttf, str.c_str(), &posX, &height);
    posX = line->scale*posX+line->posX-line->content.width;
    return;
}

unsigned int GUILabel::getCharCountThatFitsIn(unsigned int warpWidth, const std::string& text) {
    int l = text.size()/2, w, h;
    for(unsigned int size = l; size >= 1; size >>= 1) {
        std::string segment = text.substr(0, l);
        TTF_SizeUTF8(font->ttf, segment.c_str(), &w, &h);
        if(0.5*fontHeight/h*w > warpWidth)
            l -= size;
        else
            l += size;
        if(l > text.size())
            return text.size();
    }
    return l;
}

unsigned char getNextCharSize(const char* str) {
    if((*str & 0xC0) != 0xC0) return 1;
    unsigned char length = 1;
    while((str[length] & 0xC0) == 0x80)
        length ++;
    return length;
}

unsigned int getUTF8Length(const char* str) {
    unsigned char byteIndex = 0, length = 0;
    while(byteIndex < strlen(str))
        if((str[byteIndex ++] & 0xC0) != 0x80)
            length ++;
    return length;
}