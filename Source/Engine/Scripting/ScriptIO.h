//
//  ScriptIO.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptIO_h
#define ScriptIO_h

#include "ScriptAnimation.h"

class ScriptMouse : public ScriptClass {
    ScriptDeclareMethod(Constructor);
    ScriptDeclareMethod(AccessX);
    ScriptDeclareMethod(AccessY);
    ScriptDeclareMethod(AccessFixed);
    public:
    ScriptMouse();
};

class ScriptKeyboard : public ScriptClass {
    ScriptDeclareMethod(Constructor);
    ScriptDeclareMethod(GetKeyCount);
    ScriptDeclareMethod(IsKeyPressed);
    public:
    ScriptKeyboard();
};

extern std::unique_ptr<ScriptMouse> scriptMouse;
extern std::unique_ptr<ScriptKeyboard> scriptKeyboard;

#endif