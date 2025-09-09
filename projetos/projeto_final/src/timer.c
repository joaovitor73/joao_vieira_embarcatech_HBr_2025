#include "include/timer.h" // Biblioteca de controle do timer
#include "include/config.h" // Biblioteca de configuração
#include "../neo_pixel.h"  // Biblioteca de controle dos LEDs na matriz

struct repeating_timer timer; // Temporizador para a contagem regressiva

bool repeating_timer_callback(struct repeating_timer *t) 
{
    // Verifica se a sessão está ativa
    if (isSessao)
    {
        ciclos = (60 - tempo) / 12; // Calcula os ciclos de respiração, 1 ciclo a cada 12 segundos

        // Define a fase correta com base no tempo restante
        int fase = ((60 - tempo) % 12) / 4; // 0 = Inspire, 1 = Segure, 2 = Expire
        const char *mensagem;               // Apaga tudo após a contagem
        // Define a mensagem correta com base na fase
        if (fase == 0)
        {
            led_index += 1; // Indice usado para acender e apaagar leds na matriz
            mensagem = "Inspire...";
        }
        else if (fase == 1)
        {
            npClear();
            mensagem = "Segure...";
        }
        else
        {
            led_index -= 1; // Indice usado para acender e apaagar leds na matriz
            mensagem = "Expire...";
        }
        // Verifica se a sessão terminou
        if (tempo <= 0)
        {
            desenha_tela_final(true); // Desenha a tela final , vitoria 
            npClear(); // Apaga todos os leds da matriz
            return true; // Retorna true para finalizar a sessão
        }
        else
        {
            desenha_tela_monitor(ciclos, mensagem); // Desenha a tela de monitoramento
        }
        npCountdown(); // função usada para acender os leds da matriz
        tempo = tempo - 1; // Decrementa o tempo restante
        return true; // Retorna true para continuar a sessão
    }
    else
    {
        npClear(); // Apaga todos os leds da matriz
    }
}

void setup_timer()
{
    add_repeating_timer_ms(1000, repeating_timer_callback, NULL, &timer); // Configura o temporizador para 1 segundo
    printf("Timer configurado\n");
    npInit(LED_PIN); // Inicializa a matriz de LEDs
}