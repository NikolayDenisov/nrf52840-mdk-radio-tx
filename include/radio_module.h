#ifndef RADIO_MODULE_H
#define RADIO_MODULE_H

#include <stdint.h>

void radio_init(void);
void send_packet(uint32_t *packet);

#endif