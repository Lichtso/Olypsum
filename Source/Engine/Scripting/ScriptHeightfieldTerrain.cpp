//
//  ScriptHeightfieldTerrain.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 05.04.13.
//
//

#include "ScriptHeightfieldTerrain.h"

v8::Handle<v8::Value> ScriptHeightfieldTerrain::GetWidth(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    HeightfieldTerrain* objectPtr = getDataOfInstance<HeightfieldTerrain>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->width));
}

v8::Handle<v8::Value> ScriptHeightfieldTerrain::GetLength(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    HeightfieldTerrain* objectPtr = getDataOfInstance<HeightfieldTerrain>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->length));
}

v8::Handle<v8::Value> ScriptHeightfieldTerrain::GetBitDepth(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    HeightfieldTerrain* objectPtr = getDataOfInstance<HeightfieldTerrain>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->bitDepth << 2));
}

void ScriptHeightfieldTerrain::SetBitDepth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32() || value->Uint32Value() % 4 != 0 || value->Uint32Value() == 0 || value->Uint32Value() > 8)
        v8::ThrowException(v8::String::New("HeightfieldTerrain bitDepth=: Invalid argument"));
    HeightfieldTerrain* objectPtr = getDataOfInstance<HeightfieldTerrain>(info.This());
    objectPtr->bitDepth = value->Uint32Value() >> 2;
}

v8::Handle<v8::Value> ScriptHeightfieldTerrain::UpdateModel(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    HeightfieldTerrain* objectPtr = getDataOfInstance<HeightfieldTerrain>(args.This());
    objectPtr->updateModel();
    return v8::Undefined();
}

ScriptHeightfieldTerrain::ScriptHeightfieldTerrain() :ScriptPhysicObject("HeightfieldTerrain") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("width"), GetWidth);
    objectTemplate->SetAccessor(v8::String::New("length"), GetLength);
    objectTemplate->SetAccessor(v8::String::New("bitDepth"), GetBitDepth, SetBitDepth);
    objectTemplate->Set(v8::String::New("updateModel"), v8::FunctionTemplate::New(UpdateModel));
    
    functionTemplate->Inherit(scriptPhysicObject.functionTemplate);
}

ScriptHeightfieldTerrain scriptHeightfieldTerrain;