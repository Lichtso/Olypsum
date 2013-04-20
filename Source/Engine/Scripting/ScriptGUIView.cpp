//
//  ScriptGUIView.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 18.04.13.
//
//

#include "ScriptGUIView.h"

v8::Handle<v8::Value> ScriptGUIView::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("GUIView Constructor: Invalid argument"));
    
    GUIView *objectPtr = new GUIView();
    objectPtr->scriptInstance = v8::Persistent<v8::Object>::New(args.This());
    args.This()->SetInternalField(0, v8::External::New(objectPtr));
    getDataOfInstance<GUIView>(args[0])->addChild(objectPtr);
    return args.This();
}

v8::Handle<v8::Value> ScriptGUIView::GetChildCount(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIView* objectPtr = getDataOfInstance<GUIView>(info.This());
    return v8::Integer::New(objectPtr->children.size());
}

v8::Handle<v8::Value> ScriptGUIView::GetChild(uint32_t index, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIView* objectPtr = getDataOfInstance<GUIView>(info.This());
    if(index >= objectPtr->children.size())
        return v8::ThrowException(v8::String::New("GUIView []: Invalid argument"));
    return objectPtr->children[index]->scriptInstance;
}

ScriptGUIView::ScriptGUIView() :ScriptGUIView("GUIView", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("length"), GetChildCount);
    objectTemplate->SetIndexedPropertyHandler(GetChild);
    
    functionTemplate->Inherit(scriptGUIRect.functionTemplate);
}



v8::Handle<v8::Value> ScriptGUIFramedView::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("GUIFramedView Constructor: Invalid argument"));
    
    GUIFramedView* objectPtr = new GUIFramedView();
    objectPtr->scriptInstance = v8::Persistent<v8::Object>::New(args.This());
    args.This()->SetInternalField(0, v8::External::New(objectPtr));
    GUIView* parent = getDataOfInstance<GUIView>(args[0]);
    parent->addChild(objectPtr);
    return args.This();
}

v8::Handle<v8::Value> ScriptGUIFramedView::AccessTopColor(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(args.This());
    if(args.Length() == 1 && scriptQuaternion.isCorrectInstance(args[0])) {
        objectPtr->content.topColor = Color4(scriptQuaternion.getDataOfInstance(args[0]));
        return args[0];
    }else
        return handleScope.Close(scriptQuaternion.newInstance(objectPtr->content.topColor.getQuaternion()));
}

v8::Handle<v8::Value> ScriptGUIFramedView::AccessBottomColor(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(args.This());
    if(args.Length() == 1 && scriptQuaternion.isCorrectInstance(args[0])) {
        objectPtr->content.bottomColor = Color4(scriptQuaternion.getDataOfInstance(args[0]));
        return args[0];
    }else
        return handleScope.Close(scriptQuaternion.newInstance(objectPtr->content.bottomColor.getQuaternion()));
}

v8::Handle<v8::Value> ScriptGUIFramedView::AccessBorderColor(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(args.This());
    if(args.Length() == 1 && scriptQuaternion.isCorrectInstance(args[0])) {
        objectPtr->content.borderColor = Color4(scriptQuaternion.getDataOfInstance(args[0]));
        return args[0];
    }else
        return handleScope.Close(scriptQuaternion.newInstance(objectPtr->content.borderColor.getQuaternion()));
}

v8::Handle<v8::Value> ScriptGUIFramedView::GetInnerShadow(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->content.innerShadow));
}

void ScriptGUIFramedView::SetInnerShadow(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32()) return;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(info.This());
    objectPtr->content.innerShadow = value->IntegerValue();
}

v8::Handle<v8::Value> ScriptGUIFramedView::GetCornerRadius(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->content.cornerRadius));
}

void ScriptGUIFramedView::SetCornerRadius(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    GUIFramedView* objectPtr = getDataOfInstance<GUIFramedView>(info.This());
    objectPtr->content.cornerRadius = value->IntegerValue();
}

ScriptGUIFramedView::ScriptGUIFramedView() :ScriptGUIFramedView("GUIFramedView", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->Set(v8::String::New("topColor"), v8::FunctionTemplate::New(AccessTopColor));
    objectTemplate->Set(v8::String::New("bottomColor"), v8::FunctionTemplate::New(AccessBottomColor));
    objectTemplate->Set(v8::String::New("borderColor"), v8::FunctionTemplate::New(AccessBorderColor));
    objectTemplate->SetAccessor(v8::String::New("innerShadow"), GetInnerShadow, SetInnerShadow);
    objectTemplate->SetAccessor(v8::String::New("cornerRadius"), GetCornerRadius, SetCornerRadius);
    
    functionTemplate->Inherit(scriptGUIView.functionTemplate);
}



v8::Handle<v8::Value> ScriptGUIScreenView::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    
    if(!args.IsConstructCall()) {
        v8::Persistent<v8::Object> instance = currentScreenView->scriptInstance;
        if(instance.IsEmpty()) {
            v8::Handle<v8::Value> external = v8::External::New(currentScreenView);
            instance = v8::Persistent<v8::Object>::New(scriptGUIScreenView.functionTemplate->GetFunction()->NewInstance(1, &external));
            currentScreenView->scriptInstance = instance;
        }
        return instance;
    }
    
    if(args.Length() != 1 || !args[0]->IsExternal())
        return v8::ThrowException(v8::String::New("GUIScreenView Constructor: Class can't be instantiated"));
    
    args.This()->SetInternalField(0, args[0]);
    return args.This();
}

v8::Handle<v8::Value> ScriptGUIScreenView::GetModalView(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIScreenView* objectPtr = getDataOfInstance<GUIScreenView>(info.This());
    if(objectPtr->modalView) return v8::Undefined();
    return objectPtr->modalView->scriptInstance;
}

void ScriptGUIScreenView::SetModalView(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIScreenView* objectPtr = getDataOfInstance<GUIScreenView>(info.This());
    if(property.IsEmpty() || !scriptGUIView.isCorrectInstance(property))
        objectPtr->setModalView(NULL);
    else
        objectPtr->setModalView(scriptGUIView.getDataOfInstance<GUIView>(property));
}

v8::Handle<v8::Value> ScriptGUIScreenView::GetFocused(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUIScreenView* objectPtr = getDataOfInstance<GUIScreenView>(info.This());
    if(objectPtr->focused) return v8::Undefined();
    return objectPtr->focused->scriptInstance;
}

ScriptGUIScreenView::ScriptGUIScreenView() :ScriptGUIScreenView("GUIScreenView", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("modalView"), GetModalView, SetModalView);
    objectTemplate->SetAccessor(v8::String::New("focused"), GetFocused);
    
    functionTemplate->Inherit(scriptGUIView.functionTemplate);
}



ScriptGUIView scriptGUIView;
ScriptGUIFramedView scriptGUIFramedView;
ScriptGUIScreenView scriptGUIScreenView;