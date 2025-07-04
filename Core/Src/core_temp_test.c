/*
 * core_temp_test.c
 *
 *  Created on: Jul 2, 2025
 *      Author: steve
 */
#include <dsp_test.h>
extern ADC_HandleTypeDef hadc1;
extern float buffer0_128[128];
extern float buffer1_128[128];
extern const float firCoeffs[FIR_TAP_NUM];
extern float firState[ADC_SIZE + FIR_TAP_NUM + 1];  // CMSIS requires: state length = numTaps + blockSize - 1


void test_mcu_core_temp(void)
{
	HAL_ADC_DeInit(&hadc1);  // Force reset
	Temp_ADC1_Init();
	HAL_Delay(1);               // Wait for temp sensor to stabilize

	struct ru_vec vtemps_raw;
	struct ru_vec vfiltered_buffer;
	ru_vec_init(&vtemps_raw, buffer0_128, 128, 128);
	ru_vec_init(&vfiltered_buffer, buffer1_128, 128, 128);

	for (int i = 0; i < 128; ++i) {
		vtemps_raw.pbuf[i] = read_core_temp_celsius();
	    HAL_Delay(50); // Sample every 50ms = 6.4s total
	}

	arm_fir_instance_f32 S;
	arm_fir_init_f32(&S, FIR_TAP_NUM, (float32_t *)firCoeffs, firState, 128);
	arm_fir_f32(&S, vtemps_raw.pbuf, vfiltered_buffer.pbuf, 128);

	sigprintf("CORE ");
	for(int i=0; i < 128; i++) {
		printf("Raw: %.2f °C, Filtered: %.2f °C\n", vtemps_raw.pbuf[i], vfiltered_buffer.pbuf[i]);
		 sigprintf("%8.5f ", vfiltered_buffer.pbuf[i]);
	}
	sigprintf("END\n");

	HAL_ADC_DeInit(&hadc1);  // Force reset
	Reset_ADC1_Init();
	HAL_Delay(1);               // Wait for temp sensor to stabilize
}

float read_core_temp_celsius(void)
{
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK) {
        return -273.15f; // Error case
    }

    uint32_t raw = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    return ADCToTemperature(raw);
}

