// /**
//  * @file esp_gpio.c
//  * @author Hamxa Islam {hamzaislam170@gamil.com}
//  * @brief Remotewell_PRO
//  * @version 0.1
//  * @date 2022-11-04
//  *
//  * @copyright Copyright (c) 2022
//  *
//  */
#include <driver/gpio.h>
#include "system_gpio.h"
#include "freertos_handler.h"

uint8_t PINS[PIN_ARRAY_SIZE][3] = {{TRIP_PIN,IN,PULLDOWN}, {RUN_PIN, IN, PULLDOWN}, {WIFI_LED_PIN, OUT, PULLDOWN}, {ON_RELAY_PIN, OUT, PULLUP}, {OFF_RELAY_PIN, OUT, PULLUP}, {VFD_RELAY_PIN, OUT, PULLUP}, {MULTI_BUTTON_PIN, IN, PULLUP}};

bool ConfigureGpio(e_gpio_config *config)
{
    esp_err_t err;

    /*First reset the pin>*/

    gpio_reset_pin(config->pin_num);

    /*Set the mode of Gpio*/

    err = config->mode ? gpio_set_direction(config->pin_num, GPIO_MODE_INPUT) : gpio_set_direction(config->pin_num, GPIO_MODE_OUTPUT);
    err = config->pull_state ? gpio_set_pull_mode(config->pin_num, GPIO_PULLUP_ONLY) : gpio_set_pull_mode(config->pin_num, GPIO_PULLDOWN_ONLY);

    /* going to set the default direction*/
    if(!config->mode)
    err = config->pull_state ? gpio_set_level(config->pin_num, true) : gpio_set_level(config->pin_num, false);

    return err;
}

void GpioInit(void *param)
{
    e_gpio_config gp_config = {0x00};

    for (uint8_t index = 0; index < PIN_ARRAY_SIZE; index++)
    {

        gp_config.pin_num = PINS[index][0];
        gp_config.mode = PINS[index][1];
        gp_config.pull_state = PINS[index][2];

        ConfigureGpio(&gp_config);
    }

    /*< Initilization for sleep pin >*/
}
