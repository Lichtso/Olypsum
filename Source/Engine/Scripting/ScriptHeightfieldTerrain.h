//
//  ScriptHeightfieldTerrain.h
//  Olypsum
//
//  Created by Alexander Meißner on 05.04.13.
//
//

#include "ScriptDisplayObject.h"

#ifndef ScriptHeightfieldTerrain_h
#define ScriptHeightfieldTerrain_h

class ScriptHeightfieldTerrain : public ScriptPhysicObject {
    static v8::Handle<v8::Value> GetWidth(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetLength(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetBitDepth(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetBitDepth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> UpdateModel(const v8::Arguments& args);
    public:
    ScriptHeightfieldTerrain();
};

extern ScriptHeightfieldTerrain scriptHeightfieldTerrain;

#endif
