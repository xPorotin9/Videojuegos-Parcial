#ifndef RENDERING_H
#define RENDERING_H

#include "game_entities.h"
#include <vector>

// Proyección isométrica con ángulos de 120°
inline Point2f toIsometric(Point3f point3d, const Camera& camera, float offsetX, float offsetY, float scale) {
    Point3f relative = point3d - camera.getOffset();
    
    float angle1 = 210.0f * M_PI / 180.0f;
    float angle2 = 330.0f * M_PI / 180.0f;
    
    float x = relative.x * cos(angle1) * scale + relative.z * cos(angle2) * scale;
    float y = relative.x * sin(angle1) * scale + relative.z * sin(angle2) * scale + relative.y * scale;
    
    return Point2f(x + offsetX, y + offsetY);
}

// Proyección superior 2D (vista desde arriba)
inline Point2f toTopDown(Point3f point3d, const Camera& camera, float offsetX, float offsetY, float scale) {
    Point3f relative = point3d - camera.getOffset();
    
    // Vista directa desde arriba: X -> X, Z -> Y de la pantalla
    float x = relative.x * scale;
    float y = relative.z * scale;
    
    return Point2f(x + offsetX, y + offsetY);
}

// Función genérica de proyección que decide según el modo
inline Point2f projectPoint(Point3f point3d, const Camera& camera, float offsetX, float offsetY, 
                           float scale, bool isTopDown) {
    if (isTopDown) {
        return toTopDown(point3d, camera, offsetX, offsetY, scale);
    } else {
        return toIsometric(point3d, camera, offsetX, offsetY, scale);
    }
}

// Dibujar un cubo (isométrico o vista superior)
inline void drawCube(Mat& img, const Platform& platform, const Camera& camera, 
                    float offsetX, float offsetY, float scale, bool isTopDown) {
    
    if (isTopDown) {
        // Vista superior 2D simple
        Point3f p = platform.position;
        float w = platform.width;
        float d = platform.depth;
        
        // Proyectar las 4 esquinas de la plataforma
        vector<Point2f> corners2d;
        corners2d.push_back(toTopDown(Point3f(p.x, p.y, p.z), camera, offsetX, offsetY, scale));
        corners2d.push_back(toTopDown(Point3f(p.x - w, p.y, p.z), camera, offsetX, offsetY, scale));
        corners2d.push_back(toTopDown(Point3f(p.x - w, p.y, p.z + d), camera, offsetX, offsetY, scale));
        corners2d.push_back(toTopDown(Point3f(p.x, p.y, p.z + d), camera, offsetX, offsetY, scale));
        
        // Convertir a puntos enteros
        vector<Point> pts;
        for (const auto& pt : corners2d) {
            pts.push_back(Point((int)pt.x, (int)pt.y));
        }
        
        // Color basado en altura (más claro = más alto)
        float heightFactor = 0.5f + abs(platform.position.y) * 0.1f;
        heightFactor = min(1.0f, heightFactor);
        Scalar color(
            platform.color[0] * heightFactor,
            platform.color[1] * heightFactor,
            platform.color[2] * heightFactor
        );
        
        // Dibujar la plataforma
        fillConvexPoly(img, pts, color);
        
        // Borde más grueso para plataformas más altas
        int borderThickness = 2 + (int)(abs(platform.position.y) * 0.5f);
        polylines(img, pts, true, Scalar(0, 0, 0), borderThickness);
        
        // Efecto de elevación con sombra desplazada
        
        // Marcadores especiales
        Point center((pts[0].x + pts[2].x) / 2, (pts[0].y + pts[2].y) / 2);
        
        if (platform.id == 0) {
            circle(img, center, scale/2, Scalar(255, 255, 255), 2);
            putText(img, "START", center - Point(25, -5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
        } else if (platform.id == 99) {
            // Estrella para meta
            vector<Point> star;
            for (int i = 0; i < 10; i++) {
                float angle = i * M_PI / 5;
                float r = (i % 2 == 0) ? scale/2 : scale/4;
                star.push_back(Point(
                    center.x + r * cos(angle - M_PI/2),
                    center.y + r * sin(angle - M_PI/2)
                ));
            }
            fillPoly(img, star, Scalar(255, 255, 0));
            putText(img, "GOAL", center - Point(20, -5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
        } else if (platform.id > 0) {
            // Solo mostrar el número de plataforma, sin altura
            char info[20];
            sprintf(info, "%d", platform.id);
            putText(img, info, center - Point(5, 5), 
                    FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
        }
        
    } else {
        // Vista isométrica (código existente)
        Point3f p = platform.position;
        float w = platform.width;
        float d = platform.depth;
        float h = platform.height * 3.0f;
        
        vector<Point3f> vertices = {
            Point3f(p.x, p.y, p.z),                        // 0
            Point3f(p.x, p.y + h, p.z),                    // 1
            Point3f(p.x - w, p.y + h, p.z),                // 2
            Point3f(p.x - w, p.y, p.z),                    // 3
            Point3f(p.x, p.y, p.z + d),                    // 4
            Point3f(p.x, p.y + h, p.z + d),                // 5
            Point3f(p.x - w, p.y + h, p.z + d),            // 6
            Point3f(p.x - w, p.y, p.z + d)                 // 7
        };
        
        vector<Point2f> points2d;
        for (const auto& v : vertices) {
            points2d.push_back(toIsometric(v, camera, offsetX, offsetY, scale));
        }
        
        vector<Point> pts2d;
        for (const auto& pt : points2d) {
            pts2d.push_back(Point((int)pt.x, (int)pt.y));
        }
        
        float depthFactor = 1.0f - (platform.position.z * 0.01f);
        depthFactor = max(0.7f, min(1.0f, depthFactor));
        
        vector<Point> topFace = {pts2d[0], pts2d[3], pts2d[7], pts2d[4]};
        Scalar topColor(
            platform.color[0] * depthFactor,
            platform.color[1] * depthFactor,
            platform.color[2] * depthFactor
        );
        
        vector<Point> rightFace = {pts2d[0], pts2d[4], pts2d[5], pts2d[1]};
        float rightShade = 0.65f * depthFactor;
        Scalar rightColor(
            platform.color[0] * rightShade,
            platform.color[1] * rightShade,
            platform.color[2] * rightShade
        );
        
        vector<Point> leftFace = {pts2d[2], pts2d[3], pts2d[7], pts2d[6]};
        float leftShade = 0.45f * depthFactor;
        Scalar leftColor(
            platform.color[0] * leftShade,
            platform.color[1] * leftShade,
            platform.color[2] * leftShade
        );
        
        fillConvexPoly(img, leftFace, leftColor);
        fillConvexPoly(img, rightFace, rightColor);
        fillConvexPoly(img, topFace, topColor);
        
        int lineThickness = 2 + (int)(abs(platform.position.y) * 0.3f);
        lineThickness = min(4, lineThickness);
        
        polylines(img, leftFace, true, Scalar(0, 0, 0), lineThickness);
        polylines(img, rightFace, true, Scalar(0, 0, 0), lineThickness);
        polylines(img, topFace, true, Scalar(0, 0, 0), lineThickness - 1);
        
        line(img, pts2d[0], pts2d[1], Scalar(0, 0, 0), lineThickness);
        line(img, pts2d[3], pts2d[2], Scalar(0, 0, 0), lineThickness);
        line(img, pts2d[4], pts2d[5], Scalar(0, 0, 0), lineThickness);
        line(img, pts2d[7], pts2d[6], Scalar(0, 0, 0), lineThickness);
        
        vector<Point> topEdgeHighlight = {pts2d[0], pts2d[3]};
        vector<Point> frontEdgeHighlight = {pts2d[0], pts2d[4]};
        
        Scalar highlightColor(
            min(255, (int)(topColor[0] * 1.4)),
            min(255, (int)(topColor[1] * 1.4)),
            min(255, (int)(topColor[2] * 1.4))
        );
        
        polylines(img, topEdgeHighlight, false, highlightColor, 2);
        polylines(img, frontEdgeHighlight, false, highlightColor, 2);
        
        // Marcadores especiales
        Point center((pts2d[0].x + pts2d[7].x) / 2, (pts2d[0].y + pts2d[7].y) / 2);
        
        if (platform.id == 0) {
            circle(img, center, scale/2, Scalar(255, 255, 255), 2);
            putText(img, "START", center - Point(25, -5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
        } else if (platform.id == 99) {
            vector<Point> star;
            for (int i = 0; i < 10; i++) {
                float angle = i * M_PI / 5;
                float r = (i % 2 == 0) ? scale/2 : scale/4;
                star.push_back(Point(
                    center.x + r * cos(angle - M_PI/2),
                    center.y + r * sin(angle - M_PI/2)
                ));
            }
            fillPoly(img, star, Scalar(255, 255, 0));
            polylines(img, star, true, Scalar(255, 200, 0), 2);
            putText(img, "GOAL", center - Point(20, -5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
        } else if (platform.id > 0 && platform.id < 99) {
            circle(img, center, 12, Scalar(0, 0, 0, 150), -1);
            char numStr[10];
            sprintf(numStr, "%d", platform.id);
            putText(img, numStr, center - Point(5, -5), 
                    FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 2);
        }
    }
}

// Dibujar el jugador (esfera o círculo según vista)
inline void drawPlayer(Mat& img, const Player& player, const vector<Platform>& platforms, 
                      const Camera& camera, float offsetX, float offsetY, float scale, bool isTopDown) {
    
    if (isTopDown) {
        // Vista superior - círculo simple con indicador de altura
        Point2f center2d = toTopDown(player.position, camera, offsetX, offsetY, scale);
        int radius2d = (int)(player.radius * scale);
        
        // Sombra para indicar altura
        if (!player.isOnGround) {
            int shadowOffset = (int)(abs(player.position.y) * 3);
            Point shadowCenter(center2d.x + shadowOffset, center2d.y + shadowOffset);
            circle(img, shadowCenter, radius2d, Scalar(50, 50, 50), -1);
        }
        
        // Círculo del jugador
        circle(img, center2d, radius2d + 2, Scalar(0, 0, 0), -1);
        circle(img, center2d, radius2d, Scalar(255, 100, 100), -1);
        circle(img, center2d, radius2d, Scalar(200, 50, 50), 2);
        
        // Indicador de altura
        if (!player.isOnGround) {
            char heightText[20];
            sprintf(heightText, "Alt: %.1f", abs(player.position.y));
            putText(img, heightText, center2d + Point2f(radius2d + 5, -5), 
                    FONT_HERSHEY_SIMPLEX, 0.3, Scalar(255, 255, 255), 1);
        }
        
        // Dirección del movimiento
        if (abs(player.velocity.x) > 0.01f || abs(player.velocity.z) > 0.01f) {
            Point3f futurePos = player.position + Point3f(player.velocity.x * 5, 0, player.velocity.z * 5);
            Point2f futurePos2d = toTopDown(futurePos, camera, offsetX, offsetY, scale);
            arrowedLine(img, center2d, futurePos2d, Scalar(0, 255, 0), 2);
        }
        
    } else {
        // Vista isométrica
        Point2f center2d = toIsometric(player.position, camera, offsetX, offsetY, scale);
        int radius2d = (int)(player.radius * scale);
        
        // Sombra sobre plataforma
        float shadowY = 10.0f;
        for (const auto& platform : platforms) {
            float platTop = platform.position.y;
            if (platTop >= player.position.y - 0.1f) {
                float distX = abs(player.position.x - platform.position.x + platform.width/2);
                float distZ = abs(player.position.z - platform.position.z - platform.depth/2);
                if (distX <= platform.width/2 + player.radius && 
                    distZ <= platform.depth/2 + player.radius) {
                    if (platTop < shadowY || shadowY == 10.0f) {
                        shadowY = platTop;
                    }
                }
            }
        }
        
        if (shadowY < 10.0f && abs(player.position.y - shadowY) < 2.0f) {
            Point2f shadowPos = toIsometric(Point3f(player.position.x, shadowY + 0.01f, player.position.z), 
                                           camera, offsetX, offsetY, scale);
            float distance = abs(player.position.y - shadowY);
            float shadowScale = max(0.4f, 1.0f - distance * 0.2f);
            int shadowRadius = (int)(radius2d * shadowScale * 0.8f);
            ellipse(img, shadowPos, Size(shadowRadius, shadowRadius/2), 0, 0, 360, 
                    Scalar(30, 30, 30), -1);
        }
        
        // Esfera con gradiente
        circle(img, center2d, radius2d + 2, Scalar(0, 0, 0), -1);
        
        for (int r = radius2d; r > 0; r--) {
            float t = (float)r / radius2d;
            float lightFactor = 0.3 + 0.7 * pow(t, 0.8f);
            Scalar color(
                255 * lightFactor,
                80 * lightFactor,
                80 * lightFactor
            );
            circle(img, center2d, r, color, -1);
        }
        
        circle(img, center2d, radius2d, Scalar(180, 50, 50), 2);
        
        // Brillo
        Point2f highlight = center2d + Point2f(-radius2d * 0.3f, -radius2d * 0.3f);
        int highlightRadius = radius2d / 2;
        for (int r = highlightRadius; r > 0; r -= 2) {
            float t = (float)r / highlightRadius;
            Scalar highlightColor(
                255,
                255 - (int)(35 * t),
                255 - (int)(35 * t)
            );
            circle(img, highlight, r, highlightColor, -1);
        }
        
        // Indicador de dirección
        if (abs(player.velocity.x) > 0.01f || abs(player.velocity.z) > 0.01f) {
            Point3f futurePos = player.position + Point3f(player.velocity.x * 5, 0, player.velocity.z * 5);
            Point2f futurePos2d = toIsometric(futurePos, camera, offsetX, offsetY, scale);
            arrowedLine(img, center2d, futurePos2d, Scalar(0, 255, 0), 2);
        }
        
        // Indicador de caída con tu mejora
        if (!player.isOnGround && player.velocity.y > 0.1f) {
            for (int i = 0; i < 3; i++) {
                Point2f highlight = center2d + Point2f(-radius2d * 0.3f, -radius2d * 0.3f);
                Point start = Point(highlight.x, highlight.y + i*4);
                Point end = start + Point(0, -8);
                line(img, start, end, Scalar(200, 200, 200, 100 - i*30), 2 - i/2);
            }
        }
    }
}

// Dibujar controles visuales actualizados
inline void drawControls(Mat& img, int x, int y, bool isTopDown) {
    rectangle(img, Point(x, y), Point(x + 180, y + 180), Scalar(30, 30, 30), -1);
    rectangle(img, Point(x, y), Point(x + 180, y + 180), Scalar(100, 100, 100), 2);
    
    putText(img, "CONTROLES", Point(x + 50, y - 5), 
            FONT_HERSHEY_SIMPLEX, 0.5, Scalar(200, 200, 200), 1);
    
    int centerX = x + 90;
    int centerY = y + 75;
    int keySize = 30;
    
    // W
    rectangle(img, Point(centerX - keySize/2, centerY - keySize - 5), 
              Point(centerX + keySize/2, centerY - 5), Scalar(80, 80, 80), -1);
    putText(img, "W", Point(centerX - 8, centerY - 15), 
            FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    
    // S
    rectangle(img, Point(centerX - keySize/2, centerY + 5), 
              Point(centerX + keySize/2, centerY + keySize + 5), Scalar(80, 80, 80), -1);
    putText(img, "S", Point(centerX - 6, centerY + 25), 
            FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    
    // A
    rectangle(img, Point(centerX - keySize - 5, centerY - keySize/2), 
              Point(centerX - 5, centerY + keySize/2), Scalar(80, 80, 80), -1);
    putText(img, "A", Point(centerX - 25, centerY + 5), 
            FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    
    // D
    rectangle(img, Point(centerX + 5, centerY - keySize/2), 
              Point(centerX + keySize + 5, centerY + keySize/2), Scalar(80, 80, 80), -1);
    putText(img, "D", Point(centerX + 15, centerY + 5), 
            FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    
    putText(img, "SPACE: Saltar", Point(x + 35, y + 130), 
            FONT_HERSHEY_SIMPLEX, 0.4, Scalar(200, 200, 200), 1);
    putText(img, "P: Cambiar Vista", Point(x + 30, y + 150), 
            FONT_HERSHEY_SIMPLEX, 0.4, Scalar(100, 255, 100), 1);
    
    // Indicador de vista actual
    string viewMode = isTopDown ? "Vista: SUPERIOR" : "Vista: ISOMETRICA";
    putText(img, viewMode, Point(x + 25, y + 170), 
            FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 200, 100), 1);
}

#endif
