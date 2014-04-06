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
    ScriptDeclareMethod(Constructor);
    ScriptDeclareMethod(ToString);
    ScriptDeclareMethod(ToJSON);
    ScriptDeclareMethod(GetAngle);
    ScriptDeclareMethod(GetSum);
    ScriptDeclareMethod(GetDifference);
    ScriptDeclareMethod(GetProduct);
    ScriptDeclareMethod(GetQuotient);
    ScriptDeclareMethod(GetDotProduct);
    ScriptDeclareMethod(GetCrossProduct);
    ScriptDeclareMethod(GetLength);
    ScriptDeclareMethod(GetNormalized);
    ScriptDeclareMethod(GetInterpolation);
    ScriptDeclareMethod(Sum);
    ScriptDeclareMethod(Subtract);
    ScriptDeclareMethod(Multiply);
    ScriptDeclareMethod(Divide);
    ScriptDeclareMethod(CrossProduct);
    ScriptDeclareMethod(Normalize);
    public:
    static btVector3 getDataOfInstance(const v8::Local<v8::Value>& object);
    static void setDataToInstance(const v8::Local<v8::Value>& object, const btVector3& vec);
    v8::Local<v8::Object> newInstance(const btVector3& vec);
    ScriptVector3();
};

class ScriptQuaternion : public ScriptClass {
    ScriptDeclareMethod(Constructor);
    ScriptDeclareMethod(ToString);
    ScriptDeclareMethod(ToJSON);
    ScriptDeclareMethod(GetAngle);
    ScriptDeclareMethod(GetAxis);
    ScriptDeclareMethod(GetInverse);
    ScriptDeclareMethod(GetSum);
    ScriptDeclareMethod(GetDifference);
    ScriptDeclareMethod(GetProduct);
    ScriptDeclareMethod(GetDotProduct);
    ScriptDeclareMethod(GetLength);
    ScriptDeclareMethod(GetNormalized);
    ScriptDeclareMethod(GetInterpolation);
    ScriptDeclareMethod(SetRotation);
    ScriptDeclareMethod(Sum);
    ScriptDeclareMethod(Subtract);
    ScriptDeclareMethod(Multiply);
    ScriptDeclareMethod(Normalize);
    public:
    static btQuaternion getDataOfInstance(const v8::Local<v8::Value>& object);
    static void setDataToInstance(const v8::Local<v8::Value>& object, const btQuaternion& quaternion);
    v8::Local<v8::Object> newInstance(const btQuaternion& quaternion);
    ScriptQuaternion();
};

class ScriptMatrix4 : public ScriptClass {
    ScriptDeclareMethod(Constructor);
    static void Destructor(const v8::WeakCallbackData<v8::Object, void>& data);
    ScriptDeclareMethod(ToString);
    ScriptDeclareMethod(ToJSON);
    ScriptDeclareMethod(AccessRowX);
    ScriptDeclareMethod(AccessRowY);
    ScriptDeclareMethod(AccessRowZ);
    ScriptDeclareMethod(AccessRowW);
    ScriptDeclareMethod(AccessRotation);
    ScriptDeclareMethod(SetIdentity);
    ScriptDeclareMethod(GetInverse);
    ScriptDeclareMethod(GetProduct);
    ScriptDeclareMethod(GetRotatedVector);
    ScriptDeclareMethod(GetTransformedVector);
    ScriptDeclareMethod(Multiply);
    ScriptDeclareMethod(Scale);
    ScriptDeclareMethod(Rotate);
    ScriptDeclareMethod(Translate);
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
