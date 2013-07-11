//
//  ScriptIO.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

v8::Handle<v8::Value> ScriptMouse::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    return v8::ThrowException(v8::String::New("Mouse Constructor: Class can't be instantiated"));
}

v8::Handle<v8::Value> ScriptMouse::AccessX(const v8::Arguments& args) {
    if(args.Length() == 1 && args[0]->IsNumber() && args[0]->NumberValue() >= -menu.screenView->width && args[0]->NumberValue() <= menu.screenView->width)
        menu.mouseX = args[0]->NumberValue();
    return v8::Number::New(menu.mouseX);
}

v8::Handle<v8::Value> ScriptMouse::AccessY(const v8::Arguments& args) {
    if(args.Length() == 1 && args[0]->IsNumber() && args[0]->NumberValue() >= -menu.screenView->height && args[0]->NumberValue() <= menu.screenView->height)
        menu.mouseY = args[0]->NumberValue();
    return v8::Number::New(menu.mouseY);
}

v8::Handle<v8::Value> ScriptMouse::AccessFixed(const v8::Arguments& args) {
    if(args.Length() == 1 && args[0]->IsBoolean())
        menu.mouseFixed = args[0]->NumberValue();
    return v8::Boolean::New(menu.mouseFixed);
}

ScriptMouse::ScriptMouse() :ScriptClass("Mouse", Constructor) {
    v8::HandleScope handleScope;
    
    functionTemplate->Set(v8::String::New("x"), v8::FunctionTemplate::New(AccessX));
    functionTemplate->Set(v8::String::New("y"), v8::FunctionTemplate::New(AccessY));
    functionTemplate->Set(v8::String::New("fixed"), v8::FunctionTemplate::New(AccessFixed));
}



v8::Handle<v8::Value> ScriptKeyboard::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    return v8::ThrowException(v8::String::New("Keyboard Constructor: Class can't be instantiated"));
}

v8::Handle<v8::Value> ScriptKeyboard::GetKeyCount(const v8::Arguments& args) {
    return v8::Integer::New(keyStateSize);
}

v8::Handle<v8::Value> ScriptKeyboard::IsKeyPressed(const v8::Arguments& args) {
    if(args.Length() < 1 || !args[0]->IntegerValue() || args[0]->IntegerValue() >= keyStateSize)
        return v8::ThrowException(v8::String::New("Keyboard isKeyPressed(): Invalid argument"));
    return v8::Boolean::New(keyState[args[0]->IntegerValue()]);
}

ScriptKeyboard::ScriptKeyboard() :ScriptClass("Keyboard", Constructor) {
    v8::HandleScope handleScope;
    
    functionTemplate->Set(v8::String::New("getKeyCount"), v8::FunctionTemplate::New(GetKeyCount));
    functionTemplate->Set(v8::String::New("isKeyPressed"), v8::FunctionTemplate::New(IsKeyPressed));
}



ScriptMouse scriptMouse;
ScriptKeyboard scriptKeyboard;