#include <stdio.h>
#include "pico/stdlib.h"
#include "neo/include/neopixel.h"
#include "display/include/display.h"
#include "buzzer/include/buzzer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "joystick/include/joystick.h"
#include "button/include/button.h"
#include "rede/mqtt/include/mqtt.h"
#include "rede/wifi/include/wifi.h"
#include "led/include/led.h"
#include "event_groups.h"
#include "hardware/watchdog.h"

uint cont = 0;
char text_buffer[100];
uint16_t vrx_value, vry_value;
bool caiu = false;
bool flag = true, pressedValue = false;
uint npOne[] = {0,0,240};
uint npTwo[] = {0,240,0};
uint npTree[] = {240,0,0};
 
bool conected_wifi = false;
bool conected_mqtt = false;
typedef enum {
    STATE_WIFI_DISCONNECTED,
    STATE_WIFI_CONNECTED
} wifi_connection_state_t;

typedef enum {
    STATE_MQTT_DISCONNECTED,
    STATE_MQTT_CONNECTED
} mqtt_connection_state_t;

EventGroupHandle_t xEventGroupWifi, xEventGroupMqtt;
#define WIFI_CONNECTED_BIT (1 << 0)
#define MQTT_CONNECTED_BIT (1 << 0)

void vNpTask() {
    while (true) {
        xEventGroupWaitBits(xEventGroupMqtt, MQTT_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
        while (conected_mqtt) {
            npSetLED(cont,npOne[0], npOne[1],npOne[2]);
            npSetLED(cont+1,npTwo[0], npTwo[1], npTwo[2]);
            npSetLED(cont+2,npTree[0], npTree[1], npTree[2]);
            npWrite();
            vTaskDelay(250/portTICK_PERIOD_MS);
            npClear();
            vTaskDelay(250/portTICK_PERIOD_MS);
        }
        npClear();
        npWrite();
    }
}

void vDpTask(){
     while(true){
        dpWriteCenter(text_buffer);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void vBuzzerTask(){
    while(true){
        xEventGroupWaitBits(xEventGroupMqtt, MQTT_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
        while (conected_mqtt) {
            if(!pressedValue)
                beep(200);
            vTaskDelay(250/portTICK_PERIOD_MS);
        }
    }
}

void vContTask(){
    while(true){
        xEventGroupWaitBits(xEventGroupMqtt, MQTT_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
        while (conected_mqtt) {
            if(flag) cont++;
            vTaskDelay(500/portTICK_PERIOD_MS);
            if(cont == 26) cont = 0;
            sprintf(text_buffer, "%u", cont); 
        }
        cont = 0;
    }
}

void vJoystickTask(){
    while(true){
        xEventGroupWaitBits(xEventGroupMqtt, MQTT_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
        while (conected_mqtt) {
            joystick_read_axis(&vrx_value, &vry_value);
            if (vrx_value < 300){
                if (cont > 0) cont--;
                else cont = 25;
                flag = false;
            }else if (vrx_value > 3500){
                if (cont < 25) cont++;
                else cont = 0;
                flag = false;
            }else{
                flag = true;
            }
            vTaskDelay(500/portTICK_PERIOD_MS);
        }
    }
}

void vButtonTask(){
    while(true){
        xEventGroupWaitBits(xEventGroupMqtt, MQTT_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
        while (conected_mqtt) {
            if(pressedValue){
                npOne[0] = 240; npOne[1] = 0; npOne[2] = 0;
                npTree[0] = 0; npTree[1] = 0; npTree[2] = 240;
                npClear();
            }else{
                npOne[0] = 0; npOne[1] = 0; npOne[2] = 240;
                npTree[0] = 240; npTree[1] = 0; npTree[2] = 0;
                npClear();
            }
            vTaskDelay(250/portTICK_PERIOD_MS);
        }
    }
}

void vSendValueCont(){
    while(true){
        xEventGroupWaitBits(xEventGroupMqtt, MQTT_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
        while (conected_mqtt) {
            mqtt_comm_publish("cont/value", text_buffer, strlen(text_buffer));
            vTaskDelay(500/portTICK_PERIOD_MS);
        }
    }
}

void vConectWifi(void *pvParameters) {
    init_wifi(text_buffer);
    wifi_connection_state_t currentState = STATE_WIFI_DISCONNECTED;
    while (true) {
        if(currentState == STATE_WIFI_DISCONNECTED){
             onRed();
            if(connect_to_wifi("ssid", "password", text_buffer)){
                currentState = STATE_WIFI_CONNECTED;
                conected_wifi = true;
                xEventGroupSetBits(xEventGroupWifi, WIFI_CONNECTED_BIT);
                caiu = false;
            }else{
                vTaskDelay(pdMS_TO_TICKS(5000)); 
            }
        }else{
            conected_wifi = is_connected();
            if (conected_wifi) {  
                offRed(); 
                vTaskDelay(pdMS_TO_TICKS(1000));
            } else {
                currentState = STATE_WIFI_DISCONNECTED;
                xEventGroupClearBits(xEventGroupWifi, WIFI_CONNECTED_BIT);
                xEventGroupClearBits(xEventGroupMqtt, MQTT_CONNECTED_BIT);
                conected_mqtt = false;
                caiu = true;
            }
        }
    }
}

void vConectMqtt(void * pvParameters){
    mqtt_connection_state_t currentState = STATE_MQTT_DISCONNECTED;
    while(true){
        xEventGroupWaitBits(xEventGroupWifi, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
        offRed();
        while (conected_wifi) {
            if(currentState == STATE_MQTT_DISCONNECTED){
                onGreen();
                mqtt_setup("bitdog5", "ip", "aluno", "senha123", text_buffer);

                int tentativas = 0;
                while (!conected_mqtt && tentativas < 50) {
                    vTaskDelay(pdMS_TO_TICKS(100));
                    tentativas++;
                }

                conected_mqtt = mqtt_is_connected();
                if(conected_mqtt){
                    currentState = STATE_MQTT_CONNECTED;
                    xEventGroupSetBits(xEventGroupMqtt, MQTT_CONNECTED_BIT);
                }else{
                    vTaskDelay(pdMS_TO_TICKS(5000));
                }
            }else{       
                offGreen();   
                conected_mqtt = mqtt_is_connected();
                if (conected_mqtt) {
                    vTaskDelay(pdMS_TO_TICKS(1000));
                } else {
                    currentState = STATE_MQTT_DISCONNECTED;
                    xEventGroupClearBits(xEventGroupMqtt, MQTT_CONNECTED_BIT);
                }
            }
        }
        if (!conected_wifi) {
            currentState = STATE_MQTT_DISCONNECTED;
            xEventGroupClearBits(xEventGroupMqtt, MQTT_CONNECTED_BIT);
            conected_mqtt = false;
            continue;
        }
    }
}

void vStatusLed(){
    while(true){
        xEventGroupWaitBits(xEventGroupMqtt, MQTT_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
        while (conected_mqtt) {
            onBlue();
            vTaskDelay(50 / portTICK_PERIOD_MS );
            offBlue();
            vTaskDelay(950 / portTICK_PERIOD_MS);
        }
    }
}


void vWatchdogTask() {
    while (true) {
        if (!caiu) {
            watchdog_update();
        }
        vTaskDelay(pdMS_TO_TICKS(500)); // alimentar o watchdog a cada 500ms
    }
}


int main() {
    stdio_init_all();
    TaskHandle_t wifi_handle, display_handle, cont_handle, buzzy_handle, joystick_handle, neo_handle, button_handle, led_status_handle, send_value_handle, mqtt_handle, watchdog_handle;

    xEventGroupWifi = xEventGroupCreate();
    xEventGroupMqtt = xEventGroupCreate();

    init_leds();
    npInit();
    dpInit();
    pwm_init_buzzer();
    init_joystick();
    init_buttons(&pressedValue);

    watchdog_enable(4000, 1);

    xTaskCreate(vDpTask, "Dp task", 2048 ,NULL, 1, &display_handle);
    xTaskCreate(vConectWifi, "Connect to wifi", 8100, NULL,1, &wifi_handle);
    xTaskCreate(vNpTask, "Np task", 128, NULL, 1, &neo_handle);
    xTaskCreate(vBuzzerTask, "Buzzer task", 128, NULL, 1, &buzzy_handle);
    xTaskCreate(vContTask, "Cont task", 128, NULL, 1, &cont_handle);
    xTaskCreate(vJoystickTask, "Joystick task", 128, NULL, 1, &joystick_handle);
    xTaskCreate(vButtonTask, "Button task", 128, NULL, 1, &button_handle);
    xTaskCreate(vSendValueCont, "Send value cont", 528, NULL, 1, &send_value_handle);
    xTaskCreate(vConectMqtt, "Conect Mqtt", 8100, NULL, 1, &mqtt_handle);
    xTaskCreate(vStatusLed, "Status led", 64, NULL, 1, &led_status_handle);
    xTaskCreate(vWatchdogTask, "Watchdog task", 64, NULL, 1, &watchdog_handle);


    vTaskCoreAffinitySet(mqtt_handle, (1 << 0));
    vTaskCoreAffinitySet(wifi_handle, (1 << 0));  // WiFi on core 0
    vTaskCoreAffinitySet(display_handle, (1 << 1)); // Display on core 1
    vTaskCoreAffinitySet(cont_handle, (1 << 1));
    vTaskCoreAffinitySet(buzzy_handle, (1 << 1));
    vTaskCoreAffinitySet(joystick_handle, (1 << 1));
    vTaskCoreAffinitySet(button_handle, (1 << 1));
    vTaskCoreAffinitySet(neo_handle, (1 << 1));
    vTaskCoreAffinitySet(send_value_handle, (1 << 1));
    vTaskCoreAffinitySet(led_status_handle, (1 << 1));
    vTaskCoreAffinitySet(watchdog_handle, (1 << 1));
   
    vTaskStartScheduler();
    while (true) {}
}
