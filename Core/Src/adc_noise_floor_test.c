/*
 * adc_noise_floor_test.c
 *
 *  Created on: Jul 2, 2025
 *      Author: steve
 */
#include <dsp_test.h>

extern ADC_HandleTypeDef hadc1;

#define ADC_NOISE_FFT_SIZE  128
static uint16_t adc_samples[ADC_NOISE_FFT_SIZE];
extern float buffer0_128[128]; //adc_f32
extern float buffer1_128[128]; //fft_output
extern float buffer0_256[256]; //fft_input

void test_adc_noise_floor(void)
{
    HAL_ADC_DeInit(&hadc1);
    Vrefint_ADC1_Init();
    HAL_Delay(1);

    struct ru_vec vadc_f32;
    struct ru_vec vfft_output;
    struct ru_vec vfft_input;

    ru_vec_init(&vadc_f32, buffer0_128, 128, 128);
    ru_vec_init(&vfft_output, buffer0_128, 128, 128);
    ru_vec_init(&vfft_input, buffer0_256, 256, 256);

    printf("Capturing %d ADC samples from VREFINT...\n", ADC_NOISE_FFT_SIZE);

    for (int i = 0; i < ADC_NOISE_FFT_SIZE; ++i) {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
        adc_samples[i] = HAL_ADC_GetValue(&hadc1);
    }

    HAL_ADC_DeInit(&hadc1);
    Reset_ADC1_Init();
    HAL_Delay(1);

    // Convert to float and normalize (assuming Vref = 3.3V)
    for (int i = 0; i < ADC_NOISE_FFT_SIZE; ++i) {
        vadc_f32.pbuf[i] = ((float32_t)adc_samples[i]) * (3.3f / 4095.0f);
        vfft_input.pbuf[2 * i] = vadc_f32.pbuf[i];     // real
        vfft_input.pbuf[2 * i + 1] = 0.0f;       // imag
    }

    // Apply Hamming window (optional, reduces leakage)
    for (int i = 0; i < ADC_NOISE_FFT_SIZE; ++i) {
        float32_t w = 0.54f - 0.46f * arm_cos_f32((2.0f * PI * i) / (ADC_NOISE_FFT_SIZE - 1));
        vfft_input.pbuf[2 * i] *= w;
    }

    // Setup and run FFT
    arm_cfft_radix4_instance_f32 fft_inst;
    arm_cfft_radix4_init_f32(&fft_inst, ADC_NOISE_FFT_SIZE * 2, 0, 1);
    arm_cfft_radix4_f32(&fft_inst, vfft_input.pbuf);

    // Compute magnitude
    arm_cmplx_mag_f32(vfft_input.pbuf, vfft_output.pbuf, ADC_NOISE_FFT_SIZE);

    // Print the first few bins (e.g., 0–32 Hz equivalent bins)
    printf("FFT magnitude (bins 0–15):\n");
    sigprintf("NOISE ");
    for (int i = 0; i < 128; ++i) {
        printf("  Bin %2d: %.5f\n", i, vfft_output.pbuf[i]);
        sigprintf("%8.5f ", vfft_output.pbuf[i]);
    }
    sigprintf("END\n");

}

void Vrefint_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    /** Enable VREFINT channel */
#if defined(STM32F4)
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    SYSCFG->CMPCR |= SYSCFG_CMPCR_CMP_PD;  // Required on some F4s
#elif defined(STM32G4)
    HAL_ADCEx_EnableVREFINT();
#endif

    /** Configure channel for VREFINT */
    sConfig.Channel = ADC_CHANNEL_VREFINT;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;  // Longer sample time is best for internal channels

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}
