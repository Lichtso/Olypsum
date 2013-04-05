//
//  ScriptDisplayObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 04.04.13.
//
//

#include "ScriptManager.h"

v8::Handle<v8::Value> ScriptModelObject::GetIntegrity(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ModelObject* objectPtr = getDataOfInstance<ModelObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->integrity));
}

void ScriptModelObject::SetIntegrity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber())
        v8::ThrowException(v8::String::New("ModelObject integrity=: Invalid argument"));
    ModelObject* objectPtr = getDataOfInstance<ModelObject>(info.This());
    if(objectPtr->integrity <= 0.0) return;
    objectPtr->integrity = fmax(0.0, value->NumberValue());
}

v8::Handle<v8::Value> ScriptModelObject::GetModel(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ModelObject* objectPtr = getDataOfInstance<ModelObject>(info.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<Model>(objectPtr->model, name);
    if(!filePackage) return v8::Undefined();
    return handleScope.Close(v8::String::New(fileManager.getResourcePath(filePackage, name).c_str()));
}

void ScriptModelObject::SetModel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsString())
        v8::ThrowException(v8::String::New("ModelObject model=: Invalid argument"));
    ModelObject* objectPtr = getDataOfInstance<ModelObject>(info.This());
    auto model = fileManager.initResource<Model>(scriptManager->stdStringOf(value->ToString()));
    if(model) objectPtr->setModel(model);
}

v8::Handle<v8::Value> ScriptModelObject::GetTextureAnimation(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1 || !args[0]->IsInt32())
        return v8::ThrowException(v8::String::New("ModelObject getTextureAnimation: Invalid argument"));
    ModelObject* objectPtr = getDataOfInstance<ModelObject>(args.This());
    if(args[0]->Uint32Value() > objectPtr->textureAnimation.size())
        return v8::ThrowException(v8::String::New("ModelObject getTextureAnimation: Out of bounds"));
    return handleScope.Close(v8::Number::New(objectPtr->textureAnimation[args[0]->Uint32Value()]));
}

v8::Handle<v8::Value> ScriptModelObject::SetTextureAnimation(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2 || !args[0]->IsInt32() || !args[1]->IsNumber())
        return v8::ThrowException(v8::String::New("ModelObject setTextureAnimation: Invalid argument"));
    ModelObject* objectPtr = getDataOfInstance<ModelObject>(args.This());
    if(args[0]->Uint32Value() > objectPtr->textureAnimation.size())
        return v8::ThrowException(v8::String::New("ModelObject getTextureAnimation: Out of bounds"));
    objectPtr->textureAnimation[args[0]->Uint32Value()] = args[1]->NumberValue();
    return v8::Undefined();
}

ScriptModelObject::ScriptModelObject(const char* name) :ScriptPhysicObject(name) {
    
}

ScriptModelObject::ScriptModelObject() :ScriptModelObject("ModelObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("integrity"), GetIntegrity, SetIntegrity);
    objectTemplate->SetAccessor(v8::String::New("model"), GetModel, SetModel);
    objectTemplate->Set(v8::String::New("getTextureAnimation"), v8::FunctionTemplate::New(GetTextureAnimation));
    objectTemplate->Set(v8::String::New("setTextureAnimation"), v8::FunctionTemplate::New(SetTextureAnimation));
    
    functionTemplate->Inherit(scriptPhysicObject.functionTemplate);
}



v8::Handle<v8::Value> ScriptRigidObject::GetMass(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(v8::Number::New((body->getInvMass() == 0.0) ? 0.0 : 1.0/body->getInvMass()));
}

void ScriptRigidObject::SetMass(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber() || value->NumberValue() < 0.0)
        v8::ThrowException(v8::String::New("RigidObject mass=: Invalid argument"));
    btVector3 inertia;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    body->getCollisionShape()->calculateLocalInertia(value->NumberValue(), inertia);
    body->setMassProps(value->NumberValue(), inertia);
}

v8::Handle<v8::Value> ScriptRigidObject::GetAngularVelocity(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(scriptVector3.newInstance(body->getAngularVelocity()));
}

void ScriptRigidObject::SetAngularVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!scriptVector3.isCorrectInstance(value))
        v8::ThrowException(v8::String::New("RigidObject angularVelocity=: Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    body->setAngularVelocity(scriptVector3.getDataOfInstance(value));
}

v8::Handle<v8::Value> ScriptRigidObject::GetLinearVelocity(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(scriptVector3.newInstance(body->getLinearVelocity()));
}

void ScriptRigidObject::SetLinearVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!scriptVector3.isCorrectInstance(value))
        v8::ThrowException(v8::String::New("RigidObject linearVelocity=: Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    body->setLinearVelocity(scriptVector3.getDataOfInstance(value));
}

v8::Handle<v8::Value> ScriptRigidObject::GetAngularFactor(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(scriptVector3.newInstance(body->getAngularFactor()));
}

void ScriptRigidObject::SetAngularFactor(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!scriptVector3.isCorrectInstance(value))
        v8::ThrowException(v8::String::New("RigidObject angularFactor=: Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    body->setAngularFactor(scriptVector3.getDataOfInstance(value));
}

v8::Handle<v8::Value> ScriptRigidObject::GetLinearFactor(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(scriptVector3.newInstance(body->getLinearFactor()));
}

void ScriptRigidObject::SetLinearFactor(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!scriptVector3.isCorrectInstance(value))
        v8::ThrowException(v8::String::New("RigidObject linearFactor=: Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    body->setLinearFactor(scriptVector3.getDataOfInstance(value));
}

v8::Handle<v8::Value> ScriptRigidObject::GetAngularDamping(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(v8::Number::New(body->getAngularDamping()));
}

void ScriptRigidObject::SetAngularDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber())
        v8::ThrowException(v8::String::New("RigidObject angularDamping=: Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    body->setAngularDamping(value->NumberValue());
}

v8::Handle<v8::Value> ScriptRigidObject::GetLinearDamping(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(v8::Number::New(body->getLinearDamping()));
}

void ScriptRigidObject::SetLinearDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber())
        v8::ThrowException(v8::String::New("RigidObject linearDamping=: Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    body->setLinearDamping(value->NumberValue());
}

v8::Handle<v8::Value> ScriptRigidObject::ApplyImpulseAtPoint(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2)
        return v8::ThrowException(v8::String::New("RigidObject applyImpulseAtPoint(): To less arguments"));
    if(!scriptVector3.isCorrectInstance(args[0]) || !scriptVector3.isCorrectInstance(args[1]))
        v8::ThrowException(v8::String::New("RigidObject applyImpulseAtPoint(): Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    body->applyImpulse(scriptVector3.getDataOfInstance(args[0]), scriptVector3.getDataOfInstance(args[1]));
    return v8::Undefined();
}

v8::Handle<v8::Value> ScriptRigidObject::ApplyAngularImpulse(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return v8::ThrowException(v8::String::New("RigidObject applyAngularImpulse(): To less arguments"));
    if(!scriptVector3.isCorrectInstance(args[0]))
        v8::ThrowException(v8::String::New("RigidObject applyAngularImpulse(): Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    body->applyTorqueImpulse(scriptVector3.getDataOfInstance(args[0]));
    return v8::Undefined();
}

v8::Handle<v8::Value> ScriptRigidObject::ApplyLinearImpulse(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return v8::ThrowException(v8::String::New("RigidObject applyLinearImpulse(): To less arguments"));
    if(!scriptVector3.isCorrectInstance(args[0]))
        v8::ThrowException(v8::String::New("RigidObject applyLinearImpulse(): Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    body->applyCentralImpulse(scriptVector3.getDataOfInstance(args[0]));
    return v8::Undefined();
}

ScriptRigidObject::ScriptRigidObject() :ScriptModelObject("RigidObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("mass"), GetMass, SetMass);
    objectTemplate->SetAccessor(v8::String::New("angularVelocity"), GetAngularVelocity, SetAngularVelocity);
    objectTemplate->SetAccessor(v8::String::New("linearVelocity"), GetLinearVelocity, SetLinearVelocity);
    objectTemplate->SetAccessor(v8::String::New("angularFactor"), GetAngularFactor, SetAngularFactor);
    objectTemplate->SetAccessor(v8::String::New("linearFactor"), GetLinearFactor, SetLinearFactor);
    objectTemplate->SetAccessor(v8::String::New("angularDamping"), GetAngularDamping, SetAngularDamping);
    objectTemplate->SetAccessor(v8::String::New("linearDamping"), GetLinearDamping, SetLinearDamping);
    objectTemplate->Set(v8::String::New("applyImpulseAtPoint"), v8::FunctionTemplate::New(ApplyImpulseAtPoint));
    objectTemplate->Set(v8::String::New("applyAngularImpulse"), v8::FunctionTemplate::New(ApplyAngularImpulse));
    objectTemplate->Set(v8::String::New("applyLinearImpulse"), v8::FunctionTemplate::New(ApplyLinearImpulse));
    
    functionTemplate->Inherit(scriptModelObject.functionTemplate);
}



ScriptModelObject scriptModelObject;
ScriptRigidObject scriptRigidObject;