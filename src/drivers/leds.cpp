#include "leds.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"


// Constructor: Initializes the LEDs controller with the specified pin, number of LEDs, PIO instance, 
// and state machine (SM). It sets up the PIO program and initializes all LEDs to "off".
LEDs::LEDs(uint pin, uint num_leds, PIO pio, uint sm) : _pin(pin), _num_leds(num_leds), _pio(pio), _sm(sm) {
    _led_data.resize(num_leds, 0); // Initialize all LEDs to off
    uint offset = pio_add_program(pio, &ws2812_program);  // ws2812_program should be compatible
    ws2812_program_init(pio, sm, offset, pin, 800000, false);
}

// setColor(): Allows the user to set the color of an individual LED by its index. 
// Colors are stored as 32-bit values (with 8 bits unused) in _led_data.
void LEDs::setColor(uint led_index, uint8_t red, uint8_t green, uint8_t blue) {
    if (led_index < _num_leds) {
        _led_data[led_index] = (red << 24) | (green << 16) | (blue << 8);
    }
}

// update(): Sends the current color data for all LEDs to the hardware. This method abstracts the complexity 
// of interacting with the PIO, making it easier for the user to just deal with LED colors.
void LEDs::update() {
    for (uint32_t color : _led_data) {
        pio_sm_put_blocking(_pio, _sm, color);
    }
}

// clear(): Sets all LEDs to off by filling the _led_data array with zeros and optionally updates the LEDs immediately
void LEDs::clear() {
    std::fill(_led_data.begin(), _led_data.end(), 0);
    update(); // Optionally update immediately
}

// _sendData(): A private method that is responsible for the actual sending of data to the LEDs. 
// It's used by update() to send the current state of all LEDs to the hardware.
void LEDs::_sendData() {
    for (uint32_t color : _led_data) {
        pio_sm_put_blocking(_pio, _sm, color);
    }
}
