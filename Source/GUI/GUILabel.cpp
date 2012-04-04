//
//  GUILabel.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUILabel.h"

GUILabel::GUILabel() {
    type = GUIType_Label;
    font = mainFont;
    color.r = color.g = color.b = 0;
    color.unused = 255;
    textAlign = GUITextAlign_Middle;
    autoSize = true;
    fontHeight = 30;
}

GUILabel::~GUILabel() {
    for(unsigned int i = 0; i < lines.size(); i ++)
        glDeleteTextures(1, &lines[i].texture);
    lines.clear();
}

void GUILabel::updateContent() {
    if(autoSize)
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
            if(autoSize) {
                width = fmax(width, line.width);
                height += line.height;
            }
            lastPos = pos;
        }
    
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
            line->clipRect.maxPosX, line->clipRect.minPosY,
            maxFactorX, maxFactorY,
            line->clipRect.maxPosX, line->clipRect.maxPosY,
            maxFactorX, minFactorY,
            line->clipRect.minPosX, line->clipRect.maxPosY,
            minFactorX, minFactorY,
            line->clipRect.minPosX, line->clipRect.minPosY,
            minFactorX, maxFactorY
        };
        
        spriteShaderProgram->use();
        spriteShaderProgram->setAttribute(VERTEX_ATTRIBUTE, 2, 4*sizeof(float), vertices);
        spriteShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, 4*sizeof(float), &vertices[2]);
        glBindTexture(GL_TEXTURE_2D, lines[i].texture);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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
    TTF_SizeText(font->ttf, str.c_str(), &posX, &height);
    posX = (float)fontHeight/height*posX+line->posX-line->width;
    return;
}