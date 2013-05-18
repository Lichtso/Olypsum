//
//  ScriptIO.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.13.
//
//

#ifndef ScriptIO_h
#define ScriptIO_h

#include "ScriptAnimation.h"

class ScriptMouse : public ScriptClass {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessX(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessY(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessFixed(const v8::Arguments& args);
    public:
    ScriptMouse();
};

class ScriptKeyboard : public ScriptClass {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments &args);
    static v8::Handle<v8::Value> GetKeyCount(const v8::Arguments& args);
    static v8::Handle<v8::Value> IsKeyPressed(const v8::Arguments& args);
    public:
    ScriptKeyboard();
};

extern ScriptMouse scriptMouse;
extern ScriptKeyboard scriptKeyboard;

#endif