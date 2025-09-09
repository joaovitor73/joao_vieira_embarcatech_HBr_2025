#ifndef NEO_PIXEL_H
#define NEO_PIXEL_H

#include "pico/stdlib.h"     // Biblioteca padrão do Pico
#include "hardware/pio.h"    // Biblioteca de comunicação PIO
#include "hardware/clocks.h" // Biblioteca de controle de clock

#define LED_COUNT 4 // Quantidade de LEDs na contagem regressiva.
#define LED_PIN 7   // Pino de controle dos LEDs.

#define NUM_PIXELS 25 // Número de pixels na matriz

// Estrutura de pixel
typedef struct
{
    uint8_t G, R, B;
} pixel_t;

// Funções de controle
void npInit(uint pin);                                      // Inicializa a matriz de LEDs.
void npSetLED(uint index, uint8_t r, uint8_t g, uint8_t b); // Define a cor de um LED específico.
void npClear();                                             // Limpa todos os LEDs.
void npWrite();                                             // Envia os dados para os LEDs.
void npCountdown();                                         // Função para acender os LEDs da matriz de acordo com o tempo restante.

#endif // NEO_PIXEL_H
