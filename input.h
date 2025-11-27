#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

#define KEY_UP      17
#define KEY_DOWN    22
#define KEY_LEFT    27
#define KEY_RIGHT   23
#define KEY_PRESS   4
#define KEY_A       5
#define KEY_B       6

void Input_Init(void);
bool Input_IsPressed(uint8_t key);

#endif