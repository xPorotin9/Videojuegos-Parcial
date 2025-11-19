#ifndef LVL4_H
#define LVL4_H

#include "level_base.h"

class Level4 : public ILevel {
public:
    Level create() override {
        Level level;

        const string NIVEL_NOMBRE = "Finichs level";
        const Point3f POSICION_INICIAL = Point3f(0, -0.4f, 0);
        const Point3f CAMARA_INICIAL = Point3f(0, -6, 5);
        
        const Scalar COLOR_CIELO = Scalar(150, 100, 100);
        const Scalar COLOR_HORIZONTE = Scalar(100, 50, 50);
        
        // IMAGEN DE FONDO
        const string IMAGEN_FONDO = "terraria4.png";
        
        // ============================================
        // CONFIGURACION DE FISICA
        // ============================================
        const float GRAVEDAD = 0.010f;
        const float FUERZA_SALTO = 0.42f;
        const float VELOCIDAD_MOV = 0.16f;
        const float FRICCION_SUELO = 0.35f;
        const float FRICCION_AIRE = 0.84f;
        
        // ============================================
        // CONFIGURACION DE SECCIONES
        // ============================================
        
        // SECCION 1: SALTOS DE PRECISION
        const float S1_SIZE_PLATAFORMA = 2.5f;
        const float S1_DISTANCIA_SALTO = 4.5f;
        const float S1_VARIACION_ALTURA = 0.5f;
        
        // SECCION 2: PLATAFORMAS DIMINUTAS
        const float S2_SIZE_MINIMO = 1.8f;
        const float S2_SIZE_MAXIMO = 2.3f;
        const float S2_DISTANCIA = 3.8f;
        
        // SECCION 3: ESCALADA FINAL
        const float S3_ALTURA_ESCALON = 1.0f;
        const float S3_SIZE_ESCALON = 2.2f;
        const float S3_DISTANCIA_ESCALON = 4.0f;
        
        // ============================================
        // COLORES POR SECCION
        // ============================================
        const Scalar COLOR_INICIO = Scalar(100, 200, 100);
        const Scalar COLOR_META = Scalar(255, 200, 100);
        const Scalar COLOR_SECCION1 = Scalar(200, 150, 150);
        const Scalar COLOR_SECCION2 = Scalar(150, 150, 200);
        const Scalar COLOR_SECCION3 = Scalar(180, 180, 180);
        
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
            FRICCION_SUELO, FRICCION_AIRE, "Quien sabe"
        );
        
        // PLATAFORMA INICIAL
        level.platforms.push_back({
            Point3f(0, 0, 0),
            4.0f, 4.0f, 0.5f,
            COLOR_INICIO, 0
        });
        
        float current_z = 0;
        int platform_id = 1;
        
        // SECCION 1: SALTOS DE PRECISION
        for (int i = 0; i < 3; i++) {
            current_z += S1_DISTANCIA_SALTO;
            float x = (i == 1) ? -3.0f : (i == 2) ? 3.0f : 0;
            float y = -(i * S1_VARIACION_ALTURA + 0.5f);
            
            level.platforms.push_back({
                Point3f(x, y, current_z),
                S1_SIZE_PLATAFORMA, S1_SIZE_PLATAFORMA, 0.5f,
                COLOR_SECCION1, platform_id++
            });
        }
        
        // SECCION 2: PLATAFORMAS DIMINUTAS
        current_z += 2;
        
        for (int i = 0; i < 4; i++) {
            current_z += S2_DISTANCIA;
            float x = (i % 2 == 0) ? -2.5f : 2.5f;
            float y = -2.0f - (i * 0.4f);
            float pSize = (i % 2 == 0) ? S2_SIZE_MINIMO : S2_SIZE_MAXIMO;
            
            level.platforms.push_back({
                Point3f(x, y, current_z),
                pSize, pSize, 0.5f,
                COLOR_SECCION2, platform_id++
            });
        }
        
        // SECCION 3: ESCALADA FINAL
        current_z += 3;
        
        for (int i = 0; i < 4; i++) {
            current_z += S3_DISTANCIA_ESCALON;
            float y = -4.0f - (i * S3_ALTURA_ESCALON);
            float x = (i == 1) ? 2.0f : (i == 2) ? -2.0f : 0;
            float pSize = S3_SIZE_ESCALON - (i * 0.1f);
            
            level.platforms.push_back({
                Point3f(x, y, current_z),
                pSize, pSize, 0.5f,
                COLOR_SECCION3, platform_id++
            });
        }
        
        // PLATAFORMA META
        current_z += S3_DISTANCIA_ESCALON + 2;
        
        level.platforms.push_back({
            Point3f(0, -8.0f, current_z),
            6.0f, 6.0f, 0.5f,
            COLOR_META, 99
        });
        
        level.goalPlatformIndex = level.platforms.size() - 1;
        
        // Cargar la imagen de fondo
        level.loadBackgroundImage();
        
        return level;
    }
};
 
#endif