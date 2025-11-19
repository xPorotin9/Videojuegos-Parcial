#ifndef LVL3_H
#define LVL3_H

#include "level_base.h"

class Level3 : public ILevel {
public:
    Level create() override {
        Level level;

        const string NIVEL_NOMBRE = "Escaleras";
        const Point3f POSICION_INICIAL = Point3f(0, -0.4f, 0);
        const Point3f CAMARA_INICIAL = Point3f(0, -8, 5);
        
        const Scalar COLOR_CIELO = Scalar(200, 180, 150);
        const Scalar COLOR_HORIZONTE = Scalar(150, 120, 100);
        
        // IMAGEN DE FONDO
        const string IMAGEN_FONDO = "terraria3.png";
        
        // ============================================
        // CONFIGURACION DE FISICA
        // ============================================
        const float GRAVEDAD = 0.015f;
        const float FUERZA_SALTO = 0.35f;
        const float VELOCIDAD_MOV = 0.15f;
        const float FRICCION_SUELO = 0.3f;
        const float FRICCION_AIRE = 0.85f;
        
        // ============================================
        // CONFIGURACION DE ESCALERA ESPIRAL
        // ============================================
        const float SIZE_INICIO = 4.0f;
        const float SIZE_META = 6.0f;
        const float GROSOR = 0.5f;
        
        const int NUMERO_ESCALONES = 12;
        const float RADIO_ESPIRAL = 4.5f;
        const float VELOCIDAD_ROTACION = 0.6f;
        const float ALTURA_POR_ESCALON = 0.7f;
        const float AVANCE_POR_ESCALON = 3.2f;
        
        const float SIZE_INICIAL_ESCALON = 3.5f;
        const float REDUCCION_POR_ESCALON = 0.12f;
        const float SIZE_MINIMO = 2.0f;
        
        // ============================================
        // COLORES
        // ============================================
        const Scalar COLOR_INICIO = Scalar(100, 200, 100);
        const Scalar COLOR_META = Scalar(255, 200, 100);
        const Scalar COLOR_BASE_ESCALON = Scalar(200, 100, 150);
        
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
            FRICCION_SUELO, FRICCION_AIRE, "Normal"
        );
        
        // PLATAFORMA INICIAL
        level.platforms.push_back({
            Point3f(0, 0, 0),
            SIZE_INICIO, SIZE_INICIO, GROSOR,
            COLOR_INICIO, 0
        });
        
        // GENERAR ESCALERA ESPIRAL
        for (int i = 1; i <= NUMERO_ESCALONES; i++) {
            float angulo = i * VELOCIDAD_ROTACION;
            float x = sin(angulo) * RADIO_ESPIRAL;
            float y = -(i * ALTURA_POR_ESCALON);
            float z = i * AVANCE_POR_ESCALON;
            
            float platSize = SIZE_INICIAL_ESCALON - (i * REDUCCION_POR_ESCALON);
            platSize = max(platSize, SIZE_MINIMO);
            
            Scalar color(
                COLOR_BASE_ESCALON[0] - i * 8,
                COLOR_BASE_ESCALON[1] + i * 8,
                COLOR_BASE_ESCALON[2]
            );
            
            level.platforms.push_back({
                Point3f(x, y, z),
                platSize, platSize, GROSOR,
                color, i
            });
        }
        
        // PLATAFORMA META
        float meta_y = -((NUMERO_ESCALONES + 1) * ALTURA_POR_ESCALON);
        float meta_z = (NUMERO_ESCALONES + 1) * AVANCE_POR_ESCALON;
        
        level.platforms.push_back({
            Point3f(0, meta_y, meta_z),
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