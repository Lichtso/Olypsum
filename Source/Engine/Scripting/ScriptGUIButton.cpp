//
//  ScriptGUIButton.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 26.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptGUIButton.h"

void ScriptGUIButton::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return args.ScriptException("GUIButton Constructor: Invalid argument");
    
    GUIButton* objectPtr = new GUIButton();
    objectPtr->onClick = [](GUIButton* objectPtr) {
        callFunction(objectPtr->scriptInstance, "onclick", { });
    };
    args.GetReturnValue().Set(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUIButton::GetPaddingX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    info.GetReturnValue().Set(objectPtr->paddingX);
}

void ScriptGUIButton::SetPaddingX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IntegerValue() || value->IntegerValue() < 0) return;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    objectPtr->paddingX = value->IntegerValue();
}

void ScriptGUIButton::GetPaddingY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    info.GetReturnValue().Set(objectPtr->paddingY);
}

void ScriptGUIButton::SetPaddingY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IntegerValue() || value->IntegerValue() < 0) return;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    objectPtr->paddingY = value->IntegerValue();
}

void ScriptGUIButton::GetEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    info.GetReturnValue().Set(objectPtr->enabled);
}

void ScriptGUIButton::SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    objectPtr->enabled = value->BooleanValue();
}

void ScriptGUIButton::GetState(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    switch(objectPtr->state) {
        case GUIButton::State::Released:
            info.GetReturnValue().Set(v8::String::New("released"));
            return;
        case GUIButton::State::Highlighted:
            info.GetReturnValue().Set(v8::String::New("highlighted"));
            return;
        case GUIButton::State::Pressed:
            info.GetReturnValue().Set(v8::String::New("pressed"));
            return;
    }
}

void ScriptGUIButton::SetState(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    const char* str = cStrOfV8(value);
    if(strcmp(str, "released") == 0)
        objectPtr->state = GUIButton::State::Released;
    else if(strcmp(str, "highlighted") == 0)
        objectPtr->state = GUIButton::State::Highlighted;
    else if(strcmp(str, "pressed") == 0)
        objectPtr->state = GUIButton::State::Pressed;
}

void ScriptGUIButton::GetType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    switch(objectPtr->type) {
        case GUIButton::Type::Normal:
            info.GetReturnValue().Set(v8::String::New("normal"));
            return;
        case GUIButton::Type::Delete:
            info.GetReturnValue().Set(v8::String::New("delete"));
            return;
        case GUIButton::Type::Add:
            info.GetReturnValue().Set(v8::String::New("add"));
            return;
        case GUIButton::Type::Edit:
            info.GetReturnValue().Set(v8::String::New("edit"));
            return;
        case GUIButton::Type::Lockable:
            info.GetReturnValue().Set(v8::String::New("lockable"));
            return;
    }
}

void ScriptGUIButton::SetType(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    const char* str = cStrOfV8(value);
    if(strcmp(str, "normal") == 0)
        objectPtr->type = GUIButton::Type::Normal;
    else if(strcmp(str, "delete") == 0)
        objectPtr->type = GUIButton::Type::Delete;
    else if(strcmp(str, "add") == 0)
        objectPtr->type = GUIButton::Type::Add;
    else if(strcmp(str, "edit") == 0)
        objectPtr->type = GUIButton::Type::Edit;
    else if(strcmp(str, "lockable") == 0)
        objectPtr->type = GUIButton::Type::Lockable;
}

ScriptGUIButton::ScriptGUIButton() :ScriptGUIFramedView("GUIButton", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("sizeAlignment"), GetSizeAlignment<GUIButton>, SetSizeAlignment<GUIButton>);
    objectTemplate->SetAccessor(v8::String::New("paddingX"), GetPaddingX, SetPaddingX);
    objectTemplate->SetAccessor(v8::String::New("paddingY"), GetPaddingY, SetPaddingY);
    objectTemplate->SetAccessor(v8::String::New("enabled"), GetEnabled, SetEnabled);
    objectTemplate->SetAccessor(v8::String::New("state"), GetState, SetState);
    objectTemplate->SetAccessor(v8::String::New("type"), GetType, SetType);
    
    functionTemplate->Inherit(scriptGUIFramedView.functionTemplate);
}



void ScriptGUICheckBox::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return args.ScriptException("GUICheckBox Constructor: Invalid argument");
    
    GUICheckBox* objectPtr = new GUICheckBox();
    objectPtr->onClick = [](GUICheckBox* objectPtr) {
        callFunction(objectPtr->scriptInstance, "onclick", { });
    };
    args.GetReturnValue().Set(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

ScriptGUICheckBox::ScriptGUICheckBox() :ScriptGUIButton("GUICheckBox", Constructor) {
    //v8::HandleScope handleScope;
    
    functionTemplate->Inherit(scriptGUIButton.functionTemplate);
}



void ScriptGUITabs::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return args.ScriptException("GUITabs Constructor: Invalid argument");
    
    GUITabs* objectPtr = new GUITabs();
    objectPtr->onChange = [](GUITabs* objectPtr) {
        callFunction(objectPtr->scriptInstance, "onchange", { });
    };
    args.GetReturnValue().Set(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUITabs::GetSelected(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUITabs* objectPtr = getDataOfInstance<GUITabs>(info.This());
    info.GetReturnValue().Set(objectPtr->selected);
}

void ScriptGUITabs::SetSelected(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IntegerValue() || value->IntegerValue() < -1) return;
    GUITabs* objectPtr = getDataOfInstance<GUITabs>(info.This());
    objectPtr->selected = value->IntegerValue();
}

void ScriptGUITabs::GetDeactivatable(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUITabs* objectPtr = getDataOfInstance<GUITabs>(info.This());
    info.GetReturnValue().Set(objectPtr->deactivatable);
}

void ScriptGUITabs::SetDeactivatable(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->BooleanValue()) return;
    GUITabs* objectPtr = getDataOfInstance<GUITabs>(info.This());
    objectPtr->deactivatable = value->BooleanValue();
}

ScriptGUITabs::ScriptGUITabs() :ScriptGUIView("GUITabs", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("sizeAlignment"), GetSizeAlignment<GUITabs>, SetSizeAlignment<GUITabs>);
    objectTemplate->SetAccessor(v8::String::New("orientation"), GetOrientation<GUITabs>, SetOrientation<GUITabs>);
    objectTemplate->SetAccessor(v8::String::New("selected"), GetSelected, SetSelected);
    objectTemplate->SetAccessor(v8::String::New("deactivatable"), GetDeactivatable, SetDeactivatable);
    
    functionTemplate->Inherit(scriptGUIView.functionTemplate);
}



ScriptGUIButton scriptGUIButton;
ScriptGUICheckBox scriptGUICheckBox;
ScriptGUITabs scriptGUITabs;