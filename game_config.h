#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

struct PhysicsConfig {
    float gravity;        // Gravedad (0.015f normal)
    float jumpForce;      // Fuerza de salto (0.35f normal)
    float moveSpeed;      // Velocidad de movimiento (0.15f normal)
    float groundFriction; // Fricción en el suelo (0.3f normal)
    float airFriction;    // Fricción en el aire (0.85f normal)
    
    PhysicsConfig() : 
        gravity(0.015f),
        jumpForce(0.35f),
        moveSpeed(0.15f),
        groundFriction(0.3f),
        airFriction(0.85f) {}
    
    PhysicsConfig(float g, float j, float m, float gf, float af) :
        gravity(g), jumpForce(j), moveSpeed(m), 
        groundFriction(gf), airFriction(af) {}
};

// DIFERENTES TIPOS DE FÍSICA PREDEFINIDOS
class PhysicsStrategy {
public:
    virtual ~PhysicsStrategy() {}
    virtual PhysicsConfig getConfig() const = 0;
    virtual const char* getName() const = 0;
};

// 1. FÍSICA NORMAL
class NormalPhysics : public PhysicsStrategy {
public:
    PhysicsConfig getConfig() const override {
        return PhysicsConfig(
            0.015f,  // gravity
            0.35f,   // jumpForce
            0.15f,   // moveSpeed
            0.3f,    // groundFriction
            0.85f    // airFriction
        );
    }
    const char* getName() const override { return "Normal"; }
};

// 2. SALTO ALTO (para plataformas lejanas)
class HighJumpPhysics : public PhysicsStrategy {
public:
    PhysicsConfig getConfig() const override {
        return PhysicsConfig(
            0.015f,  // gravity (normal)
            0.50f,   // jumpForce (MÁS ALTO)
            0.15f,   // moveSpeed
            0.3f,    // groundFriction
            0.85f    // airFriction
        );
    }
    const char* getName() const override { return "Salto Alto"; }
};

// 3. BAJA GRAVEDAD (estilo lunar)
class LowGravityPhysics : public PhysicsStrategy {
public:
    PhysicsConfig getConfig() const override {
        return PhysicsConfig(
            0.008f,  // gravity (MENOS GRAVEDAD)
            0.35f,   // jumpForce
            0.15f,   // moveSpeed
            0.3f,    // groundFriction
            0.92f    // airFriction (menos fricción)
        );
    }
    const char* getName() const override { return "Baja Gravedad"; }
};

// 4. SÚPER SALTO
class SuperJumpPhysics : public PhysicsStrategy {
public:
    PhysicsConfig getConfig() const override {
        return PhysicsConfig(
            0.018f,  // gravity (un poco más)
            0.70f,   // jumpForce (SÚPER ALTO)
            0.12f,   // moveSpeed (un poco más lento)
            0.4f,    // groundFriction
            0.88f    // airFriction
        );
    }
    const char* getName() const override { return "Super Salto"; }
};

class PrecisionPhysics : public PhysicsStrategy {
public:
    PhysicsConfig getConfig() const override {
        return PhysicsConfig(
            0.020f,  // gravity (más gravedad)
            0.30f,   // jumpForce (salto controlado)
            0.10f,   // moveSpeed (MÁS LENTO)
            0.5f,    // groundFriction (más fricción)
            0.80f    // airFriction
        );
    }
    const char* getName() const override { return "Precision"; }
};

class CustomPhysics : public PhysicsStrategy {
private:
    PhysicsConfig config;
    string name;
    
public:
    CustomPhysics(float g, float j, float m, float gf, float af, const string& n) :
        config(g, j, m, gf, af), name(n) {}
    
    PhysicsConfig getConfig() const override { return config; }
    const char* getName() const override { return name.c_str(); }
};

#endif