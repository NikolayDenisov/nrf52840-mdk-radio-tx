#include "nrf52840.h"
#include "nrf52840_bitfields.h"
#include "nrf_delay.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Настройка радиопередатчика nRF52840 для передачи данных
void radio_init() {
  // Включение радиомодуля
  NRF_RADIO->POWER = RADIO_POWER_POWER_Enabled;

  // Настройка базовых параметров радио
  NRF_RADIO->MODE = RADIO_MODE_MODE_Ble_1Mbit; // Используем BLE с пропускной
                                               // способностью 1 Mbit

  // Настройка адреса передатчика
  NRF_RADIO->PREFIX0 = 0x8E;     // Префикс адреса, например 0x8E
  NRF_RADIO->BASE0 = 0x89ABCDEF; // Базовый адрес, например 0x89ABCDEF
  NRF_RADIO->TXADDRESS = 0;      // Используем адрес 0 для передачи
  NRF_RADIO->RXADDRESSES =
      RADIO_RXADDRESSES_ADDR0_Disabled; // Отключаем приемные адреса

  // Настройка каналов и частоты
  NRF_RADIO->FREQUENCY = 2; // Настраиваем частоту на 2402 MHz (канал 2)
  NRF_RADIO->DATAWHITEIV =
      37; // Инициализация переменной дециметрии данных на канал 37

  // Настройка пакетного конфигурации
  NRF_RADIO->PCNF0 =
      (1 << RADIO_PCNF0_S0LEN_Pos) |
      (8 << RADIO_PCNF0_LFLEN_Pos); // Длина поля S0 и поля длины данных
  NRF_RADIO->PCNF1 =
      (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos) |
      (RADIO_PCNF1_ENDIAN_Little << RADIO_PCNF1_ENDIAN_Pos) |
      (24 << RADIO_PCNF1_MAXLEN_Pos); // Макс. длина пакета данных 24 байта

  // Настройка адреса начала передачи
  NRF_RADIO->TASKS_TXEN = 1; // Включаем режим передачи
}

// Функция для отправки данных
void radio_send(uint8_t *data, uint8_t length) {
  NRF_RADIO->PACKETPTR = (uint32_t)data; // Указатель на данные для передачи

  NRF_RADIO->EVENTS_END = 0;  // Сброс события окончания передачи
  NRF_RADIO->TASKS_START = 1; // Запуск передачи

  // Ожидание окончания передачи
  while (!NRF_RADIO->EVENTS_END) {
    // Ждем завершения передачи
  }

  // Сброс события окончания передачи
  NRF_RADIO->EVENTS_END = 0;
}

// Основная функция
int main(void) {
  // Инициализация радиомодуля
  radio_init();

  // Данные для передачи
  uint8_t data[] = "Hello, nRF52840!";

  // Основной цикл программы
  while (1) {
    // Отправка данных
    radio_send(data, sizeof(data));

    // Задержка перед следующей передачей
    nrf_delay_ms(3000);
  }
}
