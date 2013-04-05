//
//  Controls.h
//  Olypsum
//
//  Created by Alexander Meißner on 09.01.13.
//
//

#include "Menu.h"

#ifndef Controls_h
#define Controls_h

class ControlsMangager {
    PhysicObject* grabbedObject;
    btVector3 relGrabbPos;
    btVector3 rot, rotVelocity;
    public:
    ControlsMangager();
    ~ControlsMangager();
    void handleMouseDown(int mouseX, int mouseY, SDL_Event& event);
    void handleMouseUp(int mouseX, int mouseY, SDL_Event& event);
    void handleMouseMove(int mouseX, int mouseY, SDL_Event& event);
    void handleMouseWheel(int mouseX, int mouseY, float delta);
    void gameTick();
};

extern std::unique_ptr<ControlsMangager> controlsMangager;

#endif