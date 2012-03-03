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

class GUIButtonList : public GUIView {
    public:
    void addChild(GUIButton* child);
    bool autoSize, vertical;
    GUIButtonList();
    void updateContent();
    void draw(Matrix4& parentTransform, GUIClipRect* parentClipRect);
};

#endif
