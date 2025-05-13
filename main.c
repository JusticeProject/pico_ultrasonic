#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/sync.h"

#include "ultrasonic_distance.h"

//************************************************************************************************************

// We need two GPIO, so we are using GPIO 2 and 3. PIO likes consecutive GPIOs.
// GPIO 2 = trigger
// GPIO 3 = echo
#define ULTRASONIC_GPIO_PIN_BASE 2

// Uncomment the following line if you want to use Bit Banging. Comment out the following line
// if you want to use PIO.
//#define USE_BIT_BANG 1

// shared data between main thread and interrupt handler, use a critical section to protect it
float current_measurement = -1.0f;
critical_section_t crit_sec;

//************************************************************************************************************

void interrupt_handler(void)
{
    current_measurement = ultrasonic_get_distance_pio();
}

//************************************************************************************************************

int main() {
    stdio_init_all();
    bool init_success = false;
    critical_section_init(&crit_sec);

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

            printf("Menu:\n");
            printf("i = initialize\n");
            printf("s = start continuous measurements (PIO only)\n");
            printf("q = query distance\n");
        }
        else if ('i' == c)
        {
            // only do this if it wasn't already initialized
            if (init_success)
            {
                printf("already initialized\n");
            }
            else
            {
                #ifdef USE_BIT_BANG
                    init_success = ultrasonic_init_bit_bang(ULTRASONIC_GPIO_PIN_BASE);
                #else
                    init_success = ultrasonic_distance_init_pio(ULTRASONIC_GPIO_PIN_BASE, interrupt_handler);
                #endif

                if (init_success)
                {
                    printf("initialization complete\n");
                }
                else
                {
                    printf("initialization FAILED\n");
                }
            }
        }
        else if ('s' == c)
        {
            #ifdef USE_BIT_BANG
                printf("nothing to do\n");
            #else
                if (init_success)
                {
                    printf("starting continuous measurements\n");
                    ultrasonic_start_measuring_pio();
                }
                else
                {
                    printf("not initialized yet\n");
                }
            #endif
        }
        else if ('q' == c)
        {
            #ifdef USE_BIT_BANG
                printf("starting to query for distance\n");
                float value = ultrasonic_get_distance_bit_bang();
                printf("distance = %fcm\n", value);
            #else
                // turn off interrupts for a short time so we can grab the latest data
                critical_section_enter_blocking(&crit_sec);
                float copied_data = current_measurement;
                critical_section_exit(&crit_sec);
                printf("distance = %fcm\n", copied_data);
            #endif
        }
        else
        {
            printf("unknown cmd %c\n", c);
        }
    }
}
