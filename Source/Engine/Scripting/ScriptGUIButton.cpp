//
//  ScriptGUIButton.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 26.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptGUIButton.h"

v8::Handle<v8::Value> ScriptGUIButton::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("GUIButton Constructor: Invalid argument"));
    
    GUIButton* objectPtr = new GUIButton();
    objectPtr->onClick = [](GUIButton* objectPtr) {
        callFunction(objectPtr->scriptInstance, "onclick", { });
    };
    return initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr);
}

v8::Handle<v8::Value> ScriptGUIButton::GetPaddingX(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    return v8::Integer::New(objectPtr->paddingX);
}

void ScriptGUIButton::SetPaddingX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IntegerValue() || value->IntegerValue() < 0) return;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    objectPtr->paddingX = value->IntegerValue();
}

v8::Handle<v8::Value> ScriptGUIButton::GetPaddingY(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    return v8::Integer::New(objectPtr->paddingY);
}

void ScriptGUIButton::SetPaddingY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IntegerValue() || value->IntegerValue() < 0) return;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    objectPtr->paddingY = value->IntegerValue();
}

v8::Handle<v8::Value> ScriptGUIButton::GetEnabled(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    return v8::Boolean::New(objectPtr->enabled);
}

void ScriptGUIButton::SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    objectPtr->enabled = value->BooleanValue();
}

v8::Handle<v8::Value> ScriptGUIButton::GetState(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    switch(objectPtr->state) {
        case GUIButton::State::Released:
            return handleScope.Close(v8::String::New("released"));
        case GUIButton::State::Highlighted:
            return handleScope.Close(v8::String::New("highlighted"));
        case GUIButton::State::Pressed:
            return handleScope.Close(v8::String::New("pressed"));
    }
}

void ScriptGUIButton::SetState(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
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

v8::Handle<v8::Value> ScriptGUIButton::GetType(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIButton* objectPtr = getDataOfInstance<GUIButton>(info.This());
    switch(objectPtr->type) {
        case GUIButton::Type::Normal:
            return handleScope.Close(v8::String::New("normal"));
        case GUIButton::Type::Delete:
            return handleScope.Close(v8::String::New("delete"));
        case GUIButton::Type::Add:
            return handleScope.Close(v8::String::New("add"));
        case GUIButton::Type::Edit:
            return handleScope.Close(v8::String::New("edit"));
        case GUIButton::Type::Lockable:
            return handleScope.Close(v8::String::New("lockable"));
    }
}

void ScriptGUIButton::SetType(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
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



v8::Handle<v8::Value> ScriptGUICheckBox::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("GUICheckBox Constructor: Invalid argument"));
    
    GUICheckBox* objectPtr = new GUICheckBox();
    objectPtr->onClick = [](GUICheckBox* objectPtr) {
        callFunction(objectPtr->scriptInstance, "onclick", { });
    };
    return initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr);
}

ScriptGUICheckBox::ScriptGUICheckBox() :ScriptGUIButton("GUICheckBox", Constructor) {
    //v8::HandleScope handleScope;
    
    functionTemplate->Inherit(scriptGUIButton.functionTemplate);
}



v8::Handle<v8::Value> ScriptGUITabs::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("GUITabs Constructor: Invalid argument"));
    
    GUITabs* objectPtr = new GUITabs();
    objectPtr->onChange = [](GUITabs* objectPtr) {
        callFunction(objectPtr->scriptInstance, "onchange", { });
    };
    return initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr);
}

v8::Handle<v8::Value> ScriptGUITabs::GetSelected(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUITabs* objectPtr = getDataOfInstance<GUITabs>(info.This());
    return v8::Integer::New(objectPtr->selected);
}

void ScriptGUITabs::SetSelected(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IntegerValue() || value->IntegerValue() < -1) return;
    GUITabs* objectPtr = getDataOfInstance<GUITabs>(info.This());
    objectPtr->selected = value->IntegerValue();
}

v8::Handle<v8::Value> ScriptGUITabs::GetDeactivatable(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUITabs* objectPtr = getDataOfInstance<GUITabs>(info.This());
    return v8::Boolean::New(objectPtr->deactivatable);
}

void ScriptGUITabs::SetDeactivatable(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
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