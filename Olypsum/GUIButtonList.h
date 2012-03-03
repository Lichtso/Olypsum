//
//  GUIButtonList.h
//  Olypsum
//
//  Created by Alexander Meißner on 03.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIButton.h"

#ifndef GUIButtonList_h
#define GUIButtonList_h

enum GUIOrientation {
    GUIOrientationLeft = 1,
    GUIOrientationRight = 2,
    GUIOrientationTop = 4,
    GUIOrientationBottom = 8,
    GUIOrientationHorizontal = 3,
    GUIOrientationVertical = 12
};

class GUIButtonList : public GUIView {
    public:
    void addChild(GUIButton* child);
    bool autoSize;
    GUIOrientation orientation;
    GUIButtonList();
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect* parentClipRect);
};

#endif
