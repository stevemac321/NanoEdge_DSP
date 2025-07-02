/*
 * fft_test.c
 *
 *  Created on: Jun 30, 2025
 *      Author: steve
 */
#include <dsp_test.h>
#include "stm32f4xx_hal.h"   // Or whatever matches your STM32 series
#include "arm_math.h"
#include "stdio.h"


// Buffers
extern float adc_samples[ADC_SIZE];
extern float filtered_samples[ADC_SIZE];
extern float fft_input[FFT_BUFFER_SIZE];
extern float fft_magnitude[ADC_SIZE];
// FIR filter coefficients (simple moving average)
extern float firCoeffs[FIR_TAP_NUM];

arm_fir_instance_f32 firInstance;
extern ADC_HandleTypeDef hadc1;

void test_voltage(void)
{
	// 1. Acquire raw ADC data (hardware/ADC driver)
    for (int i = 0; i < ADC_SIZE; ++i) {
        if (HAL_ADC_Start(&hadc1) != HAL_OK) {
            printf("ADC start failed at sample %d\n", i);
            break;
        }
        if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
            uint16_t raw = HAL_ADC_GetValue(&hadc1);
            float voltage = (float)raw * (3.3f / 4095.0f);
            adc_samples[i] = voltage;
        } else {
            adc_samples[i] = 0.0f;
        }
        HAL_ADC_Stop(&hadc1);
    }

    // 2. FIR filtering of raw samples (CMSIS-DSP FIR functions)
    arm_fir_init_f32(&firInstance, FIR_TAP_NUM, firCoeffs, filtered_samples, ADC_SIZE);
    arm_fir_f32(&firInstance, adc_samples, filtered_samples, ADC_SIZE);

    // 3. Prepare fft_input (interleaved complex array) for FFT (data prep)
    // 3a. Calculate mean of filtered_samples

    float mean = 0.0f;
    for (int i = 0; i < ADC_SIZE; i++) {
        mean += filtered_samples[i];
    }
    mean /= ADC_SIZE;

    // 3b. Prepare fft_input with zero-mean data (complex array)
    for (int i = 0; i < ADC_SIZE; i++) {
        fft_input[2 * i] = filtered_samples[i] - mean;
        fft_input[2 * i + 1] = 0.0f;
    }

    // 4. Perform FFT (CMSIS-DSP arm_cfft_f32)
    arm_cfft_instance_f32 S;
    arm_cfft_init_f32(&S, ADC_SIZE);
    arm_cfft_f32(&S, fft_input, 0, 1);

    // 5. Calculate magnitude of FFT bins (CMSIS-DSP arm_cmplx_mag_f32)
    arm_cmplx_mag_f32(fft_input, fft_magnitude, ADC_SIZE);

    // 6. Compute RMS of filtered samples (CMSIS-DSP arm_rms_f32)
    float rms_val;
    arm_rms_f32(filtered_samples, ADC_SIZE, &rms_val);

    // 7. Envelope detection: average magnitude of first 5 FFT bins (simple math)
    float envelope = 0.0f;
    const int envelope_bins = 5;
    for (int i = 0; i < envelope_bins; i++) {
        envelope += fft_magnitude[i];
    }
    envelope /= envelope_bins;

    // 8. Print results
    printf("Voltage RMS: %f V\n", rms_val);
    printf("Envelope (avg mag of first %d bins): %f\n", envelope_bins, envelope);
    for (int i = 0; i < FFT_BIN_COUNT; i++) {
        printf("Bin %3d: %8.5f\n", i, fft_magnitude[i]);
    }
}




