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
        script = v8::Persistent<v8::Script>::New(v8::Isolate::GetCurrent(), scriptLocal);
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
    context.Dispose();
    exports.Dispose();
}

bool ScriptFile::load(FilePackage* filePackageB, const std::string& nameB) {
    name = nameB;
    std::string filePath = filePackageB->getPathOfFile("Scripts", name+".js");
    std::unique_ptr<char[]> data = readFile(filePath, true);
    if(!data) return false;
    v8::TryCatch tryCatch;
    v8::HandleScope handleScope;
    context = v8::Persistent<v8::Context>::New(v8::Isolate::GetCurrent(),
              v8::Context::New(v8::Isolate::GetCurrent(), NULL, scriptManager->globalTemplate));
    context->Enter();
    context->Global()->Set(v8::String::New("exports"), v8::Object::New());
    
    v8::Local<v8::Script> scriptLocal = v8::Script::Compile(v8::String::New(data.get()), v8::String::New(name.c_str()));
    if(scriptManager->tryCatch(&tryCatch)) {
        filePackage = filePackageB;
        script = v8::Persistent<v8::Script>::New(v8::Isolate::GetCurrent(), scriptLocal);
        v8::Handle<v8::Object>::Cast(run());
        v8::Local<v8::Value> objectLocal = context->Global()->Get(v8::String::New("exports"));
        exports = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), v8::Local<v8::Object>::Cast(objectLocal));
    }
    
    return !tryCatch.HasCaught();
}

bool ScriptFile::checkFunction(const char* functionName) {
    if(exports.IsEmpty()) return false;
    v8::HandleScope handleScope;
    v8::Handle<v8::Function> function = v8::Local<v8::Function>::Cast(exports->GetRealNamedProperty(v8::String::New(functionName)));
    return (!function.IsEmpty() && function->IsFunction());
}

v8::Handle<v8::Value> ScriptFile::callFunction(const char* functionName, bool recvFirstArg, std::vector<v8::Handle<v8::Value>> args) {
    if(exports.IsEmpty()) return v8::Undefined();
    v8::HandleScope handleScope;
    v8::Handle<v8::Function> function = v8::Local<v8::Function>::Cast(exports->GetRealNamedProperty(v8::String::New(functionName)));
    if(function.IsEmpty() || !function->IsFunction()) return v8::Undefined();
    v8::TryCatch tryCatch;
    v8::Handle<v8::Value> result;
    if(recvFirstArg && args.size() > 0 && !args[0].IsEmpty() && args[0]->IsObject())
        result = function->CallAsFunction(v8::Handle<v8::Object>::Cast(args[0]), args.size()-1, &args[1]);
    else
        result = function->CallAsFunction(exports, args.size(), &args[0]);
    if(scriptManager->tryCatch(&tryCatch))
        return result;
    else
        return v8::Undefined();
}



ScriptClass::ScriptClass(const char* nameB, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :name(nameB) {
    v8::HandleScope handleScope;
    functionTemplate = v8::Persistent<v8::FunctionTemplate>::New(v8::Isolate::GetCurrent(), v8::FunctionTemplate::New(constructor));
    functionTemplate->SetClassName(v8::String::New(name));
    functionTemplate->PrototypeTemplate()->Set(v8::String::New("className"), v8::String::New(name));
}

ScriptClass::~ScriptClass() {
    functionTemplate.Dispose();
}

v8::Handle<v8::Value> ScriptClass::callFunction(v8::Handle<v8::Object> scriptInstance, const char* functionName, std::vector<v8::Handle<v8::Value>> args) {
    if(scriptInstance.IsEmpty()) return v8::Undefined();
    v8::HandleScope handleScope;
    v8::Handle<v8::Function> function = v8::Local<v8::Function>::Cast(scriptInstance->GetRealNamedProperty(v8::String::New(functionName)));
    if(function.IsEmpty() || !function->IsFunction()) return v8::Undefined();
    v8::TryCatch tryCatch;
    v8::Handle<v8::Value> result = function->CallAsFunction(scriptInstance, args.size(), &args[0]);
    if(scriptManager->tryCatch(&tryCatch))
        return result;
    else
        return v8::Undefined();
}

bool ScriptClass::isCorrectInstance(const v8::Local<v8::Value>& object) {
    v8::HandleScope handleScope;
    if(!object->IsObject()) return false;
    return functionTemplate->HasInstance(object);
}

void ScriptClass::init(const v8::Persistent<v8::ObjectTemplate>& globalTemplate) {
    globalTemplate->Set(v8::String::New(name), functionTemplate);
}