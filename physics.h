#ifndef PHYSICS_H
#define PHYSICS_H

#include "game_entities.h"
#include <vector>

inline bool checkCollision(Player& player, Platform& platform) {
    float px = player.position.x;
    float py = player.position.y;
    float pz = player.position.z;
    float pr = player.radius;
    
    float platLeft = platform.position.x - platform.width;
    float platRight = platform.position.x;
    float platFront = platform.position.z;
    float platBack = platform.position.z + platform.depth;
    float platTop = platform.position.y;
    
    bool inX = (px + pr > platLeft) && (px - pr < platRight);
    bool inZ = (pz + pr > platFront) && (pz - pr < platBack);
    
    float playerBottom = py + pr;
    bool verticalCollision = (playerBottom >= platTop - 0.1f) && 
                            (playerBottom <= platTop + 0.5f) && 
                            (player.velocity.y >= 0);
    
    return inX && inZ && verticalCollision;
}

inline bool isPlayerOnAnyPlatform(const Player& player, const vector<Platform>& platforms) {
    float px = player.position.x;
    float py = player.position.y;
    float pz = player.position.z;
    float pr = player.radius;
    
    for (const auto& platform : platforms) {
        float platLeft = platform.position.x - platform.width;
        float platRight = platform.position.x;
        float platFront = platform.position.z;
        float platBack = platform.position.z + platform.depth;
        float platTop = platform.position.y;
        
        bool inX = (px > platLeft - pr * 0.5f) && (px < platRight + pr * 0.5f);
        bool inZ = (pz > platFront - pr * 0.5f) && (pz < platBack + pr * 0.5f);
        
        float playerBottom = py + pr;
        bool onTop = abs(playerBottom - platTop) < 0.15f;
        
        if (inX && inZ && onTop) {
            return true;
        }
    }
    return false;
}

#endif