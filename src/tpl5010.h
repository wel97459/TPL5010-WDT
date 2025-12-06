#pragma once

#include <Arduino.h>
#include <stdint.h>
#include "nrf_gpio.h"


class TPL5010 {
  public:
    /**
     * @brief Send a >100 µs pulse on DONE pin (TPL5010 requirement)
     *        300 µs gives plenty of margin and works with long traces/capacitance
     */
    void kick(void) const{
        digitalWrite(DONE_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(DONE_PIN, LOW);
    }

    /**
     * @brief Initialize GPIOs for TPL5010 watchdog
     *        Must be called very early (before USB, before SoftDevice, etc.)
     */
    void begin(void) const {
            pinMode(DONE_PIN, OUTPUT);
            digitalWrite(DONE_PIN, LOW);

            pinMode(WAKE_PIN, INPUT_PULLDOWN);
            
            // Enable SENSE on rising edge (TPL5010 pulls WAKE high when timeout)
            // This uses the nRF52's ultra-low-power latch mechanism
            NRF_GPIO->PIN_CNF[WAKE_PIN] &= ~GPIO_PIN_CNF_SENSE_Msk;
            NRF_GPIO->PIN_CNF[WAKE_PIN] |= (GPIO_PIN_CNF_SENSE_High << GPIO_PIN_CNF_SENSE_Pos);

            // Immediate kick on startup — prevents reset if timer was already running
            kick();
    }
    /**
     * @brief Check if TPL5010 needs to be kicked (WAKE pin latched)
     *        If so, kick it and re-arm the SENSE mechanism
     */
    uint8_t kick_if_needed(void) const {
          // Check if WAKE pin latch is set (TPL5010 is asking "are you alive?")
        if (NRF_GPIO->LATCH & (1UL << WAKE_PIN)) {
            // Clear the latch first — critical!
            NRF_GPIO->LATCH = (1UL << WAKE_PIN);

            // Send DONE pulse to TPL5010
            kick();

            // Re-arm the SENSE mechanism (most reliable method)
            NRF_GPIO->PIN_CNF[WAKE_PIN] &= ~GPIO_PIN_CNF_SENSE_Msk;
            NRF_GPIO->PIN_CNF[WAKE_PIN] |= (GPIO_PIN_CNF_SENSE_High << GPIO_PIN_CNF_SENSE_Pos);
            return 1;
        }
        return 0;
    }

};