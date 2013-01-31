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

class ScriptVector3 {
    v8::Persistent<v8::FunctionTemplate> functionTemplate;
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> toString(const v8::Arguments& args);
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
    btVector3 getVector3(const v8::Local<v8::Object>& object);
    void setVector3(const v8::Local<v8::Object>& object, const btVector3& vec);
    v8::Local<v8::Object> newVector3(const btVector3& vec);
    bool isVector3(const v8::Local<v8::Value>& object);
    ScriptVector3();
    ~ScriptVector3();
    void init(const v8::Persistent<v8::ObjectTemplate>& globalTemplate);
};

extern ScriptVector3 scriptVector3;

#endif
