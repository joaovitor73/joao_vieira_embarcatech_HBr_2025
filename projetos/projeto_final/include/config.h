#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>           // Biblioteca padrão de entrada e saída
#include "pico/stdlib.h"     // Biblioteca padrão do Pico
#include "hardware/i2c.h"    // Biblioteca de comunicação I2C
#include "hardware/pwm.h"    // Biblioteca de controle de PWM
#include "hardware/timer.h"  // Biblioteca de timer
#include "hardware/clocks.h" // Biblioteca de clock
#include "pico/cyw43_arch.h" // Biblioteca de comunicação CYW43, para uso do WiFi
#include "hardware/gpio.h"   // Biblioteca de controle de GPIO
#include "inc/ssd1306.h"     // Biblioteca de controle do display

// Definição do tempo de debounce para os botões (evita múltiplas leituras por clique)
#define DEBOUNCE_TIME 200000

// Frequência padrão do buzzer
#define BUZZER_FREQUENCY 100

// Variáveis globais compartilhadas entre os módulos
extern volatile uint tempo;          // Tempo da sessão 60 segundos
extern volatile bool isSessao;       // Flag de sessão
extern volatile bool buttonPressed;  // Flag de botão pressionado
extern volatile bool stop_buzzer;    // Flag de parada do buzzer
extern volatile uint ciclos;         // Ciclos de respiração concluídos
extern volatile uint led_index;      // Índice do LED na matriz
extern volatile uint duracao_sessao; // Duração da sessão

// Definição dos pinos usados na aplicação
#define BTN_A 5       // Botão A
#define BUZZER_PIN 21 // Pino do buzzer
#define I2C_SDA 14    // Pino de dados I2C (display)
#define I2C_SCL 15    // Pino de clock I2C (display)

void reset_sessao(); // Reseta a sessão

#endif // CONFIG_H
