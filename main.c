#include "app_timer.h"
#include "boards.h"
#include "bsp.h"
#include "nordic_common.h"
#include "nrf_error.h"
#include "nrf_gpio.h"
#include "radio_config.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_radio.h"

static uint32_t packet; /**< Packet to transmit. */

void radio_init() {
  NRF_RADIO->TXPOWER =
      (RADIO_TXPOWER_TXPOWER_0dBm << RADIO_TXPOWER_TXPOWER_Pos);
  NRF_RADIO->FREQUENCY = 7UL; // Frequency bin 7, 2407MHz
  NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_1Mbit << RADIO_MODE_MODE_Pos);
  NRF_RADIO->PREFIX0 = 0xC3C28303;
  NRF_RADIO->PREFIX1 = 0xE3630023;

  NRF_RADIO->BASE0 = 0x80C4A2E6UL;
  NRF_RADIO->BASE1 = 0x91D5B3F7UL;
  NRF_RADIO->TXADDRESS =
      0x00UL; // Set device address 0 to use when transmitting
  NRF_RADIO->RXADDRESSES = 0x01UL; // Enable device address 0 to use to select
                                   // which addresses to receive
  // Packet configuration
  NRF_RADIO->PCNF0 =
      (0UL << RADIO_PCNF0_S1LEN_Pos) | (0UL << RADIO_PCNF0_S0LEN_Pos) |
      (0UL
       << RADIO_PCNF0_LFLEN_Pos); // lint !e845 "The right argument to operator
                                  // '|' is certain to be 0"

  // Packet configuration
  NRF_RADIO->PCNF1 =
      (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos) |
      (RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos) |
      (PACKET_BASE_ADDRESS_LENGTH << RADIO_PCNF1_BALEN_Pos) |
      (PACKET_STATIC_LENGTH << RADIO_PCNF1_STATLEN_Pos) |
      (PACKET_PAYLOAD_MAXSIZE
       << RADIO_PCNF1_MAXLEN_Pos); // lint !e845 "The right argument to operator
                                   // '|' is certain to be 0"
}

void send_packet() {
  packet = 12;
  NRF_RADIO->PACKETPTR = (uint32_t)&packet;
  NRF_RADIO->EVENTS_READY = 0;
  NRF_RADIO->TASKS_TXEN = 1;
  while (!NRF_RADIO->EVENTS_READY) {
    // wait
  }
  NRF_RADIO->EVENTS_END = 0;
  NRF_RADIO->TASKS_START = 1;
  while (!NRF_RADIO->EVENTS_END) {
    // wait
  }

  uint32_t err_code = bsp_indication_set(BSP_INDICATE_SENT_OK);
  NRF_LOG_INFO("The packet_conf was sent");
  APP_ERROR_CHECK(err_code);
  nrf_radio_event_clear(NRF_RADIO_EVENT_DISABLED);
  nrf_radio_task_trigger(NRF_RADIO_TASK_DISABLE);
  while (!nrf_radio_event_check(NRF_RADIO_EVENT_DISABLED)) {
    /* wait */
  }
}

void bsp_evt_handler(bsp_event_t evt) {
  uint32_t prep_packet = 0;
  switch (evt) {
  case BSP_EVENT_KEY_0:
    /* Fall through. */
  case BSP_EVENT_KEY_1:
    /* Fall through. */
  case BSP_EVENT_KEY_2:
    /* Fall through. */
  case BSP_EVENT_KEY_3:
    /* Fall through. */
  case BSP_EVENT_KEY_4:
    /* Fall through. */
  case BSP_EVENT_KEY_5:
    /* Fall through. */
  case BSP_EVENT_KEY_6:
    /* Fall through. */
  case BSP_EVENT_KEY_7:
    /* Get actual button state. */
    for (int i = 0; i < BUTTONS_NUMBER; i++) {
      prep_packet |= (bsp_board_button_state_get(i) ? (1 << i) : 0);
    }
    break;
  default:
    /* No implementation needed. */
    break;
  }
  packet = prep_packet;
}

/**@brief Function for initialization oscillators.
 */
void clock_initialization() {
  /* Start 16 MHz crystal oscillator */
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART = 1;

  /* Wait for the external oscillator to start up */
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
    // Do nothing.
  }

  /* Start low frequency crystal oscillator for app_timer(used by bsp)*/
  NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_LFCLKSTART = 1;

  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {
    // Do nothing.
  }
}

/**
 * @brief Function for application main entry.
 * @return 0. int return type required by ANSI/ISO standard.
 */
int main(void) {
  uint32_t err_code = NRF_SUCCESS;

  clock_initialization();

  err_code = app_timer_init();
  APP_ERROR_CHECK(err_code);

  err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_DEFAULT_BACKENDS_INIT();

  err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_evt_handler);
  APP_ERROR_CHECK(err_code);

  // Set radio configuration parameters
  radio_init();

  // Set payload pointer
  NRF_RADIO->PACKETPTR = (uint32_t)&packet;

  err_code = bsp_indication_set(BSP_INDICATE_USER_STATE_OFF);
  NRF_LOG_INFO("Radio transmitter example started.");
  NRF_LOG_INFO("Press Any Button");

  APP_ERROR_CHECK(err_code);

  while (true) {
    if (packet != 0) {
      send_packet();
      NRF_LOG_INFO("The contents of the package was %u", (unsigned int)packet);
      packet = 0;
    }
    NRF_LOG_FLUSH();
    __WFE();
  }
}
