//
//  ScriptGUIOutput.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptGUIOutput.h"

void ScriptGUILabel::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return args.ScriptException("GUILabel Constructor: Invalid argument");
    
    GUILabel* objectPtr = new GUILabel();
    args.GetReturnValue().Set(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUILabel::GetTextAlignment(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    switch(objectPtr->textAlignment) {
        case GUILabel::TextAlignment::Left:
            info.GetReturnValue().Set(v8::String::New("left"));
            return;
        case GUILabel::TextAlignment::Middle:
            info.GetReturnValue().Set(v8::String::New("middle"));
            return;
        case GUILabel::TextAlignment::Right:
            info.GetReturnValue().Set(v8::String::New("right"));
            return;
    }
}

void ScriptGUILabel::SetTextAlignment(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    const char* str = cStrOfV8(value);
    if(strcmp(str, "left") == 0)
        objectPtr->textAlignment = GUILabel::TextAlignment::Left;
    else if(strcmp(str, "middle") == 0)
        objectPtr->textAlignment = GUILabel::TextAlignment::Middle;
    else if(strcmp(str, "right") == 0)
        objectPtr->textAlignment = GUILabel::TextAlignment::Right;
}

void ScriptGUILabel::GetText(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    info.GetReturnValue().Set(v8::String::New(objectPtr->text.c_str()));
}

void ScriptGUILabel::SetText(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    objectPtr->text = stdStrOfV8(value);
}

void ScriptGUILabel::GetFont(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<TextFont>(objectPtr->font, name);
    if(filePackage)
        info.GetReturnValue().Set(v8::String::New(fileManager.getPathOfResource(filePackage, name).c_str()));
}

void ScriptGUILabel::SetFont(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    auto font = fileManager.getResourceByPath<TextFont>(levelManager.levelPackage, stdStrOfV8(value));
    if(font) objectPtr->font = font;
}

void ScriptGUILabel::GetFontHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    info.GetReturnValue().Set(objectPtr->fontHeight);
}

void ScriptGUILabel::SetFontHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    objectPtr->fontHeight = value->IntegerValue();
}

void ScriptGUILabel::AccessColor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(args.This());
    if(args.Length() == 1 && scriptQuaternion.isCorrectInstance(args[0])) {
        objectPtr->color = Color4(scriptQuaternion.getDataOfInstance(args[0]));
        args.GetReturnValue().Set(args[0]);
        return;
    }else
        args.GetReturnValue().Set(scriptQuaternion.newInstance(objectPtr->color.getQuaternion()));
}

ScriptGUILabel::ScriptGUILabel() :ScriptGUIRect("GUILabel", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("sizeAlignment"), GetSizeAlignment<GUILabel>, SetSizeAlignment<GUILabel>);
    objectTemplate->SetAccessor(v8::String::New("textAlignment"), GetTextAlignment, SetTextAlignment);
    objectTemplate->SetAccessor(v8::String::New("text"), GetText, SetText);
    objectTemplate->SetAccessor(v8::String::New("font"), GetFont, SetFont);
    objectTemplate->SetAccessor(v8::String::New("fontHeight"), GetFontHeight, SetFontHeight);
    objectTemplate->Set(v8::String::New("color"), v8::FunctionTemplate::New(AccessColor));
    
    functionTemplate->Inherit(scriptGUIRect.functionTemplate);
}



void ScriptGUIProgressBar::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return args.ScriptException("GUIProgressBar Constructor: Invalid argument");
    
    GUIProgressBar* objectPtr = new GUIProgressBar();
    args.GetReturnValue().Set(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUIProgressBar::GetValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIProgressBar* objectPtr = getDataOfInstance<GUIProgressBar>(info.This());
    info.GetReturnValue().Set(objectPtr->value);
}

void ScriptGUIProgressBar::SetValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->NumberValue()) return;
    GUIProgressBar* objectPtr = getDataOfInstance<GUIProgressBar>(info.This());
    objectPtr->value = clamp(value->NumberValue(), 0.0, 1.0);
}

ScriptGUIProgressBar::ScriptGUIProgressBar() :ScriptGUIRect("GUIProgressBar", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("orientation"), GetOrientation<GUIProgressBar>, SetOrientationDual<GUIProgressBar>);
    objectTemplate->SetAccessor(v8::String::New("value"), GetValue, SetValue);
    
    functionTemplate->Inherit(scriptGUIRect.functionTemplate);
}



void ScriptGUIImage::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 2 || !scriptGUIView.isCorrectInstance(args[0]) || !args[1]->IsString())
        return args.ScriptException("GUIImage Constructor: Invalid argument");
    
    auto image = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, stdStrOfV8(args[1]));
    if(!image)
        return args.ScriptException("GUIImage Constructor: Invalid argument");
    
    GUIImage* objectPtr = new GUIImage();
    objectPtr->texture = image;
    args.GetReturnValue().Set(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUIImage::GetImage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIImage* objectPtr = getDataOfInstance<GUIImage>(info.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<Texture>(objectPtr->texture, name);
    if(filePackage)
        info.GetReturnValue().Set(v8::String::New(fileManager.getPathOfResource(filePackage, name).c_str()));
}

void ScriptGUIImage::SetImage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    GUIImage* objectPtr = getDataOfInstance<GUIImage>(info.This());
    auto image = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, stdStrOfV8(value));
    if(image) objectPtr->texture = image;
}

ScriptGUIImage::ScriptGUIImage() :ScriptGUIRect("GUIImage", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("sizeAlignment"), GetSizeAlignment<GUIImage>, SetSizeAlignment<GUIImage>);
    objectTemplate->SetAccessor(v8::String::New("image"), GetImage, SetImage);
    
    functionTemplate->Inherit(scriptGUIRect.functionTemplate);
}