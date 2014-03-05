//
//  ScriptLinearAlgebra.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 30.01.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

void ScriptVector3::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(!args.IsConstructCall())
        return ScriptException("Vector3 Constructor: Called as a function");
    
    v8::Handle<v8::String> propertyNames[] = { ScriptString("x"), ScriptString("y"), ScriptString("z") };
    if(args.Length() == 1) {
        v8::Handle<v8::Value> param;
        if(args[0]->IsArray()) {
            v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(args[0]);
            if(array->Length() != 3)
                return ScriptException("Vector3 Constructor: Invalid argument");
            for(unsigned int i = 0; i < 3; i ++) {
                param = array->Get(i);
                if(!param->IsNumber())
                    return ScriptException("Vector3 Constructor: Invalid argument");
                args.This()->Set(propertyNames[i], param);
            }
            ScriptReturn(args.This());
            return;
        }
        
        if(!args[0]->IsObject())
            return ScriptException("Vector3 Constructor: Invalid argument");
        v8::Handle<v8::Object> object = v8::Handle<v8::Object>::Cast(args[0]);
        for(unsigned int i = 0; i < 3; i ++) {
            param = object->Get(propertyNames[i]);
            if(!param->IsNumber())
                return ScriptException("Vector3 Constructor: Invalid argument");
            args.This()->Set(propertyNames[i], param);
        }
    }else if(args.Length() == 3)
        for(unsigned int i = 0; i < 3; i ++) {
            if(!args[i]->IsNumber())
                return ScriptException("Vector3 Constructor: Invalid argument");
            args.This()->Set(propertyNames[i], args[i]);
        }
    
    ScriptReturn(args.This());
    return;
}

void ScriptVector3::ToString(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btVector3 vec = getDataOfInstance(args.This());
    ScriptReturn(ScriptString(stringOf(vec).c_str()));
}

void ScriptVector3::ToJSON(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btVector3 vec = getDataOfInstance(args.This());
    v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), 3);
    array->Set(0, v8::Number::New(v8::Isolate::GetCurrent(), vec.x()));
    array->Set(1, v8::Number::New(v8::Isolate::GetCurrent(), vec.y()));
    array->Set(2, v8::Number::New(v8::Isolate::GetCurrent(), vec.z()));
    ScriptReturn(array);
}

void ScriptVector3::IndexedPropertyGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(index > 2)
        return ScriptException("Vector3 []: Invalid index");
    const char* names[] = { "x", "y", "z" };
    ScriptReturn(args.This()->GetRealNamedProperty(ScriptString(names[index])));
}

void ScriptVector3::IndexedPropertySetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(index > 2)
        return ScriptException("Vector3 []: Invalid index");
    const char* names[] = { "x", "y", "z" };
    args.This()->Set(ScriptString(names[index]), value);
    ScriptReturn(value);
}

void ScriptVector3::GetAngle(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptVector3->isCorrectInstance(args[0]))
        return ScriptException("Vector3 getAngle: Invalid argument");
    btVector3 vecA = getDataOfInstance(args.This()),
    vecB = getDataOfInstance(args[0]);
    ScriptReturn(vecA.angle(vecB));
}

void ScriptVector3::GetSum(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptVector3->isCorrectInstance(args[0]))
        return ScriptException("Vector3 getSum: Invalid argument");
    btVector3 vec = getDataOfInstance(args.This());
    vec += getDataOfInstance(args[0]);
    ScriptReturn(scriptVector3->newInstance(vec));
}

void ScriptVector3::GetDifference(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptVector3->isCorrectInstance(args[0]))
        return ScriptException("Vector3 getDiff: Invalid argument");
    btVector3 vec = getDataOfInstance(args.This());
    vec -= getDataOfInstance(args[0]);
    ScriptReturn(scriptVector3->newInstance(vec));
}

void ScriptVector3::GetProduct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 1)
        return ScriptException("Vector3 getProduct: Too few arguments");
    btVector3 vec = getDataOfInstance(args.This());
    if(scriptVector3->isCorrectInstance(args[0]))
        ScriptReturn(scriptVector3->newInstance(vec*getDataOfInstance(args[0])));
    else if(args[0]->IsNumber())
        ScriptReturn(scriptVector3->newInstance(vec*args[0]->NumberValue()));
    else
        return ScriptException("Vector3 getProduct: Invalid argument");
}

void ScriptVector3::GetQuotient(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 1)
        return ScriptException("Vector3 getQuotient: Too few arguments");
    btVector3 vec = getDataOfInstance(args.This());
    if(scriptVector3->isCorrectInstance(args[0]))
        ScriptReturn(scriptVector3->newInstance(vec/getDataOfInstance(args[0])));
    else if(args[0]->IsNumber())
        ScriptReturn(scriptVector3->newInstance(vec/args[0]->NumberValue()));
    else
        return ScriptException("Vector3 getQuotient: Invalid argument");
}

void ScriptVector3::GetDotProduct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptVector3->isCorrectInstance(args[0]))
        return ScriptException("Vector3 getDot: Invalid argument");
    btVector3 vecA = getDataOfInstance(args.This()),
              vecB = getDataOfInstance(args[0]);
    ScriptReturn(vecA.dot(vecB));
}

void ScriptVector3::GetCrossProduct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptVector3->isCorrectInstance(args[0]))
        return ScriptException("Vector3 getCross: Invalid argument");
    btVector3 vecA = getDataOfInstance(args.This()),
              vecB = getDataOfInstance(args[0]);
    ScriptReturn(scriptVector3->newInstance(vecA.cross(vecB)));
}

void ScriptVector3::GetLength(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btVector3 vec = getDataOfInstance(args.This());
    ScriptReturn(vec.length());
}

void ScriptVector3::GetNormalized(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btVector3 vec = getDataOfInstance(args.This()).normalized();
    ScriptReturn(scriptVector3->newInstance(vec));
}

void ScriptVector3::GetInterpolation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 2 || !scriptVector3->isCorrectInstance(args[0]) || !args[1]->IsNumber())
        return ScriptException("Vector3 getInterpolation: Invalid argument");
    btVector3 vecA = getDataOfInstance(args.This()),
              vecB = getDataOfInstance(args[0]);
    ScriptReturn(scriptVector3->newInstance(vecA.lerp(vecB, args[1]->NumberValue())));
}

void ScriptVector3::Sum(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptVector3->isCorrectInstance(args[0]))
        return ScriptException("Vector3 add: Invalid argument");
    btVector3 vec = getDataOfInstance(args.This());
    vec += getDataOfInstance(args[0]);
    setDataToInstance(args.This(), vec);
    ScriptReturn(args.This());
}

void ScriptVector3::Subtract(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptVector3->isCorrectInstance(args[0]))
        return ScriptException("Vector3 sub: Invalid argument");
    btVector3 vec = getDataOfInstance(args.This());
    vec -= getDataOfInstance(args[0]);
    setDataToInstance(args.This(), vec);
    ScriptReturn(args.This());
}

void ScriptVector3::Multiply(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 1)
        return ScriptException("Vector3 mult: Too few arguments");
    btVector3 vec = getDataOfInstance(args.This());
    if(scriptVector3->isCorrectInstance(args[0]))
        setDataToInstance(args.This(), vec*getDataOfInstance(args[0]));
    else if(args[0]->IsNumber())
        setDataToInstance(args.This(), vec*args[0]->NumberValue());
    else
        return ScriptException("Vector3 mult: Invalid argument");
    ScriptReturn(args.This());
}

void ScriptVector3::Divide(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 1)
        return ScriptException("Vector3 divide: Too few arguments");
    btVector3 vec = getDataOfInstance(args.This());
    if(scriptVector3->isCorrectInstance(args[0]))
        setDataToInstance(args.This(), vec/getDataOfInstance(args[0]));
    else if(args[0]->IsNumber())
        setDataToInstance(args.This(), vec/args[0]->NumberValue());
    else
        return ScriptException("Vector3 divide: Invalid argument");
    ScriptReturn(args.This());
}

void ScriptVector3::CrossProduct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptVector3->isCorrectInstance(args[0]))
        return ScriptException("Vector3 cross: Invalid argument");
    btVector3 vecA = getDataOfInstance(args.This()),
              vecB = getDataOfInstance(args[0]);
    setDataToInstance(args.This(), vecA.cross(vecB));
    ScriptReturn(args.This());
}

void ScriptVector3::Normalize(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btVector3 vec = getDataOfInstance(args.This()).normalized();
    setDataToInstance(args.This(), vec);
    ScriptReturn(args.This());
}

btVector3 ScriptVector3::getDataOfInstance(const v8::Local<v8::Value>& value) {
    ScriptScope();
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
    return btVector3(object->GetRealNamedProperty(ScriptString("x"))->NumberValue(),
                     object->GetRealNamedProperty(ScriptString("y"))->NumberValue(),
                     object->GetRealNamedProperty(ScriptString("z"))->NumberValue());
}

void ScriptVector3::setDataToInstance(const v8::Local<v8::Value>& value, const btVector3& vec) {
    ScriptScope();
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
    object->Set(ScriptString("x"), v8::Number::New(v8::Isolate::GetCurrent(), vec.x()));
    object->Set(ScriptString("y"), v8::Number::New(v8::Isolate::GetCurrent(), vec.y()));
    object->Set(ScriptString("z"), v8::Number::New(v8::Isolate::GetCurrent(), vec.z()));
}

v8::Local<v8::Object> ScriptVector3::newInstance(const btVector3& vec) {
    //ScriptScope();
    v8::Local<v8::Object> object = (*functionTemplate)->GetFunction()->NewInstance();
    setDataToInstance(object, vec);
    return object;//handleScope.Close(object);
}

ScriptVector3::ScriptVector3() :ScriptClass("Vector3", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> instanceTemplate = (*functionTemplate)->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(0);
    instanceTemplate->Set(ScriptString("x"), v8::Number::New(v8::Isolate::GetCurrent(), 0.0), (v8::PropertyAttribute)(v8::DontDelete));
    instanceTemplate->Set(ScriptString("y"), v8::Number::New(v8::Isolate::GetCurrent(), 0.0), (v8::PropertyAttribute)(v8::DontDelete));
    instanceTemplate->Set(ScriptString("z"), v8::Number::New(v8::Isolate::GetCurrent(), 0.0), (v8::PropertyAttribute)(v8::DontDelete));
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    objectTemplate->SetIndexedPropertyHandler(IndexedPropertyGetter, IndexedPropertySetter);
    objectTemplate->Set(ScriptString("toString"), ScriptMethod(ToString));
    objectTemplate->Set(ScriptString("toJSON"), ScriptMethod(ToJSON));
    objectTemplate->Set(ScriptString("getAngle"), ScriptMethod(GetAngle));
    objectTemplate->Set(ScriptString("getSum"), ScriptMethod(GetSum));
    objectTemplate->Set(ScriptString("getDiff"), ScriptMethod(GetDifference));
    objectTemplate->Set(ScriptString("getProduct"), ScriptMethod(GetProduct));
    objectTemplate->Set(ScriptString("getQuotient"), ScriptMethod(GetQuotient));
    objectTemplate->Set(ScriptString("getDot"), ScriptMethod(GetDotProduct));
    objectTemplate->Set(ScriptString("getCross"), ScriptMethod(GetCrossProduct));
    objectTemplate->Set(ScriptString("getLength"), ScriptMethod(GetLength));
    objectTemplate->Set(ScriptString("getNormalized"), ScriptMethod(GetNormalized));
    objectTemplate->Set(ScriptString("getInterpolation"), ScriptMethod(GetInterpolation));
    objectTemplate->Set(ScriptString("add"), ScriptMethod(Sum));
    objectTemplate->Set(ScriptString("sub"), ScriptMethod(Subtract));
    objectTemplate->Set(ScriptString("mult"), ScriptMethod(Multiply));
    objectTemplate->Set(ScriptString("divide"), ScriptMethod(Divide));
    objectTemplate->Set(ScriptString("cross"), ScriptMethod(CrossProduct));
    objectTemplate->Set(ScriptString("normalize"), ScriptMethod(Normalize));
}



void ScriptQuaternion::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(!args.IsConstructCall())
        return ScriptException("Quaternion Constructor: Called as a function");
    
    std::vector<float> data;
    switch(args.Length()) {
        case 0:
            ScriptReturn(args.This());
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
                v8::Handle<v8::String> propertyNames[] = { ScriptString("x"), ScriptString("y"), ScriptString("z"), ScriptString("w") };
                v8::Handle<v8::Object> object = v8::Handle<v8::Object>::Cast(args[0]);
                for(unsigned int i = 0; i < 4; i ++) {
                    param = object->Get(propertyNames[i]);
                    if(!param->IsNumber()) break;
                    data.push_back(param->NumberValue());
                }
            }else
                return ScriptException("Quaternion Constructor: Invalid argument");
        } break;
        case 2: {
            if(!scriptVector3->isCorrectInstance(args[0]) || !args[1]->IsNumber())
                return ScriptException("Quaternion Constructor: Invalid argument");
            scriptQuaternion->setDataToInstance(args.This(), btQuaternion(scriptVector3->getDataOfInstance(args[0]), args[1]->NumberValue()));
            ScriptReturn(args.This());
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
            scriptQuaternion->setDataToInstance(args.This(), btQuaternion(data[0], data[1], data[2]));
            ScriptReturn(args.This());
            return;
        case 4:
            scriptQuaternion->setDataToInstance(args.This(), btQuaternion(data[0], data[1], data[2], data[3]));
            ScriptReturn(args.This());
            return;
    }
    
    return ScriptException("Quaternion Constructor: Invalid argument");
}

void ScriptQuaternion::toString(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btQuaternion quaternion = getDataOfInstance(args.This());
    ScriptReturn(ScriptString(stringOf(quaternion).c_str()));
}

void ScriptQuaternion::toJSON(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btQuaternion quaternion = getDataOfInstance(args.This());
    v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), 4);
    array->Set(0, v8::Number::New(v8::Isolate::GetCurrent(), quaternion.x()));
    array->Set(1, v8::Number::New(v8::Isolate::GetCurrent(), quaternion.y()));
    array->Set(2, v8::Number::New(v8::Isolate::GetCurrent(), quaternion.z()));
    array->Set(3, v8::Number::New(v8::Isolate::GetCurrent(), quaternion.w()));
    ScriptReturn(array);
}

void ScriptQuaternion::IndexedPropertyGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(index > 3)
        return ScriptException("Quaternion []: Invalid index");
    const char* names[] = { "x", "y", "z", "w" };
    ScriptReturn(args.This()->GetRealNamedProperty(ScriptString(names[index])));
}

void ScriptQuaternion::IndexedPropertySetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(index > 3)
        return ScriptException("Quaternion []: Invalid index");
    const char* names[] = { "x", "y", "z", "w" };
    args.This()->Set(ScriptString(names[index]), value);
    ScriptReturn(value);
}

void ScriptQuaternion::GetAngle(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btQuaternion quaternion = getDataOfInstance(args.This());
    if(args.Length() == 0) {
        ScriptReturn(quaternion.getAngle());
        return;
    }
    if(args.Length() != 1 || !scriptQuaternion->isCorrectInstance(args[0]))
        return ScriptException("Quaternion getAngle: Invalid argument");
    btQuaternion quaternionB = getDataOfInstance(args[0]);
    ScriptReturn(quaternion.angle(quaternionB));
}

void ScriptQuaternion::GetAxis(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btQuaternion quaternion = getDataOfInstance(args.This());
    ScriptReturn(scriptVector3->newInstance(quaternion.getAxis()));
}

void ScriptQuaternion::GetInverse(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btQuaternion quaternion = getDataOfInstance(args.This()).inverse();
    ScriptReturn(scriptQuaternion->newInstance(quaternion));
}

void ScriptQuaternion::GetSum(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptQuaternion->isCorrectInstance(args[0]))
        return ScriptException("Quaternion getSum: Invalid argument");
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion += getDataOfInstance(args[0]);
    ScriptReturn(scriptQuaternion->newInstance(quaternion));
}

void ScriptQuaternion::GetDifference(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptQuaternion->isCorrectInstance(args[0]))
        return ScriptException("Quaternion getDiff: Invalid argument");
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion -= getDataOfInstance(args[0]);
    ScriptReturn(scriptQuaternion->newInstance(quaternion));
}

void ScriptQuaternion::GetProduct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1)
        return ScriptException("Quaternion getProduct: Too few arguments");
    btQuaternion quaternion;
    if(args[0]->IsNumber()) {
        quaternion = getDataOfInstance(args.This())*args[0]->NumberValue();
    }else if(scriptQuaternion->isCorrectInstance(args[0])) {
        quaternion = getDataOfInstance(args.This())*getDataOfInstance(args[0]);
    }else
        return ScriptException("Quaternion getProduct: Invalid argument");
    ScriptReturn(scriptQuaternion->newInstance(quaternion));
}

void ScriptQuaternion::GetDotProduct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptQuaternion->isCorrectInstance(args[0]))
        return ScriptException("Quaternion getDot: Invalid argument");
    btQuaternion quaternionA = getDataOfInstance(args.This()),
                 quaternionB = getDataOfInstance(args[0]);
    ScriptReturn(quaternionA.dot(quaternionB));
}

void ScriptQuaternion::GetLength(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btQuaternion quaternion = getDataOfInstance(args.This());
    ScriptReturn(quaternion.length());
}

void ScriptQuaternion::GetNormalized(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btQuaternion quaternion = getDataOfInstance(args.This()).normalized();
    ScriptReturn(scriptQuaternion->newInstance(quaternion));
}

void ScriptQuaternion::GetInterpolation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 2 || !scriptQuaternion->isCorrectInstance(args[0]) || !args[1]->IsNumber())
        return ScriptException("Quaternion getInterpolation: Invalid argument");
    btQuaternion quaternionA = getDataOfInstance(args.This()),
                 quaternionB = getDataOfInstance(args[0]);
    ScriptReturn(scriptQuaternion->newInstance(quaternionA.slerp(quaternionB, args[1]->NumberValue())));
}

void ScriptQuaternion::SetRotation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 2 || !scriptVector3->isCorrectInstance(args[0]) || !args[1]->IsNumber())
        return ScriptException("Quaternion setRotation: Invalid argument");
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion.setRotation(scriptVector3->getDataOfInstance(args[0]), args[1]->NumberValue());
    setDataToInstance(args.This(), quaternion);
    ScriptReturn(args.This());
}

void ScriptQuaternion::Sum(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptQuaternion->isCorrectInstance(args[0]))
        return ScriptException("Quaternion add: Invalid argument");
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion += getDataOfInstance(args[0]);
    setDataToInstance(args.This(), quaternion);
    ScriptReturn(args.This());
}

void ScriptQuaternion::Subtract(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptQuaternion->isCorrectInstance(args[0]))
        return ScriptException("Quaternion sub: Invalid argument");
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion -= getDataOfInstance(args[0]);
    setDataToInstance(args.This(), quaternion);
    ScriptReturn(args.This());
}

void ScriptQuaternion::Multiply(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btQuaternion quaternion;
    if(args.Length() != 1)
        return ScriptException("Quaternion mult: Too few arguments");
    if(args[0]->IsNumber()) {
        quaternion = getDataOfInstance(args.This())*args[0]->NumberValue();
    }else if(scriptQuaternion->isCorrectInstance(args[0])) {
        quaternion = getDataOfInstance(args.This())*getDataOfInstance(args[0]);
    }else
        return ScriptException("Quaternion mult: Invalid argument");
    setDataToInstance(args.This(), quaternion);
    ScriptReturn(args.This());
}

void ScriptQuaternion::Normalize(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btQuaternion quaternion = getDataOfInstance(args.This()).normalized();
    setDataToInstance(args.This(), quaternion);
    ScriptReturn(args.This());
}

btQuaternion ScriptQuaternion::getDataOfInstance(const v8::Local<v8::Value>& value) {
    ScriptScope();
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
    return btQuaternion(object->GetRealNamedProperty(ScriptString("x"))->NumberValue(),
                        object->GetRealNamedProperty(ScriptString("y"))->NumberValue(),
                        object->GetRealNamedProperty(ScriptString("z"))->NumberValue(),
                        object->GetRealNamedProperty(ScriptString("w"))->NumberValue());
}

void ScriptQuaternion::setDataToInstance(const v8::Local<v8::Value>& value, const btQuaternion& quaternion) {
    ScriptScope();
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
    object->Set(ScriptString("x"), v8::Number::New(v8::Isolate::GetCurrent(), quaternion.x()));
    object->Set(ScriptString("y"), v8::Number::New(v8::Isolate::GetCurrent(), quaternion.y()));
    object->Set(ScriptString("z"), v8::Number::New(v8::Isolate::GetCurrent(), quaternion.z()));
    object->Set(ScriptString("w"), v8::Number::New(v8::Isolate::GetCurrent(), quaternion.w()));
}

v8::Local<v8::Object> ScriptQuaternion::newInstance(const btQuaternion& quaternion) {
    //ScriptScope();
    v8::Local<v8::Object> object = (*functionTemplate)->GetFunction()->NewInstance();
    setDataToInstance(object, quaternion);
    return object;//handleScope.Close(object);
}

ScriptQuaternion::ScriptQuaternion() :ScriptClass("Quaternion", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> instanceTemplate = (*functionTemplate)->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(0);
    instanceTemplate->Set(ScriptString("x"), v8::Number::New(v8::Isolate::GetCurrent(), 0.0), (v8::PropertyAttribute)(v8::DontDelete));
    instanceTemplate->Set(ScriptString("y"), v8::Number::New(v8::Isolate::GetCurrent(), 0.0), (v8::PropertyAttribute)(v8::DontDelete));
    instanceTemplate->Set(ScriptString("z"), v8::Number::New(v8::Isolate::GetCurrent(), 0.0), (v8::PropertyAttribute)(v8::DontDelete));
    instanceTemplate->Set(ScriptString("w"), v8::Number::New(v8::Isolate::GetCurrent(), 1.0), (v8::PropertyAttribute)(v8::DontDelete));
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    objectTemplate->SetIndexedPropertyHandler(IndexedPropertyGetter, IndexedPropertySetter);
    objectTemplate->Set(ScriptString("toString"), ScriptMethod(toString));
    objectTemplate->Set(ScriptString("toJSON"), ScriptMethod(toJSON));
    objectTemplate->Set(ScriptString("getAngle"), ScriptMethod(GetAngle));
    objectTemplate->Set(ScriptString("getAxis"), ScriptMethod(GetAxis));
    objectTemplate->Set(ScriptString("getInverse"), ScriptMethod(GetInverse));
    objectTemplate->Set(ScriptString("getSum"), ScriptMethod(GetSum));
    objectTemplate->Set(ScriptString("getDiff"), ScriptMethod(GetDifference));
    objectTemplate->Set(ScriptString("getProduct"), ScriptMethod(GetProduct));
    objectTemplate->Set(ScriptString("getDot"), ScriptMethod(GetDotProduct));
    objectTemplate->Set(ScriptString("getLength"), ScriptMethod(GetLength));
    objectTemplate->Set(ScriptString("getNormalized"), ScriptMethod(GetNormalized));
    objectTemplate->Set(ScriptString("getInterpolation"), ScriptMethod(GetInterpolation));
    objectTemplate->Set(ScriptString("setRotation"), ScriptMethod(SetRotation));
    objectTemplate->Set(ScriptString("add"), ScriptMethod(Sum));
    objectTemplate->Set(ScriptString("sub"), ScriptMethod(Subtract));
    objectTemplate->Set(ScriptString("mult"), ScriptMethod(Multiply));
    objectTemplate->Set(ScriptString("normalize"), ScriptMethod(Normalize));
}



void ScriptMatrix4::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(!args.IsConstructCall())
        return ScriptException("Matrix4 Constructor: Called as a function");
    
    std::unique_ptr<Matrix4> matrix(new Matrix4());
    if(args.Length() == 0) {
        matrix.get()->setIdentity();
    }else{
        v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(args[0]);
        for(unsigned int i = 0; i < 16; i ++) {
            v8::Handle<v8::Value> param = array->Get(i);
            if(!param->IsNumber())
                return ScriptException("Matrix4 Constructor: Invalid argument");
            matrix.get()->values[i] = param->NumberValue();
        }
    }
    
    v8::Persistent<v8::Object> object(v8::Isolate::GetCurrent(), args.This());
    object.SetWeak<void>(NULL, &Destructor);
    (*object)->SetIndexedPropertiesToExternalArrayData(&matrix.get()->values, v8::kExternalFloatArray, 16);
    (*object)->SetInternalField(0, v8::External::New(v8::Isolate::GetCurrent(), matrix.release()));
    v8::Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(sizeof(Matrix4));
    ScriptReturn(args.This());
}

void ScriptMatrix4::Destructor(const v8::WeakCallbackData<v8::Object, void>& data) {
    ScriptScope();
    Matrix4* mat = getDataOfInstance(data.GetValue());
    v8::Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(-sizeof(Matrix4));
    //value->ClearWeak();
    //value->Reset();
    //log(info_log, "~ ScriptMatrix4");
    delete mat;
}

void ScriptMatrix4::toString(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    Matrix4* mat = getDataOfInstance(args.This());
    ScriptReturn(ScriptString(stringOf(*mat).c_str()));
}

void ScriptMatrix4::toJSON(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    Matrix4* mat = getDataOfInstance(args.This());
    v8::Local<v8::Array> array = v8::Array::New(v8::Isolate::GetCurrent(), 16);
    for(char i = 0; i < 16; i ++)
        array->Set(i, v8::Number::New(v8::Isolate::GetCurrent(), mat->values[i]));
    ScriptReturn(array);
}

void ScriptMatrix4::AccessRowX(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    Matrix4* objectPtr = getDataOfInstance(args.This());
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        objectPtr->x = scriptVector3->getDataOfInstance(args[0]);
        ScriptReturn(args[0]);
        return;
    }else
        ScriptReturn(scriptVector3->newInstance(objectPtr->x));
}

void ScriptMatrix4::AccessRowY(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    Matrix4* objectPtr = getDataOfInstance(args.This());
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        objectPtr->y = scriptVector3->getDataOfInstance(args[0]);
        ScriptReturn(args[0]);
        return;
    }else
        ScriptReturn(scriptVector3->newInstance(objectPtr->y));
}

void ScriptMatrix4::AccessRowZ(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    Matrix4* objectPtr = getDataOfInstance(args.This());
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        objectPtr->z = scriptVector3->getDataOfInstance(args[0]);
        ScriptReturn(args[0]);
        return;
    }else
        ScriptReturn(scriptVector3->newInstance(objectPtr->z));
}

void ScriptMatrix4::AccessRowW(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    Matrix4* objectPtr = getDataOfInstance(args.This());
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        objectPtr->w = scriptVector3->getDataOfInstance(args[0]);
        ScriptReturn(args[0]);
        return;
    }else
        ScriptReturn(scriptVector3->newInstance(objectPtr->w));
}

void ScriptMatrix4::AccessRotation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    Matrix4* mat = getDataOfInstance(args.This());
    if(args.Length() == 1 && scriptQuaternion->isCorrectInstance(args[0])) {
        btMatrix3x3 basis;
        basis.setRotation(scriptQuaternion->getDataOfInstance(args[0]));
        *mat = Matrix4(basis, mat->w);
        ScriptReturn(args[0]);
        return;
    }else{
        btQuaternion quaternion;
        mat->getBTMatrix3x3().getRotation(quaternion);
        ScriptReturn(scriptQuaternion->newInstance(quaternion));
    }
}

void ScriptMatrix4::SetIdentity(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    getDataOfInstance(args.This())->setIdentity();
    ScriptReturn(args.This());
}

void ScriptMatrix4::GetInverse(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    Matrix4 mat = getDataOfInstance(args.This())->getInverse();
    ScriptReturn(scriptMatrix4->newInstance(mat));
}

void ScriptMatrix4::GetProduct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptMatrix4->isCorrectInstance(args[0]))
        return ScriptException("Matrix4 getProduct: Invalid argument");
    Matrix4 mat = *getDataOfInstance(args.This()) * *getDataOfInstance(args[0]);
    ScriptReturn(scriptMatrix4->newInstance(mat));
}

void ScriptMatrix4::GetRotatedVector(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptVector3->isCorrectInstance(args[0]))
        return ScriptException("Matrix4 getRotated: Invalid argument");
    btMatrix3x3 basis = getDataOfInstance(args.This())->getBTMatrix3x3();
    btVector3 vec = ScriptVector3::getDataOfInstance(args[0]);
    vec.setW(1.0);
    ScriptReturn(scriptVector3->newInstance(basis * vec));
}

void ScriptMatrix4::GetTransformedVector(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptVector3->isCorrectInstance(args[0]))
        return ScriptException("Matrix4 getTransformed: Invalid argument");
    Matrix4* mat = getDataOfInstance(args.This());
    btVector3 vec = ScriptVector3::getDataOfInstance(args[0]);
    vec.setW(1.0);
    ScriptReturn(scriptVector3->newInstance((*mat)(vec)));
}

void ScriptMatrix4::Multiply(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptMatrix4->isCorrectInstance(args[0]))
        return ScriptException("Matrix4 mult: Invalid argument");
    Matrix4* mat = getDataOfInstance(args.This());
    *mat *= *getDataOfInstance(args[0]);
    ScriptReturn(args.This());
}

void ScriptMatrix4::Scale(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptVector3->isCorrectInstance(args[0]))
        return ScriptException("Matrix4 scale: Invalid argument");
    Matrix4* mat = getDataOfInstance(args.This());
    mat->scale(ScriptVector3::getDataOfInstance(args[0]));
    ScriptReturn(args.This());
}

void ScriptMatrix4::Rotate(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    switch (args.Length()) {
        case 1: {
            if(!scriptQuaternion->isCorrectInstance(args[0]))
                break;
            Matrix4* mat = getDataOfInstance(args.This());
            mat->rotate(ScriptQuaternion::getDataOfInstance(args[0]));
            ScriptReturn(args.This());
            return;
        } case 2: {
            if(!scriptVector3->isCorrectInstance(args[0]) || !args[1]->IsNumber())
                break;
            Matrix4* mat = getDataOfInstance(args.This());
            mat->rotate(ScriptVector3::getDataOfInstance(args[0]), args[1]->NumberValue());
            ScriptReturn(args.This());
            return;
        }
    }
    return ScriptException("Matrix4 rotate: Invalid argument");
}

void ScriptMatrix4::Translate(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() != 1 || !scriptVector3->isCorrectInstance(args[0]))
        return ScriptException("Matrix4 translate: Invalid argument");
    Matrix4* mat = getDataOfInstance(args.This());
    mat->translate(ScriptVector3::getDataOfInstance(args[0]));
    ScriptReturn(args.This());
}

Matrix4* ScriptMatrix4::getDataOfInstance(const v8::Local<v8::Value>& value) {
    ScriptScope();
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
    v8::Local<v8::External> mat = v8::Local<v8::External>::Cast(object->GetInternalField(0));
    return static_cast<Matrix4*>(mat->Value());
}

void ScriptMatrix4::setDataToInstance(const v8::Local<v8::Value>& value, const Matrix4& mat) {
    ScriptScope();
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
    *getDataOfInstance(object) = mat;
}

v8::Local<v8::Object> ScriptMatrix4::newInstance(const Matrix4& mat) {
    //ScriptScope();
    v8::Local<v8::Object> object = (*functionTemplate)->GetFunction()->NewInstance();
    setDataToInstance(object, mat);
    return object;//handleScope.Close(object);
}

ScriptMatrix4::ScriptMatrix4() :ScriptClass("Matrix4", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> instanceTemplate = (*functionTemplate)->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(1);
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    objectTemplate->Set(ScriptString("toString"), ScriptMethod(toString));
    objectTemplate->Set(ScriptString("toJSON"), ScriptMethod(toJSON));
    objectTemplate->Set(ScriptString("x"), ScriptMethod(AccessRowX));
    objectTemplate->Set(ScriptString("y"), ScriptMethod(AccessRowY));
    objectTemplate->Set(ScriptString("z"), ScriptMethod(AccessRowZ));
    objectTemplate->Set(ScriptString("w"), ScriptMethod(AccessRowW));
    objectTemplate->Set(ScriptString("rotation"), ScriptMethod(AccessRotation));
    objectTemplate->Set(ScriptString("setIdentity"), ScriptMethod(SetIdentity));
    objectTemplate->Set(ScriptString("getInverse"), ScriptMethod(GetInverse));
    objectTemplate->Set(ScriptString("getProduct"), ScriptMethod(GetProduct));
    objectTemplate->Set(ScriptString("getRotated"), ScriptMethod(GetRotatedVector));
    objectTemplate->Set(ScriptString("getTransformed"), ScriptMethod(GetTransformedVector));
    objectTemplate->Set(ScriptString("mult"), ScriptMethod(Multiply));
    objectTemplate->Set(ScriptString("scale"), ScriptMethod(Scale));
    objectTemplate->Set(ScriptString("rotate"), ScriptMethod(Rotate));
    objectTemplate->Set(ScriptString("translate"), ScriptMethod(Translate));
}