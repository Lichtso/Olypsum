//
//  LeapMotion.h
//  Olypsum
//
//  Created by Alexander Meißner on 11.07.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef LeapMotion_h
#define LeapMotion_h

#include "Menu.h"

class LeapManager : public Leap::Listener {
    int64_t lastFrameID = -1;
    int prevMouseX, prevMouseY, prevZone, mouseX, mouseY, zone;
    public:
    Leap::Controller controller;
    LeapManager();
    ~LeapManager();
    /*void onInit(const Leap::Controller& controller);
    void onExit(const Leap::Controller& controller);
    void onFrame(const Leap::Controller& controller);
    void onFocusGained(const Leap::Controller& controller);
    void onFocusLost(const Leap::Controller& controller);*/
    void onConnect(const Leap::Controller& controller);
    void onDisconnect(const Leap::Controller& controller);
    void gameTick();
};

extern LeapManager leapManager;

#endif