#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"

#include "ultrasonic_distance.h"

//************************************************************************************************************

// We need two GPIO, so we are using GPIO 2 and 3. PIO likes consecutive GPIOs.
// GPIO 2 = trigger
// GPIO 3 = echo
#define ULTRASONIC_GPIO_PIN_BASE 2

// Uncomment the following line if you want to use Bit Banging. Comment out the following line
// if you want to use PIO.
//#define USE_BIT_BANG 1

//************************************************************************************************************

int main() {
    stdio_init_all();

    bool init_success = false;

    #ifdef USE_BIT_BANG
        init_success = ultrasonic_init_bit_bang(ULTRASONIC_GPIO_PIN_BASE);
    #else
        init_success = ultrasonic_distance_init_pio(ULTRASONIC_GPIO_PIN_BASE);
    #endif

    if (!init_success)
    {
        while (true)
        {
            printf("Could not init ultrasonic distance sensor\n");
            sleep_ms(2000);
        }
    }
    
    while (true)
    {
        //int c = getchar_timeout_us(0);
        int c = getchar();
        printf("you entered %c\n", c);

        if ('h' == c)
        {
            printf("Running on %s\n", PICO_BOARD);
            #ifdef USE_BIT_BANG
                printf("Using Bit-Bang API\n");
            #else
                printf("Using PIO API\n");
            #endif
            printf("\n");

            // TODO: update when new commands are added
            printf("Menu:\n");
            printf("q = query distance\n");
        }
        else if ('q' == c)
        {
            printf("starting to query for distance\n");

            #ifdef USE_BIT_BANG
                float value = ultrasonic_get_distance_bit_bang();
                printf("result = %f\n", value);
            #else
                ultrasonic_start_measure_pio();
                float value = ultrasonic_get_distance_pio();
                printf("result = %f\n", value);
            #endif
        }
        else
        {
            printf("unknown cmd %c\n", c);
        }
    }
}
