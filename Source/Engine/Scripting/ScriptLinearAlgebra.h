//
//  ScriptLinearAlgebra.h
//  Olypsum
//
//  Created by Alexander Meißner on 30.01.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef ScriptLinearAlgebra_h
#define ScriptLinearAlgebra_h

#include "../Menu/Menu.h"

btVector3 getScriptVector3(JSContextRef context, JSValueRef instance);
void setScriptVector3(JSContextRef context, JSValueRef instance, const btVector3& vec);
JSObjectRef newScriptVector3(JSContextRef context, const btVector3& vec);

btQuaternion getScriptQuaternion(JSContextRef context, JSValueRef instance);
void setScriptQuaternion(JSContextRef context, JSValueRef instance, const btQuaternion& quaternion);
JSObjectRef newScriptQuaternion(JSContextRef context, const btQuaternion& quaternion);

JSObjectRef newScriptMatrix4(JSContextRef context, const Matrix4& matrix);

#endif
