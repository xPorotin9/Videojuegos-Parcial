#ifndef LVL2_H
#define LVL2_H

#include "level_base.h"

class Level2 : public ILevel {
public:
    Level create() override {
        Level level;
        
        const string NIVEL_NOMBRE = "Chiki plataformas";
        const Point3f POSICION_INICIAL = Point3f(0, -0.4f, 0);
        const Point3f CAMARA_INICIAL = Point3f(0, -5, 5);
        
        const Scalar COLOR_CIELO = Scalar(180, 160, 200);
        const Scalar COLOR_HORIZONTE = Scalar(120, 100, 140);
        
        // IMAGEN DE FONDO
        const string IMAGEN_FONDO = "terraria2.png";
        
        // ============================================
        // CONFIGURACION DE FISICA
        // ============================================
        const float GRAVEDAD = 0.008f;
        const float FUERZA_SALTO = 0.45f;
        const float VELOCIDAD_MOV = 0.15f;
        const float FRICCION_SUELO = 0.4f;
        const float FRICCION_AIRE = 0.83f;
        
        // ============================================
        // CONFIGURACION DE PLATAFORMAS
        // ============================================
        const float SIZE_INICIO = 4.0f;
        const float SIZE_PEQUENO = 2.2f;
        const float SIZE_MEDIANO = 2.8f;
        const float SIZE_META = 5.0f;
        const float GROSOR = 0.5f;
        
        const float DISTANCIA_LATERAL = 3.5f;
        const float DISTANCIA_AVANCE = 4.0f;
        const float SUBIDA_POR_PLATAFORMA = 0.5f;
        const int NUMERO_PLATAFORMAS = 8;
        
        // ============================================
        // COLORES
        // ============================================
        const Scalar COLOR_INICIO = Scalar(100, 200, 100);
        const Scalar COLOR_META = Scalar(255, 200, 100);
        const Scalar COLOR_BASE = Scalar(150, 150, 200);
        
        // ============================================
        // CONSTRUCCION DEL NIVEL
        // ============================================
        
        level.name = NIVEL_NOMBRE;
        level.startPosition = POSICION_INICIAL;
        level.cameraStart = CAMARA_INICIAL;
        level.backgroundColor1 = COLOR_CIELO;
        level.backgroundColor2 = COLOR_HORIZONTE;
        level.backgroundImagePath = IMAGEN_FONDO;  // Asignar imagen de fondo
        
        level.physicsStrategy = make_unique<CustomPhysics>(
            GRAVEDAD, FUERZA_SALTO, VELOCIDAD_MOV,
            FRICCION_SUELO, FRICCION_AIRE, "Saltos altos"
        );
        
        // PLATAFORMA INICIAL
        level.platforms.push_back({
            Point3f(0, 0, 0),
            SIZE_INICIO, SIZE_INICIO, GROSOR,
            COLOR_INICIO, 0
        });
        
        // GENERAR ZIGZAG
        for (int i = 1; i <= NUMERO_PLATAFORMAS; i++) {
            float x = (i % 2 == 0) ? DISTANCIA_LATERAL : -DISTANCIA_LATERAL;
            float y = -(i * SUBIDA_POR_PLATAFORMA);
            float z = i * DISTANCIA_AVANCE;
            
            float platSize;
            if (i % 3 == 0) {
                platSize = SIZE_MEDIANO;
            } else if (i > 5) {
                platSize = SIZE_PEQUENO * 0.9f;
            } else {
                platSize = SIZE_PEQUENO;
            }
            
            Scalar color(
                COLOR_BASE[0] + i * 10,
                COLOR_BASE[1],
                COLOR_BASE[2] - i * 10
            );
            
            level.platforms.push_back({
                Point3f(x, y, z),
                platSize, platSize, GROSOR,
                color, i
            });
        }
        
        // PLATAFORMA META
        float meta_z = (NUMERO_PLATAFORMAS + 1) * DISTANCIA_AVANCE;
        level.platforms.push_back({
            Point3f(0, -(NUMERO_PLATAFORMAS * SUBIDA_POR_PLATAFORMA + 1), meta_z),
            SIZE_META, SIZE_META, GROSOR,
            COLOR_META, 99
        });
        
        level.goalPlatformIndex = level.platforms.size() - 1;
        
        // Cargar la imagen de fondo
        level.loadBackgroundImage();
        
        return level;
    }
};

#endif