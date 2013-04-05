//
//  ScriptObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 09.03.13.
//
//

#include "ScriptManager.h"

v8::Handle<v8::Value> ScriptBaseObject::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !args[0]->IsExternal())
        return v8::ThrowException(v8::String::New("BaseObject Constructor: Class can't be instantiated"));
    
    args.This()->SetInternalField(0, args[0]);
    return args.This();
}

v8::Handle<v8::Value> ScriptBaseObject::GetTransformation(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(info.This());
    Matrix4 transformation(objectPtr->getTransformation());
    return handleScope.Close(scriptMatrix4.newInstance(transformation));
}

void ScriptBaseObject::SetTransformation(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!scriptMatrix4.isCorrectInstance(value))
        v8::ThrowException(v8::String::New("BaseObject transform=: Invalid argument"));
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(info.This());
    objectPtr->setTransformation(scriptMatrix4.getDataOfInstance(value).getBTTransform());
}

v8::Handle<v8::Value> ScriptBaseObject::GetScriptClass(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(info.This());
    std::string path = objectPtr->scriptFile->name;
    if(objectPtr->scriptFile->filePackage != levelManager.levelPackage)
        path = std::string("../")+objectPtr->scriptFile->filePackage->name+'/'+path;
    return handleScope.Close(v8::String::New(path.c_str()));
}

void ScriptBaseObject::SetScriptClass(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsString())
        v8::ThrowException(v8::String::New("BaseObject scriptClass=: Invalid argument"));
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(info.This());
    FilePackage* filePackage;
    std::string name;
    if(fileManager.readResource(scriptManager->stdStringOf(value->ToString()), filePackage, name))
        objectPtr->scriptFile = scriptManager->getScriptFile(filePackage, name);
}

v8::Handle<v8::Value> ScriptBaseObject::GetLinkNames(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(args.This());
    v8::Handle<v8::Array> array = v8::Array::New(objectPtr->links.size());
    unsigned int i = 0;
    for(auto iterator : objectPtr->links)
        array->Set(i ++, v8::String::New(iterator.first.c_str()));
    return handleScope.Close(array);
}

v8::Handle<v8::Value> ScriptBaseObject::GetLinkedObject(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return v8::ThrowException(v8::String::New("BaseObject getLinkedObject(): To less arguments"));
    if(!args[0]->IsString())
        return v8::ThrowException(v8::String::New("BaseObject getLinkedObject(): Invalid argument"));
    BaseObject* objectPtr = getDataOfInstance<BaseObject>(args.This());
    BaseObject* linkedObject = objectPtr->findObjectByPath(scriptManager->stdStringOf(args[0]->ToString()));
    if(!linkedObject) return v8::Undefined();
    return handleScope.Close(linkedObject->scriptInstance);
}

ScriptBaseObject::ScriptBaseObject(const char* name) :ScriptClass(name, Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> instanceTemplate = functionTemplate->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(1);
}

ScriptBaseObject::ScriptBaseObject() :ScriptBaseObject("BaseObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("transformation"), GetTransformation, SetTransformation);
    objectTemplate->SetAccessor(v8::String::New("scriptClass"), GetScriptClass, SetScriptClass);
    objectTemplate->Set(v8::String::New("getLinkNames"), v8::FunctionTemplate::New(GetLinkNames));
    objectTemplate->Set(v8::String::New("getLinkedObject"), v8::FunctionTemplate::New(GetLinkedObject));
}



v8::Handle<v8::Value> ScriptPhysicObject::GetCollisionShape(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(info.This())->getBody();
    std::string buffer = levelManager.getCollisionShapeName(physicBody->getCollisionShape());
    if(buffer.size() == 0) return v8::Undefined();
    return handleScope.Close(v8::String::New(buffer.c_str()));
}

void ScriptPhysicObject::SetCollisionShape(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsString())
        v8::ThrowException(v8::String::New("BaseObject collisionShape=: Invalid argument"));
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(info.This())->getBody();
    btCollisionShape* shape = levelManager.getCollisionShape(scriptManager->stdStringOf(value->ToString()));
    if(shape)
        physicBody->setCollisionShape(shape);
    else
        v8::ThrowException(v8::String::New("BaseObject collisionShape=: Invalid name"));
}

v8::Handle<v8::Value> ScriptPhysicObject::GetCollisionShapeInfo(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btCollisionShape* shape = getDataOfInstance<PhysicObject>(args.This())->getBody()->getCollisionShape();
    v8::Handle<v8::Object> result = v8::Object::New();
    result->Set(v8::String::New("type"), v8::String::New(shape->getName()));
    switch(shape->getShapeType()) {
        case CYLINDER_SHAPE_PROXYTYPE: {
            result->Set(v8::String::New("size"), scriptVector3.newInstance(static_cast<btCylinderShape*>(shape)->getHalfExtentsWithMargin()));
        } break;
        case BOX_SHAPE_PROXYTYPE: {
            result->Set(v8::String::New("size"), scriptVector3.newInstance(static_cast<btBoxShape*>(shape)->getHalfExtentsWithMargin()));
        } break;
        case SPHERE_SHAPE_PROXYTYPE: {
            result->Set(v8::String::New("radius"), v8::Number::New(static_cast<btSphereShape*>(shape)->getRadius()));
        } break;
        case CAPSULE_SHAPE_PROXYTYPE: {
            btCapsuleShape* shape = static_cast<btCapsuleShape*>(shape);
            result->Set(v8::String::New("radius"), v8::Number::New(shape->getRadius()));
            result->Set(v8::String::New("length"), v8::Number::New(shape->getHalfHeight()));
        } break;
        case CONE_SHAPE_PROXYTYPE: {
            btConeShape* shape = static_cast<btConeShape*>(shape);
            result->Set(v8::String::New("radius"), v8::Number::New(shape->getRadius()));
            result->Set(v8::String::New("length"), v8::Number::New(shape->getHeight()));
        } break;
        case MULTI_SPHERE_SHAPE_PROXYTYPE: {
            btMultiSphereShape* shape = static_cast<btMultiSphereShape*>(shape);
            v8::Handle<v8::Array> positions = v8::Array::New(shape->getSphereCount());
            v8::Handle<v8::Array> radii = v8::Array::New(shape->getSphereCount());
            for(unsigned int i = 0; i < shape->getSphereCount(); i ++) {
                positions->Set(i, scriptVector3.newInstance(shape->getSpherePosition(i)));
                radii->Set(i, v8::Number::New(shape->getSphereRadius(i)));
            }
            result->Set(v8::String::New("positions"), positions);
            result->Set(v8::String::New("radii"), radii);
        } break;
        case COMPOUND_SHAPE_PROXYTYPE: {
            btCompoundShape* shape = static_cast<btCompoundShape*>(shape);
            
            v8::Handle<v8::Array> transformations = v8::Array::New(shape->getNumChildShapes());
            v8::Handle<v8::Array> children = v8::Array::New(shape->getNumChildShapes());
            for(unsigned int i = 0; i < shape->getNumChildShapes(); i ++) {
                transformations->Set(i, scriptMatrix4.newInstance(shape->getChildTransform(i)));
                std::string buffer = levelManager.getCollisionShapeName(shape->getChildShape(i));
                if(buffer.size())
                    children->Set(i, v8::String::New(buffer.c_str()));
            }
            result->Set(v8::String::New("transformations"), transformations);
            result->Set(v8::String::New("children"), children);
        } break;
        case CONVEX_HULL_SHAPE_PROXYTYPE: {
            btConvexHullShape* shape = static_cast<btConvexHullShape*>(shape);
            v8::Handle<v8::Array> points = v8::Array::New(shape->getNumPoints());
            for(unsigned int i = 0; i < shape->getNumPoints(); i ++)
                points->Set(i, scriptVector3.newInstance(shape->getUnscaledPoints()[i]));
            result->Set(v8::String::New("points"), points);
        } break;
        case STATIC_PLANE_PROXYTYPE: {
            btStaticPlaneShape* shape = static_cast<btStaticPlaneShape*>(shape);
            result->Set(v8::String::New("normal"), scriptVector3.newInstance(shape->getPlaneNormal()));
            result->Set(v8::String::New("distance"), v8::Number::New(shape->getPlaneConstant()));
        } break;
    }
    return handleScope.Close(result);
}

ScriptPhysicObject::ScriptPhysicObject(const char* name) :ScriptBaseObject(name) {
    
}

ScriptPhysicObject::ScriptPhysicObject() :ScriptPhysicObject("PhysicsObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("collisionShape"), GetCollisionShape, SetCollisionShape);
    objectTemplate->Set(v8::String::New("getCollisionShapeInfo"), v8::FunctionTemplate::New(GetCollisionShapeInfo));
    
    functionTemplate->Inherit(scriptBaseObject.functionTemplate);
}



ScriptBaseObject scriptBaseObject;
ScriptPhysicObject scriptPhysicObject;