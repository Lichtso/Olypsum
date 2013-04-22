//
//  ScriptGUILabel.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.04.13.
//
//

#include "ScriptGUILabel.h"

v8::Handle<v8::Value> ScriptGUILabel::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("GUILabel Constructor: Invalid argument"));
    
    GUILabel* objectPtr = new GUILabel();
    objectPtr->scriptInstance = v8::Persistent<v8::Object>::New(args.This());
    args.This()->SetInternalField(0, v8::External::New(objectPtr));
    getDataOfInstance<GUIView>(args[0])->addChild(objectPtr);
    return args.This();
}

v8::Handle<v8::Value> ScriptGUILabel::GetTextAlignment(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    switch(objectPtr->textAlignment) {
        case GUILabel::TextAlignment::Left:
            return handleScope.Close(v8::String::New("left"));
        case GUILabel::TextAlignment::Middle:
            return handleScope.Close(v8::String::New("middle"));
        case GUILabel::TextAlignment::Right:
            return handleScope.Close(v8::String::New("right"));
    }
}

void ScriptGUILabel::SetTextAlignment(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
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

v8::Handle<v8::Value> ScriptGUILabel::GetText(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    return v8::String::New(objectPtr->text.c_str());
}

void ScriptGUILabel::SetText(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    objectPtr->text = stdStrOfV8(value);
}

v8::Handle<v8::Value> ScriptGUILabel::GetFont(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<TextFont>(objectPtr->font, name);
    if(!filePackage) return v8::Undefined();
    return handleScope.Close(v8::String::New(fileManager.getResourcePath(filePackage, name).c_str()));
}

void ScriptGUILabel::SetFont(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    auto font = fileManager.initResource<TextFont>(stdStrOfV8(value));
    if(font) objectPtr->font = font;
}

v8::Handle<v8::Value> ScriptGUILabel::GetFontHeight(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    return v8::Integer::New(objectPtr->fontHeight);
}

void ScriptGUILabel::SetFontHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(info.This());
    objectPtr->fontHeight = value->IntegerValue();
}

v8::Handle<v8::Value> ScriptGUILabel::AccessColor(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    GUILabel* objectPtr = getDataOfInstance<GUILabel>(args.This());
    if(args.Length() == 1 && scriptQuaternion.isCorrectInstance(args[0])) {
        objectPtr->color = Color4(scriptQuaternion.getDataOfInstance(args[0]));
        return args[0];
    }else
        return handleScope.Close(scriptQuaternion.newInstance(objectPtr->color.getQuaternion()));
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

ScriptGUILabel scriptGUILabel;