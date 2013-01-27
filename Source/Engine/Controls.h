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
    struct ConsoleEntry {
        std::string message;
        float timeLeft;
    };
    std::vector<ConsoleEntry> consoleMessages;
    ControlsMangager();
    ~ControlsMangager();
    void consoleAdd(const std::string& message, float duration = 10.0);
    void handleMouseDown(int mouseX, int mouseY, SDL_Event& event);
    void handleMouseUp(int mouseX, int mouseY, SDL_Event& event);
    void handleMouseMove(int mouseX, int mouseY, SDL_Event& event);
    void handleMouseWheel(int mouseX, int mouseY, float delta);
    void gameTick();
};

extern ControlsMangager* controlsMangager;

#endif