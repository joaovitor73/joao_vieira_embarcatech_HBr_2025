#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h" // Biblioteca para manipulação do ADC no RP2040
#include "hardware/pwm.h" // Biblioteca para controle de PWM no RP2040
#include "hardware/clocks.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"

#define I2C_PORT i2c0
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
#define BUZZER_PIN 21

// Definição dos pinos usados para o joystick e LEDs
const int VRX = 26;          // Pino de leitura do eixo X do joystick (conectado ao ADC)
const int VRY = 27;          // Pino de leitura do eixo Y do joystick (conectado ao ADC)
const int ADC_CHANNEL_0 = 0; // Canal ADC para o eixo X do joystick
const int ADC_CHANNEL_1 = 1; // Canal ADC para o eixo Y do joystick
const int SW = 22;           // Pino de leitura do botão do joystick

const int LED_B = 13; // Pino para controle do LED azul via PWM
const int LED_R = 11;
const uint LED = 12;                     // Pino para controle do LED vermelho via PWM
const float DIVIDER_PWM = 16.0;          // Divisor fracional do clock para o PWM
const uint16_t PERIOD = 4096;            // Período do PWM (valor máximo do contador)
uint16_t led_b_level, led_r_level = 100; // Inicialização dos níveis de PWM para os LEDs
uint slice_led_b, slice_led_r;           // Variáveis para armazenar os slices de PWM correspondentes aos LEDs

const uint16_t PERIODLED = 2000; // Período do PWM (valor máximo do contador)
const uint16_t LED_STEP = 100;   // Passo de incremento/decremento para o duty cycle do LED
uint16_t led_level = 100;

const uint star_wars_notes[] = {
    330, 330, 330, 262, 392, 523, 330, 262,
    392, 523, 330, 659, 659, 659, 698, 523,
    415, 349, 330, 262, 392, 523, 330, 262,
    392, 523, 330, 659, 659, 659, 698, 523,
    415, 349, 330, 523, 494, 440, 392, 330,
    659, 784, 659, 523, 494, 440, 392, 330,
    659, 659, 330, 784, 880, 698, 784, 659,
    523, 494, 440, 392, 659, 784, 659, 523,
    494, 440, 392, 330, 659, 523, 659, 262,
    330, 294, 247, 262, 220, 262, 330, 262,
    330, 294, 247, 262, 330, 392, 523, 440,
    349, 330, 659, 784, 659, 523, 494, 440,
    392, 659, 784, 659, 523, 494, 440, 392};

// Duração das notas em milissegundos
const uint note_duration[] = {
    500,
    500,
    500,
    350,
    150,
    300,
    500,
    350,
    150,
    300,
    500,
    500,
    500,
    500,
    350,
    150,
    300,
    500,
    500,
    350,
    150,
    300,
    500,
    350,
    150,
    300,
    650,
    500,
    150,
    300,
    500,
    350,
    150,
    300,
    500,
    150,
    300,
    500,
    350,
    150,
    300,
    650,
    500,
    350,
    150,
    300,
    500,
    350,
    150,
    300,
    500,
    500,
    500,
    500,
    350,
    150,
    300,
    500,
    500,
    350,
    150,
    300,
    500,
    350,
    150,
    300,
    500,
    350,
    150,
    300,
    500,
    500,
    350,
    150,
    300,
    500,
    500,
    350,
    150,
    300,
};

static volatile bool persistirMenu = true;
int opcao = 0;
uint up_down = 1;

struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

void setup_display()
{
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    calculate_render_area_buffer_length(&frame_area);
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
    menu();
}

void setup_pwm_LED()
{
    uint slice;
    gpio_set_function(LED, GPIO_FUNC_PWM); // Configura o pino do LED para função PWM
    slice = pwm_gpio_to_slice_num(LED);    // Obtém o slice do PWM associado ao pino do LED
    pwm_set_clkdiv(slice, DIVIDER_PWM);    // Define o divisor de clock do PWM
    pwm_set_wrap(slice, PERIODLED);        // Configura o valor máximo do contador (período do PWM)
    pwm_set_gpio_level(LED, 0);            // Define o nível inicial do PWM para o pino do LED
    pwm_set_enabled(slice, true);          // Habilita o PWM no slice correspondente
}

// Inicializa o PWM no pino do buzzer
void pwm_init_buzzer(uint pin)
{
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f); // Ajusta divisor de clock
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0); // Desliga o PWM inicialmente
}

// Toca uma nota com a frequência e duração especificadas
void play_tone(uint pin, uint frequency, uint duration_ms)
{
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t top = clock_freq / frequency - 1;

    pwm_set_wrap(slice_num, top);
    pwm_set_gpio_level(pin, top / 2); // 50% de duty cycle

    sleep_ms(duration_ms);

    pwm_set_gpio_level(pin, 0); // Desliga o som após a duração
    sleep_ms(50);               // Pausa entre notas
}

// Função principal para tocar a música
void play_star_wars(uint pin)
{
    for (int i = 0; i < sizeof(star_wars_notes) / sizeof(star_wars_notes[0]); i++)
    {
        if (persistirMenu)
        {
            break;
        }
        if (star_wars_notes[i] == 0)
        {
            sleep_ms(note_duration[i]);
        }
        else
        {
            play_tone(pin, star_wars_notes[i], note_duration[i]);
        }
    }
}

// Função para configurar o joystick (pinos de leitura e ADC)
void setup_joystick()
{
    // Inicializa o ADC e os pinos de entrada analógica
    adc_init();         // Inicializa o módulo ADC
    adc_gpio_init(VRX); // Configura o pino VRX (eixo X) para entrada ADC
    adc_gpio_init(VRY); // Configura o pino VRY (eixo Y) para entrada ADC

    // Inicializa o pino do botão do joystick
    gpio_init(SW);             // Inicializa o pino do botão
    gpio_set_dir(SW, GPIO_IN); // Configura o pino do botão como entrada
    gpio_pull_up(SW);          // Ativa o pull-up no pino do botão para evitar flutuações
}

// Função para configurar o PWM de um LED (genérica para azul e vermelho)
void setup_pwm_led(uint led, uint *slice, uint16_t level)
{
    gpio_set_function(led, GPIO_FUNC_PWM); // Configura o pino do LED como saída PWM
    *slice = pwm_gpio_to_slice_num(led);   // Obtém o slice do PWM associado ao pino do LED
    pwm_set_clkdiv(*slice, DIVIDER_PWM);   // Define o divisor de clock do PWM
    pwm_set_wrap(*slice, PERIOD);          // Configura o valor máximo do contador (período do PWM)
    pwm_set_gpio_level(led, level);        // Define o nível inicial do PWM para o LED
    pwm_set_enabled(*slice, true);         // Habilita o PWM no slice correspondente ao LED
}

void setup()
{
    stdio_init_all();                      // Inicializa a porta serial para saída de dados
    setup_joystick();                      // Chama a função de configuração do joystick
    setup_pwm_led(LED_B, &slice_led_b, 0); // Configura o PWM para o LED azul
    setup_pwm_led(LED_R, &slice_led_r, 0); // Configura o PWM para o LED vermelho
    pwm_init_buzzer(BUZZER_PIN);           // Inicializa o PWM para o buzzer
    setup_pwm_LED();                       // Inicializa o PWM para o LED
    setup_display();                       // Inicializa o display OLED
}

void menu()
{
    switch (opcao)
    {
    case 0:
        cursor_display(true, false, false);
        break;
    case 1:
        cursor_display(false, true, false);
        break;
    case 2:
        cursor_display(false, false, true);
        break;
    }
}

void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value)
{
    // Leitura do valor do eixo X do joystick
    adc_select_input(ADC_CHANNEL_0); // Seleciona o canal ADC para o eixo X
    sleep_us(2);                     // Pequeno delay para estabilidade
    *vrx_value = adc_read();         // Lê o valor do eixo X (0-4095)

    // Leitura do valor do eixo Y do joystick
    adc_select_input(ADC_CHANNEL_1); // Seleciona o canal ADC para o eixo Y
    sleep_us(2);                     // Pequeno delay para estabilidade
    *vry_value = adc_read();         // Lê o valor do eixo Y (0-4095)
}

void verifica_movimento_eixo_x(uint16_t vrx_value)
{
    if (vrx_value < 300)
    {
        if (opcao < 2)
        {
            opcao++;
        }
        else
        {
            opcao = 0;
        }
        menu();
    }
    else if (vrx_value > 3500)
    {
        if (opcao > 0)
        {
            opcao--;
        }
        else
        {
            opcao = 2;
        }
        menu();
    }
}

#define DEBOUNCE_DELAY 200 // Tempo em milissegundos

volatile uint32_t last_press_time = 0;

static void joystick_pressed(uint gpio, uint32_t events)
{
    uint32_t current_time = time_us_32() / 1000; // Converte para milissegundos

    if (current_time - last_press_time > DEBOUNCE_DELAY)
    { // Verifica o tempo mínimo entre ativação
        printf("Botão pressionado!\n");
        persistirMenu = !persistirMenu;
        if (persistirMenu)
        {
            menu();
        }
        last_press_time = current_time; // Atualiza o tempo do último acionamento
    }
}

void led_pwm()
{
    pwm_set_gpio_level(LED, led_level); // Define o nível atual do PWM (duty cycle)
    sleep_ms(1000);
    if (up_down)
    {
        led_level += LED_STEP; // Incrementa o nível do LED
        if (led_level >= PERIOD)
            up_down = 0; // Muda direção para diminuir quando atingir o período máximo
    }
    else
    {
        led_level -= LED_STEP; // Decrementa o nível do LED
        if (led_level <= LED_STEP)
            up_down = 1; // Muda direção para aumentar quando atingir o mínimo
    }
}

void cursor_display(bool op1, bool op2, bool op3)
{
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
    char *text[] = {
        "  1- Joystick  ",
        "  2- Buzzer   ",
        "  3- LED  "};
    ssd1306_draw_line(ssd, 0, 0, 127, 0, op1);
    ssd1306_draw_line(ssd, 0, 0, 0, 20, op1);
    ssd1306_draw_line(ssd, 127, 0, 127, 20, op1);
    ssd1306_draw_line(ssd, 0, 20, 127, 20, op1);
    ssd1306_draw_string(ssd, 2, 8, text[0]);

    ssd1306_draw_line(ssd, 0, 22, 127, 22, op2);
    ssd1306_draw_line(ssd, 0, 22, 0, 40, op2);
    ssd1306_draw_line(ssd, 127, 22, 127, 40, op2);
    ssd1306_draw_line(ssd, 0, 40, 127, 40, op2);
    ssd1306_draw_string(ssd, 5, 32, text[1]);

    ssd1306_draw_line(ssd, 0, 42, 127, 42, op3);
    ssd1306_draw_line(ssd, 0, 42, 0, 60, op3);
    ssd1306_draw_line(ssd, 127, 42, 127, 60, op3);
    ssd1306_draw_line(ssd, 0, 60, 127, 60, op3);
    ssd1306_draw_string(ssd, 5, 49, text[2]);

    render_on_display(ssd, &frame_area);
}

void switch_mode(uint16_t vrx_value, uint16_t vry_value)
{
    if (persistirMenu)
    {
        verifica_movimento_eixo_x(vrx_value);
        printf("Opção selecionada: %d\n", opcao);
        pwm_set_gpio_level(LED_B, 0);
        pwm_set_gpio_level(LED_R, 0);
        pwm_set_gpio_level(LED, 0);
    }
    else
    {
        uint8_t ssd[ssd1306_buffer_length];
        memset(ssd, 0, ssd1306_buffer_length);
        render_on_display(ssd, &frame_area);
        switch (opcao)
        {
        case 0:
            pwm_set_gpio_level(LED_B, vrx_value); // Ajusta o brilho do LED azul com o valor do eixo X
            pwm_set_gpio_level(LED_R, vry_value); // Ajusta o brilho do LED vermelho com o valor do eixo Y
            break;
        case 1:
            play_star_wars(BUZZER_PIN);
            break;
        case 2:
            led_pwm();
            break;
        }
    }
}

int main()
{
    uint16_t vrx_value, vry_value, sw_value; // Variáveis para armazenar os valores do joystick (eixos X e Y) e botão
    setup();
    gpio_set_irq_enabled_with_callback(SW, GPIO_IRQ_EDGE_FALL, true, &joystick_pressed);
    while (true)
    {

        joystick_read_axis(&vrx_value, &vry_value); // Lê os valores dos eixos do joystick
        // Ajusta os níveis PWM dos LEDs de acordo com os valores do joystick           // Verifica se o botão do joystick foi pressionado
        switch_mode(vrx_value, vry_value); // Alterna entre os modos de operação do sistema
        // Pequeno delay antes da próxima leitura
        sleep_ms(100); // Espera 100 ms antes de repetir o ciclo
    }
}
