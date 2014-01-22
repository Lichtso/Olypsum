//
//  GUILabel.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Menu.h"

GUILabel::GUILabel() :color(Color4(0.0, 1.0)), textAlignment(TextAlignment::Middle), sizeAlignment(GUISizeAlignment::All) {
    font = fileManager.getResourceByPath<TextFont>(NULL, "Core/font");
    fontHeight = menu.screenView->height*0.04;
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
        line.texture = 0;
        line.scale = 0.0;
        line.content.width = 0;
    }
    line.content.height = 0.5*fontHeight;
    lines.push_back(line);
    newHeight += line.content.height;
}

void GUILabel::addLine(unsigned int& newWidth, unsigned int& newHeight, std::string text) {
    if(sizeAlignment & GUISizeAlignment::Width)
        return addSegment(newWidth, newHeight, text);
    
    if(text.size() == 0)
        addSegment(newWidth, newHeight, "");
    
    while(text.size() > 0) {
        unsigned int length = max(1U, getCharCountThatFitsIn(width << 1, text));
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
    
    if(sizeAlignment & GUISizeAlignment::Width) width = newWidth;
    if(sizeAlignment & GUISizeAlignment::Height) height = newHeight;
    
    GUILabelLine* line;
    unsigned int renderY = height-(fontHeight>>1);
    for(unsigned int i = 0; i < lines.size(); i ++) {
        line = &lines[i];
        switch(textAlignment) {
            case TextAlignment::Left:
                line->posX = line->content.width-width;
                break;
            case TextAlignment::Middle:
                line->posX = 0;
                break;
            case TextAlignment::Right:
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

void GUILabel::getPosOfChar(unsigned int charIndex, unsigned int lineIndex, int& posX, int& posY) {
    if(lineIndex >= lines.size()) {
        posY = 0;
        switch(textAlignment) {
            case TextAlignment::Left:
                posX = -width;
            return;
            case TextAlignment::Middle:
                posX = 0;
            return;
            case TextAlignment::Right:
                posX = width;
            return;
        }
    }
    GUILabelLine* line = &lines[lineIndex];
    posY = line->posY;
    std::string str = line->text.substr(0, charIndex);
    int height;
    font->calculateTextSize(str.c_str(), posX, height);
    posX = line->scale*posX+line->posX-line->content.width;
    return;
}

unsigned int GUILabel::getCharCountThatFitsIn(unsigned int warpWidth, const std::string& text) {
    int l = 0.5+text.size()/2.0, w, h;
    for(unsigned int size = l; size >= 1; size >>= 1) {
        std::string segment = text.substr(0, l);
        font->calculateTextSize(segment.c_str(), w, h);
        if((float)fontHeight/h*w > warpWidth)
            l -= size;
        else
            l += size;
        if(l >= text.size())
            return text.size();
    }
    return l;
}

unsigned int GUILabel::getLength() {
    return toUTF8Length(text.c_str(), text.length());
}

unsigned char getNextCharSize(const char* str) {
    if((*str & 0xC0) != 0xC0) return 1;
    unsigned char length = 1;
    while((str[length] & 0xC0) == 0x80)
        length ++;
    return length;
}

unsigned int toUTF8Length(const char* str, unsigned int until) {
    unsigned char byteIndex = 0, length = 0;
    while(byteIndex < until)
        if((str[byteIndex ++] & 0xC0) != 0x80)
            length ++;
    return length;
}

unsigned int fromUTF8Length(const char* str, unsigned int until) {
    unsigned char byteIndex = 0, length = 0;
    while(length < until)
        if((str[byteIndex ++] & 0xC0) != 0x80)
            length ++;
    return byteIndex;
}