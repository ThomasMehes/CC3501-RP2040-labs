#ifndef LEDS_H
#define LEDS_H

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include <vector>

class LEDs {
public:
    LEDs(uint pin, uint num_leds, PIO pio, uint sm);
    void setColor(uint led_index, uint8_t red, uint8_t green, uint8_t blue);
    void update();
    void clear();
private:
    uint _pin;
    PIO _pio;
    uint _sm;
    uint _num_leds;
    std::vector<uint32_t> _led_data;
    void _sendData();
};

#endif // LEDS_H
