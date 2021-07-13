
#include "ft_gpio.h"
#include <sys/io.h>
#include <stdio.h>
#include <stdint.h>

static uint8_t gpio_initialized = 0;

typedef struct {
    uint8_t baseaddr;
    uint8_t bit;
} gpio_info;

gpio_info gpio_get_info(gpio_pin gpionum) {
    switch (gpionum) {
    case GPIO_1: return (gpio_info) {0xf0, 2}; // GP42
    case GPIO_2: return (gpio_info) {0xf0, 1}; // GP41
    case GPIO_3: return (gpio_info) {0xf0, 0}; // GP40
    case GPIO_4: return (gpio_info) {0xec, 7}; // GP37
    case GPIO_5: return (gpio_info) {0xec, 6}; // GP36
    case GPIO_6: return (gpio_info) {0xec, 5}; // GP35
    case GPIO_7: return (gpio_info) {0xec, 4}; // GP34
    case GPIO_8: return (gpio_info) {0xec, 3}; // GP33
    default:
        fprintf(stderr, "Error, invalid GPIO %d\n", gpionum);
        return (gpio_info) {0, 0};
    }
}

uint8_t ext_read(uint8_t addr) {
    outb(addr, 0x2e);
    return inb(0x2f);
}

void ext_write(uint8_t addr, uint8_t val) {
    outb(addr, 0x2e);
    outb(val, 0x2f);
}

uint8_t gpio_init() {
    if (ioperm(0x2e, 2, 1))
        return 0;

    // enter extended function mode
    outb(0x87, 0x2e);
    outb(0x87, 0x2e);

    // select logic device 7: GPIO
    ext_write(0x07, 0x07);

    gpio_initialized = 1;
    return 1;
}

uint8_t gpio_check_initialized() {
    if (!gpio_initialized) {
        fprintf(stderr, "Error, GPIO must be initialized by gpio_init.\n");
    }
    return gpio_initialized;
}

void gpio_exit() {
    if (!gpio_check_initialized()) return;

    outb(0xaa, 0x2e);
    ioperm(0x2e, 2, 0);

    gpio_initialized = 0;
}


void gpio_set(gpio_pin gpionum, uint8_t set) {
    gpio_info info = gpio_get_info(gpionum);
    if (!info.baseaddr) return;
    if (!gpio_check_initialized()) return;

    uint8_t val = ext_read(info.baseaddr+1);
    if (set)
        val = val | (1 << info.bit);
    else
        val = val & ~(1 << info.bit);
    ext_write(info.baseaddr+1, val);
}

uint8_t gpio_get(gpio_pin gpionum) {
    gpio_info info = gpio_get_info(gpionum);
    if (!info.baseaddr) return 0;
    if (!gpio_check_initialized()) return 0;

    uint8_t val = ext_read(info.baseaddr+1);
    return (val & (1 << info.bit)) > 0;
}

direction_t gpio_get_direction(gpio_pin gpionum) {
    gpio_info info = gpio_get_info(gpionum);
    if (!info.baseaddr) return GPIO_OUT;
    if (!gpio_check_initialized()) return GPIO_OUT;

    uint8_t val = ext_read(info.baseaddr+0);
    return (val & (1 << info.bit)) > 0;
}

void gpio_set_direction(gpio_pin gpionum, direction_t dir) {
    gpio_info info = gpio_get_info(gpionum);
    if (!info.baseaddr) return;
    if (!gpio_check_initialized()) return;

    uint8_t val = ext_read(info.baseaddr+0);
    if (dir == GPIO_IN)
        val = val | (1 << info.bit);
    else
        val = val & ~(1 << info.bit);
    ext_write(info.baseaddr+0, val);
}
