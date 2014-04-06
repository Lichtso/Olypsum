//
//  ScriptGUIOutput.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptGUIOutput.h"

void ScriptGUILabel::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() != 1 || !scriptGUIView->isCorrectInstance(args[0]))
        return ScriptException("GUILabel Constructor: Invalid argument");
    
    GUILabel* objectPtr = new GUILabel();
    ScriptReturn(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUILabel::GetTextAlignment(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(args.This());
    switch(objectPtr->textAlignment) {
        case GUILabel::TextAlignment::Left:
            ScriptReturn("left");
        case GUILabel::TextAlignment::Middle:
            ScriptReturn("middle");
        case GUILabel::TextAlignment::Right:
            ScriptReturn("right");
    }
}

void ScriptGUILabel::SetTextAlignment(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsString()) return;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(args.This());
    const char* str = cStrOfV8(value);
    if(strcmp(str, "left") == 0)
        objectPtr->textAlignment = GUILabel::TextAlignment::Left;
    else if(strcmp(str, "middle") == 0)
        objectPtr->textAlignment = GUILabel::TextAlignment::Middle;
    else if(strcmp(str, "right") == 0)
        objectPtr->textAlignment = GUILabel::TextAlignment::Right;
}

void ScriptGUILabel::GetText(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(args.This());
    ScriptReturn(objectPtr->text.c_str());
}

void ScriptGUILabel::SetText(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsString()) return;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(args.This());
    objectPtr->text = stdStrOfV8(value);
}

void ScriptGUILabel::GetFont(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(args.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<TextFont>(objectPtr->font, name);
    if(filePackage)
        ScriptReturn(fileManager.getPathOfResource(filePackage, name).c_str());
}

void ScriptGUILabel::SetFont(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsString()) return;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(args.This());
    auto font = fileManager.getResourceByPath<TextFont>(levelManager.levelPackage, stdStrOfV8(value));
    if(font) objectPtr->font = font;
}

void ScriptGUILabel::GetFontHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(args.This());
    ScriptReturn(objectPtr->fontHeight);
}

void ScriptGUILabel::SetFontHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(args.This());
    objectPtr->fontHeight = value->IntegerValue();
}

void ScriptGUILabel::AccessColor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(args.This());
    if(args.Length() == 1 && scriptQuaternion->isCorrectInstance(args[0])) {
        objectPtr->color = Color4(scriptQuaternion->getDataOfInstance(args[0]));
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptQuaternion->newInstance(objectPtr->color.getQuaternion()));
}

ScriptGUILabel::ScriptGUILabel() :ScriptGUIRect("GUILabel", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "sizeAlignment", GetSizeAlignment<GUILabel>, SetSizeAlignment<GUILabel>);
    ScriptAccessor(objectTemplate, "textAlignment", GetTextAlignment, SetTextAlignment);
    ScriptAccessor(objectTemplate, "text", GetText, SetText);
    ScriptAccessor(objectTemplate, "font", GetFont, SetFont);
    ScriptAccessor(objectTemplate, "fontHeight", GetFontHeight, SetFontHeight);
    ScriptMethod(objectTemplate, "color", AccessColor);
    
    ScriptInherit(scriptGUIRect);
}



void ScriptGUIProgressBar::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() != 1 || !scriptGUIView->isCorrectInstance(args[0]))
        return ScriptException("GUIProgressBar Constructor: Invalid argument");
    
    GUIProgressBar* objectPtr = new GUIProgressBar();
    ScriptReturn(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUIProgressBar::GetValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIProgressBar* objectPtr = getDataOfInstance<GUIProgressBar>(args.This());
    ScriptReturn(objectPtr->value);
}

void ScriptGUIProgressBar::SetValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->NumberValue()) return;
    GUIProgressBar* objectPtr = getDataOfInstance<GUIProgressBar>(args.This());
    objectPtr->value = clamp(value->NumberValue(), 0.0, 1.0);
}

ScriptGUIProgressBar::ScriptGUIProgressBar() :ScriptGUIRect("GUIProgressBar", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    objectTemplate->SetAccessor(ScriptString("orientation"), GetOrientation<GUIProgressBar>, SetOrientationDual<GUIProgressBar>);
    objectTemplate->SetAccessor(ScriptString("value"), GetValue, SetValue);
    
    ScriptInherit(scriptGUIRect);
}



void ScriptGUIImage::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() != 2 || !scriptGUIView->isCorrectInstance(args[0]) || !args[1]->IsString())
        return ScriptException("GUIImage Constructor: Invalid argument");
    
    auto image = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, stdStrOfV8(args[1]));
    if(!image)
        return ScriptException("GUIImage Constructor: Invalid argument");
    
    GUIImage* objectPtr = new GUIImage();
    objectPtr->texture = image;
    ScriptReturn(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUIImage::GetImage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIImage* objectPtr = getDataOfInstance<GUIImage>(args.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<Texture>(objectPtr->texture, name);
    if(filePackage)
        ScriptReturn(fileManager.getPathOfResource(filePackage, name).c_str());
}

void ScriptGUIImage::SetImage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsString()) return;
    GUIImage* objectPtr = getDataOfInstance<GUIImage>(args.This());
    auto image = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, stdStrOfV8(value));
    if(image) objectPtr->texture = image;
}

ScriptGUIImage::ScriptGUIImage() :ScriptGUIRect("GUIImage", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "sizeAlignment", GetSizeAlignment<GUIImage>, SetSizeAlignment<GUIImage>);
    ScriptAccessor(objectTemplate, "image", GetImage, SetImage);
    
    ScriptInherit(scriptGUIRect);
}