//
//  Script.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.01.13.
//
//

#ifndef Script_h
#define Script_h

#include "Menu.h"

class Script {
    protected:
    Script();
    public:
    v8::Persistent<v8::Script> script;
    Script(const std::string& sourceCode, const std::string& name);
    ~Script();
    v8::Handle<v8::Value> run();
};

class ScriptFile : public Script {
    public:
    ~ScriptFile();
    FilePackage* filePackage;
    std::string name;
    v8::Persistent<v8::Object> exports;
    bool load(FilePackage* filePackage, const std::string& name);
};

class ScriptClass {
    protected:
    ScriptClass(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args));
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