#pragma once
#include <Arduino.h>
#include <stdbool.h>
#include <stdint.h>
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"


class TPL5010{
  public:
    void begin(void);
    void kick_if_needed(void);
    void done_pulse(void);
  private:
};