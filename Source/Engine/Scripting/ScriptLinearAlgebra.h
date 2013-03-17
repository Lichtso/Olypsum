//
//  ScriptLinearAlgebra.h
//  Olypsum
//
//  Created by Alexander Meißner on 30.01.13.
//
//

#include "Script.h"

#ifndef ScriptLinearAlgebra_h
#define ScriptLinearAlgebra_h

class ScriptVector3 : public ScriptClass {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> toString(const v8::Arguments& args);
    static v8::Handle<v8::Value> toJSON(const v8::Arguments& args);
    static v8::Handle<v8::Value> IndexedPropertyGetter(uint32_t index, const v8::AccessorInfo &info);
    static v8::Handle<v8::Value> IndexedPropertySetter(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
    static v8::Handle<v8::Value> GetSum(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetDifference(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetProduct(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetDotProduct(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetCrossProduct(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetLength(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetNormalized(const v8::Arguments& args);
    static v8::Handle<v8::Value> Sum(const v8::Arguments& args);
    static v8::Handle<v8::Value> Subtract(const v8::Arguments& args);
    static v8::Handle<v8::Value> Multiply(const v8::Arguments& args);
    static v8::Handle<v8::Value> CrossProduct(const v8::Arguments& args);
    static v8::Handle<v8::Value> Normalize(const v8::Arguments& args);
    public:
    static btVector3 getDataOfInstance(const v8::Local<v8::Value>& object);
    static void setDataToInstance(const v8::Local<v8::Value>& object, const btVector3& vec);
    v8::Local<v8::Object> newInstance(const btVector3& vec);
    ScriptVector3();
};

class ScriptMatrix4 : public ScriptClass {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static void Destructor(v8::Persistent<v8::Value> value, void* data);
    static v8::Handle<v8::Value> toString(const v8::Arguments& args);
    static v8::Handle<v8::Value> toJSON(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetInverse(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetProduct(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetTransformedVector(const v8::Arguments& args);
    static v8::Handle<v8::Value> Multiply(const v8::Arguments& args);
    static v8::Handle<v8::Value> Scale(const v8::Arguments& args);
    static v8::Handle<v8::Value> Rotate(const v8::Arguments& args);
    static v8::Handle<v8::Value> Translate(const v8::Arguments& args);
    static v8::Handle<v8::Value> TransformVector(const v8::Arguments& args);
    public:
    static Matrix4& getDataOfInstance(const v8::Local<v8::Value>& object);
    static void setDataToInstance(const v8::Local<v8::Value>& object, const Matrix4& mat);
    v8::Local<v8::Object> newInstance(const Matrix4& mat);
    ScriptMatrix4();
};

extern ScriptVector3 scriptVector3;
extern ScriptMatrix4 scriptMatrix4;

#endif
