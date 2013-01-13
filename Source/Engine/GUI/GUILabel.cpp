//
//  GUILabel.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUILabel.h"

GUILabel::GUILabel() {
    type = GUIType_Label;
    font = mainFont;
    color = Color4(0.0, 1.0);
    textAlign = GUITextAlign_Middle;
    sizeAlignment = GUISizeAlignment_All;
    fontHeight = 30;
    height = fontHeight >> 1;
}

GUILabel::~GUILabel() {
    for(unsigned int i = 0; i < lines.size(); i ++)
        glDeleteTextures(1, &lines[i].texture);
    lines.clear();
}

void GUILabel::updateContent() {
    int prevWidth = width, prevHeight = height;
    width = height = 0;
    
    for(unsigned int i = 0; i < lines.size(); i ++)
        glDeleteTextures(1, &lines[i].texture);
    lines.clear();
    
    const char *startPos = text.c_str(), *lastPos = startPos, *endPos = startPos+text.size();
    for(const char* pos = startPos; pos < endPos; pos ++)
        if(pos == endPos-1 || pos[0] == '\n') {
            if(lastPos != startPos) lastPos ++;
            if(pos == endPos-1) pos ++;
            GUILabelLine line;
            line.text = text.substr(lastPos-startPos, pos-lastPos);
            if(line.text.size() > 0) {
                line.texture = font->renderStringToTexture(line.text.c_str(), color, true, line.content.width, line.content.height);
                line.content.width = fontHeight*0.5/line.content.height*line.content.width;
                width = fmax(width, line.content.width);
            }else{
                line.texture = NULL;
                line.content.width = 0;
            }
            line.content.height = fontHeight>>1;
            lines.push_back(line);
            height += line.content.height;
            lastPos = pos;
        }
    
    if((sizeAlignment & GUISizeAlignment_Width) == 0) width = prevWidth;
    if((sizeAlignment & GUISizeAlignment_Height) == 0) height = prevHeight;
    
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
    for(unsigned int i = 0; i < lines.size(); i ++) {
        line = &lines[i];
        
        if(!line->texture) continue;
        modelMat.setIdentity();
        modelMat.setOrigin(transform);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, line->texture);
        line->content.drawOnScreen(transform, line->posX, line->posY, parentClipRect);
    }
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
    posX = (float)fontHeight/height*posX+line->posX-line->content.width;
    return;
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