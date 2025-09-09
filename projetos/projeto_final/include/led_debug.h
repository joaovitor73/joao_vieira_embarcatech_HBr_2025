#ifndef LED_DEBUG_H
#define LED_DEBUG_H

#include "pico/stdlib.h"

#define LED_BLUE 12 // Pino do LED azul
#define LED_RED 13  // Pino do LED vermelho

void setup_leds();  // Configura os LEDs de debug
void debug_wifi();  // Acende o LED de debug do Wi-Fi
void debug_envio(); // Acende o LED de debug do envio de dados para a nuvem
void reset_leds();  // Reseta os LEDs de debug

#endif // LED_DEBUG_H
