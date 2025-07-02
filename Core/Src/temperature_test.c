/*
 * temperature_test.c
 *
 *  Created on: Jul 1, 2025
 *      Author: steve
 */

#include <dsp_test.h>

extern ADC_HandleTypeDef hadc1;
extern float adc_samples[ADC_SIZE];
extern float filtered_samples[ADC_SIZE];
extern float firCoeffs[FIR_TAP_NUM];
extern float firState[FIR_TAP_NUM + 1];

// FIR filter instance
static arm_fir_instance_f32 S;

// Helper: Convert ADC raw to temperature (STM32 internal sensor formula)
// Typical STM32 formula: Temp = ((V_SENSE - V_25)/Avg_Slope) + 25
// Adjust these constants based on your STM32 datasheet or calibration
#define VREF 3.3f
#define ADC_RESOLUTION 4096.0f
#define V_25 0.76f     // Voltage at 25°C, typical
#define AVG_SLOPE 0.0025f  // Volts per °C, typical



float ADCToTemperature(uint32_t adc_val)
{
    float v_sense = (adc_val * VREF) / ADC_RESOLUTION;
    float temp = ((v_sense - V_25) / AVG_SLOPE) + 25.0f;
    return temp;
}

void test_temperature(void)
{
	HAL_ADC_DeInit(&hadc1);  // Force reset
	Temp_ADC1_Init();
	HAL_Delay(1);               // Wait for temp sensor to stabilize

	reset_dsp_buffers();
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
            adc_samples[i] = ADCToTemperature(adc_val);
        }
        HAL_ADC_Stop(&hadc1);  // Clean up between conversions
        HAL_Delay(5);
    }

    // Apply FIR filter to smooth temperature samples
    arm_fir_f32(&S, adc_samples, filtered_samples, ADC_SIZE);

    // Compute average of filtered results as final temperature
    float sum = 0.0f;
    for (int i = 0; i < ADC_SIZE; i++)
    {
        sum += filtered_samples[i];
    }
    temp = sum / ADC_SIZE;

    // Print temperature - replace printf with your preferred output method
    printf("Filtered Temperature: %.2f °C\n", temp);
    HAL_ADC_DeInit(&hadc1);  // Force reset
    Reset_ADC1_Init();
	HAL_Delay(1);               // Wait for temp sensor to stabilize
}





