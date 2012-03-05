//
//  GUICheckBox.h
//  Olypsum
//
//  Created by Alexander Meißner on 04.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIButton.h"

#ifndef GUICheckBox_h
#define GUICheckBox_h

class GUICheckBox : public GUIButton {
    public:
    GUICheckBox();
    void addChild(GUIRect* child);
    void updateContent();
    bool handleMouseDown(int mouseX, int mouseY);
    void handleMouseUp(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
};

#endif
