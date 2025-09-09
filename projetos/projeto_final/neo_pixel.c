#include "neo_pixel.h"          // Biblioteca de controle dos LEDs na matriz
#include "ws2818b.pio.h"        // Programa para controle dos LEDs
#include "include/config.h"     // Biblioteca de configuração
static PIO np_pio;              // Máquina PIO
static uint sm;                 // State machine, para controle do PIO
static pixel_t leds[LED_COUNT]; // Array de LEDs

/**
 * Inicializa a máquina PIO para controle da matriz de LEDs.
 */
void npInit(uint pin)
{
    uint offset = pio_add_program(pio0, &ws2818b_program); // Adiciona o programa ao PIO
    np_pio = pio0;                                         // Define o PIO 0 como padrão

    sm = pio_claim_unused_sm(np_pio, false); // Reivindica uma máquina de estado
    if (sm < 0)
    {
        np_pio = pio1;                          // Define o PIO 1 como padrão
        sm = pio_claim_unused_sm(np_pio, true); // Reivindica uma máquina de estado
    }

    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f); // Inicializa o programa
    npClear();                                               // Limpa todos os LEDs
}

/**
 * Define a cor de um LED específico.
 */
void npSetLED(uint index, uint8_t r, uint8_t g, uint8_t b)
{
    // Verifica se o índice está dentro do limite
    if (index < LED_COUNT)
    {
        // Define a cor do LED
        leds[index].R = r;
        leds[index].G = g;
        leds[index].B = b;
    }
}

/**
 * Limpa todos os LEDs.
 */
void npClear()
{
    for (uint i = 0; i < LED_COUNT; i++)
    {
        npSetLED(i, 0, 0, 0);
    }
    npWrite();
}

/**
 * Envia os dados para os LEDs.
 */
void npWrite()
{
    // Envia os dados para os LEDs
    for (uint i = 0; i < LED_COUNT; i++)
    {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
}

/**
 * Executa a contagem regressiva de 4 a 1 com LEDs verdes.
 */
void npCountdown()
{
    // Acende o LED verde correspondente ao índice e apaga o anteriro e o posterior
    npSetLED(led_index, 0, 255, 0); // Verde
    npSetLED(led_index - 1, 0, 0, 0);
    npSetLED(led_index + 1, 0, 0, 0);
    npWrite(); // Envia os dados para os LEDs
}
