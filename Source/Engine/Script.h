//
//  Script.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.01.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef Script_h
#define Script_h

#define ScriptScope() v8::HandleScope handleScope(v8::Isolate::GetCurrent())
#define ScriptString(str) v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), str)
#define ScriptException(str) args.GetReturnValue().Set(v8::Isolate::GetCurrent()->ThrowException(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), str)))
#define ScriptNewInstance(class) v8::HandleScope handleScope(v8::Isolate::GetCurrent()); \
v8::Handle<v8::Value> external = v8::External::New(v8::Isolate::GetCurrent(), this); \
(*class->functionTemplate)->GetFunction()->NewInstance(1, &external);
#define ScriptMethod(method) v8::FunctionTemplate::New(v8::Isolate::GetCurrent(), method)
#define ScriptInherit(class) (*functionTemplate)->Inherit(v8::Handle<v8::FunctionTemplate>(*class->functionTemplate))
#define ScriptReturn(value) args.GetReturnValue().Set(value)
#define ScriptClassInit(class) class.reset(new decltype(class)::element_type); \
class->init(globalTemplate)

#include "FBO.h"

class ScriptFile : public FileResource {
    public:
    ~ScriptFile();
    v8::Persistent<v8::Object> exports;
    v8::Persistent<v8::Script> script;
    v8::Persistent<v8::Context> context;
    FileResourcePtr<FileResource> load(FilePackage* filePackage, const std::string& name);
    bool checkFunction(const char* functionName);
    v8::Handle<v8::Value> run();
    v8::Handle<v8::Value> callFunction(const char* functionName, bool recvFirstArg, int argc, v8::Handle<v8::Value>* argv);
};

class ScriptClass {
    protected:
    ScriptClass(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args));
    public:
    const char* name;
    v8::Persistent<v8::FunctionTemplate> functionTemplate;
    virtual ~ScriptClass();
    static v8::Handle<v8::Value> callFunction(v8::Handle<v8::Object> scriptInstance, const char* functionName, int argc, v8::Handle<v8::Value>* argv);
    bool isCorrectInstance(const v8::Local<v8::Value>& object);
    void init(const v8::Persistent<v8::ObjectTemplate>& globalTemplate);
};

static const char* cStrOfV8(v8::Handle<v8::Value> string) {
    v8::String::Utf8Value value(string->ToString());
    return *value ? *value : "<string conversion failed>";
}

static std::string stdStrOfV8(v8::Handle<v8::Value> string) {
    v8::String::Utf8Value value(string->ToString());
    return std::string(*value ? *value : "<string conversion failed>");
}

#endif