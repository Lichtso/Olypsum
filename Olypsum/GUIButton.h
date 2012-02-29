//
//  GUIButton.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUILabel.h"

#ifndef GUIButton_h
#define GUIButton_h

class GUIButton : public GUIView {
    public:
    bool autoSize;
    void handleMouseDown(Vector3 relativePos);
    void handleMouseUp(Vector3 relativePos);
    void handleMouseMove(Vector3 relativePos);
    void draw(Matrix4& parentTransform);
};

#endif