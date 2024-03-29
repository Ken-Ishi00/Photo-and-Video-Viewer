#pragma once
#include <unordered_map>
#include "SFML/Graphics.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <chrono>

class TextureManager
{
public:
    typedef std::string String;
    typedef std::vector<sf::Texture*> TextureList;
    typedef std::unordered_map<String, TextureList> HashTable;

public:
    static TextureManager* getInstance();
    void loadFromAssetList(); //loading of all assets needed for startup
    void loadSingleStreamAsset(int index); //loads a single streaming asset based on index in directory
    sf::Texture* getFromTextureMap(const String assetName, int frameIndex);
    int getNumFrames(const String assetName);

    sf::Texture* getStreamTextureFromList(const int index);
    int getNumLoadedStreamTextures() const;

    void streamAssetsFromDirectory(const std::string& directory);
    void instantiateAsTexture(String path, String assetName, bool isStreaming);

private:
    TextureManager();
    TextureManager(TextureManager const&) {};             // copy constructor is private
    TextureManager& operator=(TextureManager const&) {};  // assignment operator is private
    static TextureManager* sharedInstance;

    HashTable textureMap;
    TextureList baseTextureList;
    TextureList streamTextureList;

    const std::string STREAMING_PATH = "Media/Streaming/";
    int streamingAssetCount = 0;

    void countStreamingAssets();

    // Multithreading members
    std::vector<std::thread> threadPool;
    std::deque<int> loadQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;

    void loadThreadFunction();
};

