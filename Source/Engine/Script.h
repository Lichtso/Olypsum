//
//  Script.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.01.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef Script_h
#define Script_h
#define ScriptException(str) GetReturnValue().Set(v8::ThrowException(v8::String::New(str)))

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
    v8::Handle<v8::Value> callFunction(const char* functionName, bool recvFirstArg, std::vector<v8::Handle<v8::Value>> args);
};

class ScriptClass {
    protected:
    ScriptClass(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args));
    public:
    const char* name;
    v8::Persistent<v8::FunctionTemplate> functionTemplate;
    virtual ~ScriptClass();
    static v8::Handle<v8::Value> callFunction(v8::Handle<v8::Object> scriptInstance, const char* functionName, std::vector<v8::Handle<v8::Value>> args);
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