//
//  Script.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.01.13.
//
//

#include "Script.h"

Script::Script(const std::string& sourceCode, const std::string& name) {
    v8::HandleScope handleScope;
    v8::TryCatch tryCatch;
    v8::Handle<v8::Script> scriptLocal = v8::Script::Compile(v8::String::New(sourceCode.c_str()), v8::String::New(name.c_str()));
    if(scriptContext->tryCatch(&tryCatch)) {
        script = v8::Persistent<v8::Script>::New(scriptLocal);
    }
}

Script::~Script() {
    script.Dispose();
}

v8::Handle<v8::Value> Script::run() {
    if(script.IsEmpty()) return v8::Undefined();
    v8::HandleScope handleScope;
    v8::TryCatch tryCatch;
    v8::Handle<v8::Value> result = script->Run();
    scriptContext->tryCatch(&tryCatch);
    return handleScope.Close(result);
}

std::shared_ptr<FilePackageResource> ScriptFile::load(FilePackage* filePackageB, const std::string& name) {
    auto pointer = FilePackageResource::load(filePackageB, name);
    std::string filePath = filePackageB->getPathOfFile("Scripts", name);
    std::unique_ptr<char[]> data = readFile(filePath, true);
    v8::HandleScope handleScope;
    v8::TryCatch tryCatch;
    v8::Handle<v8::Script> scriptLocal = v8::Script::Compile(v8::String::New(data.get()), v8::String::New(name.c_str()));
    if(scriptContext->tryCatch(&tryCatch)) {
        script = v8::Persistent<v8::Script>::New(scriptLocal);
        return pointer;
    }
    return NULL;
}