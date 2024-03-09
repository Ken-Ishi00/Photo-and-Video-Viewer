#include <fstream>
#include <iostream>
#include <filesystem>
#include "TextureManager.h"
#include "StringUtils.h"

TextureManager* TextureManager::sharedInstance = NULL;

TextureManager* TextureManager::getInstance() {
    if (sharedInstance == NULL) {
        //initialize
        sharedInstance = new TextureManager();
    }

    return sharedInstance;
}

TextureManager::TextureManager() {
    this->countStreamingAssets();
}

void TextureManager::loadFromAssetList() {
    std::cout << "[TextureManager] Reading from asset list" << std::endl;
    std::ifstream stream("Media/assets.txt");
    String path;

    while (std::getline(stream, path)) {
        std::vector<String> tokens = StringUtils::split(path, '/');
        String assetName = StringUtils::split(tokens[tokens.size() - 1], '.')[0];
        this->instantiateAsTexture(path, assetName, false);
        std::cout << "[TextureManager] Loaded texture: " << assetName << std::endl;
    }
}

void TextureManager::loadSingleStreamAsset(int index) {
    int fileNum = 0;

    for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH)) {
        if (index == fileNum) {
            // Load asset here
            String assetName = "LoadedStreamingAsset" + std::to_string(index);
            // Simulate loading of very large file
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            std::cout << "[TextureManager] Loaded streaming texture: " << assetName << std::endl;
            break;
        }

        fileNum++;
    }
}

void TextureManager::streamAssetsFromDirectory(const std::string& directory) {
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        std::string path = entry.path().string();
        // Load the asset into the streamTextureList
        instantiateAsTexture(path, entry.path().filename().string(), true);
        std::cout << "Streaming asset loaded: " << entry.path().filename().string() << std::endl;
    }
}

sf::Texture* TextureManager::getFromTextureMap(const String assetName, int frameIndex) {
    if (!this->textureMap[assetName].empty()) {
        return this->textureMap[assetName][frameIndex];
    }
    else {
        std::cout << "[TextureManager] No texture found for " << assetName << std::endl;
        return NULL;
    }
}

int TextureManager::getNumFrames(const String assetName) {
    if (!this->textureMap[assetName].empty()) {
        return this->textureMap[assetName].size();
    }
    else {
        std::cout << "[TextureManager] No texture found for " << assetName << std::endl;
        return 0;
    }
}

sf::Texture* TextureManager::getStreamTextureFromList(const int index) {
    return this->streamTextureList[index];
}

int TextureManager::getNumLoadedStreamTextures() const {
    return this->streamTextureList.size();
}

void TextureManager::countStreamingAssets() {
    this->streamingAssetCount = 0;
    for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH)) {
        this->streamingAssetCount++;
    }
    std::cout << "[TextureManager] Number of streaming assets: " << this->streamingAssetCount << std::endl;
}

void TextureManager::instantiateAsTexture(String path, String assetName, bool isStreaming) {
    sf::Texture* texture = new sf::Texture();
    texture->loadFromFile(path);
    this->textureMap[assetName].push_back(texture);

    if (isStreaming) {
        this->streamTextureList.push_back(texture);
    }
    else {
        this->baseTextureList.push_back(texture);
    }
}

void TextureManager::loadThreadFunction() {
    while (true) {
        int index;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this] { return !loadQueue.empty(); });
            index = loadQueue.front();
            loadQueue.pop_front();
        }
        if (index >= streamingAssetCount) {
            break;
        }
        loadSingleStreamAsset(index);
    }
}
