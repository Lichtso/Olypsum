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
    color.r = color.g = color.b = 255;
    color.unused = 255;
    textAlign = GUITextAlign_Middle;
    autoSize = true;
    fontHeight = 50;
}

void GUILabel::recalculateSize() {
    if(!autoSize) return;
    width = height = 0;
    int w, h;
    const char *startPos = text.c_str(), *lastPos = startPos, *endPos = startPos+text.size();
    for(const char* pos = startPos; pos < endPos; pos ++) {
        if(pos[0] != '\\' && pos < endPos-1 && pos[1] != 'n') continue;
        std::string line = text.substr(startPos-lastPos, pos-lastPos);
        TTF_SizeText(font->ttf, text.c_str(), &w, &h);
        width = fmax(width, 0.5*fontHeight/(float)h*w);
        height += fontHeight>>1;
        lastPos = pos;
    }
}

struct LabelLine {
    GLuint texture;
    int posX, posY;
    unsigned int width, height;
    GUIClipRect clipRect;
};

void GUILabel::draw(Matrix4& parentTransform) {
    std::vector<LabelLine> lines;
    
    width = height = 0;
    const char *startPos = text.c_str(), *lastPos = startPos, *endPos = startPos+text.size();
    for(const char* pos = startPos; pos < endPos; pos ++)
        if(pos == endPos-1 || pos[0] == '\n') {
            if(lastPos != startPos) lastPos ++;
            if(pos == endPos-1) pos ++;
            std::string lineStr = text.substr(lastPos-startPos, pos-lastPos);
            LabelLine line;
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
    
    GUIClipRect clipRect;
    getLimSize(clipRect);
    modelMat = parentTransform;
    modelMat.translate(Vector3(posX, posY, 0.0));
    
    LabelLine* line;
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
        minFactorY = 0.5+0.5*(line->clipRect.minPosY-line->posY)/line->height;
        maxFactorX = 0.5+0.5*(line->clipRect.maxPosX-line->posX)/line->width;
        maxFactorY = 0.5+0.5*(line->clipRect.maxPosY-line->posY)/line->height;
        
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
        glDeleteTextures(1, &lines[i].texture);
    }
}