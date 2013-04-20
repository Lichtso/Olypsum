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
        return v8::ThrowException(v8::String::New("Vector3 Constructor: Called as a function"));
    
    v8::Handle<v8::String> propertyNames[] = { v8::String::New("x"), v8::String::New("y"), v8::String::New("z") };
    if(args.Length() == 1) {
        v8::Handle<v8::Value> param;
        if(args[0]->IsArray()) {
            v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(args[0]);
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
        v8::Handle<v8::Object> object = v8::Handle<v8::Object>::Cast(args[0]);
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
    btVector3 vec = getDataOfInstance(args.This());
    return handleScope.Close(v8::String::New(stringOf(vec).c_str()));
}

v8::Handle<v8::Value> ScriptVector3::toJSON(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btVector3 vec = getDataOfInstance(args.This());
    v8::Local<v8::Array> array = v8::Array::New(3);
    array->Set(0, v8::Number::New(vec.x()));
    array->Set(1, v8::Number::New(vec.y()));
    array->Set(2, v8::Number::New(vec.z()));
    return handleScope.Close(array);
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

v8::Handle<v8::Value> ScriptVector3::GetAngle(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 getAngle: Invalid argument"));
    btVector3 vecA = getDataOfInstance(args.This()),
    vecB = getDataOfInstance(args[0]);
    return handleScope.Close(v8::Number::New(vecA.angle(vecB)));
}

v8::Handle<v8::Value> ScriptVector3::GetSum(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 getSum: Invalid argument"));
    btVector3 vec = getDataOfInstance(args.This());
    vec += getDataOfInstance(args[0]);
    return handleScope.Close(scriptVector3.newInstance(vec));
}

v8::Handle<v8::Value> ScriptVector3::GetDifference(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 getDiff: Invalid argument"));
    btVector3 vec = getDataOfInstance(args.This());
    vec -= getDataOfInstance(args[0]);
    return handleScope.Close(scriptVector3.newInstance(vec));
}

v8::Handle<v8::Value> ScriptVector3::GetProduct(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !args[0]->IsNumber())
        return v8::ThrowException(v8::String::New("Vector3 getProduct: Invalid argument"));
    btVector3 vec = getDataOfInstance(args.This())*args[0]->NumberValue();
    return handleScope.Close(scriptVector3.newInstance(vec));
}

v8::Handle<v8::Value> ScriptVector3::GetDotProduct(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 getDot: Invalid argument"));
    btVector3 vecA = getDataOfInstance(args.This()),
              vecB = getDataOfInstance(args[0]);
    return handleScope.Close(v8::Number::New(vecA.dot(vecB)));
}

v8::Handle<v8::Value> ScriptVector3::GetCrossProduct(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 getCross: Invalid argument"));
    btVector3 vecA = getDataOfInstance(args.This()),
              vecB = getDataOfInstance(args[0]);
    return handleScope.Close(scriptVector3.newInstance(vecA.cross(vecB)));
}

v8::Handle<v8::Value> ScriptVector3::GetLength(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btVector3 vec = getDataOfInstance(args.This());
    return handleScope.Close(v8::Number::New(vec.length()));
}

v8::Handle<v8::Value> ScriptVector3::GetNormalized(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btVector3 vec = getDataOfInstance(args.This()).normalized();
    return handleScope.Close(scriptVector3.newInstance(vec));
}

v8::Handle<v8::Value> ScriptVector3::GetInterpolation(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 2 || !scriptVector3.isCorrectInstance(args[0]) || !args[1]->IsNumber())
        return v8::ThrowException(v8::String::New("Vector3 getInterpolation: Invalid argument"));
    btVector3 vecA = getDataOfInstance(args.This()),
              vecB = getDataOfInstance(args[0]);
    return handleScope.Close(scriptVector3.newInstance(vecA.lerp(vecB, args[1]->NumberValue())));
}

v8::Handle<v8::Value> ScriptVector3::Sum(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 add: Invalid argument"));
    btVector3 vec = getDataOfInstance(args.This());
    vec += getDataOfInstance(args[0]);
    setDataToInstance(args.This(), vec);
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptVector3::Subtract(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 sub: Invalid argument"));
    btVector3 vec = getDataOfInstance(args.This());
    vec -= getDataOfInstance(args[0]);
    setDataToInstance(args.This(), vec);
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptVector3::Multiply(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !args[0]->IsNumber())
        return v8::ThrowException(v8::String::New("Vector3 mult: Invalid argument"));
    btVector3 vec = getDataOfInstance(args.This())*args[0]->NumberValue();
    setDataToInstance(args.This(), vec);
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptVector3::CrossProduct(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Vector3 cross: Invalid argument"));
    btVector3 vecA = getDataOfInstance(args.This()),
              vecB = getDataOfInstance(args[0]);
    setDataToInstance(args.This(), vecA.cross(vecB));
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptVector3::Normalize(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btVector3 vec = getDataOfInstance(args.This()).normalized();
    setDataToInstance(args.This(), vec);
    return handleScope.Close(args.This());
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
    objectTemplate->Set(v8::String::New("toString"), v8::FunctionTemplate::New(toString));
    objectTemplate->Set(v8::String::New("toJSON"), v8::FunctionTemplate::New(toJSON));
    objectTemplate->Set(v8::String::New("getAngle"), v8::FunctionTemplate::New(GetAngle));
    objectTemplate->Set(v8::String::New("getSum"), v8::FunctionTemplate::New(GetSum));
    objectTemplate->Set(v8::String::New("getDiff"), v8::FunctionTemplate::New(GetDifference));
    objectTemplate->Set(v8::String::New("getProduct"), v8::FunctionTemplate::New(GetProduct));
    objectTemplate->Set(v8::String::New("getDot"), v8::FunctionTemplate::New(GetDotProduct));
    objectTemplate->Set(v8::String::New("getCross"), v8::FunctionTemplate::New(GetCrossProduct));
    objectTemplate->Set(v8::String::New("getLength"), v8::FunctionTemplate::New(GetLength));
    objectTemplate->Set(v8::String::New("getNormalized"), v8::FunctionTemplate::New(GetNormalized));
    objectTemplate->Set(v8::String::New("getInterpolation"), v8::FunctionTemplate::New(GetInterpolation));
    objectTemplate->Set(v8::String::New("add"), v8::FunctionTemplate::New(Sum));
    objectTemplate->Set(v8::String::New("sub"), v8::FunctionTemplate::New(Subtract));
    objectTemplate->Set(v8::String::New("mult"), v8::FunctionTemplate::New(Multiply));
    objectTemplate->Set(v8::String::New("cross"), v8::FunctionTemplate::New(CrossProduct));
    objectTemplate->Set(v8::String::New("normalize"), v8::FunctionTemplate::New(Normalize));
}



v8::Handle<v8::Value> ScriptQuaternion::Constructor(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(!args.IsConstructCall())
        return v8::ThrowException(v8::String::New("Quaternion Constructor: Called as a function"));
    
    std::vector<float> data;
    switch (args.Length()) {
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
                return v8::ThrowException(v8::String::New("Quaternion Constructor: Invalid argument"));
        } break;
        case 2: {
            if(!scriptVector3.isCorrectInstance(args[0]) || !args[1]->IsNumber())
                return v8::ThrowException(v8::String::New("Quaternion Constructor: Invalid argument"));
            scriptQuaternion.setDataToInstance(args.This(), btQuaternion(scriptVector3.getDataOfInstance(args[0]), args[1]->NumberValue()));
            return args.This();
        } break;
        default:
            for(unsigned int i = 0; i < 4; i ++) {
                if(!args[i]->IsNumber()) break;
                data.push_back(args[i]->NumberValue());
            }
        break;
    }
    
    switch (data.size()) {
        case 3: {
            scriptQuaternion.setDataToInstance(args.This(), btQuaternion(data[0], data[1], data[2]));
        } return args.This();
        case 4: {
            scriptQuaternion.setDataToInstance(args.This(), btQuaternion(data[0], data[1], data[2], data[3]));
        } return args.This();
    }
    
    return v8::ThrowException(v8::String::New("Quaternion Constructor: Invalid argument"));
}

v8::Handle<v8::Value> ScriptQuaternion::toString(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This());
    return handleScope.Close(v8::String::New(stringOf(quaternion).c_str()));
}

v8::Handle<v8::Value> ScriptQuaternion::toJSON(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This());
    v8::Local<v8::Array> array = v8::Array::New(4);
    array->Set(0, v8::Number::New(quaternion.x()));
    array->Set(1, v8::Number::New(quaternion.y()));
    array->Set(2, v8::Number::New(quaternion.z()));
    array->Set(3, v8::Number::New(quaternion.w()));
    return handleScope.Close(array);
}

v8::Handle<v8::Value> ScriptQuaternion::IndexedPropertyGetter(uint32_t index, const v8::AccessorInfo &info) {
    v8::HandleScope handleScope;
    if(index > 3)
        return v8::ThrowException(v8::String::New("Quaternion []: Invalid index"));
    const char* names[] = { "x", "y", "z", "w" };
    return handleScope.Close(info.This()->GetRealNamedProperty(v8::String::New(names[index])));
}

v8::Handle<v8::Value> ScriptQuaternion::IndexedPropertySetter(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo &info) {
    v8::HandleScope handleScope;
    if(index > 3)
        return v8::ThrowException(v8::String::New("Quaternion []: Invalid index"));
    const char* names[] = { "x", "y", "z", "w" };
    info.This()->Set(v8::String::New(names[index]), value);
    return handleScope.Close(value);
}

v8::Handle<v8::Value> ScriptQuaternion::GetAngle(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This());
    if(args.Length() == 0)
        return handleScope.Close(v8::Number::New(quaternion.getAngle()));
    if(args.Length() != 1 || !scriptQuaternion.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Quaternion getAngle: Invalid argument"));
    btQuaternion quaternionB = getDataOfInstance(args[0]);
    return handleScope.Close(v8::Number::New(quaternion.angle(quaternionB)));
}

v8::Handle<v8::Value> ScriptQuaternion::GetAxis(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This());
    return handleScope.Close(scriptVector3.newInstance(quaternion.getAxis()));
}

v8::Handle<v8::Value> ScriptQuaternion::GetInverse(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This()).inverse();
    return handleScope.Close(scriptQuaternion.newInstance(quaternion));
}

v8::Handle<v8::Value> ScriptQuaternion::GetSum(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptQuaternion.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Quaternion getSum: Invalid argument"));
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion += getDataOfInstance(args[0]);
    return handleScope.Close(scriptQuaternion.newInstance(quaternion));
}

v8::Handle<v8::Value> ScriptQuaternion::GetDifference(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptQuaternion.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Quaternion getDiff: Invalid argument"));
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion -= getDataOfInstance(args[0]);
    return handleScope.Close(scriptQuaternion.newInstance(quaternion));
}

v8::Handle<v8::Value> ScriptQuaternion::GetProduct(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1)
        return v8::ThrowException(v8::String::New("Quaternion getProduct: To less arguments"));
    btQuaternion quaternion;
    if(args[0]->IsNumber()) {
        quaternion = getDataOfInstance(args.This())*args[0]->NumberValue();
    }else if(scriptQuaternion.isCorrectInstance(args[0])) {
        quaternion = getDataOfInstance(args.This())*getDataOfInstance(args[0]);
    }else
        return v8::ThrowException(v8::String::New("Quaternion getProduct: Invalid argument"));
    return handleScope.Close(scriptQuaternion.newInstance(quaternion));
}

v8::Handle<v8::Value> ScriptQuaternion::GetDotProduct(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptQuaternion.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Quaternion getDot: Invalid argument"));
    btQuaternion quaternionA = getDataOfInstance(args.This()),
                 quaternionB = getDataOfInstance(args[0]);
    return handleScope.Close(v8::Number::New(quaternionA.dot(quaternionB)));
}

v8::Handle<v8::Value> ScriptQuaternion::GetLength(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This());
    return handleScope.Close(v8::Number::New(quaternion.length()));
}

v8::Handle<v8::Value> ScriptQuaternion::GetNormalized(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This()).normalized();
    return handleScope.Close(scriptQuaternion.newInstance(quaternion));
}

v8::Handle<v8::Value> ScriptQuaternion::GetInterpolation(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 2 || !scriptQuaternion.isCorrectInstance(args[0]) || !args[1]->IsNumber())
        return v8::ThrowException(v8::String::New("Quaternion getInterpolation: Invalid argument"));
    btQuaternion quaternionA = getDataOfInstance(args.This()),
                 quaternionB = getDataOfInstance(args[0]);
    return handleScope.Close(scriptQuaternion.newInstance(quaternionA.slerp(quaternionB, args[1]->NumberValue())));
}

v8::Handle<v8::Value> ScriptQuaternion::SetRotation(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 2 || !scriptVector3.isCorrectInstance(args[0]) || !args[1]->IsNumber())
        return v8::ThrowException(v8::String::New("Quaternion setRotation: Invalid argument"));
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion.setRotation(scriptVector3.getDataOfInstance(args[0]), args[1]->NumberValue());
    setDataToInstance(args.This(), quaternion);
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptQuaternion::SetEuler(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This());
    switch(args.Length()) {
        case 1: {
            if(!args[0]->IsArray()) break;
            v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(args[0]);
            if(array->Length() != 3 || !array->Get(0)->IsNumber() || !array->Get(1)->IsNumber() || !array->Get(2)->IsNumber()) break;
            quaternion.setEuler(array->Get(0)->NumberValue(), array->Get(1)->NumberValue(), array->Get(2)->NumberValue());
            return handleScope.Close(args.This());
        } case 3:
            if(!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber()) break;
            quaternion.setEuler(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue());
            return handleScope.Close(args.This());
    }
    return v8::ThrowException(v8::String::New("Quaternion setRotation: Invalid argument"));
}

v8::Handle<v8::Value> ScriptQuaternion::Sum(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptQuaternion.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Quaternion add: Invalid argument"));
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion += getDataOfInstance(args[0]);
    setDataToInstance(args.This(), quaternion);
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptQuaternion::Subtract(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptQuaternion.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Quaternion sub: Invalid argument"));
    btQuaternion quaternion = getDataOfInstance(args.This());
    quaternion -= getDataOfInstance(args[0]);
    setDataToInstance(args.This(), quaternion);
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptQuaternion::Multiply(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion;
    if(args.Length() != 1)
        return v8::ThrowException(v8::String::New("Quaternion mult: To less arguments"));
    if(args[0]->IsNumber()) {
        quaternion = getDataOfInstance(args.This())*args[0]->NumberValue();
    }else if(scriptQuaternion.isCorrectInstance(args[0])) {
        quaternion = getDataOfInstance(args.This())*getDataOfInstance(args[0]);
    }else
        return v8::ThrowException(v8::String::New("Quaternion mult: Invalid argument"));
    setDataToInstance(args.This(), quaternion);
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptQuaternion::Normalize(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion = getDataOfInstance(args.This()).normalized();
    setDataToInstance(args.This(), quaternion);
    return handleScope.Close(args.This());
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
    objectTemplate->Set(v8::String::New("setEuler"), v8::FunctionTemplate::New(SetEuler));
    objectTemplate->Set(v8::String::New("add"), v8::FunctionTemplate::New(Sum));
    objectTemplate->Set(v8::String::New("sub"), v8::FunctionTemplate::New(Subtract));
    objectTemplate->Set(v8::String::New("mult"), v8::FunctionTemplate::New(Multiply));
    objectTemplate->Set(v8::String::New("normalize"), v8::FunctionTemplate::New(Normalize));
}



v8::Handle<v8::Value> ScriptMatrix4::Constructor(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(!args.IsConstructCall())
        return v8::ThrowException(v8::String::New("Matrix4 Constructor: Called as a function"));
    
    std::unique_ptr<Matrix4> matrix(new Matrix4());
    if(args.Length() == 0) {
        matrix.get()->setIdentity();
    }else{
        v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(args[0]);
        for(unsigned int i = 0; i < 16; i ++) {
            v8::Handle<v8::Value> param = array->Get(i);
            if(!param->IsNumber())
                return v8::ThrowException(v8::String::New("Matrix4 Constructor: Invalid argument"));
            matrix.get()->values[i] = param->NumberValue();
        }
    }
    
    v8::Persistent<v8::Object> object = v8::Persistent<v8::Object>::New(args.This());
    object.MakeWeak(NULL, &Destructor);
    object->SetIndexedPropertiesToExternalArrayData(&matrix.get()->values, v8::kExternalFloatArray, 16);
    object->SetInternalField(0, v8::External::New(matrix.release()));
    v8::V8::AdjustAmountOfExternalAllocatedMemory(sizeof(Matrix4));
    return object;
}

void ScriptMatrix4::Destructor(v8::Persistent<v8::Value> value, void* data) {
    v8::HandleScope handleScope;
    v8::Persistent<v8::Object> object = v8::Persistent<v8::Object>::Cast(value);
    Matrix4* mat = getDataOfInstance(*object);
    v8::V8::AdjustAmountOfExternalAllocatedMemory(-sizeof(Matrix4));
    object.ClearWeak();
    object.Dispose();
    delete mat;
}

v8::Handle<v8::Value> ScriptMatrix4::toString(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    Matrix4* mat = getDataOfInstance(args.This());
    return handleScope.Close(v8::String::New(stringOf(*mat).c_str()));
}

v8::Handle<v8::Value> ScriptMatrix4::toJSON(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    Matrix4* mat = getDataOfInstance(args.This());
    v8::Local<v8::Array> array = v8::Array::New(16);
    for(char i = 0; i < 16; i ++)
        array->Set(i, v8::Number::New(mat->values[i]));
    return handleScope.Close(array);
}

v8::Handle<v8::Value> ScriptMatrix4::AccessRowX(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    Matrix4* objectPtr = getDataOfInstance(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->x = scriptVector3.getDataOfInstance(args[0]);
        return args[0];
    }else
        return handleScope.Close(scriptVector3.newInstance(objectPtr->x));
}

v8::Handle<v8::Value> ScriptMatrix4::AccessRowY(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    Matrix4* objectPtr = getDataOfInstance(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->y = scriptVector3.getDataOfInstance(args[0]);
        return args[0];
    }else
        return handleScope.Close(scriptVector3.newInstance(objectPtr->y));
}

v8::Handle<v8::Value> ScriptMatrix4::AccessRowZ(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    Matrix4* objectPtr = getDataOfInstance(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->z = scriptVector3.getDataOfInstance(args[0]);
        return args[0];
    }else
        return handleScope.Close(scriptVector3.newInstance(objectPtr->z));
}

v8::Handle<v8::Value> ScriptMatrix4::AccessRowW(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    Matrix4* objectPtr = getDataOfInstance(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->w = scriptVector3.getDataOfInstance(args[0]);
        return args[0];
    }else
        return handleScope.Close(scriptVector3.newInstance(objectPtr->w));
}

v8::Handle<v8::Value> ScriptMatrix4::GetRotation(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btQuaternion quaternion;
    getDataOfInstance(args.This())->getBTMatrix3x3().getRotation(quaternion);
    return handleScope.Close(scriptQuaternion.newInstance(quaternion));
}

v8::Handle<v8::Value> ScriptMatrix4::SetRotation(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptQuaternion.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Matrix4 setRotation: Invalid argument"));
    btMatrix3x3 basis;
    basis.setRotation(scriptQuaternion.getDataOfInstance(args[0]));
    Matrix4 mat(basis);
    mat.w = getDataOfInstance(args.This())->w;
    *getDataOfInstance(args.This()) = mat;
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptMatrix4::GetEuler(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    float values[3];
    getDataOfInstance(args.This())->getBTMatrix3x3().getEulerYPR(values[0], values[1], values[2]);
    v8::Handle<v8::Array> result = v8::Array::New(3);
    for(unsigned int i = 0; i < 3; i ++)
        result->Set(i, v8::Number::New(values[i]));
    return handleScope.Close(result);
}

v8::Handle<v8::Value> ScriptMatrix4::GetInverse(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    Matrix4 mat = getDataOfInstance(args.This())->getInverse();
    return handleScope.Close(scriptMatrix4.newInstance(mat));
}

v8::Handle<v8::Value> ScriptMatrix4::GetProduct(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptMatrix4.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Matrix4 getProduct: Invalid argument"));
    Matrix4 mat = *getDataOfInstance(args.This()) * *getDataOfInstance(args[0]);
    return handleScope.Close(scriptMatrix4.newInstance(mat));
}

v8::Handle<v8::Value> ScriptMatrix4::GetRotatedVector(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Matrix4 getRotated: Invalid argument"));
    btMatrix3x3 basis = getDataOfInstance(args.This())->getBTMatrix3x3();
    btVector3 vec = ScriptVector3::getDataOfInstance(args[0]);
    vec.setW(1.0);
    return handleScope.Close(scriptVector3.newInstance(basis * vec));
}

v8::Handle<v8::Value> ScriptMatrix4::GetTransformedVector(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Matrix4 getTransformed: Invalid argument"));
    Matrix4* mat = getDataOfInstance(args.This());
    btVector3 vec = ScriptVector3::getDataOfInstance(args[0]);
    vec.setW(1.0);
    return handleScope.Close(scriptVector3.newInstance((*mat)(vec)));
}

v8::Handle<v8::Value> ScriptMatrix4::Multiply(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptMatrix4.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Matrix4 mult: Invalid argument"));
    Matrix4* mat = getDataOfInstance(args.This());
    *mat *= *getDataOfInstance(args[0]);
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptMatrix4::Scale(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Matrix4 scale: Invalid argument"));
    Matrix4* mat = getDataOfInstance(args.This());
    mat->scale(ScriptVector3::getDataOfInstance(args[0]));
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptMatrix4::Rotate(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    switch (args.Length()) {
        case 1: {
            if(!scriptQuaternion.isCorrectInstance(args[0]))
                break;
            Matrix4* mat = getDataOfInstance(args.This());
            mat->rotate(ScriptQuaternion::getDataOfInstance(args[0]));
            return handleScope.Close(args.This());
        } case 2: {
            if(!scriptVector3.isCorrectInstance(args[0]) || !args[1]->IsNumber())
                break;
            Matrix4* mat = getDataOfInstance(args.This());
            mat->rotate(ScriptVector3::getDataOfInstance(args[0]), args[1]->NumberValue());
            return handleScope.Close(args.This());
        }
    }
    return v8::ThrowException(v8::String::New("Matrix4 rotate: Invalid argument"));
}

v8::Handle<v8::Value> ScriptMatrix4::Translate(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("Matrix4 translate: Invalid argument"));
    Matrix4* mat = getDataOfInstance(args.This());
    mat->translate(ScriptVector3::getDataOfInstance(args[0]));
    return handleScope.Close(args.This());
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
    objectTemplate->Set(v8::String::New("getRotation"), v8::FunctionTemplate::New(GetRotation));
    objectTemplate->Set(v8::String::New("setRotation"), v8::FunctionTemplate::New(SetRotation));
    objectTemplate->Set(v8::String::New("getEuler"), v8::FunctionTemplate::New(GetEuler));
    objectTemplate->Set(v8::String::New("getInverse"), v8::FunctionTemplate::New(GetInverse));
    objectTemplate->Set(v8::String::New("getProduct"), v8::FunctionTemplate::New(GetProduct));
    objectTemplate->Set(v8::String::New("getRotated"), v8::FunctionTemplate::New(GetRotatedVector));
    objectTemplate->Set(v8::String::New("getTransformed"), v8::FunctionTemplate::New(GetTransformedVector));
    objectTemplate->Set(v8::String::New("mult"), v8::FunctionTemplate::New(Multiply));
    objectTemplate->Set(v8::String::New("scale"), v8::FunctionTemplate::New(Scale));
    objectTemplate->Set(v8::String::New("rotate"), v8::FunctionTemplate::New(Rotate));
    objectTemplate->Set(v8::String::New("translate"), v8::FunctionTemplate::New(Translate));
}



ScriptVector3 scriptVector3;
ScriptQuaternion scriptQuaternion;
ScriptMatrix4 scriptMatrix4;