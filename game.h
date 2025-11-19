#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include <mmsystem.h>
#include "game_entities.h"
#include "physics.h"
#include "level_manager.h"
#include "rendering.h"
#include "game_config.h"
#include <algorithm>
#include <chrono>

#pragma comment(lib, "Winmm.lib")

class Game {
private:
    Player player;
    Camera camera;
    LevelManager levelManager;
    
    bool isRunning;
    bool hasWon;
    bool isTopDownView;
    float viewTransition;
    bool isTransitioning;
    float targetTransition;
    
    int lastKey;
    int keyHoldCounter;
    static const int keyHoldDuration = 8;
    
    int width, height;
    float offsetX, offsetY;
    float scale;
    float topDownScale;
    int cameraMode;
    
    PhysicsConfig currentPhysics;
    
    // SISTEMA DE CAÍDA Y CONTINUAR
    bool hasFallen;
    bool showContinueDialog;
    bool showQRCode;
    bool processingPayment;
    Point3f lastSafePosition;  // Última posición segura antes de caer
    int continueCost;          
    chrono::steady_clock::time_point qrStartTime;
    bool canContinue;
    int fallCount;              // Contador de caídas
    
public:
    Game(int w, int h) : width(w), height(h) {
        offsetX = width / 2.0f;
        offsetY = height / 2.0f;
        scale = 20.0f;
        topDownScale = 20.0f;
        
        isRunning = true;
        hasWon = false;
        isTopDownView = false;
        viewTransition = 0.0f;
        isTransitioning = false;
        targetTransition = 0.0f;
        lastKey = -1;
        keyHoldCounter = 0;
        cameraMode = 1;
        
        // Inicializar sistema de caída
        hasFallen = false;
        showContinueDialog = false;
        showQRCode = false;
        processingPayment = false;
        continueCost = 50; 
        canContinue = false;
        fallCount = 0;
        
        player.radius = 0.4f;
        player.position = Point3f(-2.0f, -0.4f, 0);
        player.velocity = Point3f(0, 0, 0);
        player.isJumping = false;
        player.isOnGround = true;
        
        lastSafePosition = player.position;
        
        camera.position = Point3f(0, -5, 5);
        camera.target = Point3f(0, -5, 5);
        camera.smoothness = 0.1f;
        
        initializeLevel();
    }

    void initializeLevel() {
    Level& level = levelManager.getCurrentLevel();
    
    // Pre-cargar imagen si no está cargada
    if (level.backgroundResized.empty() && !level.backgroundImagePath.empty()) {
        level.loadBackgroundImage(width, height);
    }
    
    player.radius = 0.4f;
    player.position = level.startPosition;
    player.velocity = Point3f(0, 0, 0);
    player.targetVelocity = Point3f(0, 0, 0);  // NUEVO
    player.jumpChargeTime = 0;                 // NUEVO
    player.isJumping = false;
    player.isOnGround = true;
    
    lastSafePosition = level.startPosition;
    
    camera.position = level.cameraStart;
    camera.target = level.cameraStart;
    camera.velocity = Point3f(0, 0, 0);        // NUEVO
    camera.smoothness = 0.1f;
    
    hasWon = false;
    hasFallen = false;
    showContinueDialog = false;
    showQRCode = false;
    processingPayment = false;
    canContinue = false;
    fallCount = 0;
    continueCost = 99;
    currentPhysics = level.getPhysics();
}

    void goToLevel(int index) {
        levelManager.goToLevel(index);
        initializeLevel();
    }

    void handleInput(int key) {
    if (key == 27) {
        isRunning = false;
        return;
    }
    
    if (key == -1) return;
    
    // Si está mostrando el diálogo de continuar
    if (showContinueDialog && !showQRCode) {
        if (key == 'y' || key == 'Y' || key == 13) {
            showQRCode = true;
            processingPayment = true;
            qrStartTime = chrono::steady_clock::now();
            fallCount++;
            continueCost = 5 + (fallCount * 50);
            mciSendString(TEXT("close yape"), NULL, 0, NULL);
            mciSendString(TEXT("open \"yape.mp3\" type mpegvideo alias yape"), NULL, 0, NULL);
            mciSendString(TEXT("play yape from 0"), NULL, 0, NULL);
        } else if (key == 'n' || key == 'N') {
            initializeLevel();
        }
        return;
    }
    
    if (processingPayment) {
        return;
    }
    
    // Input normal del juego
    if (!hasFallen && !showContinueDialog) {
        if (key == 'p' || key == 'P') {
            if (!isTransitioning) {
                isTransitioning = true;
                targetTransition = (viewTransition < 0.5f) ? 1.0f : 0.0f;
            }
        }
        
        if (key == 'c' || key == 'C') {
            cameraMode = (cameraMode + 1) % 3;
            switch(cameraMode) {
                case 0: camera.smoothness = 0.0f; break;
                case 1: camera.smoothness = 0.1f; break;
                case 2: camera.smoothness = 1.0f; break;
            }
        }
        
        if (key == 'r' || key == 'R') {
            initializeLevel();
            viewTransition = 0.0f;
            isTransitioning = false;
            isTopDownView = false;
            targetTransition = 0.0f;
            keyHoldCounter = 0;
            lastKey = -1;
        }
        
        if (hasWon && (key == 'n' || key == 'N')) {
            if (levelManager.nextLevel()) {
                initializeLevel();
            } else {
                levelManager.resetToFirstLevel();
                initializeLevel();
            }
        }
        
        if (key == 'b' || key == 'B') {
            if (levelManager.previousLevel()) {
                initializeLevel();
            }
        }
        
        if (key == 'w' || key == 'W' || key == 's' || key == 'S' || 
            key == 'a' || key == 'A' || key == 'd' || key == 'D') {
            lastKey = key;
            keyHoldCounter = keyHoldDuration;
        }
        
        // SALTO DIRECTO - VOLVER AL SISTEMA ORIGINAL
        if ((key == 32) && player.isOnGround && !player.isJumping) {
            player.velocity.y = -currentPhysics.jumpForce;
            player.isJumping = true;
            player.isOnGround = false;
            
            mciSendString(TEXT("close rebote"), NULL, 0, NULL);
            mciSendString(TEXT("open \"rebote.mp3\" type mpegvideo alias rebote"), NULL, 0, NULL);
            mciSendString(TEXT("play rebote from 0"), NULL, 0, NULL);
        }
    }
}

    void updateMovement() {
    if (hasFallen || showContinueDialog) return;
    const float ACCELERATION_RATE = 0.45f;     // Aceleración más rápida (antes 0.25f)
    const float DECELERATION_GROUND = 0.7f;   // Fricción fuerte en el suelo
    const float DECELERATION_AIR = 0.9f;      // Menos fricción en el aire
    const float INPUT_SMOOTHING = 0.10f;      // Solo un poco de suavizado (antes 0.3f)
    
    if (keyHoldCounter > 0) {
        float isoFactor = 1.0f - viewTransition;
        float topFactor = viewTransition;
        float speed = currentPhysics.moveSpeed;
        
        // Calcular velocidad objetivo
        Point3f targetVel(0, 0, 0);
        
        switch(lastKey) {
            case 'w':
            case 'W':
                targetVel.x = speed * (0.5f * isoFactor);
                targetVel.z = speed * (0.866f * isoFactor - 1.0f * topFactor);
                break;
            case 's':
            case 'S':
                targetVel.x = speed * (-0.5f * isoFactor);
                targetVel.z = speed * (-0.866f * isoFactor + 1.0f * topFactor);
                break;
            case 'a':
            case 'A':
                targetVel.x = speed * (0.866f * isoFactor - 1.0f * topFactor);
                targetVel.z = speed * (-0.5f * isoFactor);
                break;
            case 'd':
            case 'D':
                targetVel.x = speed * (-0.866f * isoFactor + 1.0f * topFactor);
                targetVel.z = speed * (0.5f * isoFactor);
                break;
        }
        
        // Interpolar hacia la velocidad objetivo con más respuesta
        if (player.isOnGround) {
            // En el suelo: respuesta más directa
            player.velocity.x = player.velocity.x * (1.0f - ACCELERATION_RATE) + targetVel.x * ACCELERATION_RATE;
            player.velocity.z = player.velocity.z * (1.0f - ACCELERATION_RATE) + targetVel.z * ACCELERATION_RATE;
        } else {
            // En el aire: menos control
            float airControl = ACCELERATION_RATE * 0.5f;
            player.velocity.x = player.velocity.x * (1.0f - airControl) + targetVel.x * airControl;
            player.velocity.z = player.velocity.z * (1.0f - airControl) + targetVel.z * airControl;
        }
        
        keyHoldCounter--;
        
    } else {
        // Sin input: aplicar fricción FUERTE para evitar patinaje
        if (player.isOnGround) {
            // Fricción fuerte en el suelo
            player.velocity.x *= DECELERATION_GROUND;
            player.velocity.z *= DECELERATION_GROUND;
        } else {
            // Menos fricción en el aire
            player.velocity.x *= DECELERATION_AIR;
            player.velocity.z *= DECELERATION_AIR;
        }
        
        // Detener completamente si la velocidad es muy baja
        if (abs(player.velocity.x) < 0.02f) player.velocity.x = 0;
        if (abs(player.velocity.z) < 0.02f) player.velocity.z = 0;
    }
    
    // Aplicar un pequeño suavizado final (mucho menos que antes)
    player.velocity.x *= (1.0f - INPUT_SMOOTHING) + INPUT_SMOOTHING * 0.98f;
    player.velocity.z *= (1.0f - INPUT_SMOOTHING) + INPUT_SMOOTHING * 0.98f;
}

    void updatePhysics() {
    if (hasFallen || showContinueDialog) return;
    
    // MANEJO DEL SALTO CON PEQUEÑO RETRASO
    if (player.jumpChargeTime > 0) {
        player.jumpChargeTime -= 0.016f; // Reducir el contador (asumiendo ~60 FPS)
        
        // Pequeña animación de "preparación" (opcional)
        if (player.jumpChargeTime > 0.03f && player.isOnGround) {
            // Comprimir ligeramente al jugador antes de saltar
            // (esto es solo visual, no afecta la física)
        }
        
        // Ejecutar el salto cuando termine el retraso
        if (player.jumpChargeTime <= 0 && player.isOnGround && !player.isJumping) {
            player.velocity.y = -currentPhysics.jumpForce;
            player.isJumping = true;
            player.isOnGround = false;
            player.jumpChargeTime = 0;
            
            mciSendString(TEXT("close rebote"), NULL, 0, NULL);
            mciSendString(TEXT("open \"rebote.mp3\" type mpegvideo alias rebote"), NULL, 0, NULL);
            mciSendString(TEXT("play rebote from 0"), NULL, 0, NULL);
        }
    }
    
    // GRAVEDAD NORMAL (sin cambios excesivos)
    player.velocity.y += currentPhysics.gravity;
    
    // Aplicar velocidad directamente (sin suavizado excesivo)
    player.position += player.velocity;
    
    // COLISIONES
    Level& level = levelManager.getCurrentLevel();
    bool wasOnGround = player.isOnGround;
    player.isOnGround = false;
    
    for (auto& platform : level.platforms) {
        if (checkCollision(player, platform)) {
            player.position.y = platform.position.y - player.radius;
            player.velocity.y = 0;
            player.isJumping = false;
            player.isOnGround = true;
            player.jumpChargeTime = 0; // Resetear cualquier intento de salto
            lastSafePosition = player.position;
            break;
        }
    }
    
    if (!player.isOnGround && wasOnGround && player.velocity.y >= 0) {
        player.isOnGround = isPlayerOnAnyPlatform(player, level.platforms);
        if (player.isOnGround) {
            player.velocity.y = 0;
            player.isJumping = false;
            lastSafePosition = player.position;
        }
    }
}


    void updateViewTransition() {
        if (isTransitioning) {
            float transitionSpeed = 0.12f;
            if (abs(viewTransition - targetTransition) < transitionSpeed) {
                viewTransition = targetTransition;
                isTransitioning = false;
                isTopDownView = (viewTransition > 0.5f);
            } else {
                if (viewTransition < targetTransition) {
                    viewTransition += transitionSpeed;
                } else {
                    viewTransition -= transitionSpeed;
                }
            }
        }
    }
    
    void checkVictory() {
        Level& level = levelManager.getCurrentLevel();
        
        if (level.goalPlatformIndex < 0 || level.goalPlatformIndex >= (int)level.platforms.size()) {
            return;
        }
        
        Platform& goalPlatform = level.platforms[level.goalPlatformIndex];
        
        float distX = abs(player.position.x - goalPlatform.position.x + goalPlatform.width/2);
        float distZ = abs(player.position.z - goalPlatform.position.z - goalPlatform.depth/2);
        float heightDiff = abs(player.position.y - goalPlatform.position.y);
        
        if (distX < goalPlatform.width/2 && distZ < goalPlatform.depth/2 && 
            heightDiff < 1.0f && player.isOnGround) {
            hasWon = true;
        }
    }
    
    void update() {
        // Verificar si está procesando el pago
        if (processingPayment && showQRCode) {
            auto now = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::seconds>(now - qrStartTime).count();
            
            if (elapsed >= 3) { // 3 segundos de "procesamiento"
                processingPayment = false;
                showQRCode = false;
                showContinueDialog = false;
                hasFallen = false;
                canContinue = true;
                
                // Restaurar al jugador en la última posición segura
                player.position = lastSafePosition;
                player.velocity = Point3f(0, 0, 0);
                player.isJumping = false;
                player.isOnGround = true;
                keyHoldCounter = 0;
                lastKey = -1;
            }
        }
        
        if (!hasWon && !hasFallen && !showContinueDialog) {
            updateMovement();
            updatePhysics();
            
            // Verificar caída
            if (player.position.y > 5) {
                hasFallen = true;
                showContinueDialog = true;
                player.velocity = Point3f(0, 0, 0); // Detener el movimiento
            }
            
            checkVictory();
        }
        
        updateViewTransition();
        
        if (!hasFallen) {
            camera.follow(Point3f(player.position.x - 2, player.position.y - 3, player.position.z));
        }
    }
    

    Mat render() {
    Mat img;
    Level& level = levelManager.getCurrentLevel();
    
    bool hasBackground = !level.backgroundResized.empty();
    
    if (hasBackground) {
        // Usar imagen pre-procesada del cache
        bool needsBlur = (isTransitioning || viewTransition > 0.5f);
        Mat bg = level.getProcessedBackground(needsBlur, viewTransition);
        
        if (!bg.empty()) {
            // Clonar solo si necesitamos modificar
            if (hasFallen) {
                img = bg.clone();
                img = img * 0.4; // oscurece 40% si hay caida
            } else {
                img = bg.clone();
            }
            
            // Solo agregar overlay si es necesario
            if (!isTopDownView && !hasFallen) {
                // Overlay ligero y rápido
                for (int y = 0; y < height; y += 4) { // Saltar líneas para optimizar
                    float t = (float)y / height;
                    int alpha = (int)(30 * (1 - t * 0.5));
                    if (alpha > 0) {
                        line(img, Point(0, y), Point(width, y), 
                             Scalar(alpha, alpha, alpha * 1.2), 1);
                    }
                }
            }
        }
    } 
    
    // Si no hay imagen o falló, usar gradiente simple
    if (img.empty()) {
        img = Mat::zeros(height, width, CV_8UC3);
        
        // Gradiente optimizado (menos líneas)
        for (int y = 0; y < height; y += 2) { // Dibujar cada 2 líneas
            float t = (float)y / height;
            Scalar bgColor;
            
            if (isTopDownView) {
                bgColor = Scalar(80 * (1 - t) + 60 * t, 
                                100 * (1 - t) + 80 * t, 
                                120 * (1 - t) + 100 * t);
            } else {
                bgColor = level.backgroundColor1 * (1 - t) + level.backgroundColor2 * t;
            }
            
            if (hasFallen) {
                bgColor = bgColor * 0.5;
            }
            
            // Dibujar línea más gruesa para cubrir el espacio
            line(img, Point(0, y), Point(width, y), bgColor, 2);
        }
    }
    
    // ========== RENDERIZAR PLATAFORMAS ==========
    float currentScale = getScale();
    float currentOffsetY = offsetY * (1.0f - viewTransition * 0.1f);
    
    // Solo ordenar si es necesario
    static vector<Platform> sortedPlatforms;
    static int lastPlatformCount = -1;
    
    if (lastPlatformCount != level.platforms.size() || !isTopDownView) {
        sortedPlatforms = level.platforms;
        if (!isTopDownView) {
            sort(sortedPlatforms.begin(), sortedPlatforms.end(), 
                 [](const Platform& a, const Platform& b) {
                     float depthA = a.position.x + a.position.z - a.position.y * 2;
                     float depthB = b.position.x + b.position.z - b.position.y * 2;
                     return depthA < depthB;
                 });
        }
        lastPlatformCount = level.platforms.size();
    }
    
    for (const auto& platform : sortedPlatforms) {
        drawCube(img, platform, camera, offsetX, currentOffsetY, currentScale, isTopDownView);
    }
    
    // ========== RENDERIZAR JUGADOR ==========
    if (!hasFallen || canContinue) {
        drawPlayer(img, player, level.platforms, camera, offsetX, currentOffsetY, currentScale, isTopDownView);
    }
    
    // ========== UI OPTIMIZADO ==========
    rectangle(img, Point(0, 0), Point(width, 60), Scalar(0, 0, 0), -1);
    
    string title = "Nivel " + to_string(levelManager.getCurrentLevelIndex() + 1) + 
                  "/" + to_string(levelManager.getTotalLevels()) + 
                  ": " + level.name;
    putText(img, title, Point(20, 35), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 255), 2);
    
    if (level.physicsStrategy) {
        string physicsType = "Fisica: ";
        physicsType += level.physicsStrategy->getName();
        putText(img, physicsType, Point(20, 55), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(150, 255, 150), 1);
    }
    
    // Indicador simple
    if (hasBackground) {
        circle(img, Point(width - 30, 30), 5, Scalar(0, 255, 0), -1);
    }
    
    // ========== DIALOGOS (sin cambios pero optimizados) ==========
    if (showContinueDialog && !showQRCode) {
        // Usar rectangle con transparencia en lugar de addWeighted
        for (int y = 0; y < height; y += 4) {
            for (int x = 0; x < width; x += 4) {
                Vec3b& pixel = img.at<Vec3b>(y, x);
                pixel = pixel * 0.3;
            }
        }
        
        int dialogWidth = 500;
        int dialogHeight = 300;
        int dialogX = width/2 - dialogWidth/2;
        int dialogY = height/2 - dialogHeight/2;
        
        rectangle(img, Point(dialogX, dialogY), 
                 Point(dialogX + dialogWidth, dialogY + dialogHeight), 
                 Scalar(20, 20, 20), -1);
        rectangle(img, Point(dialogX, dialogY), 
                 Point(dialogX + dialogWidth, dialogY + dialogHeight), 
                 Scalar(255, 100, 100), 3);
        
        putText(img, "PIPIPI", 
               Point(dialogX + dialogWidth/2 - 100, dialogY + 50), 
               FONT_HERSHEY_SIMPLEX, 1.2, Scalar(255, 100, 100), 2);
        
        putText(img, "Te caiste, ere bajo socio", 
               Point(dialogX + dialogWidth/2 - 140, dialogY + 100), 
               FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 1);
        
        char priceText[50];
        sprintf(priceText, "Continuar por solo S/.%.2f", continueCost / 100.0f);
        putText(img, priceText, 
               Point(dialogX + dialogWidth/2 - 120, dialogY + 140), 
               FONT_HERSHEY_SIMPLEX, 0.8, Scalar(100, 255, 100), 2);
        
        rectangle(img, Point(dialogX + 50, dialogY + 180), 
                 Point(dialogX + 200, dialogY + 230), 
                 Scalar(0, 200, 0), -1);
        putText(img, "CHI (Y)", 
               Point(dialogX + 90, dialogY + 210), 
               FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 255), 2);
        
        rectangle(img, Point(dialogX + 300, dialogY + 180), 
                 Point(dialogX + 450, dialogY + 230), 
                 Scalar(200, 0, 0), -1);
        putText(img, "NO (N)", 
               Point(dialogX + 340, dialogY + 210), 
               FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 255), 2);
        
        putText(img, "Entre mas pierdas, mas caro", 
               Point(dialogX + 40, dialogY + 270), 
               FONT_HERSHEY_SIMPLEX, 0.3, Scalar(150, 150, 150), 1);
    }
    
    // QR Code (simplificado)
    if (showQRCode) {
        // Oscurecimiento rápido
        img = img * 0.3;
        
        int qrSize = 300;
        int qrX = width/2 - qrSize/2;
        int qrY = height/2 - qrSize/2 - 50;
        
        rectangle(img, Point(qrX - 20, qrY - 20), 
                 Point(qrX + qrSize + 20, qrY + qrSize + 20), 
                 Scalar(255, 255, 255), -1);
        
        // QR simplificado
        static Mat qrImage;
        static bool qrLoaded = false;
        
        if (!qrLoaded) {
            vector<string> paths = {"qr.png", "./qr.png", "../qr.png"};
            for (const string& path : paths) {
                Mat temp = imread(path);
                if (!temp.empty()) {
                    resize(temp, qrImage, Size(qrSize, qrSize));
                    qrLoaded = true;
                    break;
                }
            }
        }
        
        if (!qrImage.empty()) {
            qrImage.copyTo(img(Rect(qrX, qrY, qrSize, qrSize)));
        }
        
        putText(img, "PROCESANDO YAPE", 
               Point(width/2 - 130, qrY - 40), 
               FONT_HERSHEY_SIMPLEX, 1.0, Scalar(100, 255, 100), 2);
        
        // Barra de progreso simple
        int barWidth = 400;
        int barX = width/2 - barWidth/2;
        int barY = qrY + qrSize + 40;
        
        auto now = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - qrStartTime).count();
        float progress = min(1.0f, elapsed / 3000.0f);
        
        rectangle(img, Point(barX, barY), 
                 Point(barX + barWidth, barY + 30), 
                 Scalar(50, 50, 50), -1);
        rectangle(img, Point(barX, barY), 
                 Point(barX + (int)(barWidth * progress), barY + 30), 
                 Scalar(0, 255, 0), -1);
    }
    
    // Victoria
    if (hasWon) {
        rectangle(img, Point(width/2 - 200, height/2 - 50), 
                 Point(width/2 + 200, height/2 + 50), 
                 Scalar(0, 0, 0), -1);
        rectangle(img, Point(width/2 - 200, height/2 - 50), 
                 Point(width/2 + 200, height/2 + 50), 
                 Scalar(255, 200, 0), 3);
        
        putText(img, "Finiquitao", Point(width/2 - 120, height/2), 
                FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 200, 0), 2);
        putText(img, "N: Siguiente | R: Reiniciar", Point(width/2 - 100, height/2 + 30), 
                FONT_HERSHEY_SIMPLEX, 0.6, Scalar(200, 200, 200), 1);
    }
    
    // Controles
    if (!isTransitioning && !showContinueDialog && !showQRCode) {
        drawControls(img, width - 200, height - 200, isTopDownView);
    }
    
    return img;
}
float getScale() const {
        float baseScale = isTopDownView ? topDownScale : scale;
        return baseScale * (1.0f - viewTransition * 0.1f);
    }
    
    bool isGameRunning() const { return isRunning; }
};

#endif