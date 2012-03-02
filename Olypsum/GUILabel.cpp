//
//  GUILabel.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUILabel.h"

GUILabel::GUILabel() {
    font = mainFont;
    color.r = color.g = color.b = 0;
    color.unused = 255;
    textAlign = GUITextAlign_Middle;
    autoSize = true;
    fontHeight = 50;
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
            std::string lineStr = text.substr(lastPos-startPos, pos-lastPos);
            GUILabelLine line;
            line.texture = font->renderStringToTexture(lineStr.c_str(), color, true, line.width, line.height);
            line.width = 0.5*fontHeight/(float)line.height*line.width;
            line.height = fontHeight>>1;
            lines.push_back(line);
            if(autoSize) {
                width = fmax(width, line.width);
                height += line.height;
            }
            lastPos = pos;
        }
}

void GUILabel::draw(Matrix4& parentTransform, GUIClipRect* parentClipRect) {
    if(!visible || text.size() == 0 || fontHeight == 0) return;
    
    if(lines.size() == 0)
        updateContent();
    
    GUIClipRect clipRect;
    getLimSize(parentClipRect, &clipRect);
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    
    GUILabelLine* line;
    unsigned int renderY = height-(fontHeight>>1);
    float minFactorX, minFactorY, maxFactorX, maxFactorY;
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