#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

#define BTN_A_PIN 5

int A_state = 0;

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

void printar_texto_na_tela(char *textoLinha1, char *textoLinha2, char *textoLinha3, char *textoLinha4)
{

    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    ssd1306_draw_string(ssd, 15, 0, textoLinha1);
    ssd1306_draw_string(ssd, 15, 20, textoLinha2);
    ssd1306_draw_string(ssd, 15, 35, textoLinha3);
    ssd1306_draw_string(ssd, 15, 50, textoLinha4);
    render_on_display(ssd, &frame_area);
}

void SinalAberto()
{
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);
    printar_texto_na_tela("SINAL", "ABERTO ", "ATRAVESSAR", "COM CUIDADO");
}

void SinalAtencao()
{
    // gpio_put(LED_R_PIN, 1);
    // gpio_put(LED_G_PIN, 1);
    // gpio_put(LED_B_PIN, 0);
    printar_texto_na_tela("SINAL", "ATENCAO ", "PREPARE SE", "");
}

void SinalFechado()
{
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);
    printar_texto_na_tela("SINAL", "FECHADO ", "NAO ATRAVESSE", "");
}

int WaitWithRead(int timeMS)
{
    for (int i = 0; i < timeMS; i = i + 100)
    {
        A_state = !gpio_get(BTN_A_PIN);
        if (A_state == 1)
        {
            return 1;
        }
        sleep_ms(100);
    }
    return 0;
}

int main()
{
    stdio_init_all();

    // INICIANDO LEDS
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    // INICIANDO BOTÄO
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);

    // INICIANDO DISPLAY
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init();

    while (true)
    {
        SinalFechado();
        A_state = WaitWithRead(13000); // espera com leitura do botäo
        // sleep_ms(8000);

        if (A_state)
        { // ALGUEM APERTOU O BOTAO - SAI DO SEMAFORO NORMAL
            // SINAL AMARELO PARA OS CARROS POR 5s
            SinalAtencao();
            sleep_ms(5000);

            // SINAL VERMELHO PARA OS CARROS POR 10s
            SinalAberto();
            sleep_ms(8000);
        }
        else
        { // NINGUEM APERTOU O BOTAO - CONTINUA NO SEMAFORO NORMAL

            // SinalAtencao();
            // sleep_ms(2000);

            // SINAL VERMELHO PARA OS CARROS POR 15s
            SinalAberto();
            sleep_ms(8000);
        }
    }

    return 0;
}
