#pragma once
#include "AGameObject.h"
#include <deque>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>


class IconObject;
/// <summary>
/// Class that deals with displaying of streamed textures
/// </summary>
class TextureDisplay: public AGameObject
{
public:
	TextureDisplay();
	void initialize();
	void processInput(sf::Event event);
	void update(sf::Time deltaTime);
	

private:
	typedef std::vector<IconObject*> IconList;
	IconList iconList;

	enum StreamingType { BATCH_LOAD = 0, SINGLE_STREAM = 1 };
	const float STREAMING_LOAD_DELAY = 200.0f;
	const StreamingType streamingType = BATCH_LOAD;
	float ticks = 0.0f;
	bool startedStreaming = false;

	static const int WINDOW_WIDTH = 800;

	int columnGrid = 0; int rowGrid = 0;
	
	const int MAX_COLUMN = 5;
	const int MAX_ROW = 4;

	int currentTextureIndex = 0;

	std::deque<int> iconLoadQueue; // Queue for icon loading
	std::mutex queueMutex;
	std::condition_variable queueCondition;
	std::vector<std::thread> threadPool;

	void spawnObject(int textureIndex);
	void loadIcons(); // Function to be run by threads in the pool
};

