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
extern float buffer0_128[128]; //e.g. adc_samples
extern float buffer1_128[128]; //e.g filtered samples
extern float buffer2_128[128]; //e.g fft_magnitude
extern float buffer0_256[256]; // e.g. fft_input
// FIR filter coefficients (simple moving average)
extern float firCoeffs[FIR_TAP_NUM];

arm_fir_instance_f32 firInstance;
extern ADC_HandleTypeDef hadc1;

void test_voltage(void)
{
	struct ru_vec vadc_samples;
	struct ru_vec vfiltered_samples;
	struct ru_vec vfft_magnitude;
	struct ru_vec vfft_input;

	ru_vec_init(&vadc_samples, buffer0_128, ADC_SIZE, ADC_SIZE);
	ru_vec_init(&vfiltered_samples, buffer1_128, ADC_SIZE, ADC_SIZE);
	ru_vec_init(&vfft_magnitude, buffer2_128, ADC_SIZE, ADC_SIZE);
	ru_vec_init(&vfft_input, buffer0_256, FFT_BUFFER_SIZE, FFT_BUFFER_SIZE);


	// 1. Acquire raw ADC data (hardware/ADC driver)
    for (int i = 0; i < ADC_SIZE; ++i) {
        if (HAL_ADC_Start(&hadc1) != HAL_OK) {
            printf("ADC start failed at sample %d\n", i);
            break;
        }
        if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
            uint16_t raw = HAL_ADC_GetValue(&hadc1);
            float voltage = (float)raw * (3.3f / 4095.0f);
            vadc_samples.pbuf[i] = voltage;
        } else {
            vadc_samples.pbuf[i] = 0.0f;
        }
        HAL_ADC_Stop(&hadc1);
    }

    // 2. FIR filtering of raw samples (CMSIS-DSP FIR functions)
    arm_fir_init_f32(&firInstance, FIR_TAP_NUM, firCoeffs, vfiltered_samples.pbuf, ADC_SIZE);
    arm_fir_f32(&firInstance, vadc_samples.pbuf, vfiltered_samples.pbuf, ADC_SIZE);

    // 3. Prepare fft_input (interleaved complex array) for FFT (data prep)
    // 3a. Calculate mean of filtered_samples

    float mean = 0.0f;
    for (int i = 0; i < ADC_SIZE; i++) {
        mean += vfiltered_samples.pbuf[i];
    }
    mean /= ADC_SIZE;

    // 3b. Prepare fft_input with zero-mean data (complex array)
    for (int i = 0; i < ADC_SIZE; i++) {
        vfft_input.pbuf[2 * i] = vfiltered_samples.pbuf[i] - mean;
        vfft_input.pbuf[2 * i + 1] = 0.0f;
    }

    // 4. Perform FFT (CMSIS-DSP arm_cfft_f32)
    arm_cfft_instance_f32 S;
    arm_cfft_init_f32(&S, ADC_SIZE);
    arm_cfft_f32(&S, vfft_input.pbuf, 0, 1);

    // 5. Calculate magnitude of FFT bins (CMSIS-DSP arm_cmplx_mag_f32)
    arm_cmplx_mag_f32(vfft_input.pbuf, vfft_magnitude.pbuf, ADC_SIZE);

    // 6. Compute RMS of filtered samples (CMSIS-DSP arm_rms_f32)
    float rms_val;
    arm_rms_f32(vfiltered_samples.pbuf, ADC_SIZE, &rms_val);

    // 7. Envelope detection: average magnitude of first 5 FFT bins (simple math)
    float envelope = 0.0f;
    const int envelope_bins = 5;
    for (int i = 0; i < envelope_bins; i++) {
        envelope += vfft_magnitude.pbuf[i];
    }
    envelope /= envelope_bins;

    // 8. Print results
    printf("Voltage RMS: %f V\n", rms_val);
    printf("Envelope (avg mag of first %d bins): %f\n", envelope_bins, envelope);

    sigprintf("VOLTAGE: ");
    for (int i = 0; i < FFT_BIN_COUNT; i++) {
        printf("Bin %3d: %8.5f\n", i, vfft_magnitude.pbuf[i]);
        sigprintf("%8.5f ", vfft_magnitude.pbuf[i]);
    }
    sigprintf("END\n");
}




