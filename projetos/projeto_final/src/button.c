#include "include/button.h" // Biblioteca de controle do botão
#include "include/config.h" // Biblioteca de configuração

uint32_t last_time = 0; // Último tempo de leitura do botão

// Função de callback para o botão
void btns_callback(uint gpio, uint32_t events)
{
    uint32_t current_time = to_us_since_boot(get_absolute_time()); // Tempo atual
    if (current_time - last_time > DEBOUNCE_TIME)
    {                      // Verifica se o tempo de debounce foi atingido
        if (gpio == BTN_A) // Verifica se o botão A foi pressionado
        {
            printf("Botão A acionado\n");
            isSessao = !isSessao;           // Inverte o estado da sessão
            buttonPressed = !buttonPressed; // Inverte o estado do botão pressionado
            stop_buzzer = !stop_buzzer;     // Inverte o estado do buzzer
            // Verifica se a sessão está ativa
            if (!isSessao)
            {
                desenha_tela_inicial();      // Desenha a tela inicial
                duracao_sessao = 60 - tempo; // Calcula a duração da sessão
                reset_sessao();              // Reseta os dados da sessão
            }
        }
        last_time = current_time; // Atualiza o tempo de leitura do botão
    }
}

void setup_buttons()
{
    gpio_init(BTN_A);                                                                   // Inicializa o botão A
    gpio_set_dir(BTN_A, GPIO_IN);                                                       // Configura o botão A como entrada
    gpio_pull_up(BTN_A);                                                                // Habilita o pull-up do botão A
    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_FALL, true, btns_callback); // Configura a interrupção para o botão A
    printf("Botões configurados\n");
}