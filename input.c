#include <bcm2835.h>
#include "input.h"

void Input_Init(void) {
    bcm2835_gpio_fsel(KEY_UP, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(KEY_DOWN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(KEY_LEFT, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(KEY_RIGHT, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(KEY_PRESS, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(KEY_A, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(KEY_B, BCM2835_GPIO_FSEL_INPT);

    bcm2835_gpio_set_pud(KEY_UP, BCM2835_GPIO_PUD_UP);
    bcm2835_gpio_set_pud(KEY_DOWN, BCM2835_GPIO_PUD_UP);
    bcm2835_gpio_set_pud(KEY_LEFT, BCM2835_GPIO_PUD_UP);
    bcm2835_gpio_set_pud(KEY_RIGHT, BCM2835_GPIO_PUD_UP);
    bcm2835_gpio_set_pud(KEY_PRESS, BCM2835_GPIO_PUD_UP);
    bcm2835_gpio_set_pud(KEY_A, BCM2835_GPIO_PUD_UP);
    bcm2835_gpio_set_pud(KEY_B, BCM2835_GPIO_PUD_UP);
}

bool Input_IsPressed(uint8_t key) {
    return (bcm2835_gpio_lev(key) == LOW);
}