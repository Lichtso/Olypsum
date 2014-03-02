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
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessX(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessY(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessFixed(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptMouse();
};

class ScriptKeyboard : public ScriptClass {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetKeyCount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void IsKeyPressed(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptKeyboard();
};

extern std::unique_ptr<ScriptMouse> scriptMouse;
extern std::unique_ptr<ScriptKeyboard> scriptKeyboard;

#endif