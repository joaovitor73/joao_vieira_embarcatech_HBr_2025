#include <stdio.h>                 // Biblioteca padrão de entrada e saída
#include "pico/stdlib.h"           // Biblioteca padrão do Pico
#include "hardware/pio.h"          // Biblioteca de comunicação PIO
#include "pico/cyw43_arch.h"       // Biblioteca de comunicação CYW43, para uso do WiFi
#include "include/buzzer.h"        // Biblioteca de controle do buzzer
#include "include/button.h"        // Biblioteca de controle dos botões
#include "include/timer.h"         // Biblioteca de controle do timer
#include "neo_pixel.h"             // Biblioteca de controle dos LEDs
#include "include/conexao_envio.h" // Biblioteca de controle da conexão e envio de dados
#include "include/led_debug.h"     // Biblioteca de controle dos LEDs de debug

volatile uint tempo = 60;               // Tempo da sessao 60 segundos
volatile bool isSessao = false;         // Flag de sessao
volatile bool buttonPressed = false;    // Flag de botao pressionado
volatile bool stop_buzzer = false;      // Flag de parada do buzzer
volatile uint ciclos = 0;               // Ciclos de respiracao concliudos
volatile bool desenhaInstrucao = false; // Flag de desenho de instrucao
volatile uint led_index = 0;            // Indice do LED na matriz
volatile uint duracao_sessao = -1;

int main()
{
    inicializa(); // Inicializa o sistema
    while (true)
    {
        reset_leds(); // Reseta os LEDs de debug
        // Verifica se a sessão está ativa
        if (send && duracao_sessao != -1)
        {
            // envia dados somente quando o temporizador permite
            debug_envio();
            send_dados();
            duracao_sessao = -1;
        }

        play_tone(BUZZER_PIN, 1000, 5000); // Toca um tom no buzzer
        sleep_ms(10);                      // Aguarda 100ms
    }
}

void inicializa()
{
    stdio_init_all();                                                               // Inicializa a comunicação serial
    setup_leds();                                                                   // Configura os LEDs de debug
    debug_wifi();                                                                   // Acende o LED de debug do Wi-Fi
    init_i2c();                                                                     // Inicializa o display
    reset_display();                                                                // Reseta o display
    desenha_tela_tentando_se_conectar();                                            // Desenha a tela de tentativa de conexão
    conecta_wifi();                                                                 // Conecta ao Wi-Fi
    ip_servidor();                                                                  // Resolve o IP do servidor
    add_repeating_timer_ms(1000, repeating_timer_callback_post, NULL, &timer_post); // temporizador para o envio de dados para o servidor
    setup_buttons();                                                                // Configura os botões
    pwm_init_buzzer(BUZZER_PIN);                                                    // Inicializa o buzzer
    desenha_tela_inicial();                                                         // Desenha a tela inicial
    setup_timer();                                                                  // Configura o timer
}
