//
//  LeapMotion.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 11.07.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "LeapMotion.h"

LeapManager::LeapManager() {
    controller.addListener(*this);
}

LeapManager::~LeapManager() {
    controller.removeListener(*this);
}

void LeapManager::onConnect(const Leap::Controller& controller) {
    log(info_log, "Leap Motion Controller connected.");
}

void LeapManager::onDisconnect(const Leap::Controller& controller) {
    log(info_log, "Leap Motion Controller disconnected.");
}

void LeapManager::gameTick() {
    if(!controller.isConnected()) return;
    
    const Leap::Frame frame = controller.frame();
    if(!frame.isValid() || frame.hands().count() == 0 || lastFrameID == frame.id()) return;
    lastFrameID = frame.id();
    
    const Leap::ScreenList screens = controller.locatedScreens();
    const Leap::Screen screen = screens[0];
    
    if(screen.bottomLeftCorner() == Leap::Vector(-200, 50, -200) &&
       screen.horizontalAxis() == Leap::Vector(400, 0, 0) &&
       screen.verticalAxis() == Leap::Vector(0, 250, 0) &&
       screen.normal() == Leap::Vector(0, 0, 1)) {
        //printf("Leap needs calibration\n");
    }else{
        const Leap::Pointable pointable = frame.pointables().frontmost();
        if(pointable.isValid()) {
            Leap::Vector position = screen.intersect(pointable.stabilizedTipPosition(), pointable.direction(), true);
            position.x *= screen.widthPixels();
            position.y *= screen.heightPixels();
            
            switch(pointable.touchZone()) {
                case Leap::Pointable::Zone::ZONE_HOVERING:
                    menu.handleMouseMove(position.x, position.y);
                    break;
                case Leap::Pointable::Zone::ZONE_TOUCHING:
                    menu.handleMouseDown(position.x, position.y, SDL_BUTTON_LEFT);
                    break;
                default:
                    break;
            }
        }
    }
    
    
}

LeapManager leapManager;