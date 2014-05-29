//
//  ScriptGUIView.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 18.04.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

static JSValueRef ScriptGUIViewGetChildCount(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIView>(instance)->children.size());
}

static JSValueRef ScriptGUIViewIterateChildren(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1)
        return ScriptException(context, exception, "GUIView iterateChildren: Expected Function");
    JSObjectRef callback = JSValueToObject(context, argv[0], NULL);
    if(!callback || !JSObjectIsFunction(context, callback))
        return ScriptException(context, exception, "GUIView iterateChildren: Expected Function");
    exception = NULL;
    auto objectPtr = getDataOfInstance<GUIView>(instance);
    for(auto child : objectPtr->children) {
        JSValueRef args[] = { child->scriptInstance };
        JSObjectCallAsFunction(context, callback, callback, 1, args, exception);
        if(exception)
            return NULL;
    }
    return JSValueMakeUndefined(context);
}

JSStaticValue ScriptGUIViewProperties[] = {
    {"childCount", ScriptGUIViewGetChildCount, 0, ScriptMethodAttributes},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptGUIViewMethods[] = {
    {"iterateChildren", ScriptGUIViewIterateChildren, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(GUIView, ScriptGUIViewProperties, ScriptGUIViewMethods);



static JSValueRef ScriptGUIScreenViewFunctionCallback(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(!menu.screenView->scriptInstance) {
        menu.screenView->scriptInstance = JSObjectMake(scriptManager->mainScript->context, ScriptClasses[ScriptGUIScreenView], menu.screenView);
        JSValueProtect(scriptManager->mainScript->context, menu.screenView->scriptInstance);
    }
    return menu.screenView->scriptInstance;
}

static JSObjectRef ScriptGUIScreenViewConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return ScriptException(context, exception, "GUIScreenView Constructor: Singleton can't be constructed");
}

static bool ScriptGUIScreenViewInstanceofCallback(JSContextRef context, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception) {
    return JSValueIsObjectOfClass(context, possibleInstance, ScriptClasses[ScriptGUIScreenView]);
}

JSClassDefinition ScriptGUIScreenViewStatic = {
    0, kJSClassAttributeNone, "GUIScreenView",
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    ScriptGUIScreenViewFunctionCallback, ScriptGUIScreenViewConstructor, ScriptGUIScreenViewInstanceofCallback, NULL
};

static JSValueRef ScriptGUIScreenViewGetModalView(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<GUIScreenView>(instance);
    return (objectPtr->modalView) ? objectPtr->modalView->scriptInstance : JSValueMakeNull(context);
}

static bool ScriptGUIScreenViewSetModalView(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsObjectOfClass(context, value, ScriptClasses[ScriptGUIView])) {
        ScriptException(context, exception, "GUIScreenView setModalView(): Expected GUIView");
        return false;
    }
    getDataOfInstance<GUIScreenView>(instance)->setModalView(getDataOfInstance<GUIView>(context, value));
    return true;
}

static JSValueRef ScriptGUIScreenViewGetFocused(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<GUIScreenView>(instance);
    return (objectPtr->focus) ? objectPtr->focus->scriptInstance : JSValueMakeNull(context);
}

JSStaticValue ScriptGUIScreenViewProperties[] = {
    {"modalView", ScriptGUIScreenViewGetModalView, ScriptGUIScreenViewSetModalView, kJSPropertyAttributeDontDelete},
    {"focus", ScriptGUIScreenViewGetFocused, 0, ScriptMethodAttributes},
    {0, 0, 0, 0}
};

ScriptClassDefinition(GUIScreenView, ScriptGUIScreenViewProperties, NULL);



static JSValueRef ScriptGUIFramedViewGetCornerRadius(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIFramedView>(instance)->content.cornerRadius);
}

static bool ScriptGUIFramedViewSetCornerRadius(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIFramedView setCornerRadius(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIFramedView>(instance);
    if(isfinite(numberValue)) {
        objectPtr->content.cornerRadius = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIFramedViewGetEdgeGradientCenter(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIFramedView>(instance)->content.edgeGradientCenter);
}

static bool ScriptGUIFramedViewSetEdgeGradientCenter(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIFramedView setEdgeGradientCenter(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIFramedView>(instance);
    if(isfinite(numberValue)) {
        objectPtr->content.edgeGradientCenter = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIFramedViewGetEdgeGradientBorder(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIFramedView>(instance)->content.edgeGradientBorder);
}

static bool ScriptGUIFramedViewSetEdgeGradientBorder(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIFramedView setEdgeGradientBorder(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIFramedView>(instance);
    if(isfinite(numberValue)) {
        objectPtr->content.edgeGradientBorder = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIFramedViewGetDecorationType(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIFramedView>(instance)->content.decorationType);
}

static bool ScriptGUIFramedViewSetDecorationType(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIButton setState(): Expected Number");
        return false;
    }
    unsigned int numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue <= GUIDecorationType::Stipple) {
        getDataOfInstance<GUIFramedView>(instance)->content.decorationType = static_cast<GUIDecorationType>(numberValue);
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIFramedViewAccessTopColor(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<GUIFramedView>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion])) {
        objectPtr->content.topColor = getScriptQuaternion(context, argv[0]);
        return argv[0];
    }else
        return newScriptQuaternion(context, objectPtr->content.topColor.getQuaternion());
}

static JSValueRef ScriptGUIFramedViewAccessBottomColor(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<GUIFramedView>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion])) {
        objectPtr->content.bottomColor = getScriptQuaternion(context, argv[0]);
        return argv[0];
    }else
        return newScriptQuaternion(context, objectPtr->content.bottomColor.getQuaternion());
}

static JSValueRef ScriptGUIFramedViewAccessBorderColor(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<GUIFramedView>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion])) {
        objectPtr->content.borderColor = getScriptQuaternion(context, argv[0]);
        return argv[0];
    }else
        return newScriptQuaternion(context, objectPtr->content.borderColor.getQuaternion());
}

JSStaticValue ScriptGUIFramedViewProperties[] = {
    {"cornerRadius", ScriptGUIFramedViewGetCornerRadius, ScriptGUIFramedViewSetCornerRadius, kJSPropertyAttributeDontDelete},
    {"edgeGradientCenter", ScriptGUIFramedViewGetEdgeGradientCenter, ScriptGUIFramedViewSetEdgeGradientCenter, kJSPropertyAttributeDontDelete},
    {"edgeGradientBorder", ScriptGUIFramedViewGetEdgeGradientBorder, ScriptGUIFramedViewSetEdgeGradientBorder, kJSPropertyAttributeDontDelete},
    {"decorationType", ScriptGUIFramedViewGetDecorationType, ScriptGUIFramedViewSetDecorationType, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptGUIFramedViewMethods[] = {
    {"topColor", ScriptGUIFramedViewAccessTopColor, ScriptMethodAttributes},
    {"bottomColor", ScriptGUIFramedViewAccessBottomColor, ScriptMethodAttributes},
    {"borderColor", ScriptGUIFramedViewAccessBorderColor, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(GUIFramedView, ScriptGUIFramedViewProperties, ScriptGUIFramedViewMethods);



static JSValueRef ScriptGUIScrollViewGetSliderX(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIScrollView>(instance)->sliderX);
}

static bool ScriptGUIScrollViewSetSliderX(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIScrollView setSliderX(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIScrollView>(instance);
    if(isfinite(numberValue)) {
        objectPtr->sliderX = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIScrollViewGetSliderY(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIScrollView>(instance)->sliderY);
}

static bool ScriptGUIScrollViewSetSliderY(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIScrollView setSliderY(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIScrollView>(instance);
    if(isfinite(numberValue)) {
        objectPtr->sliderY = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIScrollViewGetScrollX(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIScrollView>(instance)->scrollPosX);
}

static bool ScriptGUIScrollViewSetScrollX(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIScrollView setScrollX(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIScrollView>(instance);
    if(isfinite(numberValue)) {
        objectPtr->scrollPosX = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIScrollViewGetScrollY(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIScrollView>(instance)->scrollPosY);
}

static bool ScriptGUIScrollViewSetScrollY(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIScrollView setScrollY(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIScrollView>(instance);
    if(isfinite(numberValue)) {
        objectPtr->scrollPosY = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIScrollViewGetContentWidth(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIScrollView>(instance)->contentWidth);
}

static bool ScriptGUIScrollViewSetContentWidth(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIScrollView setContentWidth(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIScrollView>(instance);
    if(isfinite(numberValue)) {
        objectPtr->contentWidth = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptGUIScrollViewGetContentHeight(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<GUIScrollView>(instance)->contentHeight);
}

static bool ScriptGUIScrollViewSetContentHeight(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GUIScrollView setContentHeight(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<GUIScrollView>(instance);
    if(isfinite(numberValue)) {
        objectPtr->contentHeight = numberValue;
        return true;
    }else
        return false;
}

JSStaticValue ScriptGUIScrollViewProperties[] = {
    {"sliderX", ScriptGUIScrollViewGetSliderX, ScriptGUIScrollViewSetSliderX, kJSPropertyAttributeDontDelete},
    {"sliderY", ScriptGUIScrollViewGetSliderY, ScriptGUIScrollViewSetSliderY, kJSPropertyAttributeDontDelete},
    {"scrollX", ScriptGUIScrollViewGetScrollX, ScriptGUIScrollViewSetScrollX, kJSPropertyAttributeDontDelete},
    {"scrollY", ScriptGUIScrollViewGetScrollY, ScriptGUIScrollViewSetScrollY, kJSPropertyAttributeDontDelete},
    {"contentWidth", ScriptGUIScrollViewGetContentWidth, ScriptGUIScrollViewSetContentWidth, kJSPropertyAttributeDontDelete},
    {"contentHeight", ScriptGUIScrollViewGetContentHeight, ScriptGUIScrollViewSetContentHeight, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

ScriptClassDefinition(GUIScrollView, ScriptGUIScrollViewProperties, NULL);