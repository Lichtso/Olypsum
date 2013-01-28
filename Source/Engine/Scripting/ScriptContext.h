//
//  ScriptContext.h
//  Olypsum
//
//  Created by Alexander Meißner on 27.01.13.
//
//

#include "Controls.h"
#include <v8.h>

#ifndef ScriptContext_h
#define ScriptContext_h

class ScriptContext {
    public:
    v8::Handle<v8::ObjectTemplate> globalTemplate;
    v8::Persistent<v8::Context> context;
    ScriptContext();
    ~ScriptContext();
    const char* cStringOf(const v8::String::Utf8Value& value);
    std::string stdStringOf(const v8::String::Utf8Value& value);
    bool tryCatch(v8::TryCatch* tryCatch);
};

extern std::unique_ptr<ScriptContext> scriptContext;

#endif
