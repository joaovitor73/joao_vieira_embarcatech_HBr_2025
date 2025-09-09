#ifndef CONEXAO_ENVIO_H
#define CONEXAO_ENVIO_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"

// Definições de rede
#define API_KEY "OBED9VOECZ8RH5XF" // Chave de API do ThingSpeak
#define HOST "api.thingspeak.com"  // Host do ThingSpeak
#define SSID "Infoway_Pedro"       // SSID da rede Wi-Fi
#define PASSWORD "Kupaki@j"        // Senha da rede Wi-Fi

// Variáveis globais (apenas da conexão)
extern struct tcp_pcb *pcb;               // Conexão TCP
extern struct repeating_timer timer_post; // Temporizador para envio de dados
extern char request[200];                 // Requisição HTTP
extern ip_addr_t server_ip;               // IP do servidor
extern bool send;                         // Flag de envio

// Protótipos das funções (apenas da conexão)
bool repeating_timer_callback_post(struct repeating_timer *t); // Função de callback do temporizador
void conecta_wifi();                                           // Conectar ao Wi-Fi
void ip_servidor();                                            // Resolver o IP do servidor
void send_dados();                                             // Enviar os dados para o servidor

#endif // CONEXAO_ENVIO_H
