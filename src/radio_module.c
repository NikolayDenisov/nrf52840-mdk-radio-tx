#include "radio_module.h"
#include "app_error.h"
#include "bsp.h"
#include "nrf_log.h"
#include "nrf_radio.h"
#include "radio_config.h"

void radio_init(void) {
  NRF_RADIO->TXPOWER = 0UL;
  NRF_RADIO->FREQUENCY = 7UL;
  NRF_RADIO->MODE = 0UL;

  NRF_RADIO->BASE0 = 0x00000000;
  NRF_RADIO->PREFIX0 = 0xAA;

  NRF_RADIO->TXADDRESS = 0x00UL;

  NRF_RADIO->PCNF0 = 0x00000000;

  NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos) |
                     (RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos) |
                     (PACKET_BASE_ADDRESS_LENGTH << RADIO_PCNF1_BALEN_Pos) |
                     (PACKET_STATIC_LENGTH << RADIO_PCNF1_STATLEN_Pos) |
                     (PACKET_PAYLOAD_MAXSIZE << RADIO_PCNF1_MAXLEN_Pos);
}

void send_packet(uint32_t *packet) {
  NRF_RADIO->PACKETPTR = (uint32_t)packet;

  NRF_RADIO->EVENTS_READY = 0;
  NRF_RADIO->TASKS_TXEN = 1;
  while (!NRF_RADIO->EVENTS_READY) {
  }

  NRF_RADIO->EVENTS_END = 0;
  NRF_RADIO->TASKS_START = 1;
  while (!NRF_RADIO->EVENTS_END) {
  }

  uint32_t err_code = bsp_indication_set(BSP_INDICATE_SENT_OK);
  NRF_LOG_INFO("The packet_conf was sent");
  APP_ERROR_CHECK(err_code);

  NRF_RADIO->EVENTS_DISABLED = 0;
  NRF_RADIO->TASKS_DISABLE = 1;
  while (!NRF_RADIO->EVENTS_DISABLED) {
  }
}