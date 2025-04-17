#include "button.h"
#include "clock.h"
#include "radio_module.h"

#include "app_error.h"
#include "app_timer.h"
#include "bsp.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

int main(void) {
  uint32_t err_code;

  clock_initialization();

  err_code = app_timer_init();
  APP_ERROR_CHECK(err_code);

  err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  radio_init();
  button_init();

  NRF_LOG_INFO("Radio transmitter example started.");
  NRF_LOG_INFO("Press Button");

  uint32_t packet = 12;

  while (true) {
    if (button_was_pressed()) {
      NRF_LOG_INFO("Button was pressed");
      if (packet != 0) {
        send_packet(&packet);
        NRF_LOG_INFO("The contents of the package was %u",
                     (unsigned int)packet);
      }
    }

    NRF_LOG_FLUSH();
    __WFE();
  }
}
