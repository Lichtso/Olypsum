//
//  ScriptManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 27.01.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef ScriptManager_h
#define ScriptManager_h

#include "ScriptGUI.h"

enum ScriptClassName {
    ScriptVector3,
    ScriptQuaternion,
    ScriptMatrix4,
    ScriptBaseClass,
    ScriptBaseObject,
    ScriptCamObject,
    ScriptSoundObject,
    ScriptPhysicObject,
    ScriptParticlesObject,
    ScriptMatterObject,
    ScriptRigidObject,
    ScriptTerrainObject,
    ScriptLightObject,
    ScriptDirectionalLight,
    ScriptPositionalLight,
    ScriptSpotLight,
    ScriptBaseLink,
    ScriptBoneLink,
    ScriptTransformLink,
    ScriptPhysicLink,
    ScriptConeTwistPhysicLink,
    ScriptDof6PhysicLink,
    ScriptGearPhysicLink,
    ScriptHingePhysicLink,
    ScriptPointPhysicLink,
    ScriptSliderPhysicLink,
    ScriptGUIRect,
    ScriptGUIView,
    ScriptGUIScreenView,
    ScriptGUIFramedView,
    ScriptGUIScrollView,
    ScriptGUIButton,
    ScriptGUICheckBox,
    ScriptGUITabs,
    ScriptGUISlider,
    ScriptGUITextField,
    ScriptGUIImage,
    ScriptGUILabel,
    ScriptGUIProgressBar,
    ScriptAnimation,
    ScriptIntersection,
    ScriptMouse,
    ScriptKeyboard,
    ScriptEngine
};

extern JSClassRef ScriptClasses[ScriptClassesCount];
extern JSClassRef ScriptStatics[ScriptStaticsCount];
void initScriptClasses();

class ScriptManager {
    public:
    JSContextGroupRef contextGroup;
    JSGlobalContextRef globalContext;
    JSObjectRef staticObjects[ScriptStaticsCount];
    FileResourcePtr<ScriptFile> mainScript;
    std::unordered_set<AnimationTimer*> timers;
    static JSValueRef readCdataXMLNode(rapidxml::xml_node<xmlUsedCharType>* node, JSContextRef context);
    static void writeCdataXMLNode(rapidxml::xml_document<xmlUsedCharType>& doc,
                                  rapidxml::xml_node<xmlUsedCharType>*& parentNode,
                                  const char* name, JSContextRef context, JSValueRef value);
    ScriptManager();
    ~ScriptManager();
    void gameTick();
    void clean();
};

extern JSClassDefinition ScriptGlobalDefinition;
extern std::unique_ptr<ScriptManager> scriptManager;

#endif
