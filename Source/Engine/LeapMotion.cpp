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
    prevZone = zone;
    zone = Leap::Pointable::Zone::ZONE_NONE;
    
    if(controller.isConnected()) {
        const Leap::ScreenList screens = controller.locatedScreens();
        const Leap::Screen screen = screens[0];
        const Leap::Frame frame = controller.frame();
        
        if(frame.isValid() && lastFrameID != frame.id()) {
            lastFrameID = frame.id();
            if(frame.pointables().count() <= 1) {
                prevMouseX = mouseX;
                prevMouseY = mouseY;
            }
            mouseX = 0;
            mouseY = 0;
            unsigned int pointables = frame.pointables().count();
            
            for(int i = 0; i < pointables; i ++) {
                const Leap::Pointable pointable = frame.pointables()[i];
                if(!pointable.isValid() || pointable.touchZone() == Leap::Pointable::Zone::ZONE_NONE) continue;
                Leap::Vector position = pointable.stabilizedTipPosition();
                //Leap::Vector position = screen.intersect(pointable.stabilizedTipPosition(), pointable.direction(), true);
                
                mouseX += position.x * 0.003F * prevOptionsState.videoWidth;
                mouseY += (position.y-200.0F) * 0.005F * prevOptionsState.videoHeight;
                zone += pointable.touchZone();
            }
            
            /*if(menu.current == Menu::inGame && pointables == 0)
                menu.setPause(true);
            else*/ if(menu.current == Menu::inGame && menu.mouseFixed)
                zone = Leap::Pointable::Zone::ZONE_NONE;
            else if(pointables > 0) {
                float factor = 1.0 / pointables;
                mouseX = clamp((int) (mouseX * factor), -menu.screenView->width, menu.screenView->width);
                mouseY = clamp((int) (mouseY * factor), -menu.screenView->height, menu.screenView->height);
                zone *= factor;
                
                if(pointables == 1 && (mouseX != prevMouseX || mouseY != prevMouseY))
                    SDL_WarpMouse((mouseX + menu.screenView->width)/prevOptionsState.videoScale, (menu.screenView->height - mouseY)/prevOptionsState.videoScale);
                else if(pointables > 1)
                    menu.handleMouseWheel(prevMouseX, prevMouseY, (prevMouseX-mouseX)*1.0/menu.screenView->width, (mouseY-prevMouseY)*1.0/menu.screenView->height);
            }
        }
    }
    
    if(zone == Leap::Pointable::Zone::ZONE_TOUCHING && prevZone != Leap::Pointable::Zone::ZONE_TOUCHING)
        menu.handleMouseDown(mouseX, mouseY, SDL_BUTTON_LEFT);
    else if(zone != Leap::Pointable::Zone::ZONE_TOUCHING && prevZone == Leap::Pointable::Zone::ZONE_TOUCHING)
        menu.handleMouseUp(mouseX, mouseY, SDL_BUTTON_LEFT);
}

LeapManager leapManager;