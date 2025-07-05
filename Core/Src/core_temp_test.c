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
		float val = vfiltered_buffer.pbuf[i];

		// Clamp core temp (e.g., 20 to 80°C)
		if (val < 20.0f) val = 20.0f;
		if (val > 80.0f) val = 80.0f;

		// Normalize to 0.0–1.0
		float norm = (val - 20.0f) / (80.0f - 20.0f);

		sigprintf("%8.5f ", norm);
		vfiltered_buffer.pbuf[i] = norm;  // Update buffer for inference
	}
	sigprintf("END\n");
#ifdef SELF_DIAG_MODE
	one_inference(vfiltered_buffer.pbuf, "CORE from core_temp_test.c");
#endif
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

