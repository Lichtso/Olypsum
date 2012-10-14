//
//  FileManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 07.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "FileManager.h"

std::shared_ptr<FilePackageResource> FilePackageResource::load(FilePackage* filePackageB, const std::string& name) {
    filePackage = filePackageB;
    std::shared_ptr<FilePackageResource> pointer = std::make_shared<FilePackageResource>(this);
    std::pair<decltype(poolIndex), bool> inserted = filePackage->resources.insert(decltype(FilePackage::resources)::value_type(name, pointer));
    poolIndex = inserted.first;
    return pointer;
}

FilePackageResource::~FilePackageResource() {
    if(filePackage)
        filePackage->resources.erase(poolIndex);
}



FilePackage::FilePackage(std::string nameB) :name(nameB) {
    
}

FilePackage::~FilePackage() {
    
}

bool FilePackage::load() {
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

std::string FilePackage::getUrlOfFile(const char* groupName, const std::string& fileName) {
    return path+groupName+'/'+fileName;
}

template <class T> std::shared_ptr<T> FilePackage::getResource(const std::string& fileName) {
    std::shared_ptr<FilePackageResource> pointer;
    auto iterator = resources.find(fileName);
    if(iterator != resources.end()) {
        pointer = iterator->second.lock();
        if(!dynamic_cast<T*>(pointer.get())) {
            log(error_log, std::string("The resource ")+fileName+" in "+name+" is also used by another resource type.");
            return NULL;
        }
        return std::static_pointer_cast<T>(pointer);
    }
    return std::static_pointer_cast<T>((new T())->load(this, fileName));
}



FileManager::~FileManager() {
    clear();
}

void FileManager::clear() {
    for(auto iterator : filePackages)
        delete iterator.second;
    filePackages.clear();
}

static bool readOptionBool(rapidxml::xml_node<xmlUsedCharType>* option) {
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = option->first_attribute("value");
    return strcmp(attribute->value(), "true") == 0;
}

static unsigned int readOptionUInt(rapidxml::xml_node<xmlUsedCharType>* option) {
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = option->first_attribute("value");
    unsigned int value;
    sscanf(attribute->value(), "%d", &value);
    return value;
}

void FileManager::loadOptions() {
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
        rapidxml::xml_node<xmlUsedCharType>* graphics = options->first_node("Graphics");
        screenBlurFactor = (readOptionBool(graphics->first_node("ScreenBlurEnabled"))) ? 0.0 : -1.0;
        edgeSmoothEnabled = readOptionBool(graphics->first_node("EdgeSmoothEnabled"));
        fullScreenEnabled = readOptionBool(graphics->first_node("FullScreenEnabled"));
        cubemapsEnabled = readOptionBool(graphics->first_node("CubemapsEnabled"));
        depthOfFieldQuality = readOptionUInt(graphics->first_node("DepthOfFieldQuality"));
        bumpMappingQuality = readOptionUInt(graphics->first_node("BumpMappingQuality"));
        shadowQuality = readOptionUInt(graphics->first_node("ShadowQuality"));
        ssaoQuality = readOptionUInt(graphics->first_node("SsaoQuality"));
        blendingQuality = readOptionUInt(graphics->first_node("BlendingQuality"));
        particleCalcTarget = readOptionUInt(graphics->first_node("ParticleCalcTarget"));
    }else saveOptions();
    
    loadPackage("Default");
}

void FileManager::saveOptions() {
    rapidxml::xml_document<xmlUsedCharType> doc;
    rapidxml::xml_node<xmlUsedCharType>* options = doc.allocate_node(rapidxml::node_element);
    options->name("Options");
    doc.append_node(options);
    addXMLNode(doc, options, "Version", VERSION);
    addXMLNode(doc, options, "Language", localization.selected.c_str());
    rapidxml::xml_node<xmlUsedCharType>* graphics = doc.allocate_node(rapidxml::node_element);
    graphics->name("Graphics");
    options->append_node(graphics);
    
    char str[24];
    addXMLNode(doc, graphics, "ScreenBlurEnabled", (screenBlurFactor > -1.0) ? "true" : "false");
    addXMLNode(doc, graphics, "EdgeSmoothEnabled", (edgeSmoothEnabled) ? "true" : "false");
    addXMLNode(doc, graphics, "FullScreenEnabled", (fullScreenEnabled) ? "true" : "false");
    addXMLNode(doc, graphics, "CubemapsEnabled", (cubemapsEnabled) ? "true" : "false");
    sprintf(&str[0], "%d", depthOfFieldQuality);
    addXMLNode(doc, graphics, "DepthOfFieldQuality", &str[0]);
    sprintf(&str[4], "%d", bumpMappingQuality);
    addXMLNode(doc, graphics, "BumpMappingQuality", &str[4]);
    sprintf(&str[8], "%d", shadowQuality);
    addXMLNode(doc, graphics, "ShadowQuality", &str[8]);
    sprintf(&str[12], "%d", ssaoQuality);
    addXMLNode(doc, graphics, "SsaoQuality", &str[12]);
    sprintf(&str[16], "%d", blendingQuality);
    addXMLNode(doc, graphics, "BlendingQuality", &str[16]);
    sprintf(&str[20], "%d", particleCalcTarget);
    addXMLNode(doc, graphics, "ParticleCalcTarget", &str[20]);
    
    writeXmlFile(doc, gameDataDir+"Options.xml", true);
}

FilePackage* FileManager::loadPackage(const char* name) {
    FilePackage* package = new FilePackage(name);
    if(!package->load()) {
        delete package;
        return NULL;
    }
    filePackages[name] = package;
    return package;
}

FilePackage* FileManager::getPackage(const char* name) {
    std::map<std::string, FilePackage*>::iterator iterator = filePackages.find(name);
    if(iterator == filePackages.end())
        return loadPackage(name);
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

FileManager fileManager;