#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include "pins.h"

// GPIO digital representation
#define GPIO_HIGH true
#define GPIO_LOW false

void Board_GPIO_Init(void);

bool any_gate_fault(void);

// Digital Pins
void Board_Pin_Set(uint8_t led_gpio, uint8_t led_pin, bool state);
bool Board_Pin_Read(uint8_t gpio, uint8_t pin);
void Board_Pin_Toggle(uint8_t gpio, uint8_t pin);




#endif // ifndef _GPIO_H_