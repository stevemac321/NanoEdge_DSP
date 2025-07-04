/*
 * temperature_test.c
 *
 *  Created on: Jul 1, 2025
 *      Author: steve
 */

#include <dsp_test.h>

extern ADC_HandleTypeDef hadc1;
extern float buffer0_128[128]; //e.g. adc_samples
extern float buffer1_128[128]; //e.g filtered samples
extern float buffer2_128[128]; //e.g fft_magnitude
extern float buffer0_256[256]; // e.g. fft_input

extern float firState[FIR_TAP_NUM + 1];
extern float firCoeffs[FIR_TAP_NUM];

// FIR filter instance
static arm_fir_instance_f32 S;


void test_temperature(void)
{
	struct ru_vec vadc_samples;
	struct ru_vec vfiltered_samples;
	struct ru_vec vfft_magnitude;
	struct ru_vec vfft_input;

	ru_vec_init(&vadc_samples, buffer0_128, ADC_SIZE, ADC_SIZE);
	ru_vec_init(&vfiltered_samples, buffer1_128, ADC_SIZE, ADC_SIZE);
	ru_vec_init(&vfft_magnitude, buffer2_128, ADC_SIZE, ADC_SIZE);
	ru_vec_init(&vfft_input, buffer0_256, FFT_BUFFER_SIZE, FFT_BUFFER_SIZE);

	HAL_ADC_DeInit(&hadc1);  // Force reset
	Temp_ADC1_Init();
	HAL_Delay(1);               // Wait for temp sensor to stabilize

	//reset_dsp_buffers();
    uint32_t adc_val = 0;
    float temp = 0.0f;

    // Initialize FIR filter
    arm_fir_init_f32(&S, FIR_TAP_NUM, (float32_t *)firCoeffs, firState, ADC_SIZE);

    // Collect ADC samples from internal temperature sensor channel
    for (int i = 0; i < ADC_SIZE; i++)
    {
    	if (HAL_ADC_Start(&hadc1) != HAL_OK) {
    	            Error_Handler();
    	}
    	HAL_StatusTypeDef status = HAL_ADC_PollForConversion(&hadc1, 500);
        if (status == HAL_OK)
        {
            adc_val = HAL_ADC_GetValue(&hadc1);
            vadc_samples.pbuf[i] = ADCToTemperature(adc_val);
        }
        HAL_ADC_Stop(&hadc1);  // Clean up between conversions
        HAL_Delay(5);
    }

    // Apply FIR filter to smooth temperature samples
    arm_fir_f32(&S, vadc_samples.pbuf, vfiltered_samples.pbuf, ADC_SIZE);

    // Compute average of filtered results as final temperature
    float sum = 0.0f;

    for (int i = 0; i < ADC_SIZE; i++)
    {
        sum += vfiltered_samples.pbuf[i];

    }

    temp = sum / ADC_SIZE;

    // Print temperature - replace printf with your preferred output method
    printf("Filtered Temperature: %.2f °C\n", temp);
    HAL_ADC_DeInit(&hadc1);  // Force reset
    Reset_ADC1_Init();
	HAL_Delay(1);               // Wait for temp sensor to stabilize
}





