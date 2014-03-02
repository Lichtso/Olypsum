//
//  Script.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.01.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Scripting/ScriptManager.h"

ScriptFile::~ScriptFile() {
    /*exports.Reset();
    script.Reset();
    context.Reset();*/
}

FileResourcePtr<FileResource> ScriptFile::load(FilePackage* _filePackage, const std::string& name) {
    auto pointer = FileResource::load(_filePackage, name);
    if(!script.IsEmpty()) return NULL;
    
    std::string filePath = filePackage->getPathOfFile("Scripts/", name)+".js";
    std::unique_ptr<char[]> data = readFile(filePath, true);
    if(!data) return NULL;
    
    v8::TryCatch tryCatch;
    ScriptScope();
    context.Reset(v8::Isolate::GetCurrent(),
                  v8::Context::New(v8::Isolate::GetCurrent(), NULL, v8::Handle<v8::ObjectTemplate>(*scriptManager->globalTemplate)));
    (*context)->Enter();
    (*context)->Global()->Set(ScriptString("exports"), v8::Object::New(v8::Isolate::GetCurrent()));
    
    v8::Handle<v8::String> scriptName = ScriptString(fileManager.getPathOfResource(filePackage, name).c_str());
    v8::Handle<v8::Script> scriptLocal = v8::Script::Compile(ScriptString(data.get()), scriptName);
    if(scriptManager->tryCatch(&tryCatch)) {
        script.Reset(v8::Isolate::GetCurrent(), scriptLocal);
        run();
        v8::Handle<v8::Object> objectLocal = v8::Handle<v8::Object>::Cast((*context)->Global()->Get(ScriptString("exports")));
        exports.Reset(v8::Isolate::GetCurrent(), objectLocal);
        return pointer;
    }else
        return NULL;
}

bool ScriptFile::checkFunction(const char* functionName) {
    if(exports.IsEmpty()) return false;
    ScriptScope();
    v8::Handle<v8::Function> function = v8::Local<v8::Function>::Cast((*exports)->GetRealNamedProperty(ScriptString(functionName)));
    return (!function.IsEmpty() && function->IsFunction());
}

v8::Handle<v8::Value> ScriptFile::run() {
    if(script.IsEmpty()) return v8::Undefined(v8::Isolate::GetCurrent());
    //ScriptScope();
    v8::TryCatch tryCatch;
    v8::Handle<v8::Value> result = (*script)->Run();
    scriptManager->tryCatch(&tryCatch);
    return result; //handleScope.Close(result);
}

v8::Handle<v8::Value> ScriptFile::callFunction(const char* functionName, bool recvFirstArg, int argc, ...) {
    if(exports.IsEmpty()) return v8::Undefined(v8::Isolate::GetCurrent());
    ScriptScope();
    v8::Handle<v8::Function> function = v8::Local<v8::Function>::Cast((*exports)->GetRealNamedProperty(ScriptString(functionName)));
    if(function.IsEmpty() || !function->IsFunction()) return v8::Undefined(v8::Isolate::GetCurrent());
    v8::TryCatch tryCatch;
    
    v8::Handle<v8::Value>* argList;
    if(argc > 0) {
        argList = new v8::Handle<v8::Value>[argc];
        va_list argv;
        va_start(argv, argc);
        for(int i = 0; i < argc; i ++)
            argList[i] = v8::Handle<v8::Value>(reinterpret_cast<v8::Value*>(va_arg(argv, void*)));
        va_end(argv);
        delete [] argList;
    }
    
    v8::Handle<v8::Value> result;
    if(recvFirstArg && argc > 0 && !argList[0].IsEmpty() && argList[0]->IsObject())
        result = function->CallAsFunction(v8::Handle<v8::Object>::Cast(argList[0]), argc-1, &argList[1]);
    else
        result = function->CallAsFunction(v8::Handle<v8::Object>(*exports), argc, argList);
    
    if(scriptManager->tryCatch(&tryCatch))
        return result;
    else
        return v8::Undefined(v8::Isolate::GetCurrent());
}



ScriptClass::ScriptClass(const char* nameB, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :name(nameB) {
    ScriptScope();
    functionTemplate.Reset(v8::Isolate::GetCurrent(), ScriptMethod(constructor));
    (*functionTemplate)->SetClassName(ScriptString(name));
    (*functionTemplate)->PrototypeTemplate()->Set(ScriptString("className"), ScriptString(name));
}

ScriptClass::~ScriptClass() {
    //functionTemplate.Reset();
}

v8::Handle<v8::Value> ScriptClass::callFunction(v8::Handle<v8::Object> scriptInstance, const char* functionName, int argc, ...) {
    if(scriptInstance.IsEmpty()) return v8::Undefined(v8::Isolate::GetCurrent());
    ScriptScope();
    v8::Handle<v8::Function> function = v8::Local<v8::Function>::Cast(scriptInstance->GetRealNamedProperty(ScriptString(functionName)));
    if(function.IsEmpty() || !function->IsFunction()) return v8::Undefined(v8::Isolate::GetCurrent());
    v8::TryCatch tryCatch;
    
    v8::Handle<v8::Value>* argList;
    if(argc > 0) {
        argList = new v8::Handle<v8::Value>[argc];
        va_list argv;
        va_start(argv, argc);
        for(int i = 0; i < argc; i ++)
            argList[i] = v8::Handle<v8::Value>(reinterpret_cast<v8::Value*>(va_arg(argv, void*)));
        va_end(argv);
        delete [] argList;
    }
    
    v8::Handle<v8::Value> result = function->CallAsFunction(scriptInstance, argc, argList);
    if(scriptManager->tryCatch(&tryCatch))
        return result;
    else
        return v8::Undefined(v8::Isolate::GetCurrent());
}

bool ScriptClass::isCorrectInstance(const v8::Local<v8::Value>& object) {
    ScriptScope();
    if(!object->IsObject()) return false;
    return (*functionTemplate)->HasInstance(object);
}

void ScriptClass::init(const v8::Persistent<v8::ObjectTemplate>& globalTemplate) {
    (*globalTemplate)->Set(v8::Isolate::GetCurrent(), name, v8::Handle<v8::FunctionTemplate>(*functionTemplate));
}