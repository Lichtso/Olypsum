//
//  ScriptLinearAlgebra.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 30.01.13.
//
//

#include "ScriptManager.h"

v8::Handle<v8::Value> ScriptVector3::Constructor(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(!args.IsConstructCall())
        return v8::ThrowException(v8::String::New("Constructor cannot be called as a function."));
    
    v8::Local<v8::String> propertyNames[] = { v8::String::New("x"), v8::String::New("y"), v8::String::New("z") };
    if(args.Length() == 1) {
        v8::Local<v8::Value> param;
        if(args[0]->IsArray()) {
            v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(args[0]);
            if(array->Length() != 3)
                return v8::ThrowException(v8::String::New("Vector3 Constructor: Invalid argument"));
            for(unsigned int i = 0; i < 3; i ++) {
                param = array->Get(i);
                if(!param->IsNumber())
                    return v8::ThrowException(v8::String::New("Vector3 Constructor: Invalid argument"));
                args.This()->Set(propertyNames[i], param);
            }
            return args.This();
        }
        
        if(!args[0]->IsObject())
            return v8::ThrowException(v8::String::New("Vector3 Constructor: Invalid argument"));
        v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(args[0]);
        for(unsigned int i = 0; i < 3; i ++) {
            param = object->Get(propertyNames[i]);
            if(!param->IsNumber())
                return v8::ThrowException(v8::String::New("Vector3 Constructor: Invalid argument"));
            args.This()->Set(propertyNames[i], param);
        }
    }else if(args.Length() == 3)
        for(unsigned int i = 0; i < 3; i ++) {
            if(!args[i]->IsNumber())
                return v8::ThrowException(v8::String::New("Vector3 Constructor: Invalid argument"));
            args.This()->Set(propertyNames[i], args[i]);
        }
    
    return args.This();
}

v8::Handle<v8::Value> ScriptVector3::toString(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btVector3 vec = scriptVector3.getVector3(args.This());
    return handleScope.Close(v8::String::New(stringOf(vec).c_str()));
}

v8::Handle<v8::Value> ScriptVector3::IndexedPropertyGetter(uint32_t index, const v8::AccessorInfo &info) {
    v8::HandleScope handleScope;
    if(index > 2)
        return v8::ThrowException(v8::String::New("Vector3 []: Invalid index"));
    const char* names[] = { "x", "y", "z" };
    return handleScope.Close(info.This()->GetRealNamedProperty(v8::String::New(names[index])));
}

v8::Handle<v8::Value> ScriptVector3::IndexedPropertySetter(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo &info) {
    v8::HandleScope handleScope;
    if(index > 2)
        return v8::ThrowException(v8::String::New("Vector3 []: Invalid index"));
    const char* names[] = { "x", "y", "z" };
    info.This()->Set(v8::String::New(names[index]), value);
    return handleScope.Close(value);
}

v8::Handle<v8::Value> ScriptVector3::GetSum(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isVector3(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 getSum: Invalid argument"));
    btVector3 vec = scriptVector3.getVector3(args.This());
    vec += scriptVector3.getVector3(v8::Local<v8::Object>::Cast(args[0]));
    return handleScope.Close(scriptVector3.newVector3(vec));
}

v8::Handle<v8::Value> ScriptVector3::GetDifference(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isVector3(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 getDiff: Invalid argument"));
    btVector3 vec = scriptVector3.getVector3(args.This());
    vec -= scriptVector3.getVector3(v8::Local<v8::Object>::Cast(args[0]));
    return handleScope.Close(scriptVector3.newVector3(vec));
}

v8::Handle<v8::Value> ScriptVector3::GetProduct(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !args[0]->IsNumber())
        return v8::ThrowException(v8::String::New("Vector3 getProduct: Invalid argument"));
    btVector3 vec = scriptVector3.getVector3(args.This())*args[0]->NumberValue();
    return handleScope.Close(scriptVector3.newVector3(vec));
}

v8::Handle<v8::Value> ScriptVector3::GetDotProduct(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isVector3(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 getDotProduct: Invalid argument"));
    btVector3 vecA = scriptVector3.getVector3(args.This()),
              vecB = scriptVector3.getVector3(v8::Local<v8::Object>::Cast(args[0]));
    return handleScope.Close(v8::Number::New(vecA.dot(vecB)));
}

v8::Handle<v8::Value> ScriptVector3::GetCrossProduct(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isVector3(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 getCrossProduct: Invalid argument"));
    btVector3 vecA = scriptVector3.getVector3(args.This()),
              vecB = scriptVector3.getVector3(v8::Local<v8::Object>::Cast(args[0]));
    return handleScope.Close(scriptVector3.newVector3(vecA.cross(vecB)));
}

v8::Handle<v8::Value> ScriptVector3::GetLength(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btVector3 vec = scriptVector3.getVector3(args.This());
    return handleScope.Close(v8::Number::New(vec.length()));
}

v8::Handle<v8::Value> ScriptVector3::GetNormalized(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btVector3 vec = scriptVector3.getVector3(args.This()).normalized();
    return handleScope.Close(scriptVector3.newVector3(vec));
}

v8::Handle<v8::Value> ScriptVector3::Sum(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isVector3(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 add: Invalid argument"));
    btVector3 vec = scriptVector3.getVector3(args.This());
    vec += scriptVector3.getVector3(v8::Local<v8::Object>::Cast(args[0]));
    scriptVector3.setVector3(args.This(), vec);
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptVector3::Subtract(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isVector3(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 sub: Invalid argument"));
    btVector3 vec = scriptVector3.getVector3(args.This());
    vec -= scriptVector3.getVector3(v8::Local<v8::Object>::Cast(args[0]));
    scriptVector3.setVector3(args.This(), vec);
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptVector3::Multiply(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !args[0]->IsNumber())
        return v8::ThrowException(v8::String::New("Vector3 mult: Invalid argument"));
    btVector3 vec = scriptVector3.getVector3(args.This())*args[0]->NumberValue();
    scriptVector3.setVector3(args.This(), vec);
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptVector3::CrossProduct(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isVector3(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 crossProduct: Invalid argument"));
    btVector3 vecA = scriptVector3.getVector3(args.This()),
              vecB = scriptVector3.getVector3(v8::Local<v8::Object>::Cast(args[0]));
    scriptVector3.setVector3(args.This(), vecA.cross(vecB));
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptVector3::Normalize(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btVector3 vec = scriptVector3.getVector3(args.This()).normalized();
    scriptVector3.setVector3(args.This(), vec);
    return handleScope.Close(args.This());
}

btVector3 ScriptVector3::getVector3(const v8::Local<v8::Object>& object) {
    return btVector3(object->GetRealNamedProperty(v8::String::New("x"))->NumberValue(),
                     object->GetRealNamedProperty(v8::String::New("y"))->NumberValue(),
                     object->GetRealNamedProperty(v8::String::New("z"))->NumberValue());
}

void ScriptVector3::setVector3(const v8::Local<v8::Object>& object, const btVector3& vec) {
    object->Set(v8::String::New("x"), v8::Number::New(vec.x()));
    object->Set(v8::String::New("y"), v8::Number::New(vec.y()));
    object->Set(v8::String::New("z"), v8::Number::New(vec.z()));
}

v8::Local<v8::Object> ScriptVector3::newVector3(const btVector3& vec) {
    v8::HandleScope handleScope;
    v8::Local<v8::Object> object = functionTemplate->GetFunction()->NewInstance();
    setVector3(object, vec);
    return handleScope.Close(object);
}

bool ScriptVector3::isVector3(const v8::Local<v8::Value>& object) {
    if(!object->IsObject()) return false;
    return v8::Local<v8::Object>::Cast(object)->GetConstructor() == functionTemplate->GetFunction();
}

ScriptVector3::ScriptVector3() {
    v8::HandleScope handleScope;
    functionTemplate = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(Constructor));
    functionTemplate->SetClassName(v8::String::New("Vector3"));
    
    v8::Local<v8::ObjectTemplate> instanceTemplate = functionTemplate->InstanceTemplate();
    instanceTemplate->Set(v8::String::New("x"), v8::Number::New(0.0), (v8::PropertyAttribute)(v8::DontDelete));
    instanceTemplate->Set(v8::String::New("y"), v8::Number::New(0.0), (v8::PropertyAttribute)(v8::DontDelete));
    instanceTemplate->Set(v8::String::New("z"), v8::Number::New(0.0), (v8::PropertyAttribute)(v8::DontDelete));
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetIndexedPropertyHandler(IndexedPropertyGetter, IndexedPropertySetter);
    objectTemplate->Set(v8::String::New("toString"), v8::FunctionTemplate::New(toString));
    objectTemplate->Set(v8::String::New("getSum"), v8::FunctionTemplate::New(GetSum));
    objectTemplate->Set(v8::String::New("getDiff"), v8::FunctionTemplate::New(GetDifference));
    objectTemplate->Set(v8::String::New("getProduct"), v8::FunctionTemplate::New(GetProduct));
    objectTemplate->Set(v8::String::New("getDot"), v8::FunctionTemplate::New(GetDotProduct));
    objectTemplate->Set(v8::String::New("getCross"), v8::FunctionTemplate::New(GetCrossProduct));
    objectTemplate->Set(v8::String::New("getLength"), v8::FunctionTemplate::New(GetLength));
    objectTemplate->Set(v8::String::New("getNormalized"), v8::FunctionTemplate::New(GetNormalized));
    objectTemplate->Set(v8::String::New("add"), v8::FunctionTemplate::New(Sum));
    objectTemplate->Set(v8::String::New("sub"), v8::FunctionTemplate::New(Subtract));
    objectTemplate->Set(v8::String::New("mult"), v8::FunctionTemplate::New(Multiply));
    objectTemplate->Set(v8::String::New("cross"), v8::FunctionTemplate::New(CrossProduct));
    objectTemplate->Set(v8::String::New("normalize"), v8::FunctionTemplate::New(Normalize));
}

ScriptVector3::~ScriptVector3() {
    functionTemplate.Dispose();
}

void ScriptVector3::init(const v8::Persistent<v8::ObjectTemplate>& globalTemplate) {
    globalTemplate->Set(v8::String::New("Vector3"), functionTemplate);
}

ScriptVector3 scriptVector3;