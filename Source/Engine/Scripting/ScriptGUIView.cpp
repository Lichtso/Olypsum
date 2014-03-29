//
//  ScriptGUIView.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 18.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptGUIView.h"

void ScriptGUIView::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() != 1 || !scriptGUIView->isCorrectInstance(args[0]))
        return ScriptException("GUIView Constructor: Invalid argument");
    
    GUIView* objectPtr = new GUIView();
    ScriptReturn(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUIView::GetChildCount(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIView* objectPtr = getDataOfInstance<GUIView>(args.This());
    ScriptReturn(v8::Integer::New(v8::Isolate::GetCurrent(), objectPtr->children.size()));
}

void ScriptGUIView::GetChild(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIView* objectPtr = getDataOfInstance<GUIView>(args.This());
    if(index >= objectPtr->children.size())
        return ScriptException("GUIView []: Invalid argument");
    ScriptReturn(objectPtr->children[index]->scriptInstance);
}

void ScriptGUIView::Adopt(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 1 || !scriptGUIRect->isCorrectInstance(args[0]))
        return ScriptException("GUIView adopt(): Child is not a GUIRect");
    GUIView* objectPtr = getDataOfInstance<GUIView>(args.This());
    objectPtr->addChild(getDataOfInstance<GUIRect>(args.This()));
}

ScriptGUIView::ScriptGUIView() :ScriptGUIRect("GUIView", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    objectTemplate->SetIndexedPropertyHandler(GetChild);
    ScriptAccessor(objectTemplate, "length", GetChildCount, 0);
    ScriptMethod(objectTemplate, "adopt", Adopt);
    
    ScriptInherit(scriptGUIRect);
}



void ScriptGUIFramedView::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() != 1 || !scriptGUIView->isCorrectInstance(args[0]))
        return ScriptException("GUIFramedView Constructor: Invalid argument");
    
    GUIFramedView* objectPtr = new GUIFramedView();
    ScriptReturn(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUIFramedView::AccessTopColor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(args.This());
    if(args.Length() == 1 && scriptQuaternion->isCorrectInstance(args[0])) {
        objectPtr->content.topColor = Color4(scriptQuaternion->getDataOfInstance(args[0]));
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptQuaternion->newInstance(objectPtr->content.topColor.getQuaternion()));
}

void ScriptGUIFramedView::AccessBottomColor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(args.This());
    if(args.Length() == 1 && scriptQuaternion->isCorrectInstance(args[0])) {
        objectPtr->content.bottomColor = Color4(scriptQuaternion->getDataOfInstance(args[0]));
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptQuaternion->newInstance(objectPtr->content.bottomColor.getQuaternion()));
}

void ScriptGUIFramedView::AccessBorderColor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(args.This());
    if(args.Length() == 1 && scriptQuaternion->isCorrectInstance(args[0])) {
        objectPtr->content.borderColor = Color4(scriptQuaternion->getDataOfInstance(args[0]));
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptQuaternion->newInstance(objectPtr->content.borderColor.getQuaternion()));
}

void ScriptGUIFramedView::GetInnerShadow(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(args.This());
    ScriptReturn(objectPtr->content.innerShadow);
}

void ScriptGUIFramedView::SetInnerShadow(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsInt32()) return;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(args.This());
    objectPtr->content.innerShadow = value->IntegerValue();
}

void ScriptGUIFramedView::GetCornerRadius(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(args.This());
    ScriptReturn(objectPtr->content.cornerRadius);
}

void ScriptGUIFramedView::SetCornerRadius(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(args.This());
    objectPtr->content.cornerRadius = value->IntegerValue();
}

ScriptGUIFramedView::ScriptGUIFramedView() :ScriptGUIView("GUIFramedView", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "innerShadow", GetInnerShadow, SetInnerShadow);
    ScriptAccessor(objectTemplate, "cornerRadius", GetCornerRadius, SetCornerRadius);
    ScriptMethod(objectTemplate, "topColor", AccessTopColor);
    ScriptMethod(objectTemplate, "bottomColor", AccessBottomColor);
    ScriptMethod(objectTemplate, "borderColor", AccessBorderColor);
    
    ScriptInherit(scriptGUIView);
}



void ScriptGUIScreenView::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(!args.IsConstructCall()) {
        if(menu.screenView->scriptInstance.IsEmpty()) {
            v8::Handle<v8::Value> external = v8::External::New(v8::Isolate::GetCurrent(), menu.screenView);
            menu.screenView->scriptInstance.Reset(v8::Isolate::GetCurrent(), (*scriptGUIScreenView->functionTemplate)->GetFunction()->NewInstance(1, &external));
        }
        ScriptReturn(menu.screenView->scriptInstance);
        return;
    }
    
    if(args.Length() != 1 || !args[0]->IsExternal())
        return ScriptException("GUIScreenView Constructor: Class can't be instantiated");
    
    args.This()->SetInternalField(0, args[0]);
    ScriptReturn(args.This());
}

void ScriptGUIScreenView::GetModalView(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIScreenView* objectPtr = getDataOfInstance<GUIScreenView>(args.This());
    if(objectPtr->modalView)
        ScriptReturn(objectPtr->modalView->scriptInstance);
}

void ScriptGUIScreenView::SetModalView(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    GUIScreenView* objectPtr = getDataOfInstance<GUIScreenView>(args.This());
    if(property.IsEmpty() || !scriptGUIView->isCorrectInstance(property))
        objectPtr->setModalView(NULL);
    else
        objectPtr->setModalView(scriptGUIView->getDataOfInstance<GUIView>(property));
}

void ScriptGUIScreenView::isFocused(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIScreenView* objectPtr = getDataOfInstance<GUIScreenView>(args.This());
    if(objectPtr->focus)
        ScriptReturn(objectPtr->focus->scriptInstance);
}

ScriptGUIScreenView::ScriptGUIScreenView() :ScriptGUIView("GUIScreenView", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "modalView", GetModalView, SetModalView);
    ScriptAccessor(objectTemplate, "focus", isFocused, 0);
    
    ScriptInherit(scriptGUIView);
}



void ScriptGUIScrollView::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() != 1 || !scriptGUIView->isCorrectInstance(args[0]))
        return ScriptException("GUIScrollView Constructor: Invalid argument");
    
    GUIScrollView* objectPtr = new GUIScrollView();
    objectPtr->scriptInstance.Reset(v8::Isolate::GetCurrent(), args.This());
    args.This()->SetInternalField(0, v8::External::New(v8::Isolate::GetCurrent(), objectPtr));
    GUIView* parent = getDataOfInstance<GUIView>(args[0]);
    parent->addChild(objectPtr);
    ScriptReturn(args.This());
}

void ScriptGUIScrollView::GetSliderX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(args.This());
    ScriptReturn(objectPtr->sliderX);
}

void ScriptGUIScrollView::SetSliderX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsBoolean()) return;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(args.This());
    objectPtr->sliderX = value->BooleanValue();
}

void ScriptGUIScrollView::GetSliderY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(args.This());
    ScriptReturn(objectPtr->sliderY);
}

void ScriptGUIScrollView::SetSliderY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsBoolean()) return;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(args.This());
    objectPtr->sliderY = value->BooleanValue();
}

void ScriptGUIScrollView::GetScrollX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(args.This());
    ScriptReturn(objectPtr->scrollPosX);
}

void ScriptGUIScrollView::SetScrollX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsInt32()) return;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(args.This());
    objectPtr->scrollPosX = value->IntegerValue();
}

void ScriptGUIScrollView::GetScrollY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(args.This());
    ScriptReturn(objectPtr->scrollPosY);
}

void ScriptGUIScrollView::SetScrollY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsInt32()) return;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(args.This());
    objectPtr->scrollPosY = value->IntegerValue();
}

void ScriptGUIScrollView::GetContentWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(args.This());
    ScriptReturn(objectPtr->contentWidth);
}

void ScriptGUIScrollView::SetContentWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(args.This());
    objectPtr->contentWidth = value->IntegerValue();
}

void ScriptGUIScrollView::GetContentHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(args.This());
    ScriptReturn(objectPtr->contentHeight);
}

void ScriptGUIScrollView::SetContentHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    GUIScrollView* objectPtr = getDataOfInstance<GUIScrollView>(args.This());
    objectPtr->contentHeight = value->IntegerValue();
}

ScriptGUIScrollView::ScriptGUIScrollView() :ScriptGUIFramedView("GUIScrollView", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "sliderX", GetSliderX, SetSliderX);
    ScriptAccessor(objectTemplate, "sliderY", GetSliderY, SetSliderY);
    ScriptAccessor(objectTemplate, "scrollX", GetScrollX, SetScrollX);
    ScriptAccessor(objectTemplate, "scrollY", GetScrollY, SetScrollY);
    ScriptAccessor(objectTemplate, "contentWidth", GetContentWidth, SetContentWidth);
    ScriptAccessor(objectTemplate, "contentHeight", GetContentHeight, SetContentHeight);
    
    ScriptInherit(scriptGUIFramedView);
}