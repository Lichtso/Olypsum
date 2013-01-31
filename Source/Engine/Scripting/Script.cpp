//
//  Script.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.01.13.
//
//

#include "ScriptManager.h"

Script::Script() {
    
}

Script::Script(const std::string& sourceCode, const std::string& name) {
    v8::HandleScope handleScope;
    v8::TryCatch tryCatch;
    v8::Handle<v8::Script> scriptLocal = v8::Script::Compile(v8::String::New(sourceCode.c_str()), v8::String::New(name.c_str()));
    if(scriptManager->tryCatch(&tryCatch))
        script = v8::Persistent<v8::Script>::New(scriptLocal);
}

Script::~Script() {
    script.Dispose();
}

v8::Handle<v8::Value> Script::run() {
    if(script.IsEmpty()) return v8::Undefined();
    v8::HandleScope handleScope;
    v8::TryCatch tryCatch;
    v8::Handle<v8::Value> result = script->Run();
    scriptManager->tryCatch(&tryCatch);
    return handleScope.Close(result);
}

ScriptFile::~ScriptFile() {
    exports.Dispose();
}

bool ScriptFile::load(FilePackage* filePackageB, const std::string& nameB) {
    name = nameB+".js";
    std::string filePath = filePackageB->getPathOfFile("Scripts", name);
    std::unique_ptr<char[]> data = readFile(filePath, true);
    if(!data) return false;
    v8::HandleScope handleScope;
    v8::TryCatch tryCatch;
    v8::Handle<v8::Context> context = v8::Context::New(NULL, scriptManager->globalTemplate);
    context->Enter();
    context->Global()->Set(v8::String::New("exports"), v8::Object::New());
    
    v8::Local<v8::Script> scriptLocal = v8::Script::Compile(v8::String::New(data.get()), v8::String::New(name.c_str()));
    if(scriptManager->tryCatch(&tryCatch)) {
        filePackage = filePackageB;
        script = v8::Persistent<v8::Script>::New(scriptLocal);
        v8::Handle<v8::Object>::Cast(run());
        v8::Local<v8::Value> objectLocal = context->Global()->Get(v8::String::New("exports"));
        exports = v8::Persistent<v8::Object>::New(v8::Local<v8::Object>::Cast(objectLocal));
    }
    
    return !tryCatch.HasCaught();
}