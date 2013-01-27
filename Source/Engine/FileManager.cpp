//
//  FileManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 07.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "FileManager.h"
#include "ShaderProgram.h"

std::shared_ptr<FilePackageResource> FilePackageResource::load(FilePackage* filePackageB, const std::string& nameB) {
    filePackage = filePackageB;
    name = nameB;
    std::shared_ptr<FilePackageResource> pointer(this);
    filePackage->resources[name] = pointer;
    return pointer;
}

FilePackageResource::~FilePackageResource() {
    if(filePackage)
        filePackage->resources.erase(name);
}



FilePackage::FilePackage(std::string nameB) :name(nameB) {
    
}

FilePackage::~FilePackage() {
    
}

bool FilePackage::init() {
    path = resourcesDir+"Packages/"+name+'/';
    DIR* dir = opendir(path.c_str());
    if(dir == NULL) {
        path = gameDataDir+"Packages/"+name+'/';
        DIR* dirB = opendir(path.c_str());
        if(dirB == NULL) return false;
        closedir(dirB);
    }else closedir(dir);
    
    if(name == "Default") localization.strings.clear();
    localization.loadLocalization(path+"Languages/"+localization.selected+".xml");
    return true;
}

std::string FilePackage::getPathOfFile(const char* groupName, const std::string& fileName) {
    return path+groupName+'/'+fileName;
}



FileManager::~FileManager() {
    clear();
}

void FileManager::clear() {
    for(auto iterator : filePackages)
        delete iterator.second;
    filePackages.clear();
}

FilePackage* FileManager::getPackage(const char* name) {
    std::map<std::string, FilePackage*>::iterator iterator = filePackages.find(name);
    if(iterator == filePackages.end()) {
        FilePackage* package = new FilePackage(name);
        if(!package->init()) {
            delete package;
            return NULL;
        }
        filePackages[name] = package;
        return package;
    }
    return iterator->second;
}

void FileManager::unloadPackage(const char* nameStr) {
    std::string name;
    if(nameStr) name = nameStr;
    std::map<std::string, FilePackage*>::iterator iterator = filePackages.find(name);
    if(iterator == filePackages.end()) return;
    delete iterator->second;
    filePackages.erase(iterator);
}



static bool readOptionBool(rapidxml::xml_node<xmlUsedCharType>* option) {
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = option->first_attribute("value");
    return strcmp(attribute->value(), "true") == 0;
}

template <class T>
static T readOptionValue(rapidxml::xml_node<xmlUsedCharType>* option, const char* format) {
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = option->first_attribute("value");
    T value;
    sscanf(attribute->value(), format, &value);
    return value;
}

void OptionsState::loadOptions() {
    createDir(gameDataDir+"Saves/");
    createDir(gameDataDir+"Packages/");
    localization.selected = "English";
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> fileData = readXmlFile(doc, gameDataDir+"Options.xml", false);
    if(fileData) {
        rapidxml::xml_node<xmlUsedCharType>* options = doc.first_node("Options");
        if(strcmp(options->first_node("Version")->first_attribute("value")->value(), VERSION) != 0) {
            saveOptions();
            return;
        }
        localization.selected = options->first_node("Language")->first_attribute("value")->value();
        rapidxml::xml_node<xmlUsedCharType>* optionGroup = options->first_node("Graphics");
        optionsState.screenBlurFactor = (readOptionBool(optionGroup->first_node("ScreenBlurEnabled"))) ? 0.0 : -1.0;
        optionsState.edgeSmoothEnabled = readOptionBool(optionGroup->first_node("EdgeSmoothEnabled"));
        optionsState.fullScreenEnabled = readOptionBool(optionGroup->first_node("FullScreenEnabled"));
        optionsState.cubemapsEnabled = readOptionBool(optionGroup->first_node("CubemapsEnabled"));
        optionsState.vSyncEnabled = readOptionBool(optionGroup->first_node("VSyncEnabled"));
        optionsState.depthOfFieldQuality = readOptionValue<unsigned int>(optionGroup->first_node("DepthOfFieldQuality"), "%d");
        optionsState.bumpMappingQuality = readOptionValue<unsigned int>(optionGroup->first_node("BumpMappingQuality"), "%d");
        optionsState.shadowQuality = readOptionValue<unsigned int>(optionGroup->first_node("ShadowQuality"), "%d");
        optionsState.ssaoQuality = readOptionValue<unsigned int>(optionGroup->first_node("SsaoQuality"), "%d");
        optionsState.blendingQuality = readOptionValue<unsigned int>(optionGroup->first_node("BlendingQuality"), "%d");
        optionGroup = options->first_node("Sound");
        optionsState.globalVolume = readOptionValue<float>(optionGroup->first_node("globalVolume"), "%f");
        optionsState.musicVolume = readOptionValue<float>(optionGroup->first_node("musicVolume"), "%f");
        optionGroup = options->first_node("Mouse");
        optionsState.mouseSensitivity = readOptionValue<float>(optionGroup->first_node("mouseSensitivity"), "%f");
        optionsState.mouseSmoothing = readOptionValue<float>(optionGroup->first_node("mouseSmoothing"), "%f");
    }else saveOptions();
    
    fileManager.getPackage("Default");
}

void OptionsState::saveOptions() {
    char str[56];
    rapidxml::xml_document<xmlUsedCharType> doc;
    rapidxml::xml_node<xmlUsedCharType>* options = doc.allocate_node(rapidxml::node_element);
    options->name("Options");
    doc.append_node(options);
    addXMLNode(doc, options, "Version", VERSION);
    addXMLNode(doc, options, "Language", localization.selected.c_str());
    
    rapidxml::xml_node<xmlUsedCharType>* optionGroup = doc.allocate_node(rapidxml::node_element);
    optionGroup->name("Graphics");
    options->append_node(optionGroup);
    addXMLNode(doc, optionGroup, "ScreenBlurEnabled", (optionsState.screenBlurFactor > -1.0) ? "true" : "false");
    addXMLNode(doc, optionGroup, "EdgeSmoothEnabled", (optionsState.edgeSmoothEnabled) ? "true" : "false");
    addXMLNode(doc, optionGroup, "FullScreenEnabled", (optionsState.fullScreenEnabled) ? "true" : "false");
    addXMLNode(doc, optionGroup, "CubemapsEnabled", (optionsState.cubemapsEnabled) ? "true" : "false");
    addXMLNode(doc, optionGroup, "VSyncEnabled", (optionsState.vSyncEnabled) ? "true" : "false");
    sprintf(&str[0], "%d", optionsState.depthOfFieldQuality);
    addXMLNode(doc, optionGroup, "DepthOfFieldQuality", &str[0]);
    sprintf(&str[4], "%d", optionsState.bumpMappingQuality);
    addXMLNode(doc, optionGroup, "BumpMappingQuality", &str[4]);
    sprintf(&str[8], "%d", optionsState.shadowQuality);
    addXMLNode(doc, optionGroup, "ShadowQuality", &str[8]);
    sprintf(&str[12], "%d", optionsState.ssaoQuality);
    addXMLNode(doc, optionGroup, "SsaoQuality", &str[12]);
    sprintf(&str[16], "%d", optionsState.blendingQuality);
    addXMLNode(doc, optionGroup, "BlendingQuality", &str[16]);
    
    optionGroup = doc.allocate_node(rapidxml::node_element);
    optionGroup->name("Sound");
    options->append_node(optionGroup);
    sprintf(&str[24], "%1.5f", optionsState.globalVolume);
    addXMLNode(doc, optionGroup, "globalVolume", &str[24]);
    sprintf(&str[32], "%1.5f", optionsState.musicVolume);
    addXMLNode(doc, optionGroup, "musicVolume", &str[32]);
    
    optionGroup = doc.allocate_node(rapidxml::node_element);
    optionGroup->name("Mouse");
    options->append_node(optionGroup);
    sprintf(&str[40], "%1.5f", optionsState.mouseSensitivity);
    addXMLNode(doc, optionGroup, "mouseSensitivity", &str[40]);
    sprintf(&str[48], "%1.5f", optionsState.mouseSmoothing);
    addXMLNode(doc, optionGroup, "mouseSmoothing", &str[48]);
    
    writeXmlFile(doc, gameDataDir+"Options.xml", true);
}

FileManager fileManager;
OptionsState optionsState;