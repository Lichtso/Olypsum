//
//  FileManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 07.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Menu.h"

FileResourcePtr<FileResource> FileResource::load(FilePackage* filePackageB, const std::string& nameB) {
    filePackage = filePackageB;
    name = nameB;
    filePackage->resources[name] = this;
    return this;
}

void FileResource::remove() {
    if(filePackage)
        filePackage->resources.erase(name);
    delete this;
}



FilePackage::~FilePackage() {
    for(auto iterator : resources)
        delete iterator.second;
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
    
    hash = hashFile(path+"CollisionShapes.xml");
    hash ^= hashDir(path+"Containers/");
    hash ^= hashDir(path+"Scripts/");
    hash ^= hashDir(path+"Languages/");
    hash ^= hashScanDir(path+"Fonts/");
    hash ^= hashScanDir(path+"Models/");
    hash ^= hashScanDir(path+"Textures/");
    hash ^= hashScanDir(path+"Sounds/");
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> fileData = readXmlFile(doc, path+"/Package.xml", false);
    if(!fileData) {
        menu.setModalView("error", localization.localizeString("packageError_FilesMissing"), NULL);
        return false;
    }
    rapidxml::xml_node<xmlUsedCharType> *node, *packageNode = doc.first_node("Package");
    if(strcmp(packageNode->first_node("EngineVersion")->first_attribute("value")->value(), VERSION) != 0) {
        menu.setModalView("error", localization.localizeString("packageError_Version"), NULL);
        return false;
    }
    
    std::size_t hashCmp;
    sscanf(packageNode->first_attribute("hash")->value(), "%lx", &hashCmp);
    if(hashCmp != hash) {
#ifdef DEBUG
        printf("Hash of resoucre package %s should be %lx\n", name.c_str(), hash);
#else
        levelManager.showErrorModal(localization.localizeString("packageError_HashCorrupted"));
        return false;
#endif
    }
    
    node = packageNode->first_node("Description");
    if(node) description = node->value();
    
    FilePackage* package;
    node = packageNode->first_node("Dependencies");
    if(node) {
        packageNode = node->first_node("Package");
        while(packageNode) {
            sscanf(packageNode->first_attribute("hash")->value(), "%lx", &hashCmp);
            if(!(package = fileManager.loadPackage(packageNode->first_attribute("name")->value())) || hashCmp != package->hash) {
                menu.setModalView("error", localization.localizeString("packageError_CouldNotLoad")+'\n'+name, NULL);
                return false;
            }
            packageNode = packageNode->next_sibling("Package");
        }
    }
    
    localization.loadLocalization(this);
    return true;
}

std::string FilePackage::getPathOfFile(const char* subdir, const std::string& fileName) {
    return path+subdir+fileName;
}

std::string FilePackage::findFileByNameInSubdir(const char* subdir, const std::string& fileName) {
    std::vector<std::string> files;
    scanDir(path+subdir, files);
    for(size_t i = 0; i < files.size(); i ++)
        if(files[i].find(fileName) != -1)
            return files[i];
    return "";
}



void FileManager::clear() {
    menu.clear();
    for(auto iterator : filePackages)
        delete iterator.second;
    filePackages.clear();
    localization.strings.clear();
    loadPackage("Core");
}

FilePackage* FileManager::loadPackage(const std::string& name) {
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

void FileManager::unloadPackage(const std::string& name) {
    std::map<std::string, FilePackage*>::iterator iterator = filePackages.find(name);
    if(iterator == filePackages.end()) return;
    delete iterator->second;
    filePackages.erase(iterator);
}

void FileManager::loadAllPackages() {
    std::vector<std::string> files;
    scanDir(resourcesDir+"Packages/", files);
    scanDir(gameDataDir+"Packages/", files);
    for(size_t i = 0; i < files.size(); i ++)
        if(files[i][files[i].length()-1] == '/') {
            files[i].pop_back();
            loadPackage(files[i]);
        }
}

bool FileManager::readResourcePath(FilePackage*& filePackage, std::string& name) {
    if(name.compare(0, 1, "/") == 0) {
        unsigned int seperation = name.find('/', 2);
        std::map<std::string, FilePackage*>::iterator iterator = filePackages.find(name.substr(1, seperation-1));
        if(iterator == filePackages.end()) return false;
        filePackage = iterator->second;
        name = name.substr(seperation+1);
    }
    return true;
}

std::string FileManager::getPathOfResource(FilePackage* filePackage, std::string name) {
    if(filePackage == levelManager.levelPackage)
        return name;
    else
        return std::string("../")+filePackage->name+'/'+name;
}

rapidxml::xml_node<xmlUsedCharType>* FileManager::writeResource(rapidxml::xml_document<xmlUsedCharType>& doc, const char* nodeName,
                                                                FilePackage* filePackage, const std::string& name) {
    rapidxml::xml_node<xmlUsedCharType>* node = doc.allocate_node(rapidxml::node_element);
    node->name(nodeName);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute;
    attribute = doc.allocate_attribute();
    attribute->name("src");
    attribute->value(doc.allocate_string(getPathOfResource(filePackage, name).c_str()));
    node->append_attribute(attribute);
    return node;
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
        if(strcmp(options->first_node("EngineVersion")->first_attribute("value")->value(), VERSION) != 0) {
            saveOptions();
            return;
        }
        localization.selected = options->first_node("Language")->first_attribute("value")->value();
        rapidxml::xml_node<xmlUsedCharType>* optionGroup = options->first_node("Graphics");
        optionsState.screenBlurFactor = (readOptionBool(optionGroup->first_node("ScreenBlurEnabled"))) ? 0.0 : -1.0;
        optionsState.edgeSmoothEnabled = readOptionBool(optionGroup->first_node("EdgeSmoothEnabled"));
        optionsState.cubemapsEnabled = readOptionBool(optionGroup->first_node("CubemapsEnabled"));
        optionsState.vSyncEnabled = readOptionBool(optionGroup->first_node("VSyncEnabled"));
        optionsState.depthOfFieldQuality = readOptionValue<unsigned int>(optionGroup->first_node("DepthOfFieldQuality"), "%d");
        optionsState.surfaceQuality = readOptionValue<unsigned int>(optionGroup->first_node("SurfaceQuality"), "%d");
        optionsState.shadowQuality = readOptionValue<unsigned int>(optionGroup->first_node("ShadowQuality"), "%d");
        optionsState.ssaoQuality = readOptionValue<unsigned int>(optionGroup->first_node("SsaoQuality"), "%d");
        optionsState.blendingQuality = readOptionValue<unsigned int>(optionGroup->first_node("BlendingQuality"), "%d");
        if(optionGroup->first_node("VideoWidth"))
            optionsState.videoWidth = readOptionValue<unsigned int>(optionGroup->first_node("VideoWidth"), "%d");
        if(optionGroup->first_node("VideoHeight"))
            optionsState.videoHeight = readOptionValue<unsigned int>(optionGroup->first_node("VideoHeight"), "%d");
        if(optionGroup->first_node("VideoScale"))
            optionsState.videoScale = readOptionValue<unsigned int>(optionGroup->first_node("VideoScale"), "%d");
        optionGroup = options->first_node("Sound");
        optionsState.globalVolume = readOptionValue<float>(optionGroup->first_node("globalVolume"), "%f");
        optionsState.musicVolume = readOptionValue<float>(optionGroup->first_node("musicVolume"), "%f");
        optionGroup = options->first_node("Mouse");
        optionsState.mouseSensitivity = readOptionValue<float>(optionGroup->first_node("mouseSensitivity"), "%f");
        optionsState.mouseSmoothing = readOptionValue<float>(optionGroup->first_node("mouseSmoothing"), "%f");
    }else saveOptions();
    
    prevOptionsState = optionsState;
    fileManager.clear();
}

void OptionsState::saveOptions() {
    char str[76];
    rapidxml::xml_document<xmlUsedCharType> doc;
    rapidxml::xml_node<xmlUsedCharType>* options = doc.allocate_node(rapidxml::node_element);
    options->name("Options");
    doc.append_node(options);
    addXMLNode(doc, options, "EngineVersion", VERSION);
    addXMLNode(doc, options, "Language", localization.selected.c_str());
    
    rapidxml::xml_node<xmlUsedCharType>* optionGroup = doc.allocate_node(rapidxml::node_element);
    optionGroup->name("Graphics");
    options->append_node(optionGroup);
    addXMLNode(doc, optionGroup, "ScreenBlurEnabled", (optionsState.screenBlurFactor > -1.0) ? "true" : "false");
    addXMLNode(doc, optionGroup, "EdgeSmoothEnabled", (optionsState.edgeSmoothEnabled) ? "true" : "false");
    addXMLNode(doc, optionGroup, "CubemapsEnabled", (optionsState.cubemapsEnabled) ? "true" : "false");
    addXMLNode(doc, optionGroup, "VSyncEnabled", (optionsState.vSyncEnabled) ? "true" : "false");
    
    if(optionsState.videoWidth/optionsState.videoScale != screenSize[0] ||
       optionsState.videoHeight/optionsState.videoScale != screenSize[1]) {
        sprintf(&str[0], "%d", optionsState.videoWidth/optionsState.videoScale);
        addXMLNode(doc, optionGroup, "VideoWidth", &str[0]);
        sprintf(&str[8], "%d", optionsState.videoHeight/optionsState.videoScale);
        addXMLNode(doc, optionGroup, "VideoHeight", &str[8]);
    }
    if(optionsState.videoScale > 1) {
        sprintf(&str[16], "%d", optionsState.videoScale);
        addXMLNode(doc, optionGroup, "VideoScale", &str[16]);
    }
    
    sprintf(&str[20], "%d", optionsState.depthOfFieldQuality);
    addXMLNode(doc, optionGroup, "DepthOfFieldQuality", &str[20]);
    sprintf(&str[24], "%d", optionsState.surfaceQuality);
    addXMLNode(doc, optionGroup, "SurfaceQuality", &str[24]);
    sprintf(&str[28], "%d", optionsState.shadowQuality);
    addXMLNode(doc, optionGroup, "ShadowQuality", &str[28]);
    sprintf(&str[32], "%d", optionsState.ssaoQuality);
    addXMLNode(doc, optionGroup, "SsaoQuality", &str[32]);
    sprintf(&str[36], "%d", optionsState.blendingQuality);
    addXMLNode(doc, optionGroup, "BlendingQuality", &str[36]);
    
    optionGroup = doc.allocate_node(rapidxml::node_element);
    optionGroup->name("Sound");
    options->append_node(optionGroup);
    sprintf(&str[44], "%1.5f", optionsState.globalVolume);
    addXMLNode(doc, optionGroup, "globalVolume", &str[44]);
    sprintf(&str[52], "%1.5f", optionsState.musicVolume);
    addXMLNode(doc, optionGroup, "musicVolume", &str[52]);
    
    optionGroup = doc.allocate_node(rapidxml::node_element);
    optionGroup->name("Mouse");
    options->append_node(optionGroup);
    sprintf(&str[60], "%1.5f", optionsState.mouseSensitivity);
    addXMLNode(doc, optionGroup, "mouseSensitivity", &str[60]);
    sprintf(&str[68], "%1.5f", optionsState.mouseSmoothing);
    addXMLNode(doc, optionGroup, "mouseSmoothing", &str[68]);
    
    writeXmlFile(doc, gameDataDir+"Options.xml", true);
}

FileManager fileManager;
OptionsState optionsState, prevOptionsState;