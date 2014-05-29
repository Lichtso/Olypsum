//
//  ScriptGUIRect.h
//  Olypsum
//
//  Created by Alexander Meißner on 16.04.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef ScriptGUIRect_h
#define ScriptGUIRect_h

#include "ScriptAnimation.h"

template<typename T> static JSValueRef ScriptGUIGetSizeAlignment(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<T>(instance)->sizeAlignment);
}

template<typename T> static bool ScriptGUISetSizeAlignment(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUI setSizeAlignment(): Expected Number");
        return false;
    }
    unsigned int numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue <= GUISizeAlignment::All) {
        getDataOfInstance<T>(instance)->sizeAlignment = static_cast<GUISizeAlignment>(numberValue);
        return true;
    }else
        return false;
}

template<typename T> static JSValueRef ScriptGUIGetOrientation(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<T>(instance)->orientation);
}

template<typename T> static bool ScriptGUISetOrientation(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUI setOrientation(): Expected Number");
        return false;
    }
    unsigned int numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue <= GUIOrientation::Top || numberValue == GUIOrientation::Vertical || numberValue == GUIOrientation::Horizontal) {
        getDataOfInstance<T>(instance)->orientation = static_cast<GUIOrientation>(numberValue);
        return true;
    }else
        return false;
}

template<typename T> static bool ScriptGUISetOrientationDual(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUI setOrientation(): Expected Number");
        return false;
    }
    unsigned int numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue == GUIOrientation::Vertical || numberValue == GUIOrientation::Horizontal) {
        getDataOfInstance<T>(instance)->orientation = static_cast<GUIOrientation>(numberValue);
        return true;
    }else
        return false;
}

#endif