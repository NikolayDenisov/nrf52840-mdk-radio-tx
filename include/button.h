#ifndef BUTTONS_H
#define BUTTONS_H

#include "bsp.h"
#include <stdint.h>

void bsp_evt_handler(bsp_event_t evt);
bool button_was_pressed(void);
void button_init(void);

#endif