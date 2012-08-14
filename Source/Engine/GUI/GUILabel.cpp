//
//  GUILabel.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUILabel.h"

GUILabel::GUILabel() {
    type = GUIType_Label;
    font = mainFont;
    color = GUIColor(0, 255);
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
            line.texture = font->renderStringToTexture(line.text.c_str(), color, true, line.width, line.height);
            line.width = fontHeight*0.5/line.height*line.width;
            line.height = fontHeight>>1;
            lines.push_back(line);
            width = fmax(width, line.width);
            height += line.height;
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
                line->posX = line->width-width;
                break;
            case GUITextAlign_Middle:
                line->posX = 0;
                break;
            case GUITextAlign_Right:
                line->posX = width-line->width;
                break;
        }
        line->posY = renderY;
        renderY -= fontHeight;
    }
}

void GUILabel::draw(Matrix4& parentTransform, GUIClipRect& parentClipRect) {
    if(!visible || text.size() == 0 || fontHeight == 0) return;
    if(lines.size() == 0) updateContent();
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    
    GUILabelLine* line;
    float minFactorX, minFactorY, maxFactorX, maxFactorY;
    for(unsigned int i = 0; i < lines.size(); i ++) {
        line = &lines[i];
        
        line->clipRect.minPosX = max(clipRect.minPosX, line->posX-(int)line->width);
        line->clipRect.minPosY = max(clipRect.minPosY, line->posY-(int)line->height);
        line->clipRect.maxPosX = min(clipRect.maxPosX, line->posX+(int)line->width);
        line->clipRect.maxPosY = min(clipRect.maxPosY, line->posY+(int)line->height);
        
        if(line->clipRect.minPosX > line->clipRect.maxPosX || line->clipRect.minPosY > line->clipRect.maxPosY) continue;
        minFactorX = 0.5+0.5*(line->clipRect.minPosX-line->posX)/line->width;
        minFactorY = 0.5-0.5*(line->clipRect.maxPosY-line->posY)/line->height;
        maxFactorX = 0.5+0.5*(line->clipRect.maxPosX-line->posX)/line->width;
        maxFactorY = 0.5-0.5*(line->clipRect.minPosY-line->posY)/line->height;
        
        float vertices[] = {
            (float)line->clipRect.maxPosX, (float)line->clipRect.minPosY,
            maxFactorX, maxFactorY,
            (float)line->clipRect.maxPosX, (float)line->clipRect.maxPosY,
            maxFactorX, minFactorY,
            (float)line->clipRect.minPosX, (float)line->clipRect.maxPosY,
            minFactorX, minFactorY,
            (float)line->clipRect.minPosX, (float)line->clipRect.minPosY,
            minFactorX, maxFactorY
        };
        
        shaderPrograms[spriteSP]->use();
        shaderPrograms[spriteSP]->setAttribute(POSITION_ATTRIBUTE, 2, 4*sizeof(float), vertices);
        shaderPrograms[spriteSP]->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, 4*sizeof(float), &vertices[2]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lines[i].texture);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}

unsigned char GUILabel::getUTF8Length(unsigned int pos) {
    unsigned char len = 1;
    if((text[pos ++] & 0xC0) == 0xC0)
        while((text[pos ++] & 0xC0) == 0x80)
            len ++;
    return len;
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
    posX = (float)fontHeight/height*posX+line->posX-line->width;
    return;
}