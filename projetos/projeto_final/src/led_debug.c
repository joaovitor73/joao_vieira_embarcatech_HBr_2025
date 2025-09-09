#include "include/led_debug.h" // Biblioteca de controle dos LEDs de debug

void setup_leds()
{
    gpio_init(LED_BLUE);              // Inicializa o pino do LED azul
    gpio_set_dir(LED_BLUE, GPIO_OUT); // Configura o pino do LED azul como saída
    gpio_init(LED_RED);               // Inicializa o pino do LED vermelho
    gpio_set_dir(LED_RED, GPIO_OUT);  // Configura o pino do LED vermelho como saída
}

void debug_envio()
{
    gpio_put(LED_BLUE, 1); // Acende o LED azul
}

void debug_wifi()
{
    gpio_put(LED_RED, 1); // Acende o LED vermelho
}

void reset_leds()
{
    gpio_put(LED_BLUE, 0); // Apaga o LED azul
    gpio_put(LED_RED, 0);  // Apaga o LED vermelho
}
