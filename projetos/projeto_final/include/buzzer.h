#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdlib.h"     // Biblioteca padrão do Pico
#include "hardware/pwm.h"    // Biblioteca de controle de PWM
#include "hardware/clocks.h" // Biblioteca de controle de clock

#define BUZZER_PIN 21 // Pino do buzzer

void pwm_init_buzzer(uint pin);                             // Inicializa o buzzer
void play_tone(uint pin, uint frequency, uint duration_ms); // Toca um tom no buzzer

#endif