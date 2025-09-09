#include "include/conexao_envio.h" // Biblioteca de controle da conexão e envio de dados
#include "include/config.h"        // Biblioteca de configuração
struct tcp_pcb *pcb;               // Conexão TCP
struct repeating_timer timer_post; // Temporizador para envio de dados
char request[200];                 // Requisição HTTP
const char *text_request = "GET /update.json?api_key=" API_KEY "&field1=%d HTTP/1.1\r\n"
                           "Host: " HOST "\r\n"
                           "Connection: close\r\n\r\n"; // Requisição HTTP

ip_addr_t server_ip; // IP do servidor
bool send = false;   // Flag de envio

// Função de callback do timer, chamada a cada 1 segundo
bool repeating_timer_callback_post(struct repeating_timer *t)
{
    send = true; // Habilita o envio de dados
    return true;
}

// Conectar ao Wi-Fi
void conecta_wifi()
{
    // Inicializa o módulo CYW43 para uso do Wi-Fi e tenta conectar à rede especificada
    if (!connect_wifi(SSID, PASSWORD))
    {
        printf("Falha ao conectar ao Wi-Fi\n");
        desenha_tela_falha_ao_se_conectar(); // Desenha a tela de falha ao se conectar no oled
    }
    else
    {
        printf("Conectado ao Wi-Fi\n");
        desenha_tela_tentando_se_conectar(); // Desenha a tela de tentativa de conexão no oled
    }
}

// Resolver o IP do servidor
void ip_servidor()
{
    resolve_name(HOST, &server_ip); // transformar o nome do host em um endereço IP
    printf("IP resolvido: %s\n", ipaddr_ntoa(&server_ip));
}

// Enviar os dados para o servidor
void send_dados()
{
    snprintf(request, sizeof(request), text_request, duracao_sessao); // Preenche a requisição com o valor da duração da sessão

    pcb = tcp_new();                    // Cria um novo PCB para a conexão TCP
    client_create(pcb, &server_ip, 80); // Cria e estabelece a conexão TCP com o servidor
    client_write(pcb, request);         // Envia a requisição para o servidor
    sleep_ms(100);                      // Aguarda 100ms
    client_close(pcb);                  // Fecha a conexão TCP
}
