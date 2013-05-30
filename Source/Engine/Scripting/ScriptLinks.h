//
//  ScriptLinks.h
//  Olypsum
//
//  Created by Alexander Meißner on 29.05.13.
//
//

#ifndef ScriptLinks_h
#define ScriptLinks_h

#include "ScriptObject.h"

class ScriptBaseLink : public ScriptBaseClass {
    static v8::Handle<v8::Value> GetObjectA(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetObjectB(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    protected:
    ScriptBaseLink(const char* name) :ScriptBaseClass(name) { }
    public:
    ScriptBaseLink();
};

class ScriptPhysicLink : public ScriptBaseLink {
    
    protected:
    ScriptPhysicLink(const char* name) :ScriptBaseLink(name) { }
    public:
    ScriptPhysicLink();
};

class ScriptTransformLink : public ScriptBaseLink {
    static v8::Handle<v8::Value> AccessTransformation(const v8::Arguments& args);
    protected:
    ScriptTransformLink(const char* name) :ScriptBaseLink(name) { }
    public:
    ScriptTransformLink();
};

extern ScriptBaseLink scriptBaseLink;
extern ScriptPhysicLink scriptPhysicLink;
extern ScriptTransformLink scriptTransformLink;

#endif
