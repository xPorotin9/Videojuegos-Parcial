#ifndef GAME_ENTITIES_H
#define GAME_ENTITIES_H

#include <opencv2/opencv.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace cv;
using namespace std;

struct Platform {
    Point3f position;
    float width;
    float depth;
    float height;
    Scalar color;
    int id;
};

struct Player {
    Point3f position;
    float radius;
    Point3f velocity;
    Point3f targetVelocity;    // Velocidad objetivo para suavizado
    bool isJumping;
    bool isOnGround;
    float jumpChargeTime;       // Tiempo de preparación del salto
    
    void reset(Point3f startPos) {
        position = startPos;
        velocity = Point3f(0, 0, 0);
        targetVelocity = Point3f(0, 0, 0);
        jumpChargeTime = 0;
        isJumping = false;
        isOnGround = true;
    }
};

struct Camera {
    Point3f position;
    Point3f target;
    float smoothness;
    Point3f velocity;  // Velocidad de la cámara para suavizado
    
    void follow(const Point3f& targetPos) {
        target = targetPos;
        
        // Suavizado mejorado pero no excesivo
        Point3f diff = target - position;
        velocity = velocity * 0.7f + diff * smoothness * 0.3f;
        position += velocity;
    }
    
    Point3f getOffset() const {
        return position;
    }
    
    void reset(Point3f pos) {
        position = pos;
        target = pos;
        velocity = Point3f(0, 0, 0);
    }
};

#endif