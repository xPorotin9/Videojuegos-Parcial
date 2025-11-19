#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include "levels/level_base.h"
#include "levels/lvl1.h"
#include "levels/lvl2.h"
#include "levels/lvl3.h"
#include "levels/lvl4.h"
#include <vector>
#include <memory>

// FACTORY PATTERN
class LevelFactory {
public:
    static unique_ptr<ILevel> createLevel(int levelNumber) {
        switch(levelNumber) {
            case 1: return make_unique<Level1>();
            case 2: return make_unique<Level2>();
            case 3: return make_unique<Level3>();
            case 4: return make_unique<Level4>();
            default: return make_unique<Level1>();
        }
    }
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
        // Cargar todos los niveles usando el Factory Pattern
        for (int i = 1; i <= 4; i++) {
            auto levelCreator = LevelFactory::createLevel(i);
            levels.push_back(levelCreator->create());
        }
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
    
    void goToLevel(int index) {
        if (index >= 0 && index < levels.size()) {
            currentLevel = index;
        }
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
    
    string getCurrentLevelName() const {
        return levels[currentLevel].name;
    }
};

#endif