/*=============================================================================
 * Author: Hanes <hanessciarrone@gmail.com>
 * Date: 2022/04/30
 * Version: v1.0.0
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/

#include "userTasks.h"
#include "sapi.h"
#include "arm_math.h"
 
/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/
#define SCALE_ADC         0.003225806f    // 3.3 / 1023     10-bit ADC to Voltage
#define SCALE_DAC         310.0f          // 1023 / 3.3     Voltage to 10-bit DAC

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/
typedef struct {
     arm_matrix_instance_f32 K;
    float Ko;
} polePlacementConfig_t;

/*=====[Definitions of external public global variables]=====================*/
#define N_SAMPLES    5000

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/
int output[N_SAMPLES] = {0};
char ready = 0;

/*=====[Prototypes (declarations) of private functions]======================*/
static float poleAndPlacement(const polePlacementConfig_t* pid, const arm_matrix_instance_f32* state, const float* reference);
static char* itoa(int value, char* result, int base);

/*=====[Implementations of public functions]=================================*/

// Task implementation
void pidControlTask( void* taskParmPtr )
{
    float u = 0.0f;             // Control signal calculated
    float reference = 0.0f;     // Reference signal that supply the circuit.
    float data[2] = { 0 };      // Store state variable values
    float values[2] = { 0 };    // Store K values of the PID
    uint16_t index = 0, x1 = 0, x2 = 0, outputPid = 0;
    char bufferData[10] = { 0 };

    arm_matrix_instance_f32 states;
    polePlacementConfig_t pole_placement_config;

    values[0] = 0.72211f;
    values[1] = 0.85232f;
    pole_placement_config.Ko = 2.5744f;

    arm_mat_init_f32(&states, 2, 1, data);
    arm_mat_init_f32(&pole_placement_config.K, 1, 2, values);

    adcInit( ADC_ENABLE );
    dacInit( DAC_ENABLE );
    // ----- Task setup -----------------------------------

    // Periodic task every 1 ms
    portTickType xPeriodicity =  1 / portTICK_RATE_MS;
    portTickType xLastWakeTime = xTaskGetTickCount();

    // ----- Task repeat for ever -------------------------
    while(TRUE) {
        reference = (index >= 2500) ? 3.3f : 0.0f;

        x2 = adcRead(CH1);
        x1 = adcRead(CH2);
        data[0] = x1 * SCALE_ADC;
        data[1] = x2 * SCALE_ADC;
        u = poleAndPlacement(&pole_placement_config, &states, &reference);
        u *= SCALE_DAC;

        outputPid = (u  < 0) ? 0 : (uint16_t)u;

        dacWrite(DAC, outputPid);


        if (index < 5000) {
            output[index] = x2;
        }
        else if (index == 5000) {
            for (index = 0; index < N_SAMPLES; index++) {
            itoa(output[index], bufferData, 10);
                uartWriteString(UART_USB, bufferData);
                uartWriteByte(UART_USB, '\n');
            }
        }

        index++;

        vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
    }
}

/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/
static float poleAndPlacement(const polePlacementConfig_t* pid, const arm_matrix_instance_f32* state, const float* reference)
{
    //arm_matrix_instance_f32 output;
    float retVal = 0;

    //arm_mat_init_f32(&output, 1, 1, &retVal);

    retVal = (float)*reference * pid->Ko - ( pid->K.pData[0] * state->pData[0] + pid->K.pData[1] * state->pData[1]);

//    if (ARM_MATH_SUCCESS != arm_mat_mult_f32(&pid->K, state, &output)) {
//        return 0;
//    }
//
//    retVal -= (float)*reference * pid->Ko;
//    retVal *= -1;
    return retVal;
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
