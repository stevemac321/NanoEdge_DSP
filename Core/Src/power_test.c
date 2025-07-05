/*
 * power_test.c
 *
 *  Created on: Jul 2, 2025
 *      Author: steve
 */
#include <dsp_test.h>
#include "arm_math.h"

#define POWER_SAMPLES         64
#define SHUNT_RESISTANCE_OHMS 0.1f
#define ADC_MAX_VALUE         4095.0f
#define VREF_VOLTAGE          3.3f

extern ADC_HandleTypeDef hadc1;
extern float buffer0_128[128];  // adc_samples
extern float buffer1_128[128]; // filtered_samples


uint16_t read_adc_raw(void) {
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    return HAL_ADC_GetValue(&hadc1);
}

void test_power_draw(void)
{
	struct ru_vec vcurrents;
	struct ru_vec vpowers;
	ru_vec_init(&vcurrents, buffer0_128, POWER_SAMPLES, 128); // see dsp_test.h
	ru_vec_init(&vpowers, buffer1_128, POWER_SAMPLES, 128); // see dsp_test.h

    float rms_power = 0.0f;

    sigprintf("PWR ");
    for (int i = 0; i < 128; ++i) {
        uint16_t raw_adc = read_adc_raw();
        float shunt_voltage = ((float)raw_adc / ADC_MAX_VALUE) * VREF_VOLTAGE;
        float current = shunt_voltage / SHUNT_RESISTANCE_OHMS;
        float power = VREF_VOLTAGE * current;

        vpowers.pbuf[i] = power;

        sigprintf("%8.5f ", power);
        HAL_Delay(5);  // maintain ~200Hz sample rate
    }
    sigprintf("END\n");
#ifdef SELF_DIAG_MODE
    one_inference(vpowers.pbuf, "PWR from power_test.c");
#endif

    float avg_power = 0.0f;
    for (int i = 0; i < POWER_SAMPLES; ++i) {
        avg_power += vpowers.pbuf[i];
    }
    avg_power /= POWER_SAMPLES;

    arm_rms_f32(vpowers.pbuf, POWER_SAMPLES, &rms_power);

    float peak_power = 0.0f;
    for (int i = 0; i < POWER_SAMPLES; ++i) {
        if (vpowers.pbuf[i] > peak_power)
            peak_power = vpowers.pbuf[i];
    }

    printf("Power Draw Test:\n");
    printf("Avg Power: %.3f W\n", avg_power);
    printf("RMS Power: %.3f W\n", rms_power);
    printf("Peak Power: %.3f W\n", peak_power);
}

