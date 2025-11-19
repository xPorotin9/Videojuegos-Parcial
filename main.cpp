#include <opencv2/opencv.hpp>
#include <iostream>
#include <exception>
#include "game.h"

using namespace cv;
using namespace std;

int main() {
    try {
        const int WIDTH = 1200;
        const int HEIGHT = 800;
        
        cout << "==================================" << endl;
        cout << "  JUEGO DE PLATAFORMAS 3D       " << endl;
        cout << "==================================" << endl;
        cout << "CONTROLES:" << endl;
        cout << "  WASD     - Movimiento" << endl;
        cout << "  ESPACIO  - Saltar" << endl;
        cout << "  P        - Cambiar vista" << endl;
        cout << "  R        - Reiniciar nivel" << endl;
        cout << "  N        - Siguiente nivel" << endl;
        cout << "  B        - Nivel anterior" << endl;
        cout << "  1-4      - Ir a nivel específico" << endl;
        cout << "  C        - Cambiar camara" << endl;
        cout << "  ESC      - Salir" << endl;
        cout << "==================================" << endl;
        cout << endl;
        cout << "PATRONES DE DISEÑO IMPLEMENTADOS:" << endl;
        cout << "  - Factory Pattern (Niveles)" << endl;
        cout << "  - Strategy Pattern (Física)" << endl;
        cout << "  - Singleton (Game Manager)" << endl;
        cout << "==================================" << endl;
        
        Game game(WIDTH, HEIGHT);
        
        while (game.isGameRunning()) {
            int key = waitKey(1);
            
            // Teclas numéricas para selección directa de nivel
            if (key >= '1' && key <= '4') {
                game.goToLevel(key - '1');
            } else {
                game.handleInput(key);
            }
            
            game.update();
            
            Mat frame = game.render();
            imshow("Juego DDD y DD", frame);
        }
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return -1;
    } catch (...) {
        cerr << "Error desconocido" << endl;
        return -1;
    }
    
    destroyAllWindows();
    return 0;
}