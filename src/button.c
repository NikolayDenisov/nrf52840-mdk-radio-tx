#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpio.h"
#include <stdbool.h>

#define BUTTON_PIN NRF_GPIO_PIN_MAP(1, 0)
#define DEBOUNCE_DELAY_MS 50

static volatile bool button_pressed = false;

void gpiote_event_handler(nrf_drv_gpiote_pin_t pin,
                          nrf_gpiote_polarity_t action) {
  nrf_delay_ms(DEBOUNCE_DELAY_MS);

  if (!nrf_gpio_pin_read(BUTTON_PIN)) {
    button_pressed = true;
  }
}

void button_init(void) {
  ret_code_t err_code;

  if (!nrf_drv_gpiote_is_init()) {
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
  }

  nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
  config.pull = NRF_GPIO_PIN_PULLUP;

  err_code = nrf_drv_gpiote_in_init(BUTTON_PIN, &config, gpiote_event_handler);
  APP_ERROR_CHECK(err_code);

  nrf_drv_gpiote_in_event_enable(BUTTON_PIN, true);
}

bool button_was_pressed(void) {
  if (button_pressed) {
    button_pressed = false;
    return true;
  }
  return false;
}
