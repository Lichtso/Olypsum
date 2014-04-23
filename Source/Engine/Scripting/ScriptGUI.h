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
    if(str == "none")
        objectPtr->sizeAlignment = GUISizeAlignment::None;
    else if(str == "width")
        objectPtr->sizeAlignment = GUISizeAlignment::Width;
    else if(str == "height")
        objectPtr->sizeAlignment = GUISizeAlignment::Height;
    else if(str == "all")
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
    if(str == "left")
        objectPtr->orientation = GUIOrientation::Left;
    else if(str == "right")
        objectPtr->orientation = GUIOrientation::Right;
    else if(str == "bottom")
        objectPtr->orientation = GUIOrientation::Bottom;
    else if(str == "top")
        objectPtr->orientation = GUIOrientation::Top;
    else if(str == "vertical")
        objectPtr->orientation = GUIOrientation::Vertical;
    else if(str == "horizontal")
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
    if(str == "vertical")
        objectPtr->orientation = GUIOrientation::Vertical;
    else if(str == "horizontal")
        objectPtr->orientation = GUIOrientation::Horizontal;
    else{
        ScriptException(context, exception, "GUI setOrientation(): Invalid value");
        return false;
    }
    return true;
}

#endif