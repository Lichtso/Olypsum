//
//  ScriptGUIView.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 18.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptGUIView.h"

void ScriptGUIView::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return args.ScriptException("GUIView Constructor: Invalid argument");
    
    GUIView* objectPtr = new GUIView();
    args.GetReturnValue().Set(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUIView::GetChildCount(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIView* objectPtr = getDataOfInstance<GUIView>(info.This());
    info.GetReturnValue().Set(v8::Integer::New(objectPtr->children.size()));
}

void ScriptGUIView::GetChild(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIView* objectPtr = getDataOfInstance<GUIView>(info.This());
    if(index >= objectPtr->children.size())
        return info.ScriptException("GUIView []: Invalid argument");
    info.GetReturnValue().Set(objectPtr->children[index]->scriptInstance);
}

void ScriptGUIView::Adopt(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1 || !scriptGUIRect.isCorrectInstance(args[0]))
        return args.ScriptException("GUIView adopt(): Child is not a GUIRect");
    GUIView* objectPtr = getDataOfInstance<GUIView>(args.This());
    objectPtr->addChild(getDataOfInstance<GUIRect>(args.This()));
}

ScriptGUIView::ScriptGUIView() :ScriptGUIRect("GUIView", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("length"), GetChildCount);
    objectTemplate->SetIndexedPropertyHandler(GetChild);
    objectTemplate->Set(v8::String::New("adopt"), v8::FunctionTemplate::New(Adopt));
    
    functionTemplate->Inherit(scriptGUIRect.functionTemplate);
}



void ScriptGUIFramedView::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return args.ScriptException("GUIFramedView Constructor: Invalid argument");
    
    GUIFramedView* objectPtr = new GUIFramedView();
    args.GetReturnValue().Set(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUIFramedView::AccessTopColor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(args.This());
    if(args.Length() == 1 && scriptQuaternion.isCorrectInstance(args[0])) {
        objectPtr->content.topColor = Color4(scriptQuaternion.getDataOfInstance(args[0]));
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptQuaternion.newInstance(objectPtr->content.topColor.getQuaternion()));
}

void ScriptGUIFramedView::AccessBottomColor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(args.This());
    if(args.Length() == 1 && scriptQuaternion.isCorrectInstance(args[0])) {
        objectPtr->content.bottomColor = Color4(scriptQuaternion.getDataOfInstance(args[0]));
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptQuaternion.newInstance(objectPtr->content.bottomColor.getQuaternion()));
}

void ScriptGUIFramedView::AccessBorderColor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(args.This());
    if(args.Length() == 1 && scriptQuaternion.isCorrectInstance(args[0])) {
        objectPtr->content.borderColor = Color4(scriptQuaternion.getDataOfInstance(args[0]));
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptQuaternion.newInstance(objectPtr->content.borderColor.getQuaternion()));
}

void ScriptGUIFramedView::GetInnerShadow(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(info.This());
    info.GetReturnValue().Set(objectPtr->content.innerShadow);
}

void ScriptGUIFramedView::SetInnerShadow(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32()) return;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(info.This());
    objectPtr->content.innerShadow = value->IntegerValue();
}

void ScriptGUIFramedView::GetCornerRadius(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(info.This());
    info.GetReturnValue().Set(objectPtr->content.cornerRadius);
}

void ScriptGUIFramedView::SetCornerRadius(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(info.This());
    objectPtr->content.cornerRadius = value->IntegerValue();
}

ScriptGUIFramedView::ScriptGUIFramedView() :ScriptGUIView("GUIFramedView", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->Set(v8::String::New("topColor"), v8::FunctionTemplate::New(AccessTopColor));
    objectTemplate->Set(v8::String::New("bottomColor"), v8::FunctionTemplate::New(AccessBottomColor));
    objectTemplate->Set(v8::String::New("borderColor"), v8::FunctionTemplate::New(AccessBorderColor));
    objectTemplate->SetAccessor(v8::String::New("innerShadow"), GetInnerShadow, SetInnerShadow);
    objectTemplate->SetAccessor(v8::String::New("cornerRadius"), GetCornerRadius, SetCornerRadius);
    
    functionTemplate->Inherit(scriptGUIView.functionTemplate);
}



void ScriptGUIScreenView::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(!args.IsConstructCall()) {
        v8::Persistent<v8::Object> instance = menu.screenView->scriptInstance;
        if(instance.IsEmpty()) {
            v8::Handle<v8::Value> external = v8::External::New(menu.screenView);
            instance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(),
                       scriptGUIScreenView.functionTemplate->GetFunction()->NewInstance(1, &external));
            menu.screenView->scriptInstance = instance;
        }
        args.GetReturnValue().Set(instance);
        return;
    }
    
    if(args.Length() != 1 || !args[0]->IsExternal())
        return args.ScriptException("GUIScreenView Constructor: Class can't be instantiated");
    
    args.This()->SetInternalField(0, args[0]);
    args.GetReturnValue().Set(args.This());
}

void ScriptGUIScreenView::GetModalView(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIScreenView* objectPtr = getDataOfInstance<GUIScreenView>(info.This());
    if(objectPtr->modalView)
        info.GetReturnValue().Set(objectPtr->modalView->scriptInstance);
}

void ScriptGUIScreenView::SetModalView(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    GUIScreenView* objectPtr = getDataOfInstance<GUIScreenView>(info.This());
    if(property.IsEmpty() || !scriptGUIView.isCorrectInstance(property))
        objectPtr->setModalView(NULL);
    else
        objectPtr->setModalView(scriptGUIView.getDataOfInstance<GUIView>(property));
}

void ScriptGUIScreenView::GetFocused(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIScreenView* objectPtr = getDataOfInstance<GUIScreenView>(info.This());
    if(objectPtr->focused)
        info.GetReturnValue().Set(objectPtr->focused->scriptInstance);
}

ScriptGUIScreenView::ScriptGUIScreenView() :ScriptGUIView("GUIScreenView", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("modalView"), GetModalView, SetModalView);
    objectTemplate->SetAccessor(v8::String::New("focused"), GetFocused);
    
    functionTemplate->Inherit(scriptGUIView.functionTemplate);
}



void ScriptGUIScrollView::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return args.ScriptException("GUIScrollView Constructor: Invalid argument");
    
    GUIScrollView* objectPtr = new GUIScrollView();
    objectPtr->scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), args.This());
    args.This()->SetInternalField(0, v8::External::New(objectPtr));
    GUIView* parent = getDataOfInstance<GUIView>(args[0]);
    parent->addChild(objectPtr);
    args.GetReturnValue().Set(args.This());
}

void ScriptGUIScrollView::GetSliderX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(info.This());
    info.GetReturnValue().Set(objectPtr->sliderX);
}

void ScriptGUIScrollView::SetSliderX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(info.This());
    objectPtr->sliderX = value->BooleanValue();
}

void ScriptGUIScrollView::GetSliderY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(info.This());
    info.GetReturnValue().Set(objectPtr->sliderY);
}

void ScriptGUIScrollView::SetSliderY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(info.This());
    objectPtr->sliderY = value->BooleanValue();
}

void ScriptGUIScrollView::GetScrollX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(info.This());
    info.GetReturnValue().Set(objectPtr->scrollPosX);
}

void ScriptGUIScrollView::SetScrollX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32()) return;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(info.This());
    objectPtr->scrollPosX = value->IntegerValue();
}

void ScriptGUIScrollView::GetScrollY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(info.This());
    info.GetReturnValue().Set(objectPtr->scrollPosY);
}

void ScriptGUIScrollView::SetScrollY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32()) return;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(info.This());
    objectPtr->scrollPosY = value->IntegerValue();
}

void ScriptGUIScrollView::GetContentWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(info.This());
    info.GetReturnValue().Set(objectPtr->contentWidth);
}

void ScriptGUIScrollView::SetContentWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(info.This());
    objectPtr->contentWidth = value->IntegerValue();
}

void ScriptGUIScrollView::GetContentHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(info.This());
    info.GetReturnValue().Set(objectPtr->contentHeight);
}

void ScriptGUIScrollView::SetContentHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(info.This());
    objectPtr->contentHeight = value->IntegerValue();
}

ScriptGUIScrollView::ScriptGUIScrollView() :ScriptGUIFramedView("GUIScrollView", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("sliderX"), GetSliderX, SetSliderX);
    objectTemplate->SetAccessor(v8::String::New("sliderY"), GetSliderY, SetSliderY);
    objectTemplate->SetAccessor(v8::String::New("scrollX"), GetScrollX, SetScrollX);
    objectTemplate->SetAccessor(v8::String::New("scrollY"), GetScrollY, SetScrollY);
    objectTemplate->SetAccessor(v8::String::New("contentWidth"), GetContentWidth, SetContentWidth);
    objectTemplate->SetAccessor(v8::String::New("contentHeight"), GetContentHeight, SetContentHeight);
    
    functionTemplate->Inherit(scriptGUIFramedView.functionTemplate);
}



ScriptGUIView scriptGUIView;
ScriptGUIFramedView scriptGUIFramedView;
ScriptGUIScreenView scriptGUIScreenView;
ScriptGUIScrollView scriptGUIScrollView;