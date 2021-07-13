
#include "ft_gpio.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char** argv) {
    if (!gpio_init()) {
        printf("Error: No permission to access GPIO. Please run this as root.\n");
        exit(1);
    }

    gpio_set_direction(GPIO_1, GPIO_IN);
    
    gpio_set_direction(GPIO_2, GPIO_OUT);
    gpio_set(GPIO_2, 0);

    for (int i = 1; i <= 8; i++) {
        printf("GPIO %d: value=%d  direction=%s\n", i, gpio_get(i),
            gpio_get_direction(i) == GPIO_IN ? "in " : "out");
    }

    gpio_exit();
    return 0;
}
