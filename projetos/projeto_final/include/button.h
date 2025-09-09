#ifndef BUTTON_H
#define BUTTON_H

#include "pico/stdlib.h" // Biblioteca padrão do Pico

#define BTN_A 5              // Botão A
#define DEBOUNCE_TIME 200000 // Tempo de debounce para os botões (evita múltiplas leituras por clique), em microssegundos

void btns_callback(uint gpio, uint32_t events); // Função de callback para os botões
void setup_buttons();                           // Configura os botões

#endif