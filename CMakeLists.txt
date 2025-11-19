cmake_minimum_required(VERSION 3.10)
project(3DGAME)

# Versión de C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cambiar el nombre del ejecutable para evitar conflictos
set(EXECUTABLE_NAME "3DGAME_App")

# Modificar la carpeta de salida (opcional)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Encontrar OpenCV
set(OpenCV_DIR "E:/opencv/build")  # Ajusta esta ruta a tu instalación
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

# Archivos fuente y headers
set(SOURCES
    main.cpp
)

set(HEADERS
    game_entities.h
    rendering.h
)

# Agregar el ejecutable con el nuevo nombre
add_executable(${EXECUTABLE_NAME} ${SOURCES} ${HEADERS})

# Vincular las bibliotecas de OpenCV
target_link_libraries(${EXECUTABLE_NAME} ${OpenCV_LIBS})

# Mensaje informativo al finalizar la configuración
message(STATUS "==================================")
message(STATUS "Configuración completada")
message(STATUS "Ejecutable: ${EXECUTABLE_NAME}")
message(STATUS "Ruta de salida: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "Archivos fuente: ${SOURCES}")
message(STATUS "Archivos header: ${HEADERS}")
message(STATUS "==================================")
# Instrucciones para compilar y ejecutar