#include <stdio.h>            // Biblioteca padrão de entrada e saída
#include <string.h>           // Biblioteca de manipulação de strings
#include <stdlib.h>           // Biblioteca padrão do C
#include <ctype.h>            // Biblioteca de manipulação de caracteres
#include "pico/stdlib.h"      // Biblioteca padrão do Pico
#include "pico/binary_info.h" // Biblioteca de informações binárias
#include "inc/ssd1306.h"      // Biblioteca de controle do display
#include "hardware/i2c.h"     // Biblioteca de comunicação I2C
#include "include/config.h"   // Biblioteca de configuração

// Definição da área de renderização do display
struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

// Buffer de renderização do display
uint8_t ssd_buffer[ssd1306_buffer_length];

void init_i2c()
{
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);  // Inicializa o barramento I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Configura o pino SDA como I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Configura o pino SCL como I2C
    gpio_pull_up(I2C_SDA);                     // Habilita o pull-up no pino SDA
    gpio_pull_up(I2C_SCL);                     // Habilita o pull-up no pino SCL
    ssd1306_init();                            // Inicializa o display
    printf("Display inicializado\n");
}

void reset_display()
{
    calculate_render_area_buffer_length(&frame_area); // Calcula o tamanho do buffer de renderização
    memset(ssd_buffer, 0, ssd1306_buffer_length);     // Limpa o buffer de renderização
    render_on_display(ssd_buffer, &frame_area);       // Renderiza o buffer no display
}

void desenha_tela_inicial()
{
    reset_display();
    // Textos a serem exibidos na tela
    char titulo[] = "Monitor de";
    char subtitulo[] = "Respiracao";
    char instrucao[] = "Clique Btn A";
    char instrucao2[] = "para iniciar";
    // Desenha os textos na tela
    ssd1306_draw_string(&ssd_buffer, 25, 5, titulo);
    ssd1306_draw_string(&ssd_buffer, 24, 18, subtitulo);
    ssd1306_draw_string(&ssd_buffer, 15, 32, instrucao);
    ssd1306_draw_string(&ssd_buffer, 18, 50, instrucao2);
    // Renderiza o buffer no display
    render_on_display(&ssd_buffer, &frame_area);
}

void desenha_tela_monitor(int ciclos, char *mensagem)
{
    reset_display();
    // Textos a serem exibidos na tela
    char tempo_str[10];
    sprintf(tempo_str, "%ds", tempo);

    char ciclos_str[10];
    sprintf(ciclos_str, "%d", ciclos);
    // Desenha os textos na tela
    ssd1306_draw_string(&ssd_buffer, 95, 5, tempo_str);
    ssd1306_draw_string(&ssd_buffer, 5, 5, ciclos_str);
    ssd1306_draw_string(&ssd_buffer, 30, 30, mensagem);
    ssd1306_draw_string(&ssd_buffer, 45, 55, "Btn A Sair");
    // Renderiza o buffer no display
    render_on_display(&ssd_buffer, &frame_area);
}

void desenha_tela_final(bool venceu)
{
    reset_display();
    // Textos a serem exibidos na tela
    char titulo[] = "Fim da Sessao";
    char resultado[20];
    char instrucao[] = "Clique Btn A";
    char instrucao2[] = "para reiniciar";
    // Define o resultado da sessão
    if (venceu)
        strcpy(resultado, "Voce Venceu!");
    else // não foi desenvolvido a parte de perder
        strcpy(resultado, "Tente Novamente!");
    // Desenha os textos na tela
    ssd1306_draw_string(&ssd_buffer, 18, 5, titulo);
    ssd1306_draw_string(&ssd_buffer, 20, 20, resultado);
    ssd1306_draw_string(&ssd_buffer, 15, 35, instrucao);
    ssd1306_draw_string(&ssd_buffer, 18, 55, instrucao2);
    // Renderiza o buffer no display
    render_on_display(&ssd_buffer, &frame_area);
}

void desenha_tela_tentando_se_conectar()
{
    reset_display();
    ssd1306_draw_string(&ssd_buffer, 25, 15, "Conectando");
    ssd1306_draw_string(&ssd_buffer, 40, 25, "com o");
    ssd1306_draw_string(&ssd_buffer, 50, 45, "Wifi");
    render_on_display(&ssd_buffer, &frame_area);
}

void desenha_tela_falha_ao_se_conectar()
{
    reset_display();
    ssd1306_draw_string(&ssd_buffer, 45, 15, "Falha");
    ssd1306_draw_string(&ssd_buffer, 45, 25, "ao se");
    ssd1306_draw_string(&ssd_buffer, 40, 45, "Conectar");
    render_on_display(&ssd_buffer, &frame_area);
}