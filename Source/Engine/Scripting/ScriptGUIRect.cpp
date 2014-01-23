//
//  ScriptGUIRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 16.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptGUIView.h"

void ScriptGUIRect::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    return args.ScriptException("GUIRect Constructor: Class can't be instantiated");
}

void ScriptGUIRect::GetPosX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    args.GetReturnValue().Set(objectPtr->posX);
}

void ScriptGUIRect::SetPosX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsInt32()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->posX = value->IntegerValue();
}

void ScriptGUIRect::GetPosY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    args.GetReturnValue().Set(objectPtr->posY);
}

void ScriptGUIRect::SetPosY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsInt32()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->posY = value->IntegerValue();
}

void ScriptGUIRect::GetWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    args.GetReturnValue().Set(objectPtr->width);
}

void ScriptGUIRect::SetWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsInt32()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->width = value->IntegerValue();
}

void ScriptGUIRect::GetHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    args.GetReturnValue().Set(objectPtr->height);
}

void ScriptGUIRect::SetHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsInt32()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->height = value->IntegerValue();
}

void ScriptGUIRect::GetVisible(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    args.GetReturnValue().Set(objectPtr->visible);
}

void ScriptGUIRect::SetVisible(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->visible = value->BooleanValue();
}

void ScriptGUIRect::GetFocus(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    args.GetReturnValue().Set(objectPtr->getFocus());
}

void ScriptGUIRect::SetFocus(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->setFocus(value->BooleanValue());
}

void ScriptGUIRect::GetParent(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    args.GetReturnValue().Set(objectPtr->parent->scriptInstance);
}

void ScriptGUIRect::Remove(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->parent->deleteChild(objectPtr->parent->getIndexOfChild(objectPtr));
}

void ScriptGUIRect::UpdateContent(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->updateContent();
    args.GetReturnValue().Set(args.This());
}

ScriptGUIRect::ScriptGUIRect(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptClass(name, constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> instanceTemplate = functionTemplate->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(1);
}

v8::Handle<v8::Value> ScriptGUIRect::initInstance(v8::Local<v8::Object> instance, GUIView* parent, GUIRect* child) {
    if(!parent->addChild(child)) {
        delete child;
        return v8::ThrowException(v8::String::New("GUIView adopt(): Failed"));
    }
    child->scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), instance);
    instance->SetInternalField(0, v8::External::New(child));
    return instance;
}

ScriptGUIRect::ScriptGUIRect() :ScriptGUIRect("GUIRect", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("x"), GetPosX, SetPosX);
    objectTemplate->SetAccessor(v8::String::New("y"), GetPosY, SetPosY);
    objectTemplate->SetAccessor(v8::String::New("width"), GetWidth, SetWidth);
    objectTemplate->SetAccessor(v8::String::New("height"), GetHeight, SetHeight);
    objectTemplate->SetAccessor(v8::String::New("visible"), GetVisible, SetVisible);
    objectTemplate->SetAccessor(v8::String::New("focus"), GetFocus, SetFocus);
    objectTemplate->SetAccessor(v8::String::New("parent"), GetParent);
    objectTemplate->Set(v8::String::New("remove"), v8::FunctionTemplate::New(Remove));
    objectTemplate->Set(v8::String::New("updateContent"), v8::FunctionTemplate::New(UpdateContent));
}