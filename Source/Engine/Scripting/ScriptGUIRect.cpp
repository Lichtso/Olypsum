//
//  ScriptGUIRect.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 16.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptGUIView.h"

void ScriptGUIRect::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    return ScriptException("GUIRect Constructor: Class can't be instantiated");
}

void ScriptGUIRect::GetPosX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    ScriptReturn(objectPtr->posX);
}

void ScriptGUIRect::SetPosX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsInt32()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->posX = value->IntegerValue();
}

void ScriptGUIRect::GetPosY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    ScriptReturn(objectPtr->posY);
}

void ScriptGUIRect::SetPosY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsInt32()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->posY = value->IntegerValue();
}

void ScriptGUIRect::GetWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    ScriptReturn(objectPtr->width);
}

void ScriptGUIRect::SetWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsInt32()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->width = value->IntegerValue();
}

void ScriptGUIRect::GetHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    ScriptReturn(objectPtr->height);
}

void ScriptGUIRect::SetHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsInt32()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->height = value->IntegerValue();
}

void ScriptGUIRect::GetVisible(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    ScriptReturn(objectPtr->visible);
}

void ScriptGUIRect::SetVisible(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsBoolean()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->visible = value->BooleanValue();
}

void ScriptGUIRect::GetFocused(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    ScriptReturn(objectPtr->isFocused());
}

void ScriptGUIRect::SetFocused(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsBoolean()) return;
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->setFocused(value->BooleanValue());
}

void ScriptGUIRect::GetParent(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    ScriptReturn(objectPtr->parent->scriptInstance);
}

void ScriptGUIRect::Remove(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->parent->deleteChild(objectPtr->parent->getIndexOfChild(objectPtr));
}

void ScriptGUIRect::UpdateContent(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIRect* objectPtr = getDataOfInstance<GUIRect>(args.This());
    objectPtr->updateContent();
    ScriptReturn(args.This());
}

ScriptGUIRect::ScriptGUIRect(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptClass(name, constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> instanceTemplate = (*functionTemplate)->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(1);
}

v8::Handle<v8::Value> ScriptGUIRect::initInstance(v8::Local<v8::Object> instance, GUIView* parent, GUIRect* child) {
    if(!parent->addChild(child)) {
        delete child;
        return v8::Isolate::GetCurrent()->ThrowException(ScriptString("GUIView adopt(): Failed"));
    }
    child->scriptInstance.Reset(v8::Isolate::GetCurrent(), instance);
    instance->SetInternalField(0, v8::External::New(v8::Isolate::GetCurrent(), child));
    return instance;
}

ScriptGUIRect::ScriptGUIRect() :ScriptGUIRect("GUIRect", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "x", GetPosX, SetPosX);
    ScriptAccessor(objectTemplate, "y", GetPosY, SetPosY);
    ScriptAccessor(objectTemplate, "width", GetWidth, SetWidth);
    ScriptAccessor(objectTemplate, "height", GetHeight, SetHeight);
    ScriptAccessor(objectTemplate, "visible", GetVisible, SetVisible);
    ScriptAccessor(objectTemplate, "focused", GetFocused, SetFocused);
    ScriptAccessor(objectTemplate, "parent", GetParent, 0);
    ScriptMethod(objectTemplate, "remove", Remove);
    ScriptMethod(objectTemplate, "updateContent", UpdateContent);
}