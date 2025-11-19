# Videojuegos - Parcial

## Qué es

Este repositorio contiene un proyecto de juego 2D desarrollado como parte de un parcial. El juego es una pequeña demo/prototipo con lógica de físicas básica, renderizado y niveles (incluye varios ficheros de niveles en `levels/`).

## Cómo funciona (resumen técnico)

- Motor: proyecto en C++ con CMake y ficheros de proyecto para Visual Studio.
- Estructura: los archivos principales están en la raíz (`main.cpp`, `game.h`, `level_manager.h`, etc.).
- Niveles: los niveles están en la carpeta `levels/`.
- Build: se usa CMake para generar los ficheros de proyecto; luego compilar con Visual Studio o `cmake --build`.

## Ejecutar localmente (Windows / PowerShell)

1. Crear carpeta de build y configurar:

```
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

2. Ejecutar el binario generado (ruta posible):

```
# Ajusta la ruta según tu configuración y nombre del ejecutable
.\build\Debug\3DGAME_App.exe
```

También puedes abrir la solución en Visual Studio (`build/3DGAME.sln`) y compilar/ejecutar desde el IDE.

## Archivos importantes

- `main.cpp`: punto de entrada.
- `game.h`, `game_config.h`, `physics.h`, `rendering.h`: lógica principal y configuración.
- `level_manager.h`, `levels/`: definición y contenidos de niveles.
- `build/`: archivos de construcción generados (no incluir en el repo).

## Integrantes

- José Machaca
- Jhosep Mollapaza



