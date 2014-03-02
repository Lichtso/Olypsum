//
//  ScriptIO.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

void ScriptMouse::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    return ScriptException("Mouse Constructor: Class can't be instantiated");
}

void ScriptMouse::AccessX(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if(args.Length() == 1 && args[0]->IsNumber() && args[0]->NumberValue() >= -menu.screenView->width && args[0]->NumberValue() <= menu.screenView->width)
        menu.mouseX = args[0]->NumberValue();
    ScriptReturn(menu.mouseX);
}

void ScriptMouse::AccessY(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if(args.Length() == 1 && args[0]->IsNumber() && args[0]->NumberValue() >= -menu.screenView->height && args[0]->NumberValue() <= menu.screenView->height)
        menu.mouseY = args[0]->NumberValue();
    ScriptReturn(menu.mouseY);
}

void ScriptMouse::AccessFixed(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if(args.Length() == 1 && args[0]->IsBoolean())
        menu.mouseFixed = args[0]->NumberValue();
    ScriptReturn(menu.mouseFixed);
}

ScriptMouse::ScriptMouse() :ScriptClass("Mouse", Constructor) {
    ScriptScope();
    
    (*functionTemplate)->Set(ScriptString("x"), ScriptMethod(AccessX));
    (*functionTemplate)->Set(ScriptString("y"), ScriptMethod(AccessY));
    (*functionTemplate)->Set(ScriptString("fixed"), ScriptMethod(AccessFixed));
}



void ScriptKeyboard::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    return ScriptException("Keyboard Constructor: Class can't be instantiated");
}

void ScriptKeyboard::GetKeyCount(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptReturn(keyStateSize);
}

void ScriptKeyboard::IsKeyPressed(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if(args.Length() < 1 || !args[0]->IntegerValue() || args[0]->IntegerValue() >= keyStateSize)
        return ScriptException("Keyboard isKeyPressed(): Invalid argument");
    ScriptReturn(keyState[args[0]->IntegerValue()]);
}

ScriptKeyboard::ScriptKeyboard() :ScriptClass("Keyboard", Constructor) {
    ScriptScope();
    
    (*functionTemplate)->Set(ScriptString("getKeyCount"), ScriptMethod(GetKeyCount));
    (*functionTemplate)->Set(ScriptString("isKeyPressed"), ScriptMethod(IsKeyPressed));
}