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
    if(!value->IsNumber()) return;
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
    if(!value->IsString()) return;
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
    if(!value->IsNumber() || value->NumberValue() < 0.0) return;
    btVector3 inertia;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    body->getCollisionShape()->calculateLocalInertia(value->NumberValue(), inertia);
    body->setMassProps(value->NumberValue(), inertia);
    body->setActivationState(ACTIVE_TAG);
}

v8::Handle<v8::Value> ScriptRigidObject::GetAngularVelocity(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(scriptVector3.newInstance(body->getAngularVelocity()));
}

v8::Handle<v8::Value> ScriptRigidObject::SetAngularVelocity(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("RigidObject setAngularVelocity(): Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    body->setAngularVelocity(scriptVector3.getDataOfInstance(args[0]));
    body->setActivationState(ACTIVE_TAG);
    return args.This();
}

v8::Handle<v8::Value> ScriptRigidObject::GetLinearVelocity(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(scriptVector3.newInstance(body->getLinearVelocity()));
}

v8::Handle<v8::Value> ScriptRigidObject::SetLinearVelocity(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("RigidObject setLinearVelocity(): Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    body->setLinearVelocity(scriptVector3.getDataOfInstance(args[0]));
    body->setActivationState(ACTIVE_TAG);
    return args.This();
}

v8::Handle<v8::Value> ScriptRigidObject::GetAngularFactor(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(scriptVector3.newInstance(body->getAngularFactor()));
}

v8::Handle<v8::Value> ScriptRigidObject::SetAngularFactor(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("RigidObject setAngularFactor(): Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    body->setAngularFactor(scriptVector3.getDataOfInstance(args[0]));
    return args.This();
}

v8::Handle<v8::Value> ScriptRigidObject::GetLinearFactor(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(scriptVector3.newInstance(body->getLinearFactor()));
}

v8::Handle<v8::Value> ScriptRigidObject::SetLinearFactor(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("RigidObject setLinearFactor(): Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    body->setLinearFactor(scriptVector3.getDataOfInstance(args[0]));
    return args.This();
}

v8::Handle<v8::Value> ScriptRigidObject::GetAngularDamping(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    return handleScope.Close(v8::Number::New(body->getAngularDamping()));
}

void ScriptRigidObject::SetAngularDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
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
    if(!value->IsNumber()) return;
    btRigidBody* body = getDataOfInstance<RigidObject>(info.This())->getBody();
    body->setLinearDamping(value->NumberValue());
}

v8::Handle<v8::Value> ScriptRigidObject::SetTransformation(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptMatrix4.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("RigidObject setTransformation(): Invalid argument"));
    RigidObject* objectPtr = getDataOfInstance<RigidObject>(args.This());
    objectPtr->setTransformation(scriptMatrix4.getDataOfInstance(args[0])->getBTTransform());
    objectPtr->getBody()->setActivationState(ACTIVE_TAG);
    return args.This();
}

v8::Handle<v8::Value> ScriptRigidObject::ApplyImpulseAtPoint(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2)
        return v8::ThrowException(v8::String::New("RigidObject applyImpulseAtPoint(): To less arguments"));
    if(!scriptVector3.isCorrectInstance(args[0]) || !scriptVector3.isCorrectInstance(args[1]))
        return v8::ThrowException(v8::String::New("RigidObject applyImpulseAtPoint(): Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    body->applyImpulse(scriptVector3.getDataOfInstance(args[0]), scriptVector3.getDataOfInstance(args[1]));
    body->setActivationState(ACTIVE_TAG);
    return v8::Undefined();
}

v8::Handle<v8::Value> ScriptRigidObject::ApplyAngularImpulse(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return v8::ThrowException(v8::String::New("RigidObject applyAngularImpulse(): To less arguments"));
    if(!scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("RigidObject applyAngularImpulse(): Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    body->applyTorqueImpulse(scriptVector3.getDataOfInstance(args[0]));
    body->setActivationState(ACTIVE_TAG);
    return v8::Undefined();
}

v8::Handle<v8::Value> ScriptRigidObject::ApplyLinearImpulse(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1)
        return v8::ThrowException(v8::String::New("RigidObject applyLinearImpulse(): To less arguments"));
    if(!scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("RigidObject applyLinearImpulse(): Invalid argument"));
    btRigidBody* body = getDataOfInstance<RigidObject>(args.This())->getBody();
    body->applyCentralImpulse(scriptVector3.getDataOfInstance(args[0]));
    body->setActivationState(ACTIVE_TAG);
    return v8::Undefined();
}

ScriptRigidObject::ScriptRigidObject() :ScriptModelObject("RigidObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("mass"), GetMass, SetMass);
    objectTemplate->SetAccessor(v8::String::New("angularVelocity"), GetAngularVelocity);
    objectTemplate->Set(v8::String::New("setAngularVelocity"), v8::FunctionTemplate::New(SetAngularVelocity));
    objectTemplate->SetAccessor(v8::String::New("linearVelocity"), GetLinearVelocity);
    objectTemplate->Set(v8::String::New("setLinearVelocity"), v8::FunctionTemplate::New(SetLinearVelocity));
    objectTemplate->SetAccessor(v8::String::New("angularFactor"), GetAngularFactor);
    objectTemplate->Set(v8::String::New("setAngularFactor"), v8::FunctionTemplate::New(SetAngularFactor));
    objectTemplate->SetAccessor(v8::String::New("linearFactor"), GetLinearFactor);
    objectTemplate->Set(v8::String::New("setLinearFactor"), v8::FunctionTemplate::New(SetLinearFactor));
    objectTemplate->SetAccessor(v8::String::New("angularDamping"), GetAngularDamping, SetAngularDamping);
    objectTemplate->SetAccessor(v8::String::New("linearDamping"), GetLinearDamping, SetLinearDamping);
    objectTemplate->Set(v8::String::New("setTransformation"), v8::FunctionTemplate::New(SetTransformation));
    objectTemplate->Set(v8::String::New("applyImpulseAtPoint"), v8::FunctionTemplate::New(ApplyImpulseAtPoint));
    objectTemplate->Set(v8::String::New("applyAngularImpulse"), v8::FunctionTemplate::New(ApplyAngularImpulse));
    objectTemplate->Set(v8::String::New("applyLinearImpulse"), v8::FunctionTemplate::New(ApplyLinearImpulse));
    
    functionTemplate->Inherit(scriptModelObject.functionTemplate);
}



v8::Handle<v8::Value> ScriptTerrainObject::GetWidth(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->width));
}

v8::Handle<v8::Value> ScriptTerrainObject::GetLength(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->length));
}

v8::Handle<v8::Value> ScriptTerrainObject::GetBitDepth(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->bitDepth << 2));
}

void ScriptTerrainObject::SetBitDepth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32() || value->Uint32Value() % 4 != 0 || value->Uint32Value() == 0 || value->Uint32Value() > 8) return;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(info.This());
    objectPtr->bitDepth = value->Uint32Value() >> 2;
}

v8::Handle<v8::Value> ScriptTerrainObject::UpdateModel(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    TerrainObject* objectPtr = getDataOfInstance<TerrainObject>(args.This());
    objectPtr->updateModel();
    return v8::Undefined();
}

ScriptTerrainObject::ScriptTerrainObject() :ScriptPhysicObject("TerrainObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("width"), GetWidth);
    objectTemplate->SetAccessor(v8::String::New("length"), GetLength);
    objectTemplate->SetAccessor(v8::String::New("bitDepth"), GetBitDepth, SetBitDepth);
    objectTemplate->Set(v8::String::New("updateModel"), v8::FunctionTemplate::New(UpdateModel));
    
    functionTemplate->Inherit(scriptPhysicObject.functionTemplate);
}



ScriptModelObject scriptModelObject;
ScriptRigidObject scriptRigidObject;
ScriptTerrainObject scriptTerrainObject;