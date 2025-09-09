#include "include/buzzer.h" // Biblioteca de controle do buzzer
#include "include/config.h" // Biblioteca de configuração

void pwm_init_buzzer(uint pin)
{
    gpio_set_function(pin, GPIO_FUNC_PWM);        // Configura o pino do buzzer como PWM
    uint slice_num = pwm_gpio_to_slice_num(pin);  // Obtém o número do slice do PWM
    pwm_config config = pwm_get_default_config(); // Configuração padrão do PWM
    pwm_config_set_clkdiv(&config, 4.0f);         // Define o divisor de clock
    pwm_init(slice_num, &config, true);           // Inicializa o PWM
    pwm_set_gpio_level(pin, 0);                   // Desliga o buzzer
    printf("Buzzer inicializado\n");
}

void play_tone(uint pin, uint frequency, uint duration_ms)
{
    uint slice_num = pwm_gpio_to_slice_num(pin); // Obtém o número do slice do PWM
    uint32_t clock_freq = clock_get_hz(clk_sys); // Obtém a frequência do clock
    uint32_t top = clock_freq / frequency - 1;   // Calcula o valor de TOP, baseado na frequência, para gerar o tom desejado
    pwm_set_wrap(slice_num, top);                // Define o valor de TOP
    pwm_set_gpio_level(pin, top / 2);            // 50% de duty cycle

    uint32_t start_time = to_ms_since_boot(get_absolute_time()); // Tempo inicial
    // Aguarda o tempo de duração do tom, ou até que a flag de parada seja acionada com interrupção do botão
    while ((to_ms_since_boot(get_absolute_time()) - start_time < duration_ms) && !stop_buzzer)
    {
        sleep_ms(10); // Pequeno delay para reduzir consumo de CPU
    }
    pwm_set_gpio_level(pin, 0); // Para o buzzer
}