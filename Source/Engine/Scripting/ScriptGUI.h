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
    auto objectPtr = getDataOfInstance<T>(instance);
    switch(objectPtr->sizeAlignment) {
        case GUISizeAlignment::None:
            return ScriptStringNone.getJSStr(context);
        case GUISizeAlignment::Width:
            return ScriptStringWidth.getJSStr(context);
        case GUISizeAlignment::Height:
            return ScriptStringHeight.getJSStr(context);
        case GUISizeAlignment::All:
            return ScriptStringAll.getJSStr(context);
        default:
            return ScriptException(context, exception, "GUI getSizeAlignment(): Internal error");
    }
}

template<typename T> static bool ScriptGUISetSizeAlignment(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "GUI setSizeAlignment(): Expected String");
        return false;
    }
    ScriptString strValue(context, value);
    std::string str = strValue.getStdStr();
    auto objectPtr = getDataOfInstance<T>(instance);
    if(str == ScriptStringNone.getStdStr())
        objectPtr->sizeAlignment = GUISizeAlignment::None;
    else if(str == ScriptStringWidth.getStdStr())
        objectPtr->sizeAlignment = GUISizeAlignment::Width;
    else if(str == ScriptStringHeight.getStdStr())
        objectPtr->sizeAlignment = GUISizeAlignment::Height;
    else if(str == ScriptStringAll.getStdStr())
        objectPtr->sizeAlignment = GUISizeAlignment::All;
    else{
        ScriptException(context, exception, "GUI setSizeAlignment(): Invalid value");
        return false;
    }
    return true;
}

template<typename T> static JSValueRef ScriptGUIGetOrientation(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<T>(instance);
    switch(objectPtr->orientation) {
        case GUIOrientation::Left:
            return ScriptStringLeft.getJSStr(context);
        case GUIOrientation::Right:
            return ScriptStringRight.getJSStr(context);
        case GUIOrientation::Bottom:
            return ScriptStringBottom.getJSStr(context);
        case GUIOrientation::Top:
            return ScriptStringTop.getJSStr(context);
        case GUIOrientation::Vertical:
            return ScriptStringVertical.getJSStr(context);
        case GUIOrientation::Horizontal:
            return ScriptStringHorizontal.getJSStr(context);
        default:
            return ScriptException(context, exception, "GUI getOrientation(): Internal error");
    }
}

template<typename T> static bool ScriptGUISetOrientation(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "GUI setOrientation(): Expected String");
        return false;
    }
    ScriptString strValue(context, value);
    std::string str = strValue.getStdStr();
    auto objectPtr = getDataOfInstance<T>(instance);
    if(str == ScriptStringLeft.getStdStr())
        objectPtr->orientation = GUIOrientation::Left;
    else if(str == ScriptStringRight.getStdStr())
        objectPtr->orientation = GUIOrientation::Right;
    else if(str == ScriptStringBottom.getStdStr())
        objectPtr->orientation = GUIOrientation::Bottom;
    else if(str == ScriptStringTop.getStdStr())
        objectPtr->orientation = GUIOrientation::Top;
    else if(str == ScriptStringVertical.getStdStr())
        objectPtr->orientation = GUIOrientation::Vertical;
    else if(str == ScriptStringHorizontal.getStdStr())
        objectPtr->orientation = GUIOrientation::Horizontal;
    else{
        ScriptException(context, exception, "GUI setOrientation(): Invalid value");
        return false;
    }
    return true;
}

template<typename T> static bool ScriptGUISetOrientationDual(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "GUI setOrientation(): Expected String");
        return false;
    }
    ScriptString strValue(context, value);
    std::string str = strValue.getStdStr();
    auto objectPtr = getDataOfInstance<T>(instance);
    if(str == ScriptStringVertical.getStdStr())
        objectPtr->orientation = GUIOrientation::Vertical;
    else if(str == ScriptStringHorizontal.getStdStr())
        objectPtr->orientation = GUIOrientation::Horizontal;
    else{
        ScriptException(context, exception, "GUI setOrientation(): Invalid value");
        return false;
    }
    return true;
}

#endif