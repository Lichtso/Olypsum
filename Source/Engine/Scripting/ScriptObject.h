//
//  ScriptObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 09.03.13.
//
//

#include "ScriptLinearAlgebra.h"

#ifndef ScriptObject_h
#define ScriptObject_h

class ScriptBaseObject : public ScriptClass {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> SetTransformation(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetTransformation(const v8::Arguments& args);
    public:
    static BaseObject* getDataOfInstance(const v8::Local<v8::Value>& object);
    v8::Local<v8::Object> newInstance(BaseObject* object);
    ScriptBaseObject();
};

extern ScriptBaseObject scriptBaseObject;

#endif
