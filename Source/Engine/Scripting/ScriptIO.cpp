//
//  ScriptIO.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

void ScriptMouse::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    return args.ScriptException("Mouse Constructor: Class can't be instantiated");
}

void ScriptMouse::AccessX(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if(args.Length() == 1 && args[0]->IsNumber() && args[0]->NumberValue() >= -menu.screenView->width && args[0]->NumberValue() <= menu.screenView->width)
        menu.mouseX = args[0]->NumberValue();
    args.GetReturnValue().Set(menu.mouseX);
}

void ScriptMouse::AccessY(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if(args.Length() == 1 && args[0]->IsNumber() && args[0]->NumberValue() >= -menu.screenView->height && args[0]->NumberValue() <= menu.screenView->height)
        menu.mouseY = args[0]->NumberValue();
    args.GetReturnValue().Set(menu.mouseY);
}

void ScriptMouse::AccessFixed(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if(args.Length() == 1 && args[0]->IsBoolean())
        menu.mouseFixed = args[0]->NumberValue();
    args.GetReturnValue().Set(menu.mouseFixed);
}

ScriptMouse::ScriptMouse() :ScriptClass("Mouse", Constructor) {
    v8::HandleScope handleScope;
    
    functionTemplate->Set(v8::String::New("x"), v8::FunctionTemplate::New(AccessX));
    functionTemplate->Set(v8::String::New("y"), v8::FunctionTemplate::New(AccessY));
    functionTemplate->Set(v8::String::New("fixed"), v8::FunctionTemplate::New(AccessFixed));
}



void ScriptKeyboard::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    return args.ScriptException("Keyboard Constructor: Class can't be instantiated");
}

void ScriptKeyboard::GetKeyCount(const v8::FunctionCallbackInfo<v8::Value>& args) {
    args.GetReturnValue().Set(keyStateSize);
}

void ScriptKeyboard::IsKeyPressed(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if(args.Length() < 1 || !args[0]->IntegerValue() || args[0]->IntegerValue() >= keyStateSize)
        return args.ScriptException("Keyboard isKeyPressed(): Invalid argument");
    args.GetReturnValue().Set(keyState[args[0]->IntegerValue()]);
}

ScriptKeyboard::ScriptKeyboard() :ScriptClass("Keyboard", Constructor) {
    v8::HandleScope handleScope;
    
    functionTemplate->Set(v8::String::New("getKeyCount"), v8::FunctionTemplate::New(GetKeyCount));
    functionTemplate->Set(v8::String::New("isKeyPressed"), v8::FunctionTemplate::New(IsKeyPressed));
}



ScriptMouse scriptMouse;
ScriptKeyboard scriptKeyboard;