/*=============================================================================
 * Author: Hanes <hanessciarrone@gmail.com>
 * Date: 2022/04/16
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "Control_digital.h"
#include "sapi.h"
#include "sapi_timer_modified.h"

/*=====[Definition macros of private constants]==============================*/
#define TICKRATE_DAC_HZ             20  // 2 generate 1 Hz, 20 generate 10 Hz
#define TICKRATE_ADC_HZ             200 // 200 generate 100 Hz

#define DAC_REFERENCE_VALUE_HIGH   1023  // 1023 = 3.3V, 666 = 2.15V
#define DAC_REFERENCE_VALUE_LOW    0     // 1023 = 3.3V, 0 = 0V

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/
static char* itoa(int value, char* result, int base);
static bool_t dacCallback(void* ptr);
static bool_t adcCallback(void *ptr);

int main( void )
{
    uint8_t bufferData[10] = {0};
    uint16_t value = 0;
    // ----- Setup -----------------------------------
    boardInit();
    dacInit(DAC_ENABLE);
    adcInit(ADC_ENABLE);
    uartInit(UART_USB, 115200);

    // Configure TIMER1 in TICKER mode
    timerTickerConfig(TIMER1, TICKRATE_DAC_HZ);
    timerTickerSetTickEvent(TIMER1, dacCallback);

    // Enable TIMER1
    timerSetPower(TIMER1, ON);

    // ----- Repeat for ever -------------------------
    while( true ) {
        value = adcRead(CH1);
        itoa(value, bufferData, 10);
        uartWriteString(UART_USB, bufferData);
        uartWriteByte(UART_USB, '\n');
        delay(1);
    }

    // YOU NEVER REACH HERE, because this program runs directly or on a
    // microcontroller and is not called by any Operating System, as in the
    // case of a PC program.
    return 0;
}

static char* itoa(int value, char* result, int base) {
   // check that the base if valid
   if (base < 2 || base > 36) { *result = '\0'; return result; }

   char* ptr = result, *ptr1 = result, tmp_char;
   int tmp_value;

   do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
   } while ( value );

   // Apply negative sign
   if (tmp_value < 0) *ptr++ = '-';
   *ptr-- = '\0';
   while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
   }
   return result;
}

static bool_t dacCallback(void* ptr)
{
   static uint16_t lastDacValue = DAC_REFERENCE_VALUE_LOW;

   dacWrite( DAC, lastDacValue );

   if( lastDacValue == DAC_REFERENCE_VALUE_HIGH ){
      lastDacValue = DAC_REFERENCE_VALUE_LOW;
   } else{
      lastDacValue = DAC_REFERENCE_VALUE_HIGH;
   }
}

