#include "TextureDisplay.h"
#include <iostream>
#include "TextureManager.h"
#include "BaseRunner.h"
#include "GameObjectManager.h"
#include "IconObject.h"
#include "FPSCounter.h"
#include "BGObject.h"

TextureDisplay::TextureDisplay(): AGameObject("TextureDisplay")
{
	TextureManager::getInstance()->streamAssetsFromDirectory("Media/Streaming/");

    //new
    for (int i = 0; i < std::thread::hardware_concurrency(); ++i) {
        threadPool.emplace_back(&TextureDisplay::loadIcons, this);
    }
}

void TextureDisplay::initialize()
{
	
}

void TextureDisplay::processInput(sf::Event event)
{

}

// Original
//void TextureDisplay::update(sf::Time deltaTime)
//{
//	this->ticks += BaseRunner::TIME_PER_FRAME.asMilliseconds();
//	
//	//<code here for spawning icon object periodically>
//	if (this->ticks >= STREAMING_LOAD_DELAY && !startedStreaming) {
//		startedStreaming = true;
//		spawnObject();
//	}
//}
//
//void TextureDisplay::spawnObject()
//{
//	String objectName = "Icon_" + to_string(this->iconList.size());
//	IconObject* iconObj = new IconObject(objectName, this->iconList.size());
//	this->iconList.push_back(iconObj);
//
//	//set position
//	int IMG_WIDTH = 68; int IMG_HEIGHT = 68;
//	float x = this->columnGrid * IMG_WIDTH;
//	float y = this->rowGrid * IMG_HEIGHT;
//	iconObj->setPosition(x, y);
//
//	std::cout << "Set position: " << x << " " << y << std::endl;
//
//	this->columnGrid++;
//	if(this->columnGrid == this->MAX_COLUMN)
//	{
//		this->columnGrid = 0;
//		this->rowGrid++;
//	}
//	GameObjectManager::getInstance()->addObject(iconObj);
//}

//Normal Barrage of Threads
//void TextureDisplay::update(sf::Time deltaTime) {
//    
//    this->ticks += BaseRunner::TIME_PER_FRAME.asMilliseconds();
//
//    // Check if enough time has passed to spawn a new object
//    if (ticks >= STREAMING_LOAD_DELAY && iconList.size() < 50) {
//        int numLoadedTextures = TextureManager::getInstance()->getNumLoadedStreamTextures();
//        if (currentTextureIndex < numLoadedTextures) { // Check if there are more textures to load
//            spawnObject(currentTextureIndex); // Spawn the next object
//            currentTextureIndex++; // Move to the next texture index
//            this->ticks = 0; // Reset the timer
//        }
//        else {
//            startedStreaming = true; // Set to true when all textures are loaded
//        }
//    }
//}
//
//void TextureDisplay::spawnObject(int textureIndex) {
//    String objectName = "Icon_" + std::to_string(this->iconList.size());
//    IconObject* iconObj = new IconObject(objectName, textureIndex);
//    this->iconList.push_back(iconObj);
//
//    // Set position
//    int IMG_WIDTH = 68;
//    int IMG_HEIGHT = 68;
//    float x = this->columnGrid * IMG_WIDTH;
//    float y = this->rowGrid * IMG_HEIGHT;
//    iconObj->setPosition(x, y);
//
//    std::cout << "Set position: " << x << " " << y << std::endl;
//
//    this->columnGrid++;
//    if (this->columnGrid == this->MAX_COLUMN) {
//        this->columnGrid = 0;
//        this->rowGrid++;
//    }
//    GameObjectManager::getInstance()->addObject(iconObj);
//}

void TextureDisplay::update(sf::Time deltaTime)
{
    this->ticks += BaseRunner::TIME_PER_FRAME.asMilliseconds();

    // Check if enough time has passed to spawn a new object
    if (ticks >= STREAMING_LOAD_DELAY && iconList.size() < 50) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            iconLoadQueue.push_back(currentTextureIndex++);
            ticks = 0; // Reset the timer
        }
        queueCondition.notify_one();
    }
}

void TextureDisplay::spawnObject(int textureIndex)
{
    String objectName = "Icon_" + std::to_string(this->iconList.size());
    IconObject* iconObj = new IconObject(objectName, textureIndex);
    this->iconList.push_back(iconObj);

    // Set position
    int IMG_WIDTH = 68;
    int IMG_HEIGHT = 68;

    float x = this->columnGrid * (IMG_WIDTH);
    float y = this->rowGrid * (IMG_HEIGHT);
    iconObj->setPosition(x, y);

    std::cout << "Set position: " << x << " " << y << std::endl;

    this->columnGrid++;
    if (this->columnGrid == this->WINDOW_WIDTH/ IMG_WIDTH) {
        this->columnGrid = 0;
        this->rowGrid++;
    }
    GameObjectManager::getInstance()->addObject(iconObj);
}

void TextureDisplay::loadIcons()
{
    while (true) {
        int textureIndex;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this] { return !iconLoadQueue.empty(); });
            textureIndex = iconLoadQueue.front();
            iconLoadQueue.pop_front();
        }
        if (textureIndex >= TextureManager::getInstance()->getNumLoadedStreamTextures()) {
            startedStreaming = true;
            break;
        }
        spawnObject(textureIndex);
    }
}
