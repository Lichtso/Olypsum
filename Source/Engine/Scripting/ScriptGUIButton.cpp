//
//  ScriptGUIButton.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 26.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptGUIButton.h"

void ScriptGUIButton::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() != 1 || !scriptGUIView->isCorrectInstance(args[0]))
        return ScriptException("GUIButton Constructor: Invalid argument");
    
    GUIButton* objectPtr = new GUIButton();
    objectPtr->onClick = [](GUIButton* objectPtr) {
        callFunction(v8::Handle<v8::Object>(*objectPtr->scriptInstance), "onclick", 0);
    };
    ScriptReturn(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUIButton::GetPaddingX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(args.This());
    ScriptReturn(objectPtr->paddingX);
}

void ScriptGUIButton::SetPaddingX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IntegerValue() || value->IntegerValue() < 0) return;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(args.This());
    objectPtr->paddingX = value->IntegerValue();
}

void ScriptGUIButton::GetPaddingY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(args.This());
    ScriptReturn(objectPtr->paddingY);
}

void ScriptGUIButton::SetPaddingY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IntegerValue() || value->IntegerValue() < 0) return;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(args.This());
    objectPtr->paddingY = value->IntegerValue();
}

void ScriptGUIButton::GetEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(args.This());
    ScriptReturn(objectPtr->enabled);
}

void ScriptGUIButton::SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsBoolean()) return;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(args.This());
    objectPtr->enabled = value->BooleanValue();
}

void ScriptGUIButton::GetState(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(args.This());
    switch(objectPtr->state) {
        case GUIButton::State::Released:
            ScriptReturn(ScriptString("released"));
            return;
        case GUIButton::State::Highlighted:
            ScriptReturn(ScriptString("highlighted"));
            return;
        case GUIButton::State::Pressed:
            ScriptReturn(ScriptString("pressed"));
            return;
    }
}

void ScriptGUIButton::SetState(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsString()) return;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(args.This());
    const char* str = cStrOfV8(value);
    if(strcmp(str, "released") == 0)
        objectPtr->state = GUIButton::State::Released;
    else if(strcmp(str, "highlighted") == 0)
        objectPtr->state = GUIButton::State::Highlighted;
    else if(strcmp(str, "pressed") == 0)
        objectPtr->state = GUIButton::State::Pressed;
}

void ScriptGUIButton::GetType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(args.This());
    switch(objectPtr->type) {
        case GUIButton::Type::Normal:
            ScriptReturn(ScriptString("normal"));
            return;
        case GUIButton::Type::Delete:
            ScriptReturn(ScriptString("delete"));
            return;
        case GUIButton::Type::Add:
            ScriptReturn(ScriptString("add"));
            return;
        case GUIButton::Type::Edit:
            ScriptReturn(ScriptString("edit"));
            return;
        case GUIButton::Type::Lockable:
            ScriptReturn(ScriptString("lockable"));
            return;
    }
}

void ScriptGUIButton::SetType(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsString()) return;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(args.This());
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
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    objectTemplate->SetAccessor(ScriptString("sizeAlignment"), GetSizeAlignment<GUIButton>, SetSizeAlignment<GUIButton>);
    objectTemplate->SetAccessor(ScriptString("paddingX"), GetPaddingX, SetPaddingX);
    objectTemplate->SetAccessor(ScriptString("paddingY"), GetPaddingY, SetPaddingY);
    objectTemplate->SetAccessor(ScriptString("enabled"), GetEnabled, SetEnabled);
    objectTemplate->SetAccessor(ScriptString("state"), GetState, SetState);
    objectTemplate->SetAccessor(ScriptString("type"), GetType, SetType);
    
    ScriptInherit(scriptGUIFramedView);
}



void ScriptGUICheckBox::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() != 1 || !scriptGUIView->isCorrectInstance(args[0]))
        return ScriptException("GUICheckBox Constructor: Invalid argument");
    
    GUICheckBox* objectPtr = new GUICheckBox();
    objectPtr->onClick = [](GUICheckBox* objectPtr) {
        callFunction(v8::Handle<v8::Object>(*objectPtr->scriptInstance), "onclick", 0);
    };
    ScriptReturn(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

ScriptGUICheckBox::ScriptGUICheckBox() :ScriptGUIButton("GUICheckBox", Constructor) {
    ScriptScope();
    
    ScriptInherit(scriptGUIButton);
}



void ScriptGUITabs::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() != 1 || !scriptGUIView->isCorrectInstance(args[0]))
        return ScriptException("GUITabs Constructor: Invalid argument");
    
    GUITabs* objectPtr = new GUITabs();
    objectPtr->onChange = [](GUITabs* objectPtr) {
        callFunction(v8::Handle<v8::Object>(*objectPtr->scriptInstance), "onchange", 0);
    };
    ScriptReturn(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUITabs::GetSelected(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUITabs* objectPtr = getDataOfInstance<GUITabs>(args.This());
    ScriptReturn(objectPtr->selected);
}

void ScriptGUITabs::SetSelected(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IntegerValue() || value->IntegerValue() < -1) return;
    GUITabs* objectPtr = getDataOfInstance<GUITabs>(args.This());
    objectPtr->selected = value->IntegerValue();
}

void ScriptGUITabs::GetDeactivatable(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUITabs* objectPtr = getDataOfInstance<GUITabs>(args.This());
    ScriptReturn(objectPtr->deactivatable);
}

void ScriptGUITabs::SetDeactivatable(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->BooleanValue()) return;
    GUITabs* objectPtr = getDataOfInstance<GUITabs>(args.This());
    objectPtr->deactivatable = value->BooleanValue();
}

ScriptGUITabs::ScriptGUITabs() :ScriptGUIView("GUITabs", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    objectTemplate->SetAccessor(ScriptString("sizeAlignment"), GetSizeAlignment<GUITabs>, SetSizeAlignment<GUITabs>);
    objectTemplate->SetAccessor(ScriptString("orientation"), GetOrientation<GUITabs>, SetOrientation<GUITabs>);
    objectTemplate->SetAccessor(ScriptString("selected"), GetSelected, SetSelected);
    objectTemplate->SetAccessor(ScriptString("deactivatable"), GetDeactivatable, SetDeactivatable);
    
    ScriptInherit(scriptGUIView);
}