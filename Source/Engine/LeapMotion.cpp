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
    if(menu.current == Menu::inGame) return;
    
    float touches = 0.0;
    zone = Leap::Pointable::Zone::ZONE_NONE;
    prevMouseX = mouseX;
    prevMouseY = mouseY;
    prevZone = zone;
    
    if(controller.isConnected()) {
        const Leap::ScreenList screens = controller.locatedScreens();
        const Leap::Screen screen = screens[0];
        const Leap::Frame frame = controller.frame();
        
        if(frame.isValid() && lastFrameID != frame.id()) {
            lastFrameID = frame.id();
            
            for(int i = 0; i < frame.pointables().count(); i ++) {
                const Leap::Pointable pointable = frame.pointables()[i];
                if(!pointable.isValid() || pointable.touchZone() == Leap::Pointable::Zone::ZONE_NONE) continue;
                Leap::Vector position = screen.intersect(pointable.stabilizedTipPosition(), pointable.direction(), true);
                
                mouseX += (position.x - 0.5) * prevOptionsState.videoWidth;
                mouseY += (position.y - 0.5) * prevOptionsState.videoHeight;
                zone += pointable.touchZone();
                touches += 1.0;
            }
            
            touches = (touches > 0.0) ? 1.0 / touches : 0.0;
            mouseX *= touches;
            mouseY *= touches;
            zone *= touches;
        }
    }
    
    if(zone == Leap::Pointable::Zone::ZONE_TOUCHING && prevZone != Leap::Pointable::Zone::ZONE_TOUCHING)
        menu.handleMouseDown(mouseX, mouseY, SDL_BUTTON_LEFT);
    else if(zone != Leap::Pointable::Zone::ZONE_TOUCHING && prevZone == Leap::Pointable::Zone::ZONE_TOUCHING)
        menu.handleMouseUp(mouseX, mouseY, SDL_BUTTON_LEFT);
    
    if(mouseX != prevMouseX || mouseY != prevMouseY) {
        if(touches == 1.0) {
            SDL_WarpMouse(mouseX + menu.screenView->width, mouseY + menu.screenView->height);
            //menu.handleMouseMove(touch->mouseX, touch->mouseY);
        }else{
            menu.handleMouseWheel(menu.mouseX + menu.screenView->width, menu.mouseY + menu.screenView->height,
                                  (mouseY-prevMouseY) * 3.0/menu.screenView->height);
        }
    }
}

LeapManager leapManager;