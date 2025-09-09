#include "include/config.h" // Biblioteca de configuração

void reset_sessao()
{
    tempo = 60;            // Tempo da sessão 60 segundos
    isSessao = false;      // Flag de sessão
    buttonPressed = false; // Flag de botão pressionado
    stop_buzzer = false;   // Flag de parada do buzzer
    ciclos = 0;            // Ciclos de respiração concluídos
    led_index = 0;         // Índice do LED na matriz
    printf("Sessão resetada\n");
}