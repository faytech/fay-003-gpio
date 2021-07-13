# fay-003 GPIO usage

This code allows you to control and read the GPIOs (DIOs) of the faytech fay-003 mainboards N4200 and N3350. The GPIOs use a standard RJ45 connector. Voltage levels are 0 and 5V. When used as input, the GPIO pins have a positive pull-up. To switch them to off, simply connect to ground.

:warning: Please note the following caveats:
* The connector has no ground pin, so it has to be taken from e.g. the power plug or the serial port. Some devices are equipped with a backwards-compatible 10 pin connector that has a ground pin.
* There is no protection circuitry on the GPIO pins. The user must take care of appropriate ESD measures: Limit cable length, use ESD protection when touching the wires, protect from voltage spikes etc.

# API

We provide a simple API "as-is" for use on Linux. Include `ft_gpio.h` and link with `ft_gpio.c` to use the API.

## gpio_init
```c
uint8_t gpio_init();
```
Initializes the GPIOs. This must be called before any other GPIO function. When you are finished, call `gpio_exit()`.

## gpio_exit
```c
void gpio_exit();
```
Closes the GPIOs.

## gpio_set
```c
void gpio_set(gpio_pin gpionum, uint8_t value);
```
Sets a GPIO that is in output mode to a given value, 0 or 1. A value of 1 corresponds to 5 V.

## gpio_get
```c
uint8_t gpio_get(gpio_pin gpionum);
```
Gets the value of a GPIO. The GPIOs have a pull-up resistor and default to 1. If you connect a GPIO with ground, it will read 0.

## gpio_get_direction
```c
direction_t gpio_get_direction(gpio_pin gpionum);
```
Gets the direction of the GPIO, either `GPIO_IN` for reading or `GPIO_OUT` for switching.

## gpio_set_direction
```c
void gpio_set_direction(gpio_pin gpionum, direction_t dir);
```
Sets the direction of a GPIO.

# Example code

See `gpio.c` for an example:

```c
#include "ft_gpio.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char** argv) {
    if (!gpio_init()) {
        printf("Error: No permission to access GPIO. Please run this as root.\n");
        exit(1);
    }

    // Set GPIO 1 to input
    gpio_set_direction(GPIO_1, GPIO_IN);
    
    // Set GPIO 2 to output, 0 V
    gpio_set_direction(GPIO_2, GPIO_OUT);
    gpio_set(GPIO_2, 0);

    // Show all GPIOs
    for (int i = 1; i <= 8; i++) {
        printf("GPIO %d: value=%d  direction=%s\n", i, gpio_get(i),
            gpio_get_direction(i) == GPIO_IN ? "in " : "out");
    }

    gpio_exit();
    return 0;
}
```

Compile the example code with `make`, and run with `sudo ./gpio`. You can run it periodically with
```bash
sudo watch -d -n0.2 ./gpio
```
and if you connect GPIO 1 to ground, you will see it switches to 0.

# Low-Level
The fay-003 Mainboards contain a Nuvoton NCT6116D SuperIO chip that handles GPIOs, among other things. The GPIOs are connected as follows:

GPIO | 1    | 2    | 3    | 4    |  5   | 6    | 7    | 8
-----|------|------|------|------|------|------|------|------
Pin  | GP42 | GP41 | GP40 | GP37 | GP36 | GP35 | GP34 | GP33

The external GPIOs are using parts of GPIO banks 3 and 4 of the SuperIO chip, and can be controlled using IO ports. In order to access them, the following sequence has to be used:

```c
// enter extended function mode
// write 0x87 to port 0x2e twice
outb(0x87, 0x2e);
outb(0x87, 0x2e);

// select logical device 7: GPIO
outb(0x07, 0x2e);
outb(0x07, 0x2f);  // this 7 is the logical device

// read port 0xf1 - data for GPIO bank 4
outb(0xf1, 0x2e);
uint8_t ch = inb(0x2f);
// bit 1 is the state of GPIO3
uint8_t gpio3 = (ch & 0x01) > 0 

// setting GPIO3 to 5V:
ch = ch | 0x01;
outb(0xf1, 0x2e);
outb(ch, 0x2f);

// leave extended function mode
outb(0xaa, 0x2e);
```

The relevant addresses are:

GPIO bank | Direction | Data | GPIO to bit mapping
----------|-----------|------|---------------------
Bank 3    | 0xEC      | 0xED | `45678___`
Bank 4    | 0xF0      | 0xF1 | `_____123`

For example, the GPIO4 direction is in the most significant bit (7) of register `0xEC`, and data is in bit 7 of register `0xED`.