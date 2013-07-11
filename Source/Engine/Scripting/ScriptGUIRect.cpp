//
//  ScriptGUIRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 16.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptGUIView.h"

v8::Handle<v8::Value> ScriptGUIRect::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    return v8::ThrowException(v8::String::New("GUIRect Constructor: Class can't be instantiated"));
}

v8::Handle<v8::Value> ScriptGUIRect::GetPosX(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->posX));
}

void ScriptGUIRect::SetPosX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(info.This());
    objectPtr->posX = value->IntegerValue();
}

v8::Handle<v8::Value> ScriptGUIRect::GetPosY(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->posY));
}

void ScriptGUIRect::SetPosY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(info.This());
    objectPtr->posY = value->IntegerValue();
}

v8::Handle<v8::Value> ScriptGUIRect::GetWidth(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->width));
}

void ScriptGUIRect::SetWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(info.This());
    objectPtr->width = value->IntegerValue();
}

v8::Handle<v8::Value> ScriptGUIRect::GetHeight(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->height));
}

void ScriptGUIRect::SetHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(info.This());
    objectPtr->height = value->IntegerValue();
}

v8::Handle<v8::Value> ScriptGUIRect::GetVisible(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(info.This());
    return handleScope.Close(v8::Boolean::New(objectPtr->visible));
}

void ScriptGUIRect::SetVisible(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(info.This());
    objectPtr->visible = value->BooleanValue();
}

v8::Handle<v8::Value> ScriptGUIRect::GetFocus(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(info.This());
    return handleScope.Close(v8::Boolean::New(objectPtr->getFocus()));
}

void ScriptGUIRect::SetFocus(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(info.This());
    objectPtr->setFocus(value->BooleanValue());
}

v8::Handle<v8::Value> ScriptGUIRect::GetParent(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(info.This());
    return handleScope.Close(objectPtr->parent->scriptInstance);
}

v8::Handle<v8::Value> ScriptGUIRect::Remove(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->parent->deleteChild(objectPtr->parent->getIndexOfChild(objectPtr));
    return v8::Undefined();
}

v8::Handle<v8::Value> ScriptGUIRect::UpdateContent(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->updateContent();
    return args.This();
}

ScriptGUIRect::ScriptGUIRect(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptClass(name, constructor) {
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

ScriptGUIRect scriptGUIRect;