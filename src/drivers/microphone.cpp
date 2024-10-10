#include "microphone.h"
#include <stdio.h>
#include "hardware/adc.h"  // Ensure that the ADC library is included

#define DC_OFFSET 2048  // Define a constant for the DC offset

// Constructor: Initialize microphone with a default GPIO pin
microphone::microphone()
    : gpio_pin(26) {}

/*! \brief Initialize the microphone by setting up the ADC.
 *
 * This method configures the GPIO for microphone input (GPIO26 by default),
 * and sets up the ADC in free-running mode.
 *
 * \param gpio_pin The GPIO pin to read microphone data from (default: GPIO26).
 */
void microphone::init(uint gpio_pin)
{
    this->gpio_pin = gpio_pin;

    // Map the GPIO pin to the corresponding ADC input channel
    uint8_t adc_input = 0;
    if (gpio_pin == 26) {
        adc_input = 0;
    } else if (gpio_pin == 27) {
        adc_input = 1;
    } else if (gpio_pin == 28) {
        adc_input = 2;
    } else {
        printf("Error: Invalid GPIO pin for ADC input. Using default GPIO26.\n");
        adc_input = 0;
    }

    // Initialize GPIO for analogue use
    adc_gpio_init(this->gpio_pin);

    // Initialize and configure the ADC
    adc_init();
    adc_select_input(adc_input);  // Select the ADC input channel based on the GPIO pin
    adc_set_clkdiv(1087);          // Set clock divider for sampling rate (adjust as needed)
    adc_fifo_setup(
        true,   // Write each completed conversion to the sample FIFO
        false,  // Disable DMA data request (DREQ)
        1,      // Trigger when at least 1 sample is present in the FIFO
        false,  // Disable error bits
        true    // Shift results to 12 bits for ADC result (0-4095 range)
    );
    adc_run(true); // Start ADC in free-running mode
}

/*! \brief Blocking read of ADC samples.
 *
 * This function reads samples from the ADC and stores them in the provided buffer.
 * It enables the ADC in free-running mode and blocks until all the requested samples
 * are read into the buffer. After reading the specified number of samples, the function
 * stops the ADC from free-running mode and drains the FIFO buffer to discard any
 * remaining samples that were collected by the ADC but not yet processed.
 *
 * \param microphone_data Pointer to the buffer to store ADC samples.
 * \param buffer_size The size of the buffer (i.e., the number of samples to read).
 *
 * \note This function drains the FIFO after the required number of samples are read,
 *       ensuring no leftover data remains in the FIFO.
 */
void microphone::read_blocking(int16_t *microphone_data, size_t buffer_size)
{
    adc_run(true);  // Start ADC in free-running mode

    for (size_t i = 0; i < buffer_size; ++i)
    {
        uint16_t adc_value = adc_fifo_get_blocking();  // Read the next sample from the ADC FIFO
        microphone_data[i] = (int16_t)(adc_value - DC_OFFSET);  // Subtract DC offset
    }

    adc_run(false);   // Stop ADC free-running mode after reading required samples
    adc_fifo_drain(); // Drain any leftover samples in the FIFO to clean up

    // Process the microphone samples for Q15 format (optional, can be done in main task instead)
    for (size_t i = 0; i < buffer_size; ++i)
    {
        microphone_data[i] = (int16_t)(microphone_data[i] << 5);  // Left shift by 5 to scale into Q15 range
    }
}
