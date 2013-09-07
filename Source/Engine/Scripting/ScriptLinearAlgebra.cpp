//
//  ScriptLinearAlgebra.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 30.01.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

void ScriptVector3::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(!args.IsConstructCall())
        return args.ScriptException("Vector3 Constructor: Called as a function");
    
    v8::Handle<v8::String> propertyNames[] = { v8::String::New("x"), v8::String::New("y"), v8::String::New("z") };
    if(args.Length() == 1) {
        v8::Handle<v8::Value> param;
        if(args[0]->IsArray()) {
            v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(args[0]);
            if(array->Length() != 3)
                return args.ScriptException("Vector3 Constructor: Invalid argument");
            for(unsigned int i = 0; i < 3; i ++) {
                param = array->Get(i);
                if(!param->IsNumber())
                    return args.ScriptException("Vector3 Constructor: Invalid argument");
                args.This()->Set(propertyNames[i], param);
            }
            args.GetReturnValue().Set(args.This());
            return;
        }
        
        if(!args[0]->IsObject())
            return args.ScriptException("Vector3 Constructor: Invalid argument");
        v8::Handle<v8::Object> object = v8::Handle<v8::Object>::Cast(args[0]);
        for(unsigned int i = 0; i < 3; i ++) {
            param = object->Get(propertyNames[i]);
            if(!param->IsNumber())
                return args.ScriptException("Vector3 Constructor: Invalid argument");
            args.This()->Set(propertyNames[i], param);
        }
    }else if(args.Length() == 3)
        for(unsigned int i = 0; i < 3; i ++) {
            if(!args[i]->IsNumber())
                return args.ScriptException("Vector3 Constructor: Invalid argument");
            args.This()->Set(propertyNames[i], args[i]);
        }
    
    args.GetReturnValue().Set(args.This());
    return;
}

void ScriptVector3::ToString(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btVector3 vec = getDataOfInstance(args.This());
    args.GetReturnValue().Set(v8::String::New(stringOf(vec).c_str()));
}

void ScriptVector3::ToJSON(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btVector3 vec = getDataOfInstance(args.This());
    v8::Local<v8::Array> array = v8::Array::New(3);
    array->Set(0, v8::Number::New(vec.x()));
    array->Set(1, v8::Number::New(vec.y()));
    array->Set(2, v8::Number::New(vec.z()));
    args.GetReturnValue().Set(array);
}

void ScriptVector3::IndexedPropertyGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    if(index > 2)
        return info.ScriptException("Vector3 []: Invalid index");
    const char* names[] = { "x", "y", "z" };
    info.GetReturnValue().Set(info.This()->GetRealNamedProperty(v8::String::New(names[index])));
}

void ScriptVector3::IndexedPropertySetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    if(index > 2)
        return info.ScriptException("Vector3 []: Invalid index");
    const char* names[] = { "x", "y", "z" };
    info.This()->Set(v8::String::New(names[index]), value);
    info.GetReturnValue().Set(value);
}

void ScriptVector3::GetAngle(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("Vector3 getAngle: Invalid argument");
    btVector3 vecA = getDataOfInstance(args.This()),
    vecB = getDataOfInstance(args[0]);
    args.GetReturnValue().Set(vecA.angle(vecB));
}

void ScriptVector3::GetSum(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("Vector3 getSum: Invalid argument");
    btVector3 vec = getDataOfInstance(args.This());
    vec += getDataOfInstance(args[0]);
    args.GetReturnValue().Set(scriptVector3.newInstance(vec));
}

void ScriptVector3::GetDifference(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("Vector3 getDiff: Invalid argument");
    btVector3 vec = getDataOfInstance(args.This());
    vec -= getDataOfInstance(args[0]);
    args.GetReturnValue().Set(scriptVector3.newInstance(vec));
}

void ScriptVector3::GetProduct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return args.ScriptException("Vector3 getProduct: Too few arguments");
    btVector3 vec = getDataOfInstance(args.This());
    if(scriptVector3.isCorrectInstance(args[0]))
        args.GetReturnValue().Set(scriptVector3.newInstance(vec*getDataOfInstance(args[0])));
    else if(args[0]->IsNumber())
        args.GetReturnValue().Set(scriptVector3.newInstance(vec*args[0]->NumberValue()));
    else
        return args.ScriptException("Vector3 getProduct: Invalid argument");
}

void ScriptVector3::GetQuotient(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return args.ScriptException("Vector3 getQuotient: Too few arguments");
    btVector3 vec = getDataOfInstance(args.This());
    if(scriptVector3.isCorrectInstance(args[0]))
        args.GetReturnValue().Set(scriptVector3.newInstance(vec/getDataOfInstance(args[0])));
    else if(args[0]->IsNumber())
        args.GetReturnValue().Set(scriptVector3.newInstance(vec/args[0]->NumberValue()));
    else
        return args.ScriptException("Vector3 getQuotient: Invalid argument");
}

void ScriptVector3::GetDotProduct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("Vector3 getDot: Invalid argument");
    btVector3 vecA = getDataOfInstance(args.This()),
              vecB = getDataOfInstance(args[0]);
    args.GetReturnValue().Set(vecA.dot(vecB));
}

void ScriptVector3::GetCrossProduct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("Vector3 getCross: Invalid argument");
    btVector3 vecA = getDataOfInstance(args.This()),
              vecB = getDataOfInstance(args[0]);
    args.GetReturnValue().Set(scriptVector3.newInstance(vecA.cross(vecB)));
}

void ScriptVector3::GetLength(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btVector3 vec = getDataOfInstance(args.This());
    args.GetReturnValue().Set(vec.length());
}

void ScriptVector3::GetNormalized(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btVector3 vec = getDataOfInstance(args.This()).normalized();
    args.GetReturnValue().Set(scriptVector3.newInstance(vec));
}

void ScriptVector3::GetInterpolation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 2 || !scriptVector3.isCorrectInstance(args[0]) || !args[1]->IsNumber())
        return args.ScriptException("Vector3 getInterpolation: Invalid argument");
    btVector3 vecA = getDataOfInstance(args.This()),
              vecB = getDataOfInstance(args[0]);
    args.GetReturnValue().Set(scriptVector3.newInstance(vecA.lerp(vecB, args[1]->NumberValue())));
}

void ScriptVector3::Sum(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("Vector3 add: Invalid argument");
    btVector3 vec = getDataOfInstance(args.This());
    vec += getDataOfInstance(args[0]);
    setDataToInstance(args.This(), vec);
    args.GetReturnValue().Set(args.This());
}

void ScriptVector3::Subtract(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("Vector3 sub: Invalid argument");
    btVector3 vec = getDataOfInstance(args.This());
    vec -= getDataOfInstance(args[0]);
    setDataToInstance(args.This(), vec);
    args.GetReturnValue().Set(args.This());
}

void ScriptVector3::Multiply(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return args.ScriptException("Vector3 mult: Too few arguments");
    btVector3 vec = getDataOfInstance(args.This());
    if(scriptVector3.isCorrectInstance(args[0]))
        setDataToInstance(args.This(), vec*getDataOfInstance(args[0]));
    else if(args[0]->IsNumber())
        setDataToInstance(args.This(), vec*args[0]->NumberValue());
    else
        return args.ScriptException("Vector3 mult: Invalid argument");
    args.GetReturnValue().Set(args.This());
}

void ScriptVector3::Divide(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return args.ScriptException("Vector3 divide: Too few arguments");
    btVector3 vec = getDataOfInstance(args.This());
    if(scriptVector3.isCorrectInstance(args[0]))
        setDataToInstance(args.This(), vec/getDataOfInstance(args[0]));
    else if(args[0]->IsNumber())
        setDataToInstance(args.This(), vec/args[0]->NumberValue());
    else
        return args.ScriptException("Vector3 divide: Invalid argument");
    args.GetReturnValue().Set(args.This());
}

void ScriptVector3::CrossProduct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("Vector3 cross: Invalid argument");
    btVector3 vecA = getDataOfInstance(args.This()),
              vecB = getDataOfInstance(args[0]);
    setDataToInstance(args.This(), vecA.cross(vecB));
    args.GetReturnValue().Set(args.This());
}

void ScriptVector3::Normalize(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btVector3 vec = getDataOfInstance(args.This()).normalized();
    setDataToInstance(args.This(), vec);
    args.GetReturnValue().Set(args.This());
}

btVector3 ScriptVector3::getDataOfInstance(const v8::Local<v8::Value>& value) {
    v8::HandleScope handleScope;
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
    return btVector3(object->GetRealNamedProperty(v8::String::New("x"))->NumberValue(),
                     object->GetRealNamedProperty(v8::String::New("y"))->NumberValue(),
                     object->GetRealNamedProperty(v8::String::New("z"))->NumberValue());
}

void ScriptVector3::setDataToInstance(const v8::Local<v8::Value>& value, const btVector3& vec) {
    v8::HandleScope handleScope;
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
    object->Set(v8::String::New("x"), v8::Number::New(vec.x()));
    object->Set(v8::String::New("y"), v8::Number::New(vec.y()));
    object->Set(v8::String::New("z"), v8::Number::New(vec.z()));
}

v8::Local<v8::Object> ScriptVector3::newInstance(const btVector3& vec) {
    v8::HandleScope handleScope;
    v8::Local<v8::Object> object = functionTemplate->GetFunction()->NewInstance();
    setDataToInstance(object, vec);
    return handleScope.Close(object);
}

ScriptVector3::ScriptVector3() :ScriptClass("Vector3", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> instanceTemplate = functionTemplate->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(0);
    instanceTemplate->Set(v8::String::New("x"), v8::Number::New(0.0), (v8::PropertyAttribute)(v8::DontDelete));
    instanceTemplate->Set(v8::String::New("y"), v8::Number::New(0.0), (v8::PropertyAttribute)(v8::DontDelete));
    instanceTemplate->Set(v8::String::New("z"), v8::Number::New(0.0), (v8::PropertyAttribute)(v8::DontDelete));
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetIndexedPropertyHandler(IndexedPropertyGetter, IndexedPropertySetter);
    objectTemplate->Set(v8::String::New("toString"), v8::FunctionTemplate::New(ToString));
    objectTemplate->Set(v8::String::New("toJSON"), v8::FunctionTemplate::New(ToJSON));
    objectTemplate->Set(v8::String::New("getAngle"), v8::FunctionTemplate::New(GetAngle));
    objectTemplate->Set(v8::String::New("getSum"), v8::FunctionTemplate::New(GetSum));
    objectTemplate->Set(v8::String::New("getDiff"), v8::FunctionTemplate::New(GetDifference));
    objectTemplate->Set(v8::String::New("getProduct"), v8::FunctionTemplate::New(GetProduct));
    objectTemplate->Set(v8::String::New("getQuotient"), v8::FunctionTemplate::New(GetQuotient));
    objectTemplate->Set(v8::String::New("getDot"), v8::FunctionTemplate::New(GetDotProduct));
    objectTemplate->Set(v8::String::New("getCross"), v8::FunctionTemplate::New(GetCrossProduct));
    objectTemplate->Set(v8::String::New("getLength"), v8::FunctionTemplate::New(GetLength));
    objectTemplate->Set(v8::String::New("getNormalized"), v8::FunctionTemplate::New(GetNormalized));
    objectTemplate->Set(v8::String::New("getInterpolation"), v8::FunctionTemplate::New(GetInterpolation));
    objectTemplate->Set(v8::String::New("add"), v8::FunctionTemplate::New(Sum));
    objectTemplate->Set(v8::String::New("sub"), v8::FunctionTemplate::New(Subtract));
    objectTemplate->Set(v8::String::New("mult"), v8::FunctionTemplate::New(Multiply));
    objectTemplate->Set(v8::String::New("divide"), v8::FunctionTemplate::New(Divide));
    objectTemplate->Set(v8::String::New("cross"), v8::FunctionTemplate::New(CrossProduct));
    objectTemplate->Set(v8::String::New("normalize"), v8::FunctionTemplate::New(Normalize));
}



void ScriptQuaternion::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(!args.IsConstructCall())
        return args.ScriptException("Quaternion Constructor: Called as a function");
    
    std::vector<float> data;
    switch(args.Length()) {
        case 0:
            args.GetReturnValue().Set(args.This());
            return;
        case 1: {
            v8::Handle<v8::Value> param;
            if(args[0]->IsArray()) {
                v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(args[0]);
                for(unsigned int i = 0; i < array->Length(); i ++) {
                    param = array->Get(i);
                    if(!param->IsNumber()) break;
                    data.push_back(param->NumberValue());
                }
            }else if(args[0]->IsObject()) {
                v8::Handle<v8::String> propertyNames[] = { v8::String::New("x"), v8::String::New("y"), v8::String::New("z"), v8::String::New("w") };
                v8::Handle<v8::Object> object = v8::Handle<v8::Object>::Cast(args[0]);
                for(unsigned int i = 0; i < 4; i ++) {
                    param = object->Get(propertyNames[i]);
                    if(!param->IsNumber()) break;
                    data.push_back(param->NumberValue());
                }
            }else
                return args.ScriptException("Quaternion Constructor: Invalid argument");
        } break;
        case 2: {
            if(!scriptVector3.isCorrectInstance(args[0]) || !args[1]->IsNumber())
                return args.ScriptException("Quaternion Constructor: Invalid argument");
            scriptQuaternion.setDataToInstance(args.This(), btQuaternion(scriptVector3.getDataOfInstance(args[0]), args[1]->NumberValue()));
            args.GetReturnValue().Set(args.This());
            return;
        } break;
        default:
            for(unsigned int i = 0; i < 4; i ++) {
                if(!args[i]->IsNumber()) break;
                data.push_back(args[i]->NumberValue());
            }
        break;
    }
    
    switch(data.size()) {
        case 3:
            scriptQuaternion.setDataToInstance(args.This(), btQuaternion(data[0], data[1], data[2]));
            args.GetReturnValue().Set(args.This());
            return;
        case 4:
            scriptQuaternion.setDataToInstance(args.This(), btQuaternion(data[0], data[1], data[2], data[3]));
            args.GetReturnValue().Set(args.This());
            return;
    }
    
    return args.ScriptException("Quaternion Constructor: Invalid argument");
}

void ScriptQuaternion::toString(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This());
    args.GetReturnValue().Set(v8::String::New(stringOf(quaternion).c_str()));
}

void ScriptQuaternion::toJSON(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This());
    v8::Local<v8::Array> array = v8::Array::New(4);
    array->Set(0, v8::Number::New(quaternion.x()));
    array->Set(1, v8::Number::New(quaternion.y()));
    array->Set(2, v8::Number::New(quaternion.z()));
    array->Set(3, v8::Number::New(quaternion.w()));
    args.GetReturnValue().Set(array);
}

void ScriptQuaternion::IndexedPropertyGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    if(index > 3)
        return info.ScriptException("Quaternion []: Invalid index");
    const char* names[] = { "x", "y", "z", "w" };
    info.GetReturnValue().Set(info.This()->GetRealNamedProperty(v8::String::New(names[index])));
}

void ScriptQuaternion::IndexedPropertySetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    if(index > 3)
        return info.ScriptException("Quaternion []: Invalid index");
    const char* names[] = { "x", "y", "z", "w" };
    info.This()->Set(v8::String::New(names[index]), value);
    info.GetReturnValue().Set(value);
}

void ScriptQuaternion::GetAngle(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This());
    if(args.Length() == 0) {
        args.GetReturnValue().Set(quaternion.getAngle());
        return;
    }
    if(args.Length() != 1 || !scriptQuaternion.isCorrectInstance(args[0]))
        return args.ScriptException("Quaternion getAngle: Invalid argument");
    btQuaternion quaternionB = getDataOfInstance(args[0]);
    args.GetReturnValue().Set(quaternion.angle(quaternionB));
}

void ScriptQuaternion::GetAxis(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This());
    args.GetReturnValue().Set(scriptVector3.newInstance(quaternion.getAxis()));
}

void ScriptQuaternion::GetInverse(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This()).inverse();
    args.GetReturnValue().Set(scriptQuaternion.newInstance(quaternion));
}

void ScriptQuaternion::GetSum(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptQuaternion.isCorrectInstance(args[0]))
        return args.ScriptException("Quaternion getSum: Invalid argument");
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion += getDataOfInstance(args[0]);
    args.GetReturnValue().Set(scriptQuaternion.newInstance(quaternion));
}

void ScriptQuaternion::GetDifference(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptQuaternion.isCorrectInstance(args[0]))
        return args.ScriptException("Quaternion getDiff: Invalid argument");
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion -= getDataOfInstance(args[0]);
    args.GetReturnValue().Set(scriptQuaternion.newInstance(quaternion));
}

void ScriptQuaternion::GetProduct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1)
        return args.ScriptException("Quaternion getProduct: Too few arguments");
    btQuaternion quaternion;
    if(args[0]->IsNumber()) {
        quaternion = getDataOfInstance(args.This())*args[0]->NumberValue();
    }else if(scriptQuaternion.isCorrectInstance(args[0])) {
        quaternion = getDataOfInstance(args.This())*getDataOfInstance(args[0]);
    }else
        return args.ScriptException("Quaternion getProduct: Invalid argument");
    args.GetReturnValue().Set(scriptQuaternion.newInstance(quaternion));
}

void ScriptQuaternion::GetDotProduct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptQuaternion.isCorrectInstance(args[0]))
        return args.ScriptException("Quaternion getDot: Invalid argument");
    btQuaternion quaternionA = getDataOfInstance(args.This()),
                 quaternionB = getDataOfInstance(args[0]);
    args.GetReturnValue().Set(quaternionA.dot(quaternionB));
}

void ScriptQuaternion::GetLength(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This());
    args.GetReturnValue().Set(quaternion.length());
}

void ScriptQuaternion::GetNormalized(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This()).normalized();
    args.GetReturnValue().Set(scriptQuaternion.newInstance(quaternion));
}

void ScriptQuaternion::GetInterpolation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 2 || !scriptQuaternion.isCorrectInstance(args[0]) || !args[1]->IsNumber())
        return args.ScriptException("Quaternion getInterpolation: Invalid argument");
    btQuaternion quaternionA = getDataOfInstance(args.This()),
                 quaternionB = getDataOfInstance(args[0]);
    args.GetReturnValue().Set(scriptQuaternion.newInstance(quaternionA.slerp(quaternionB, args[1]->NumberValue())));
}

void ScriptQuaternion::SetRotation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 2 || !scriptVector3.isCorrectInstance(args[0]) || !args[1]->IsNumber())
        return args.ScriptException("Quaternion setRotation: Invalid argument");
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion.setRotation(scriptVector3.getDataOfInstance(args[0]), args[1]->NumberValue());
    setDataToInstance(args.This(), quaternion);
    args.GetReturnValue().Set(args.This());
}

void ScriptQuaternion::Sum(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptQuaternion.isCorrectInstance(args[0]))
        return args.ScriptException("Quaternion add: Invalid argument");
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion += getDataOfInstance(args[0]);
    setDataToInstance(args.This(), quaternion);
    args.GetReturnValue().Set(args.This());
}

void ScriptQuaternion::Subtract(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptQuaternion.isCorrectInstance(args[0]))
        return args.ScriptException("Quaternion sub: Invalid argument");
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion -= getDataOfInstance(args[0]);
    setDataToInstance(args.This(), quaternion);
    args.GetReturnValue().Set(args.This());
}

void ScriptQuaternion::Multiply(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion;
    if(args.Length() != 1)
        return args.ScriptException("Quaternion mult: Too few arguments");
    if(args[0]->IsNumber()) {
        quaternion = getDataOfInstance(args.This())*args[0]->NumberValue();
    }else if(scriptQuaternion.isCorrectInstance(args[0])) {
        quaternion = getDataOfInstance(args.This())*getDataOfInstance(args[0]);
    }else
        return args.ScriptException("Quaternion mult: Invalid argument");
    setDataToInstance(args.This(), quaternion);
    args.GetReturnValue().Set(args.This());
}

void ScriptQuaternion::Normalize(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This()).normalized();
    setDataToInstance(args.This(), quaternion);
    args.GetReturnValue().Set(args.This());
}

btQuaternion ScriptQuaternion::getDataOfInstance(const v8::Local<v8::Value>& value) {
    v8::HandleScope handleScope;
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
    return btQuaternion(object->GetRealNamedProperty(v8::String::New("x"))->NumberValue(),
                        object->GetRealNamedProperty(v8::String::New("y"))->NumberValue(),
                        object->GetRealNamedProperty(v8::String::New("z"))->NumberValue(),
                        object->GetRealNamedProperty(v8::String::New("w"))->NumberValue());
}

void ScriptQuaternion::setDataToInstance(const v8::Local<v8::Value>& value, const btQuaternion& quaternion) {
    v8::HandleScope handleScope;
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
    object->Set(v8::String::New("x"), v8::Number::New(quaternion.x()));
    object->Set(v8::String::New("y"), v8::Number::New(quaternion.y()));
    object->Set(v8::String::New("z"), v8::Number::New(quaternion.z()));
    object->Set(v8::String::New("w"), v8::Number::New(quaternion.w()));
}

v8::Local<v8::Object> ScriptQuaternion::newInstance(const btQuaternion& quaternion) {
    v8::HandleScope handleScope;
    v8::Local<v8::Object> object = functionTemplate->GetFunction()->NewInstance();
    setDataToInstance(object, quaternion);
    return handleScope.Close(object);
}

ScriptQuaternion::ScriptQuaternion() :ScriptClass("Quaternion", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> instanceTemplate = functionTemplate->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(0);
    instanceTemplate->Set(v8::String::New("x"), v8::Number::New(0.0), (v8::PropertyAttribute)(v8::DontDelete));
    instanceTemplate->Set(v8::String::New("y"), v8::Number::New(0.0), (v8::PropertyAttribute)(v8::DontDelete));
    instanceTemplate->Set(v8::String::New("z"), v8::Number::New(0.0), (v8::PropertyAttribute)(v8::DontDelete));
    instanceTemplate->Set(v8::String::New("w"), v8::Number::New(1.0), (v8::PropertyAttribute)(v8::DontDelete));
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetIndexedPropertyHandler(IndexedPropertyGetter, IndexedPropertySetter);
    objectTemplate->Set(v8::String::New("toString"), v8::FunctionTemplate::New(toString));
    objectTemplate->Set(v8::String::New("toJSON"), v8::FunctionTemplate::New(toJSON));
    objectTemplate->Set(v8::String::New("getAngle"), v8::FunctionTemplate::New(GetAngle));
    objectTemplate->Set(v8::String::New("getAxis"), v8::FunctionTemplate::New(GetAxis));
    objectTemplate->Set(v8::String::New("getInverse"), v8::FunctionTemplate::New(GetInverse));
    objectTemplate->Set(v8::String::New("getSum"), v8::FunctionTemplate::New(GetSum));
    objectTemplate->Set(v8::String::New("getDiff"), v8::FunctionTemplate::New(GetDifference));
    objectTemplate->Set(v8::String::New("getProduct"), v8::FunctionTemplate::New(GetProduct));
    objectTemplate->Set(v8::String::New("getDot"), v8::FunctionTemplate::New(GetDotProduct));
    objectTemplate->Set(v8::String::New("getLength"), v8::FunctionTemplate::New(GetLength));
    objectTemplate->Set(v8::String::New("getNormalized"), v8::FunctionTemplate::New(GetNormalized));
    objectTemplate->Set(v8::String::New("getInterpolation"), v8::FunctionTemplate::New(GetInterpolation));
    objectTemplate->Set(v8::String::New("setRotation"), v8::FunctionTemplate::New(SetRotation));
    objectTemplate->Set(v8::String::New("add"), v8::FunctionTemplate::New(Sum));
    objectTemplate->Set(v8::String::New("sub"), v8::FunctionTemplate::New(Subtract));
    objectTemplate->Set(v8::String::New("mult"), v8::FunctionTemplate::New(Multiply));
    objectTemplate->Set(v8::String::New("normalize"), v8::FunctionTemplate::New(Normalize));
}



void ScriptMatrix4::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(!args.IsConstructCall())
        return args.ScriptException("Matrix4 Constructor: Called as a function");
    
    std::unique_ptr<Matrix4> matrix(new Matrix4());
    if(args.Length() == 0) {
        matrix.get()->setIdentity();
    }else{
        v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(args[0]);
        for(unsigned int i = 0; i < 16; i ++) {
            v8::Handle<v8::Value> param = array->Get(i);
            if(!param->IsNumber())
                return args.ScriptException("Matrix4 Constructor: Invalid argument");
            matrix.get()->values[i] = param->NumberValue();
        }
    }
    
    v8::Persistent<v8::Object> object = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), args.This());
    
    object.MakeWeak<void>(NULL, &Destructor);
    object->SetIndexedPropertiesToExternalArrayData(&matrix.get()->values, v8::kExternalFloatArray, 16);
    object->SetInternalField(0, v8::External::New(matrix.release()));
    v8::V8::AdjustAmountOfExternalAllocatedMemory(sizeof(Matrix4));
    args.GetReturnValue().Set(args.This());
}

void ScriptMatrix4::Destructor(v8::Isolate* isolate, v8::Persistent<v8::Object>* value, void* data) {
    v8::HandleScope handleScope;
    Matrix4* mat = getDataOfInstance(**value);
    v8::V8::AdjustAmountOfExternalAllocatedMemory(-sizeof(Matrix4));
    value->ClearWeak();
    value->Dispose();
    //printf("~ ScriptMatrix4\n");
    delete mat;
}

void ScriptMatrix4::toString(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    Matrix4* mat = getDataOfInstance(args.This());
    args.GetReturnValue().Set(v8::String::New(stringOf(*mat).c_str()));
}

void ScriptMatrix4::toJSON(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    Matrix4* mat = getDataOfInstance(args.This());
    v8::Local<v8::Array> array = v8::Array::New(16);
    for(char i = 0; i < 16; i ++)
        array->Set(i, v8::Number::New(mat->values[i]));
    args.GetReturnValue().Set(array);
}

void ScriptMatrix4::AccessRowX(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    Matrix4* objectPtr = getDataOfInstance(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->x = scriptVector3.getDataOfInstance(args[0]);
        args.GetReturnValue().Set(args[0]);
        return;
    }else
        args.GetReturnValue().Set(scriptVector3.newInstance(objectPtr->x));
}

void ScriptMatrix4::AccessRowY(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    Matrix4* objectPtr = getDataOfInstance(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->y = scriptVector3.getDataOfInstance(args[0]);
        args.GetReturnValue().Set(args[0]);
        return;
    }else
        args.GetReturnValue().Set(scriptVector3.newInstance(objectPtr->y));
}

void ScriptMatrix4::AccessRowZ(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    Matrix4* objectPtr = getDataOfInstance(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->z = scriptVector3.getDataOfInstance(args[0]);
        args.GetReturnValue().Set(args[0]);
        return;
    }else
        args.GetReturnValue().Set(scriptVector3.newInstance(objectPtr->z));
}

void ScriptMatrix4::AccessRowW(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    Matrix4* objectPtr = getDataOfInstance(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->w = scriptVector3.getDataOfInstance(args[0]);
        args.GetReturnValue().Set(args[0]);
        return;
    }else
        args.GetReturnValue().Set(scriptVector3.newInstance(objectPtr->w));
}

void ScriptMatrix4::AccessRotation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    Matrix4* mat = getDataOfInstance(args.This());
    if(args.Length() == 1 && scriptQuaternion.isCorrectInstance(args[0])) {
        btMatrix3x3 basis;
        basis.setRotation(scriptQuaternion.getDataOfInstance(args[0]));
        *mat = Matrix4(basis, mat->w);
        args.GetReturnValue().Set(args[0]);
        return;
    }else{
        btQuaternion quaternion;
        mat->getBTMatrix3x3().getRotation(quaternion);
        args.GetReturnValue().Set(scriptQuaternion.newInstance(quaternion));
    }
}

void ScriptMatrix4::SetIdentity(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    getDataOfInstance(args.This())->setIdentity();
    args.GetReturnValue().Set(args.This());
}

void ScriptMatrix4::GetInverse(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    Matrix4 mat = getDataOfInstance(args.This())->getInverse();
    args.GetReturnValue().Set(scriptMatrix4.newInstance(mat));
}

void ScriptMatrix4::GetProduct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptMatrix4.isCorrectInstance(args[0]))
        return args.ScriptException("Matrix4 getProduct: Invalid argument");
    Matrix4 mat = *getDataOfInstance(args.This()) * *getDataOfInstance(args[0]);
    args.GetReturnValue().Set(scriptMatrix4.newInstance(mat));
}

void ScriptMatrix4::GetRotatedVector(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("Matrix4 getRotated: Invalid argument");
    btMatrix3x3 basis = getDataOfInstance(args.This())->getBTMatrix3x3();
    btVector3 vec = ScriptVector3::getDataOfInstance(args[0]);
    vec.setW(1.0);
    args.GetReturnValue().Set(scriptVector3.newInstance(basis * vec));
}

void ScriptMatrix4::GetTransformedVector(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("Matrix4 getTransformed: Invalid argument");
    Matrix4* mat = getDataOfInstance(args.This());
    btVector3 vec = ScriptVector3::getDataOfInstance(args[0]);
    vec.setW(1.0);
    args.GetReturnValue().Set(scriptVector3.newInstance((*mat)(vec)));
}

void ScriptMatrix4::Multiply(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptMatrix4.isCorrectInstance(args[0]))
        return args.ScriptException("Matrix4 mult: Invalid argument");
    Matrix4* mat = getDataOfInstance(args.This());
    *mat *= *getDataOfInstance(args[0]);
    args.GetReturnValue().Set(args.This());
}

void ScriptMatrix4::Scale(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("Matrix4 scale: Invalid argument");
    Matrix4* mat = getDataOfInstance(args.This());
    mat->scale(ScriptVector3::getDataOfInstance(args[0]));
    args.GetReturnValue().Set(args.This());
}

void ScriptMatrix4::Rotate(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    switch (args.Length()) {
        case 1: {
            if(!scriptQuaternion.isCorrectInstance(args[0]))
                break;
            Matrix4* mat = getDataOfInstance(args.This());
            mat->rotate(ScriptQuaternion::getDataOfInstance(args[0]));
            args.GetReturnValue().Set(args.This());
            return;
        } case 2: {
            if(!scriptVector3.isCorrectInstance(args[0]) || !args[1]->IsNumber())
                break;
            Matrix4* mat = getDataOfInstance(args.This());
            mat->rotate(ScriptVector3::getDataOfInstance(args[0]), args[1]->NumberValue());
            args.GetReturnValue().Set(args.This());
            return;
        }
    }
    return args.ScriptException("Matrix4 rotate: Invalid argument");
}

void ScriptMatrix4::Translate(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("Matrix4 translate: Invalid argument");
    Matrix4* mat = getDataOfInstance(args.This());
    mat->translate(ScriptVector3::getDataOfInstance(args[0]));
    args.GetReturnValue().Set(args.This());
}

Matrix4* ScriptMatrix4::getDataOfInstance(const v8::Local<v8::Value>& value) {
    v8::HandleScope handleScope;
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
    v8::Local<v8::External> mat = v8::Local<v8::External>::Cast(object->GetInternalField(0));
    return static_cast<Matrix4*>(mat->Value());
}

void ScriptMatrix4::setDataToInstance(const v8::Local<v8::Value>& value, const Matrix4& mat) {
    v8::HandleScope handleScope;
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
    *getDataOfInstance(object) = mat;
}

v8::Local<v8::Object> ScriptMatrix4::newInstance(const Matrix4& mat) {
    v8::HandleScope handleScope;
    v8::Local<v8::Object> object = functionTemplate->GetFunction()->NewInstance();
    setDataToInstance(object, mat);
    return handleScope.Close(object);
}

ScriptMatrix4::ScriptMatrix4() :ScriptClass("Matrix4", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> instanceTemplate = functionTemplate->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(1);
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->Set(v8::String::New("toString"), v8::FunctionTemplate::New(toString));
    objectTemplate->Set(v8::String::New("toJSON"), v8::FunctionTemplate::New(toJSON));
    objectTemplate->Set(v8::String::New("x"), v8::FunctionTemplate::New(AccessRowX));
    objectTemplate->Set(v8::String::New("y"), v8::FunctionTemplate::New(AccessRowY));
    objectTemplate->Set(v8::String::New("z"), v8::FunctionTemplate::New(AccessRowZ));
    objectTemplate->Set(v8::String::New("w"), v8::FunctionTemplate::New(AccessRowW));
    objectTemplate->Set(v8::String::New("rotation"), v8::FunctionTemplate::New(AccessRotation));
    objectTemplate->Set(v8::String::New("setIdentity"), v8::FunctionTemplate::New(SetIdentity));
    objectTemplate->Set(v8::String::New("getInverse"), v8::FunctionTemplate::New(GetInverse));
    objectTemplate->Set(v8::String::New("getProduct"), v8::FunctionTemplate::New(GetProduct));
    objectTemplate->Set(v8::String::New("getRotated"), v8::FunctionTemplate::New(GetRotatedVector));
    objectTemplate->Set(v8::String::New("getTransformed"), v8::FunctionTemplate::New(GetTransformedVector));
    objectTemplate->Set(v8::String::New("mult"), v8::FunctionTemplate::New(Multiply));
    objectTemplate->Set(v8::String::New("scale"), v8::FunctionTemplate::New(Scale));
    objectTemplate->Set(v8::String::New("rotate"), v8::FunctionTemplate::New(Rotate));
    objectTemplate->Set(v8::String::New("translate"), v8::FunctionTemplate::New(Translate));
}