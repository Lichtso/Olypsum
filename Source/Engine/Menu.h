//
//  Menu.h
//  Olypsum
//
//  Created by Alexander Meißner on 22.07.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef Menu_h
#define Menu_h

#include "LevelManager.h"

class Menu {
    float mouseVelocityX = 0.0, mouseVelocityY = 0.0;
    public:
    GUIScreenView* screenView;
    enum Name {
        none,
        loading,
        main,
        options,
        videoResolution,
        languages,
        credits,
        inGame,
        gameEsc,
        saveGames,
        newGame,
        multiplayer
    } current;
    struct ConsoleEntry {
        std::string message;
        float timeLeft;
    };
    std::vector<ConsoleEntry> consoleMessages;
    float mouseX = 0.0, mouseY = 0.0;
    bool mouseFixed = true;
    
    Menu();
    void consoleAdd(const std::string& message, float duration = 10.0);
    void handleActiveEvent(bool active);
    void handleMouseDown(SDL_Event& event);
    void handleMouseUp(SDL_Event& event);
    void handleMouseMove(SDL_Event& event);
    void handleMouseWheel(SDL_Event& event);
    void handleKeyDown(SDL_Event& event);
    void handleKeyUp(SDL_Event& event);
    void gameTick();
    void clear();
    void setPause(bool active);
    void setMenu(Name menu);
};

extern Menu menu;

#endif