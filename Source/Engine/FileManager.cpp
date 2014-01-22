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
    path = resourcesPath+"Packages/"+name+'/';
    if(!checkDir(path)) {
        path = supportPath+"Packages/"+name+'/';
        if(!checkDir(path))
            return false;
    }
    
    auto hashFileContent = [this](const std::string& path, std::string name) {
        hash ^= hashFile(path+name);
    };
    
    auto hashName = [this](const std::string& path, std::string name) {
        hash ^= std::hash<std::string>()(name);
    };
    
    auto hashDirectoryName = [this](const std::string& path, std::string name) {
        hash ^= std::hash<std::string>()(name);
        return true;
    };
    
    hash = hashFile(path+"CollisionShapes.xml");
    hash ^= forEachInDir(path+"Containers/", hashFileContent, hashDirectoryName, NULL);
    hash ^= forEachInDir(path+"Scripts/", hashFileContent, hashDirectoryName, NULL);
    hash ^= forEachInDir(path+"Languages/", hashFileContent, NULL, NULL);
    hash ^= forEachInDir(path+"Fonts/", hashName, hashDirectoryName, NULL);
    hash ^= forEachInDir(path+"Models/", hashName, hashDirectoryName, NULL);
    hash ^= forEachInDir(path+"Textures/", hashName, hashDirectoryName, NULL);
    hash ^= forEachInDir(path+"Sounds/", hashName, hashDirectoryName, NULL);
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> fileData = readXmlFile(doc, path+"/Package.xml", false);
    if(!fileData) {
        menu.setModalView("error", fileManager.localizeString("packageError_FilesMissing"), NULL);
        return false;
    }
    rapidxml::xml_node<xmlUsedCharType> *node, *packageNode = doc.first_node("Package");
    if(compareVersions(packageNode->first_node("EngineVersion")->first_attribute()->value(), VERSION) == -1) {
        menu.setModalView("error", fileManager.localizeString("packageError_Version"), NULL);
        return false;
    }
    
    std::size_t hashCmp;
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = packageNode->first_attribute("hash");
    if(attribute) {
        sscanf(attribute->value(), "%lx", &hashCmp);
        if(hashCmp != hash) {
            #ifdef DEBUG
            printf("Hash of resoucre package %s should be %lx\n", name.c_str(), hash);
            #else
            menu.setModalView("error", fileManager.localizeString("packageError_HashCorrupted"), NULL);
            return false;
            #endif
        }
    }
    
    node = packageNode->first_node("Description");
    if(node) {
        if(node->first_node() && node->first_node()->type() == rapidxml::node_cdata)
            node = node->first_node();
        description = node->value();
    }
    
    node = packageNode->first_node("InitialContainer");
    if(node) initialContainer = node->first_attribute()->value();
    
    FilePackage* package;
    node = packageNode->first_node("Dependencies");
    if(node) {
        packageNode = node->first_node("Package");
        while(packageNode) {
            attribute = packageNode->first_attribute("hash");
            if(!attribute) continue;
            sscanf(attribute->value(), "%lx", &hashCmp);
            
            attribute = packageNode->first_attribute("name");
            if(!attribute) continue;
            std::string dependencyName = attribute->value();
            if(dependencyName == name)
                log(warning_log, std::string("Package ")+name+" dependens on its self.");
            
            if(!(package = fileManager.loadPackage(dependencyName)) || hashCmp != package->hash) {
                menu.setModalView("error", fileManager.localizeString("packageError_CouldNotLoad")+'\n'+name, NULL);
                return false;
            }
            
            dependencies.insert(package);
            packageNode = packageNode->next_sibling("Package");
        }
    }
    
    return true;
}

std::string FilePackage::getPathOfFile(const char* subdir, const std::string& fileName) {
    return path+subdir+fileName;
}

bool FilePackage::findFileByNameInSubdir(const char* directoryPath, std::string& fileName) {
    bool found = false;
    
    forEachInDir(path+directoryPath, [&found, &fileName](const std::string& path, std::string name) {
        if(!found && name.find(fileName) != -1) {
            fileName = name;
            found = true;
        }
    }, NULL, NULL);
    
    return found;
}

bool FilePackage::getLocalizableLanguages(std::vector<std::string>& languages) {
    return forEachInDir(path+"Languages/", [&languages](const std::string& directoryPath, std::string name) {
        if(name.length() > 4 && name.compare(name.length()-4, 4, ".xml") == 0)
            languages.push_back(name.substr(0, name.length()-4));
    }, NULL, NULL);
}

bool FilePackage::loadLocalization() {
    std::string filePath = path+"Languages/"+optionsState.language+".xml";
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> fileData = readXmlFile(doc, filePath.c_str(), false);
    if(!fileData) return false;
    
    rapidxml::xml_node<xmlUsedCharType> *localizationNode, *entryNode;
    rapidxml::xml_attribute<xmlUsedCharType> *entryKeyAttribute;
    
    localizationNode = doc.first_node("Localization");
    if(!localizationNode) return false;
    
    localization.clear();
    entryNode = localizationNode->first_node("Entry");
    while(entryNode) {
        entryKeyAttribute = entryNode->first_attribute("key");
        if(!entryKeyAttribute) return false;
        
        localization[std::string(entryKeyAttribute->value())] = std::string(entryNode->value());
        entryNode = entryNode->next_sibling("Entry");
    }
    
    return true;
}



static void addPackagesToKeep(std::unordered_set<FilePackage*>& packagesToKeep, FilePackage* filePackage) {
    packagesToKeep.insert(filePackage);
    for(auto iterator : filePackage->dependencies)
        addPackagesToKeep(packagesToKeep, iterator);
}

void FileManager::clear() {
    menu.clear();
    
    std::unordered_set<FilePackage*> packagesToKeep;
    addPackagesToKeep(packagesToKeep, loadPackage("Core"));
    if(levelManager.levelPackage)
        addPackagesToKeep(packagesToKeep, levelManager.levelPackage);
    
    foreach_e(filePackages, iterator)
        if(packagesToKeep.find(iterator->second) == packagesToKeep.end()) {
            delete iterator->second;
            filePackages.erase(iterator);
        }else if(iterator->second->name != "Core")
            iterator->second->loadLocalization();
}

FilePackage* FileManager::loadPackage(const std::string& name) {
    auto iterator = filePackages.find(name);
    if(iterator == filePackages.end()) {
        FilePackage* package = fileManager.filePackages[name] = new FilePackage(name);
        if(!package->init()) {
            delete package;
            filePackages.erase(name);
            return NULL;
        }else
            return package;
    }
    return iterator->second;
}

void FileManager::unloadPackage(const std::string& name) {
    auto iterator = filePackages.find(name);
    if(iterator == filePackages.end()) return;
    delete iterator->second;
    filePackages.erase(iterator);
}

void FileManager::loadAllPackages() {
    auto enterDirectory = [](const std::string& directoryPath, std::string name) {
        name.pop_back();
        fileManager.loadPackage(name);
        return false;
    };
    forEachInDir(resourcesPath+"Packages/", NULL, enterDirectory, NULL);
    forEachInDir(supportPath+"Packages/", NULL, enterDirectory, NULL);
}

bool FileManager::readResourcePath(FilePackage*& filePackage, std::string& name) {
    if(name.compare(0, 1, "/") == 0) {
        unsigned int seperation = name.find('/', 2);
        auto iterator = filePackages.find(name.substr(1, seperation-1));
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
        return std::string("/")+filePackage->name+'/'+name;
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

const std::string& FileManager::localizeString(const std::string& key) {
    for(auto iterator : filePackages) {
        auto localization = iterator.second->localization.find(key);
        if(localization != iterator.second->localization.end())
            return localization->second;
    }
    log(error_log, std::string("No localization found for key: ")+key.c_str());
    return key;
}



static bool readOptionBool(rapidxml::xml_node<xmlUsedCharType>* option) {
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = option->first_attribute();
    return strcmp(attribute->value(), "true") == 0;
}

template <class T>
static T readOptionValue(rapidxml::xml_node<xmlUsedCharType>* option, const char* format) {
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = option->first_attribute();
    T value;
    sscanf(attribute->value(), format, &value);
    return value;
}

void OptionsState::loadOptions() {
    createDir(supportPath+"Saves/");
    createDir(supportPath+"Packages/");
    language = "English";
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> fileData = readXmlFile(doc, supportPath+"Options.xml", false);
    if(fileData) {
        rapidxml::xml_node<xmlUsedCharType>* options = doc.first_node("Options");
        if(compareVersions(options->first_node("EngineVersion")->first_attribute()->value(), VERSION) >= 0) {
            language = options->first_node("Language")->first_attribute()->value();
            rapidxml::xml_node<xmlUsedCharType>* optionGroup = options->first_node("Multiplayer");
            nickname = optionGroup->first_node("Nickname")->first_attribute()->value();
            ipVersion = optionGroup->first_node("IpVersion")->first_attribute()->value();
            optionGroup = options->first_node("Graphics");
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
                optionsState.videoWidth = max(optionsState.videoWidth, readOptionValue<int>(optionGroup->first_node("VideoWidth"), "%d"));
            if(optionGroup->first_node("VideoHeight"))
                optionsState.videoHeight = max(optionsState.videoHeight, readOptionValue<int>(optionGroup->first_node("VideoHeight"), "%d"));
            if(optionGroup->first_node("VideoScale"))
                optionsState.videoScale = readOptionValue<int>(optionGroup->first_node("VideoScale"), "%d");
            optionGroup = options->first_node("Sound");
            optionsState.globalVolume = readOptionValue<float>(optionGroup->first_node("globalVolume"), "%f");
            optionsState.musicVolume = readOptionValue<float>(optionGroup->first_node("musicVolume"), "%f");
            optionGroup = options->first_node("Mouse");
            optionsState.mouseSensitivity = readOptionValue<float>(optionGroup->first_node("mouseSensitivity"), "%f");
            optionsState.mouseSmoothing = readOptionValue<float>(optionGroup->first_node("mouseSmoothing"), "%f");
        }else
            saveOptions();
    }else
        saveOptions();
    
    fileManager.loadPackage("Core")->loadLocalization();
}

void OptionsState::saveOptions() {
    char str[76];
    rapidxml::xml_document<xmlUsedCharType> doc;
    rapidxml::xml_node<xmlUsedCharType>* options = doc.allocate_node(rapidxml::node_element);
    options->name("Options");
    doc.append_node(options);
    addXMLNode(doc, options, "EngineVersion", VERSION);
    addXMLNode(doc, options, "Language", language.c_str());
    
    rapidxml::xml_node<xmlUsedCharType>* optionGroup = doc.allocate_node(rapidxml::node_element);
    optionGroup->name("Multiplayer");
    options->append_node(optionGroup);
    addXMLNode(doc, optionGroup, "Nickname", nickname.c_str());
    addXMLNode(doc, optionGroup, "IpVersion", ipVersion.c_str());
    
    optionGroup = doc.allocate_node(rapidxml::node_element);
    optionGroup->name("Graphics");
    options->append_node(optionGroup);
    addXMLNode(doc, optionGroup, "ScreenBlurEnabled", (optionsState.screenBlurFactor > -1.0) ? "true" : "false");
    addXMLNode(doc, optionGroup, "EdgeSmoothEnabled", (optionsState.edgeSmoothEnabled) ? "true" : "false");
    addXMLNode(doc, optionGroup, "CubemapsEnabled", (optionsState.cubemapsEnabled) ? "true" : "false");
    addXMLNode(doc, optionGroup, "VSyncEnabled", (optionsState.vSyncEnabled) ? "true" : "false");
    
    sprintf(&str[0], "%d", optionsState.videoWidth);
    addXMLNode(doc, optionGroup, "VideoWidth", &str[0]);
    sprintf(&str[8], "%d", optionsState.videoHeight);
    addXMLNode(doc, optionGroup, "VideoHeight", &str[8]);
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
    
    writeXmlFile(doc, supportPath+"Options.xml", true);
}

FileManager fileManager;
OptionsState optionsState;