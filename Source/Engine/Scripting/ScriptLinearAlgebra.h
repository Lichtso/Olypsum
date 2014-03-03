//
//  ScriptLinearAlgebra.h
//  Olypsum
//
//  Created by Alexander Meißner on 30.01.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptLinearAlgebra_h
#define ScriptLinearAlgebra_h

#include "../Menu/Menu.h"

class ScriptVector3 : public ScriptClass {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ToString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ToJSON(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void IndexedPropertyGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void IndexedPropertySetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void GetAngle(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetSum(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetDifference(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetProduct(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetQuotient(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetDotProduct(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetCrossProduct(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetLength(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetNormalized(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetInterpolation(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Sum(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Subtract(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Multiply(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Divide(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void CrossProduct(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Normalize(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    static btVector3 getDataOfInstance(const v8::Local<v8::Value>& object);
    static void setDataToInstance(const v8::Local<v8::Value>& object, const btVector3& vec);
    v8::Local<v8::Object> newInstance(const btVector3& vec);
    ScriptVector3();
};

class ScriptQuaternion : public ScriptClass {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void toString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void toJSON(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void IndexedPropertyGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void IndexedPropertySetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void GetAngle(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetAxis(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetInverse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetSum(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetDifference(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetProduct(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetDotProduct(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetLength(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetNormalized(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetInterpolation(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SetRotation(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Sum(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Subtract(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Multiply(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Normalize(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    static btQuaternion getDataOfInstance(const v8::Local<v8::Value>& object);
    static void setDataToInstance(const v8::Local<v8::Value>& object, const btQuaternion& quaternion);
    v8::Local<v8::Object> newInstance(const btQuaternion& quaternion);
    ScriptQuaternion();
};

class ScriptMatrix4 : public ScriptClass {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Destructor(const v8::WeakCallbackData<v8::Object, void>& data);
    static void toString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void toJSON(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessRowX(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessRowY(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessRowZ(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessRowW(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessRotation(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SetIdentity(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetInverse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetProduct(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetRotatedVector(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetTransformedVector(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Multiply(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Scale(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Rotate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Translate(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    static Matrix4* getDataOfInstance(const v8::Local<v8::Value>& object);
    static void setDataToInstance(const v8::Local<v8::Value>& object, const Matrix4& mat);
    v8::Local<v8::Object> newInstance(const Matrix4& mat);
    ScriptMatrix4();
};

extern std::unique_ptr<ScriptVector3> scriptVector3;
extern std::unique_ptr<ScriptQuaternion> scriptQuaternion;
extern std::unique_ptr<ScriptMatrix4> scriptMatrix4;

#endif
