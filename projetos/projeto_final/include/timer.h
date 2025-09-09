#ifndef TIMER_H
#define TIMER_H

#include "pico/stdlib.h" // Biblioteca padrão do Pico

bool repeating_timer_callback(struct repeating_timer *t); // Função de callback do temporizador
void setup_timer();                                       // Configura o temporizador

#endif