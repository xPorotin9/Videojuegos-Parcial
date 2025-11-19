#ifndef LEVELS_H
#define LEVELS_H

#include "game_entities.h"
#include <vector>

struct Level {
    string name;
    Point3f startPosition;
    Point3f cameraStart;
    vector<Platform> platforms;
    int goalPlatformIndex;
    Scalar backgroundColor1;
    Scalar backgroundColor2;
    std::string backgroundImagePath; // Ruta de la imagen de fondo
};

class LevelManager {
private:
    vector<Level> levels;
    int currentLevel;
    
public:
    LevelManager() : currentLevel(0) {
        initializeLevels();
    }
    
    void initializeLevels() {
        // NIVEL 1: Tutorial
        Level level1;
        level1.name = "Tutorial - Saltos Basicos";
        level1.startPosition = Point3f(-2.0f, -0.4f, 0);
        level1.cameraStart = Point3f(0, -5, 5);
        level1.backgroundColor1 = Scalar(160, 190, 220);
        level1.backgroundColor2 = Scalar(100, 140, 180);
        
        float cubeSize = 4.0f;
        level1.platforms.push_back({Point3f(0, 0, 0), cubeSize, cubeSize, 0.5f, Scalar(100, 200, 100), 0});
        level1.platforms.push_back({Point3f(2, -0.5f, 5), cubeSize, cubeSize, 0.5f, Scalar(150, 150, 200), 1});
        level1.platforms.push_back({Point3f(-2, -1.0f, 10), cubeSize, cubeSize, 0.5f, Scalar(200, 150, 150), 2});
        level1.platforms.push_back({Point3f(4, -1.5f, 14), cubeSize, cubeSize, 0.5f, Scalar(200, 200, 100), 3});
        level1.platforms.push_back({Point3f(-1, -2.0f, 18), cubeSize, cubeSize, 0.5f, Scalar(180, 120, 200), 4});
        level1.platforms.push_back({Point3f(3, -2.5f, 22), cubeSize, cubeSize, 0.5f, Scalar(100, 180, 180), 5});
        level1.platforms.push_back({Point3f(-2, -3.0f, 26), cubeSize, cubeSize, 0.5f, Scalar(150, 200, 100), 6});
        level1.platforms.push_back({Point3f(2, -3.5f, 30), cubeSize, cubeSize, 0.5f, Scalar(200, 150, 200), 7});
        level1.platforms.push_back({Point3f(5, -4.0f, 34), cubeSize, cubeSize, 0.5f, Scalar(150, 150, 150), 8});
        level1.platforms.push_back({Point3f(0, -4.5f, 38), cubeSize * 1.5f, cubeSize * 1.5f, 0.5f, Scalar(255, 200, 100), 99});
        level1.goalPlatformIndex = 9;
        
        levels.push_back(level1);
        
        // NIVEL 2: Saltos de Precision
        Level level2;
        level2.name = "Precision - Plataformas Pequeñas";
        level2.startPosition = Point3f(0, -0.4f, 0);
        level2.cameraStart = Point3f(0, -5, 5);
        level2.backgroundColor1 = Scalar(180, 160, 200);
        level2.backgroundColor2 = Scalar(120, 100, 140);
        
        float smallSize = 2.5f;
        level2.platforms.push_back({Point3f(0, 0, 0), cubeSize, cubeSize, 0.5f, Scalar(100, 200, 100), 0});
        // Plataformas pequeñas en zigzag
        for (int i = 1; i <= 8; i++) {
            float x = (i % 2 == 0) ? 4.0f : -4.0f;
            float y = -i * 0.7f;
            float z = i * 5.0f;
            level2.platforms.push_back({Point3f(x, y, z), smallSize, smallSize, 0.5f, 
                                       Scalar(100 + i*20, 150, 200 - i*20), i});
        }
        level2.platforms.push_back({Point3f(0, -6.0f, 45), cubeSize * 1.5f, cubeSize * 1.5f, 0.5f, 
                                   Scalar(255, 200, 100), 99});
        level2.goalPlatformIndex = 9;
        
        levels.push_back(level2);
        
        // NIVEL 3: Escalera al Cielo
        Level level3;
        level3.name = "Escalera al Cielo";
        level3.startPosition = Point3f(0, -0.4f, 0);
        level3.cameraStart = Point3f(0, -8, 5);
        level3.backgroundColor1 = Scalar(200, 180, 150);
        level3.backgroundColor2 = Scalar(150, 120, 100);
        
        level3.platforms.push_back({Point3f(0, 0, 0), cubeSize, cubeSize, 0.5f, Scalar(100, 200, 100), 0});
        // Escalera ascendente
        for (int i = 1; i <= 12; i++) {
            float x = sin(i * 0.5f) * 3.0f;
            float y = -i * 0.8f;
            float z = i * 3.5f;
            float size = cubeSize - (i * 0.1f);
            level3.platforms.push_back({Point3f(x, y, z), size, size, 0.5f, 
                                       Scalar(200 - i*10, 100 + i*10, 150), i});
        }
        level3.platforms.push_back({Point3f(0, -10.0f, 45), cubeSize * 2.0f, cubeSize * 2.0f, 0.5f, 
                                   Scalar(255, 200, 100), 99});
        level3.goalPlatformIndex = 13;
        
        levels.push_back(level3);
    }
    
    Level& getCurrentLevel() {
        return levels[currentLevel];
    }
    
    bool nextLevel() {
        if (currentLevel < levels.size() - 1) {
            currentLevel++;
            return true;
        }
        return false;
    }
    
    bool previousLevel() {
        if (currentLevel > 0) {
            currentLevel--;
            return true;
        }
        return false;
    }
    
    void resetToFirstLevel() {
        currentLevel = 0;
    }
    
    int getCurrentLevelIndex() const {
        return currentLevel;
    }
    
    int getTotalLevels() const {
        return levels.size();
    }
};

#endif