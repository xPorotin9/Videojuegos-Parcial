#ifndef LVL1_H
#define LVL1_H

#include "level_base.h"

class Level1 : public ILevel {
public:
    Level create() override {
        Level level;
        
        // INFORMACION BASICA
        const string NIVEL_NOMBRE = "Tuto";
        const Point3f POSICION_INICIAL = Point3f(-2.0f, -0.4f, 0);
        const Point3f CAMARA_INICIAL = Point3f(0, -5, 5);
        
        // COLORES DEL FONDO
        const Scalar COLOR_CIELO = Scalar(160, 190, 220);
        const Scalar COLOR_HORIZONTE = Scalar(100, 140, 180);
        
        // IMAGEN DE FONDO
        const string IMAGEN_FONDO = "terraria1.png";
        
        // ============================================
        // CONFIGURACION DE FISICA
        // ============================================
        const float GRAVEDAD = 0.012f;
        const float FUERZA_SALTO = 0.35f;
        const float VELOCIDAD_MOV = 0.15f;
        const float FRICCION_SUELO = 0.3f;
        const float FRICCION_AIRE = 0.85f;
        
        // ============================================
        // CONFIGURACION DE PLATAFORMAS
        // ============================================
        const float SIZE_NORMAL = 4.0f;      // Tamano estandar
        const float SIZE_META = 6.0f;        // Tamano de meta
        const float GROSOR_PLATAFORMA = 0.5f;
        
        const float DISTANCIA_Z_BASE = 5.0f;
        const float VARIACION_X = 3.0f;
        const float INCREMENTO_ALTURA = 0.5f;
        
        // ============================================
        // COLORES DE PLATAFORMAS
        // ============================================
        const Scalar COLOR_INICIO = Scalar(100, 200, 100);
        const Scalar COLOR_META = Scalar(255, 200, 100);
        
        const vector<Scalar> COLORES_PLATAFORMAS = {
            Scalar(150, 150, 200),
            Scalar(200, 150, 150),
            Scalar(200, 200, 100),
            Scalar(180, 120, 200),
            Scalar(100, 180, 180),
            Scalar(150, 200, 100),
            Scalar(200, 150, 200),
            Scalar(150, 150, 150)
        };
        
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
            FRICCION_SUELO, FRICCION_AIRE, "Tutorial"
        );
        
        // Plataforma INICIO
        level.platforms.push_back({
            Point3f(0, 0, 0),
            SIZE_NORMAL, SIZE_NORMAL, GROSOR_PLATAFORMA,
            COLOR_INICIO, 0
        });
        
        // Configuracion de plataformas
        struct PlataformaConfig {
            float x, y, z;
            float size;
        };
        
        vector<PlataformaConfig> configuraciones = {
            {2.0f,  -0.5f,  5.0f,  SIZE_NORMAL},
            {-2.0f, -1.0f,  10.0f, SIZE_NORMAL},
            {4.0f,  -1.5f,  14.0f, SIZE_NORMAL},
            {-1.0f, -2.0f,  18.0f, SIZE_NORMAL},
            {3.0f,  -2.5f,  22.0f, SIZE_NORMAL},
            {-2.0f, -3.0f,  26.0f, SIZE_NORMAL},
            {2.0f,  -3.5f,  30.0f, SIZE_NORMAL},
            {5.0f,  -4.0f,  34.0f, SIZE_NORMAL}
        };
        
        for (size_t i = 0; i < configuraciones.size(); i++) {
            auto& config = configuraciones[i];
            level.platforms.push_back({
                Point3f(config.x, config.y, config.z),
                config.size, config.size, GROSOR_PLATAFORMA,
                COLORES_PLATAFORMAS[i % COLORES_PLATAFORMAS.size()],
                (int)(i + 1)
            });
        }
        
        // Plataforma META
        level.platforms.push_back({
            Point3f(0, -4.5f, 38),
            SIZE_META, SIZE_META, GROSOR_PLATAFORMA,
            COLOR_META, 99
        });
        
        level.goalPlatformIndex = level.platforms.size() - 1;
        
        // Cargar la imagen de fondo
        level.loadBackgroundImage();
        
        return level;
    }
};

#endif