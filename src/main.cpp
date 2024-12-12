/*
    Tamaño de la pantalla 240x320 pixeles
*/

// Librería para el protocolo de datos seriales síncronos SPI (Serial Peripheral Interface)
#include "SPI.h"
// Librería para gráficos, con un conjunto de primitivas: puntos, líneas, círculos, etc.
#include "Adafruit_GFX.h" 
// Librería para el display LCD TFT (Thin Film Transistor) 
#include "Adafruit_ILI9341.h"

// Include Sprites
#include "Sprite.h"

// Se definen los pines para la conexión entre el Arduino Mega y el TFT LCD ILI9341
#define TFT_DC 7
#define TFT_CS 6
#define TFT_MOSI 11
#define TFT_CLK 13
#define TFT_RST 10
#define TFT_MISO 12

#define buzzerPin 8
#define botonRight 18
#define botonDown 19
#define botonUp 20
#define botonLeft 21

const int XMAX = 240; // Alto del display
const int YMAX = 320; // Ancho del display
int x = XMAX/2; // Posición x del jugador
int y = YMAX/2; // Posición y del jugador
int xc = XMAX - 65;
int yc = YMAX -64;
int xg = XMAX + 15;
int yg = YMAX/2 - 40;
int xcas = XMAX + 75;
int ycas = YMAX - 45;
int xm = XMAX/2 - 35;
int ym = YMAX/2 + 35;
int xp = XMAX + 80;
int yp = YMAX/2 + 50;
int puntaje = 0; //Variable que almacena el valor de puntaje a mostrar
const uint8_t UP = 0;
const uint8_t RIGHT = 1;
const uint8_t DOWN = 2;
const uint8_t LEFT = 3;
bool comidaMz = false;
bool comidaPt = false;
bool reiniciar = false;

// Se instancia el objeto screen del tipo Adafruit_ILI9341
Adafruit_ILI9341 screen = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

GFXcanvas1 canvas(64, 64);

// Declaración de funciones
void mostrar_nombre(void);
void mostrar_tiempo_jugado(void);
void setPlayerPosition(int x, int y);
void animatePlayer(void);
void moverPlayerDerecha(void);
void moverPlayerIzq(void);
void moverPlayerSaltar(void);
void moverPlayerBajar(void);
void moverPlayer(uint8_t direccion);
void sonidoMov (uint8_t direccion);
void mostrar_objeto(void);
void moverManzana (void);
void moverCactus (void);
void moverGusano (void);
void moverPlatano (void);
void moverCascara (void);
bool checkCollision(int playerX, int playerY, int playerWidth, int playerHeight, 
int objectX, int objectY, int objectWidth, int objectHeight);
void mostrarPuntaje (void);
void sonidoPremio (void);
void reaparecerP(int &objectX, int &objectY,int objectWidth, int objectHeight,int XMAX, int YMAX);
void resetObj(int &x, int &y, int width, int height,int XMAX, int YMAX);
void sonidoG_O (void);
void gameOver(void);


// Variable para llevar el tiempo en milisegundos
unsigned long tiempoInicio;

//CONFIGURACIÓN
void setup() {
    Serial.begin(9600);
    Serial.println("SERIAL INICIADO");

    //se agregan las interrupcines externas
    attachInterrupt(digitalPinToInterrupt(botonRight), moverPlayerDerecha, HIGH);
    attachInterrupt(digitalPinToInterrupt(botonDown), moverPlayerBajar, HIGH);
    attachInterrupt(digitalPinToInterrupt(botonUp), moverPlayerSaltar, HIGH);
    attachInterrupt(digitalPinToInterrupt(botonLeft), moverPlayerIzq, HIGH);

    //configuración de la pantalla
    setPlayerPosition(32 - 32, YMAX - 64);  
    screen.begin(); // Inicialización
    screen.fillScreen(ILI9341_BLACK); // Pinta la pantalla de negro

    mostrar_nombre(); // Llama a la función para mostrar el nombre del juego

    // Iniciar el tiempo
    tiempoInicio = millis();
    sei();     
}

//LAZO INFITO, EL CÓDIGO SE REPETIRÁ CONSTANTEMENTE
void loop() {
    mostrar_tiempo_jugado();
    animatePlayer();
    mostrar_objeto();
    moverManzana();
    moverCactus();
    moverPlatano();
    moverGusano();
    moverCascara();

    //verifica si hay colision entre la manzana y el jugador 
    if (checkCollision(x, y, 50, 50, xm, ym, 10, 10)) {
        comidaMz = true;
        puntaje += 1.5;
        sonidoPremio ();
        mostrarPuntaje();
        Serial.println("colision manzana");
        Serial.println(x);
        Serial.println(y);
        reaparecerP(xm, ym, 64, 64, XMAX, YMAX);
    }
    //verifica si hay colision entre el cactus y el jugador
    if (checkCollision(x, y, 40, 40, xc, yc, 20, 20)) {
        Serial.println("colision cactus");
        Serial.println(x);
        Serial.println(y);
        gameOver();
    } 
    //verifica si hay colision entre el platano y el jugador
    if (checkCollision(x, y, 45, 45, xp, yp, 10, 10)) {
        comidaPt = true;
        puntaje += 3.5;
        sonidoPremio ();
        mostrarPuntaje();
        Serial.println("colision platano");
        Serial.println(x);
        Serial.println(y);
        reaparecerP(xp, yp, 64, 64, XMAX, YMAX);
        
    } 
    //verifica si hay colision entre el gusano y el jugador
    if (checkCollision(x, y, 45, 45, xc, yc,  20, 20)) {
        Serial.println("colision gusano");
        Serial.println(x);
        Serial.println(y);
        gameOver();
    } 
    //verifica si hay colision entre la cascara y el jugador
    if (checkCollision(x, y, 45, 45, xcas, ycas, 20, 20)) {
        Serial.println("colision cascara");
        Serial.println(x);
        Serial.println(y);
        gameOver();
    } 

    //permite que los obstaculos vuelvan a salir en pantalla en posiciones aleatorias
    resetObj(xc,yc,64,64, XMAX, YMAX);
    resetObj(xg, yg, 64, 64, XMAX, YMAX);
    resetObj(xcas, ycas, 64, 64, XMAX, YMAX);
}



// FUNCIÓN PARA MOSTRAR EL NOMBRE DEL JUEGO
void mostrar_nombre() {
    screen.setTextColor(ILI9341_CYAN);
    screen.setTextSize(4);
    screen.setCursor(XMAX / 2 - 50, YMAX / 2 - 70); // Tamaño grande para el texto
    screen.print("PORKI"); // Posiciona el texto en el centro de la pantalla
    screen.drawBitmap(XMAX / 2 - 45, YMAX / 2 - 25, Principal, 100, 100, ILI9341_PINK, ILI9341_BLACK);
    screen.drawBitmap(XMAX / 2 - 95, YMAX /2 - 30, Banana, 64, 64, ILI9341_YELLOW, ILI9341_BLACK);
    screen.drawBitmap(XMAX / 2 + 40, YMAX / 2, Manzana, 64, 64, ILI9341_RED, ILI9341_BLACK);

    delay(250);  // Muestra el texto por 250 ms
    screen.fillScreen(ILI9341_BLACK);
}

// FUNCIÓN QUE MUESTRA EL TIEMPO DEL JUGADOR 
void mostrar_tiempo_jugado() {
    unsigned long tiempoActual = millis();
    unsigned long tiempoJugado = (tiempoActual - tiempoInicio) / 1000; // Tiempo en segundos

    // Limpiar la parte superior derecha de la pantalla
    screen.fillRect(15, 40, 110, 40, ILI9341_BLACK);
    
    // Mostrar el tiempo en la esquina superior derecha
    screen.setTextColor(ILI9341_WHITE);
    screen.setTextSize(2);
    screen.setCursor(15, 40);
    screen.print("Tiempo:");
    screen.print(tiempoJugado);
    screen.print("s");
}

// FIJA LA POSICION DEL JUGADOR EN x Y y 
void setPlayerPosition(int x1, int y1) {
    x = x1;
    y = y1;
}

// FINCIÓN QUE ANIMA AL JUGADOR 
void animatePlayer(void) {
    static int count = 0; // Usar variable estática para mantener el estado
    canvas.fillScreen(0);
    canvas.drawBitmap(0, 0, Player[count], 64, 64, ILI9341_WHITE);
    screen.drawBitmap(x, y, canvas.getBuffer(), canvas.width(), canvas.height(), ILI9341_PINK, ILI9341_BLACK);
    
    count++;
    if (count == 2)
        count = 0;
}

// FUNCIÓN QUE MUEVE AL JUGADOR HACIA LA DERECHA
void moverPlayerDerecha() {
    delay(10);
    if (digitalRead(botonRight) == HIGH) {
        Serial.println("Boton RIGHT");
        moverPlayer(RIGHT);
    }    
}

// FUNCIÓN QUE MUEVE AL JUGADOR HACIA LA IZQUIERDA
void moverPlayerIzq() {
    delay(10);
    if(digitalRead(botonLeft) == HIGH) {
        Serial.println("Boton LEFT");
        moverPlayer(LEFT);
    }
}

// FUNCIÓN QUE MUEVE AL JUGADOR HACIA ARRIBA
void moverPlayerSaltar() {
    delay(10);
    if (digitalRead(botonUp) == HIGH) {
        Serial.println("Boton UP");
        moverPlayer(UP);
    }  
}
// FUNCIÓN QUE MUEVE AL JUGADOR HACIA ABAJO
void moverPlayerBajar() {
    delay(10);
    if (digitalRead(botonDown) == HIGH) {
        Serial.println("Boton DOWN");
        moverPlayer(DOWN);
    }    
}

//FUNCIÓN PARA REPRODICIR UN SONIDO BASADO EN LA DIRECCIÓN
void sonidoMov (uint8_t direccion) {
    switch (direccion) {
        case UP : //ARRIBA
            tone (buzzerPin, 800, 100); //TONO DE 800 HZ DURANTE 100MS
            break;
        case DOWN : //ABAJO
            tone (buzzerPin, 600, 100); //TONO DE 600 HZ DURANTE 100MS
            break;
        case LEFT : // IZQUIERDA
            tone (buzzerPin, 400, 100); //TONO DE 400 HZ DURANTE 100MS
            break;
        case RIGHT: //DERECHA
            tone (buzzerPin, 1000, 100); //TONO DE 1000 HZ DURANTE 100MS
            break;
        default: 
        //Si la dirección no es válida, no emite sonido
            break;
    }
}

// FUNCIÓN QUE MUEVE AL JUGADOR
void moverPlayer(uint8_t direccion) {
    uint8_t delta = 1;
    switch (direccion) {
        case UP: // Saltar
            y = y - 1.3 * delta;
            break;
        case RIGHT: // Avanzar
            x = x + 2 * delta;
            break;
        case DOWN: // Bajar
            y = y + 1.3 * delta;
            break;
        case LEFT : //Retroceder 
            x = x - 2 * delta;    
    }
    sonidoMov(direccion); //Reproduce el sonido según la dirección
}

//FUNCIÓN QUE MUEVE LA MANZANA 
void moverManzana () {
    uint8_t gamma = 4;
    xm -= gamma;
}
//FUNCIÓN QUE MUEVE EL CACTUS
void moverCactus() {
    uint8_t gama = 6;
    xc = xc - gama;
}
//FUNCIÓN QUE MUEVE EL GUSANO
void moverGusano() {
    uint8_t gama = 5;
    xg = xg - gama;
}
void moverPlatano() {
    uint8_t gama = 4;
    xp = xp - gama;
}
void moverCascara() {
    uint8_t gama = 4;
    xcas = xcas - gama;
}

//FUNCION QUE MUESTRA LOS OBJETOS (PREMIOS Y OBSTACULOS)
void mostrar_objeto() {
    screen.drawBitmap(xc, yc, Cactus, 64, 64, ILI9341_GREENYELLOW, ILI9341_BLACK);
    screen.drawBitmap(xg, yg, gusano, 64, 64, ILI9341_ORANGE, ILI9341_BLACK);
    screen.drawBitmap(xcas, ycas, Cascara, 64, 64, ILI9341_YELLOW, ILI9341_BLACK);

    if (comidaMz == false) {
        screen.drawBitmap(xm, ym, Manzana, 64, 64, ILI9341_RED, ILI9341_BLACK);
    }

    if (comidaPt == false) {
        screen.drawBitmap(xp, yp, Platano, 64, 64, ILI9341_YELLOW, ILI9341_BLACK);
    }
}

//FUNCIÓN QUE VERIFICA LA COLISIÓN ENTRE EL JUGADOR Y OBJETOS
bool checkCollision(int playerX, int playerY, int playerWidth, int playerHeight, 
int objectX, int objectY, int objectWidth, int objectHeight) {
    return !(playerX > objectX + objectWidth || playerX + playerWidth < objectX || 
            playerY > objectY + objectHeight ||  playerY + playerHeight < objectY);
 }

//FUNCIÓN PARA ACTUALIZAR Y MOSTRAR EL PUNTAJE
void mostrarPuntaje() {
    screen.fillRect(15, 0, 110, 40, ILI9341_BLACK);
    //actualizarP();

    // Mostrar el puntaje en la esquina superior izquierda
    screen.setTextColor(ILI9341_WHITE);
    screen.setTextSize(2);
    screen.setCursor(15, 0);
    screen.print("Puntaje:");
    screen.print(puntaje);
}

//FUNCIÓN PARA REPRODUCIR UN SONIDO AL ATRAPAR UN PREMIO
void sonidoPremio () {
    tone(buzzerPin, 1000, 200); // Tono agudo (1000 Hz) por 200 ms
    delay(200);                 // Pausa
    tone(buzzerPin, 1200, 150); // Tono un poco más agudo (1200 Hz) por 150 ms
    delay(150);                 // Pausa
    tone(buzzerPin, 1400, 100); // Tono más agudo (1400 Hz) por 100 ms
    delay(100);                 // Pausa final
    noTone(buzzerPin);          // Apaga el zumbador
}

//FUNCÍON QUE REAPARECE PREMIOS EN LA PANTALLA
void reaparecerP(int &objectX, int &objectY,int objectWidth, int objectHeight,int XMAX, int YMAX) {
    objectX = random(XMAX+15, XMAX+40); //Reinicia el objeto entre ese intervalo
    objectY = random(YMAX/2 - 10, YMAX - 60); 
}

//FUNCION QUE PERMITE QUE LOS OBJETOS VUELVAN HA APARECER EN PANTALLA
void resetObj (int &x, int &y, int width, int height, int XMAX, int YMAX) {
    if (x + width < 0) {  // Si el objeto sale por la izquierda
        x = XMAX;  // Reinicia en el borde derecho
      y = random(YMAX/2 - 10, YMAX - 60);  // Nueva posición aleatoria en y
    }
 }

//FUNCIÓN QUE REPRODECIE SONIDO CUANDO SE ACAA EL JUEGO
void sonidoG_O () {
    // Secuencia de tonos (frecuencia en Hz y duración en ms)
    tone(buzzerPin, 440, 300); // Tono 1: A4 (440 Hz)
    delay(300);
    tone(buzzerPin, 330, 300); // Tono 2: E4 (330 Hz)
    delay(300);
    tone(buzzerPin, 294, 300); // Tono 3: D4 (294 Hz)
    delay(300);
    tone(buzzerPin, 262, 500); // Tono 4: C4 (262 Hz)
    delay(500);
    noTone(buzzerPin);         // Detener el sonido
}

//FUNCIÓN QUE FINALIZA EL JUEGO 
void gameOver() {
    screen.fillScreen(ILI9341_RED); // Pantalla roja para indicar el fin del juego

    sonidoG_O (); //Llama a la función para cuando se acaba el juego

    screen.setTextColor(ILI9341_WHITE,  ILI9341_RED);
    screen.setTextSize(3);       
    screen.setCursor(XMAX / 2 - 70, YMAX / 2 - 20);
    screen.print("GAME OVER");

    screen.setTextSize(2);
    screen.setCursor(XMAX / 2 - 80, YMAX / 2 + 20);
    screen.print("PUNTAJE TOTAL:");
    screen.print(puntaje);

    //HACE UE NOS QUEDEMOS DETENIDOS 
    while (true);
}


