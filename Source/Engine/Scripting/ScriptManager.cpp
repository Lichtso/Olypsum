//
//  ScriptContext.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 27.01.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "AppMain.h"

#define ScriptInitClass(name) extern JSClassDefinition Script##name##Definition;\
extern JSClassDefinition Script##name##Static;\
ScriptClasses[Script##name] = JSClassCreate(&Script##name##Definition);\
ScriptStatics[Script##name] = JSClassCreate(&Script##name##Static);\
ScriptStringClassNames[Script##name].str = JSStringCreateWithUTF8CString(#name);
#define ScriptInitClassWithParent(name, parent) extern JSClassDefinition Script##name##Definition;\
extern JSClassDefinition Script##name##Static;\
Script##name##Definition.parentClass = ScriptClasses[Script##parent];\
ScriptClasses[Script##name] = JSClassCreate(&Script##name##Definition);\
ScriptStatics[Script##name] = JSClassCreate(&Script##name##Static);\
ScriptStringClassNames[Script##name].str = JSStringCreateWithUTF8CString(#name);
#define ScriptInitSingleton(name) extern JSClassDefinition Script##name##Definition;\
ScriptStatics[Script##name] = JSClassCreate(&Script##name##Definition);\
ScriptStringClassNames[Script##name].str = JSStringCreateWithUTF8CString(#name);
#define ScriptAddSingleton(name) staticObjects[Script##name] = JSObjectMake(globalContext, ScriptStatics[Script##name], NULL);

void initScriptClasses() {
    ScriptInitClass(Vector3);
    ScriptInitClass(Quaternion);
    ScriptInitClass(Matrix4);
    ScriptInitClass(BaseClass);
    
    ScriptInitClassWithParent(BaseObject, BaseClass);
    ScriptInitClassWithParent(CamObject, BaseObject);
    ScriptInitClassWithParent(SoundObject, BaseObject);
    ScriptInitClassWithParent(PhysicObject, BaseObject);
    ScriptInitClassWithParent(ParticlesObject, PhysicObject);
    ScriptInitClassWithParent(MatterObject, PhysicObject);
    ScriptInitClassWithParent(RigidObject, MatterObject);
    ScriptInitClassWithParent(TerrainObject, MatterObject);
    ScriptInitClassWithParent(LightObject, BaseObject);
    ScriptInitClassWithParent(DirectionalLight, LightObject);
    ScriptInitClassWithParent(PositionalLight, LightObject);
    ScriptInitClassWithParent(SpotLight, LightObject);
    
    ScriptInitClassWithParent(BaseLink, BaseClass);
    //ScriptInitClassWithParent(BoneLink, BaseLink);
    //ScriptInitClassWithParent(TransformLink, BaseLink);
    ScriptInitClassWithParent(PhysicLink, BaseLink);
    //ScriptInitClassWithParent(ConeTwistPhysicLink, PhysicLink);
    //ScriptInitClassWithParent(Dof6PhysicLink, PhysicLink);
    //ScriptInitClassWithParent(GearPhysicLink, PhysicLink);
    //ScriptInitClassWithParent(HingePhysicLink, PhysicLink);
    //ScriptInitClassWithParent(PointPhysicLink, PhysicLink);
    //ScriptInitClassWithParent(SliderPhysicLink, PhysicLink);
    
    /*ScriptInitClass(GUIRect);
    ScriptInitClassWithParent(GUIView, GUIRect);
    ScriptInitClassWithParent(GUIFramedView, GUIView);
    ScriptInitClassWithParent(GUIScreenView, GUIView);
    ScriptInitClassWithParent(GUIScrollView, GUIView);
    ScriptInitClassWithParent(GUIButton, GUIRect);
    ScriptInitClassWithParent(GUICheckBox, GUIRect);
    ScriptInitClassWithParent(GUISlider, GUIRect);
    ScriptInitClassWithParent(GUITabs, GUIRect);
    ScriptInitClassWithParent(GUITextField, GUIRect);
    ScriptInitClassWithParent(GUIImage, GUIRect);
    ScriptInitClassWithParent(GUILabel, GUIRect);
    ScriptInitClassWithParent(GUIProgressBar, GUIRect);*/
    
    ScriptInitSingleton(Animation);
    ScriptInitSingleton(Intersection);
    ScriptInitSingleton(Mouse);
    ScriptInitSingleton(Keyboard);
    ScriptInitSingleton(Engine);
}



JSValueRef ScriptManager::readCdataXMLNode(rapidxml::xml_node<xmlUsedCharType>* node, JSContextRef context) {
    if(!node || !(node = node->first_node("Data")) || !(node = node->first_node()) || node->type() != rapidxml::node_cdata)
        return JSValueMakeNull(context);
    JSStringRef str = JSStringCreateWithUTF8CString(node->value());
    JSValueRef result = JSValueMakeString(context, str);
    JSStringRelease(str);
    return result;
}

void ScriptManager::writeCdataXMLNode(rapidxml::xml_document<xmlUsedCharType>& doc,
                                      rapidxml::xml_node<xmlUsedCharType>*& parentNode,
                                      const char* name, JSContextRef context, JSValueRef value) {
    rapidxml::xml_node<xmlUsedCharType>* dataNode = parentNode->first_node(name);
    if(!value || JSValueIsUndefined(context, value)) {
        if(dataNode)
            parentNode->remove_node(dataNode);
        return;
    }
    if(!dataNode) {
        dataNode = doc.allocate_node(rapidxml::node_element);
        dataNode->name(name);
        parentNode->append_node(dataNode);
    }
    rapidxml::xml_node<xmlUsedCharType>* cdata = dataNode->first_node();
    if(!cdata) {
        cdata = doc.allocate_node(rapidxml::node_cdata);
        dataNode->append_node(cdata);
    }
    JSStringRef str = JSValueToStringCopy(context, value, NULL);
    auto size = JSStringGetMaximumUTF8CStringSize(str);
    char* buffer = doc.allocate_string(NULL, size);
    JSStringGetUTF8CString(str, buffer, size);
    cdata->value(buffer);
}

ScriptManager::ScriptManager() {
    contextGroup = JSContextGroupCreate();
    globalContext = JSGlobalContextCreateInGroup(contextGroup, NULL);
    
    for(unsigned i = 0; i < ScriptStaticsCount; i ++)
        staticObjects[i] = NULL; //TODO: Remove
    
    ScriptAddSingleton(Vector3);
    ScriptAddSingleton(Quaternion);
    ScriptAddSingleton(Matrix4);
    ScriptAddSingleton(BaseClass);
    ScriptAddSingleton(BaseObject);
    ScriptAddSingleton(PhysicObject);
    ScriptAddSingleton(ParticlesObject);
    ScriptAddSingleton(MatterObject);
    ScriptAddSingleton(RigidObject);
    ScriptAddSingleton(TerrainObject);
    ScriptAddSingleton(CamObject);
    ScriptAddSingleton(SoundObject);
    ScriptAddSingleton(LightObject);
    ScriptAddSingleton(DirectionalLight);
    ScriptAddSingleton(SpotLight);
    ScriptAddSingleton(PositionalLight);
    ScriptAddSingleton(BaseLink);
    ScriptAddSingleton(PhysicLink);
    
    ScriptAddSingleton(Animation);
    ScriptAddSingleton(Intersection);
    ScriptAddSingleton(Mouse);
    ScriptAddSingleton(Keyboard);
    ScriptAddSingleton(Engine);
}

ScriptManager::~ScriptManager() {
    //for(auto iterator : animations)
    //    delete iterator.second;
    JSGlobalContextRelease(globalContext);
    JSContextGroupRelease(contextGroup);
}

void ScriptManager::gameTick() {
    /*foreach_e(animations, iterator)
        if(iterator->second->gameTick(iterator->first)) {
            delete iterator->second;
            animations.erase(iterator);
        }*/
    auto timeNow = getTime();
    foreach_e(timers, iterator)
        if((*iterator)->timeNext < timeNow && (*iterator)->gameTick(timeNow)) {
            delete *iterator;
            timers.erase(iterator);
        }
}

void ScriptManager::clean() {
    for(auto iterator : timers)
        delete iterator;
}

JSClassRef ScriptClasses[ScriptClassesCount];
JSClassRef ScriptStatics[ScriptStaticsCount];
std::unique_ptr<ScriptManager> scriptManager;