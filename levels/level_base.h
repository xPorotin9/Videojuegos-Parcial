#ifndef LEVEL_BASE_H
#define LEVEL_BASE_H

#include "../game_entities.h"
#include "../game_config.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <memory>
#include <string>

struct Level {
    string name;
    Point3f startPosition;
    Point3f cameraStart;
    vector<Platform> platforms;
    int goalPlatformIndex;
    Scalar backgroundColor1;
    Scalar backgroundColor2;
    string backgroundImagePath;
    Mat backgroundImage;          // Imagen original
    Mat backgroundResized;        
    Mat backgroundBlurred;        // Imagen con blur 
    Size cachedSize;             
    unique_ptr<PhysicsStrategy> physicsStrategy;
    
    PhysicsConfig getPhysics() const {
        if (physicsStrategy) {
            return physicsStrategy->getConfig();
        }
        return PhysicsConfig();
    }
    
    void loadBackgroundImage(int windowWidth = 1200, int windowHeight = 800) {
        if (!backgroundImagePath.empty()) {
            vector<string> paths = {
                backgroundImagePath,
                "./" + backgroundImagePath,
                "../" + backgroundImagePath,
                "../../" + backgroundImagePath,
                "../../../" + backgroundImagePath,
                "build/bin/Debug/" + backgroundImagePath,
                "build/bin/" + backgroundImagePath
            };
            
            for (const string& path : paths) {
                backgroundImage = imread(path);
                if (!backgroundImage.empty()) {
                    cout << "Fondo cargado: " << path << endl;
                    
                    // PRE-PROCESAR: Redimensionar una sola vez
                    resize(backgroundImage, backgroundResized, Size(windowWidth, windowHeight));
                    cachedSize = Size(windowWidth, windowHeight);
                    
                    // PRE-PROCESAR: Crear versión con blur
                    GaussianBlur(backgroundResized, backgroundBlurred, Size(21, 21), 0);
                    
                    cout << "Imagen pre-procesada y cacheada" << endl;
                    break;
                }
            }
            
            if (backgroundImage.empty()) {
                cout << "Advertencia: No se pudo cargar: " << backgroundImagePath << endl;
            }
        }
    }
    
    // Obtener imagen ya procesada según el estado
    Mat getProcessedBackground(bool needsBlur = false, float blurAmount = 0) {
        if (backgroundResized.empty()) return Mat();
        
        if (needsBlur && blurAmount > 0.5f) {
            return backgroundBlurred;
        }
        return backgroundResized;
    }
};

// Interface para los niveles
class ILevel {
public:
    virtual ~ILevel() {}
    virtual Level create() = 0;
};

#endif