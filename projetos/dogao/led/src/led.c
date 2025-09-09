#include "../include/led.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"

void init_led_pwm()
{
    uint slice;
    gpio_set_function(LED_R, GPIO_FUNC_PWM); 
    slice = pwm_gpio_to_slice_num(LED_R);  
    pwm_set_gpio_level(LED_R, led_level);   
    
    
    pwm_set_clkdiv(slice, DIVIDER_PWM);   
    pwm_set_wrap(slice, PERIOD);           
    pwm_set_enabled(slice, true);        
}

void onRedpwm(){
    pwm_set_gpio_level(LED_R, led_level);
}

void ofRedpwm(){
    pwm_set_gpio_level(LED_R, 0); 
}

void init_leds(){
    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_pull_up(LED_R);

    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_pull_up(LED_G);

    gpio_init(LED_B);
    gpio_set_dir(LED_B, GPIO_OUT);
    gpio_pull_up(LED_B);
}

void onRed(){
    gpio_put(LED_R, 1);
}

void offRed(){
    gpio_put(LED_R, 0);
}

void onGreen(){
    gpio_put(LED_G, 1);
}

void offGreen(){
    gpio_put(LED_G, 0);
}

void onBlue(){
    gpio_put(LED_B, 1);
}

void offBlue(){
    gpio_put(LED_B, 0);
}

