#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

#include "BaseRunner.h"
#include "TextureManager.h"
#include "StringUtils.h"

class AssetLoader {
public:
    static void loadAssets(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                loadAsset(line);
            }
        }

        file.close();
    }

private:
    static void loadAsset(const std::string& assetPath) {
        std::vector<std::string> tokens = StringUtils::split(assetPath, '/');
        std::string assetName = StringUtils::split(tokens.back(), '.')[0];

        // Load the asset into the texture manager
        TextureManager::getInstance()->instantiateAsTexture(assetPath, assetName, false);

        std::cout << "Loaded texture: " << assetName << std::endl;
    }
};


int main() {
    AssetLoader::loadAssets("Media/assets.txt");

	BaseRunner runner;
	runner.run();
    
}