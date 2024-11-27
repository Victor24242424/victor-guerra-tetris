#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <ctime>

using namespace std;

#define ARRIBA 72
#define IZQUIERDA 75
#define DERECHA 77
#define ABAJO 80

// Tamaño del tablero ajustado
const int width = 13, height = 30;
int board[height][width] = {0};
int score = 0;

int tetrominos[][4][4] = {
    {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}, // Línea
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // Cuadrado
    {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // T
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // Z
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // S
    {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // L
    {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // J
};

// Colores para las piezas (usando valores de 1 a 15)
int colors[] = {
    9, // Azul para la línea
    14, // Amarillo para el cuadrado
    13, // Rosa para T
    12, // Rojo para Z
    11, // Verde para S
    10, // Verde claro para L
    15  // Blanco para J
};

int currentPiece = 0, rotation = 0;
int x = width / 2 - 2, y = 0; // Centrar la pieza

void gotoxy(int x, int y) {
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hCon, dwPos);
}

void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.bVisible = FALSE;
    info.dwSize = 100;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void drawBorders() {
    // Dibuja las esquinas con "+"
    gotoxy(2, 3); printf("+");
    for (int i = 0; i < width; i++) {
        gotoxy(i * 2 + 3, 3); printf("-"); // Parte superior con "-"
    }
    gotoxy(width * 2 + 3, 3); printf("+");

    // Dibuja la parte inferior con "+"
    gotoxy(2, height + 4); printf("+");
    for (int i = 0; i < width; i++) {
        gotoxy(i * 2 + 3, height + 4); printf("-"); // Parte inferior con "-"
    }
    gotoxy(width * 2 + 3, height + 4); printf("+");

    // Dibuja los lados con "|"
    for (int i = 4; i < height + 4; i++) {
        gotoxy(2, i); printf("|"); // Lado izquierdo
        gotoxy(width * 2 + 3, i); printf("|"); // Lado derecho
    }
}

void drawBoard() {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            gotoxy(j * 2 + 3, i + 4);  // Ajustar la posición para que no se sobreponga con los bordes
            if (board[i][j]) {
                cout << "[]";
            } else {
                cout << "  ";
            }
        }
    }
}

bool checkCollision(int offsetX, int offsetY, int piece[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piece[i][j]) {
                int newX = x + j + offsetX;
                int newY = y + i + offsetY;
                if (newX < 0 || newX >= width || newY >= height || (newY >= 0 && board[newY][newX])) {
                    return true;
                }
            }
        }
    }
    return false;
}

void fixPiece() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetrominos[currentPiece][i][j]) {
                board[y + i][x + j] = 1;
            }
        }
    }
}

void rotatePiece() {
    int rotated[4][4] = {0};

    // Realizar la rotación de la pieza (trasponer y luego invertir columnas)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            rotated[j][3 - i] = tetrominos[currentPiece][i][j];
        }
    }

    // Comprobar si la rotación es válida antes de aplicarla
    if (!checkCollision(0, 0, rotated)) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                tetrominos[currentPiece][i][j] = rotated[i][j];
            }
        }
    }
}

void clearLines() {
    for (int i = height - 1; i >= 0; i--) {
        bool fullLine = true;
        for (int j = 0; j < width; j++) {
            if (!board[i][j]) {
                fullLine = false;
                break;
            }
        }
        if (fullLine) {
            score += 100;
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < width; j++) {
                    board[k][j] = board[k - 1][j];
                }
            }
            for (int j = 0; j < width; j++) {
                board[0][j] = 0;
            }
            i++; // Repetir comprobación para la nueva línea que se mueve hacia arriba
        }
    }
}

void drawPiece() {
    setColor(colors[currentPiece]);  // Establece el color de la pieza
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetrominos[currentPiece][i][j]) {
                gotoxy((x + j) * 2 + 3, y + i + 4);  // Ajustar la posición
                cout << "[]";
            }
        }
    }
    setColor(7);  // Restaurar color predeterminado (blanco)
}

void displayScore() {
    gotoxy(width * 2 + 5, 2);  // Ajustar la posición del puntaje
    cout << "Puntos: " << score;
}

void displayControls() {
    gotoxy(3, height + 6);  // Ajustar la posición de los controles
    cout << "Controles: ";
    gotoxy(3, height + 7);  
    cout << "Izquierda: <-    Derecha: ->    Abajo: v    Rotar: ^    Salir: ESC";
}

int main() {
    srand(time(0));
    bool gameOver = false;
    system("cls");
    hideCursor(); // Ocultar el cursor
    drawBorders();

    while (!gameOver) {
        drawBoard();
        drawPiece();
        displayScore();
        displayControls();  // Mostrar los controles debajo del puntaje

        if (_kbhit()) {
            char key = _getch();
            if (key == IZQUIERDA && !checkCollision(-1, 0, tetrominos[currentPiece])) x--;
            if (key == DERECHA && !checkCollision(1, 0, tetrominos[currentPiece])) x++;
            if (key == ABAJO && !checkCollision(0, 1, tetrominos[currentPiece])) y++;
            if (key == ARRIBA) rotatePiece();
            if (key == 27) break; // Salir con la tecla ESC
        }

        if (checkCollision(0, 1, tetrominos[currentPiece])) {
            fixPiece();
            clearLines();
            currentPiece = rand() % 7;
            rotation = 0;
            x = width / 2 - 2;
            y = 0;

            if (checkCollision(0, 0, tetrominos[currentPiece])) {
                gameOver = true;  // Fin del juego si la nueva pieza no cabe
            }
        } else {
            y++;  // Mover la pieza hacia abajo
        }

        Sleep(100);  // Controlar la velocidad de caída
    }

    return 0;
}


