
#include <stdint.h>

typedef enum _direction {
    GPIO_OUT = 0, GPIO_IN = 1
} direction_t;

typedef enum {
    GPIO_1 = 1,
    GPIO_2 = 2,
    GPIO_3 = 3,
    GPIO_4 = 4,
    GPIO_5 = 5,
    GPIO_6 = 6,
    GPIO_7 = 7,
    GPIO_8 = 8,
} gpio_pin;

uint8_t gpio_init();
void gpio_exit();
void gpio_set(gpio_pin gpionum, uint8_t set);
uint8_t gpio_get(gpio_pin gpionum);
direction_t gpio_get_direction(gpio_pin gpionum);
void gpio_set_direction(gpio_pin gpionum, direction_t dir);
