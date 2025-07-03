/*
 * dsp_test.c
 *
 *  Created on: Jun 30, 2025
 *      Author: steve
 */
//#include "stm32f4xx_hal.h"   // Or whatever matches your STM32 series
#include <dsp_test.h>

extern ADC_HandleTypeDef hadc1;

// Function table
dsp_fn dsp_tests[] = {
    test_voltage,
    test_temperature,
    test_cpu_load,
    test_ram_usage,
    test_power_draw,
    test_mcu_core_temp,
    test_loop_jitter,
    test_uart_traffic,
    test_adc_noise_floor,
    test_clock_drift,
};

float buffer0_128[128] = {0}; //e.g. adc_samples
float buffer1_128[128] = {0}; //e.g filtered samples
float buffer2_128[128] = {0}; //e.g fft_magnitude
float buffer0_256[256] = {0}; // e.g. fft_input


const float firCoeffs[FIR_TAP_NUM] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f};
float firState[ADC_SIZE + FIR_TAP_NUM + 1];  // CMSIS requires: state length = numTaps + blockSize - 1

// ru_vec implementation

void ru_vec_init(struct ru_vec *v, float *pbuf, const uint16_t length, const uint16_t capacity)
{
	// zero initialize
	v->pbuf = pbuf;
	memset(v->pbuf, 0, length * sizeof(float));
	v->len = length;
}
void ru_vec_attach(struct ru_vec *v, const struct ru_vec* old)
{
	// attaches with existing data
    v->pbuf = old->pbuf;
    v->len = old->len;
}
#if 0
// reset for those who use the buffers extern
void reset_dsp_buffers(void)
{
    for (int i = 0; i < ADC_SIZE; i++) {
        adc_samples[i] = 0.0f;
        filtered_samples[i] = 0.0f;
        fft_magnitude[i] = 0.0f;
    }

    for (int i = 0; i < FFT_BUFFER_SIZE; i++) {
        fft_input[i] = 0.0f;
    }

    for (int i = 0; i < ADC_SIZE + FIR_TAP_NUM + 1; i++) {
        firState[i] = 0.0f;
    }
}
#endif
void Temp_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
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
  /** Enable the temperature sensor and VREFINT
   *  Note: This must be done *after* ADC Init and *before* channel config.
   */
  ADC->CCR |= ADC_CCR_TSVREFE;
  HAL_Delay(1); // 1ms = safe
  /** Configure the ADC channel for temperature sensor
   *  For STM32F4, temp sensor is ADC_CHANNEL_TEMPSENSOR (channel 16)
   *  Recommended sampling time: >=10us, so use 144 cycles or higher
   */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES; // Safe value for temp sensor

  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

void Reset_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
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

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */
  /* USER CODE END ADC1_Init 2 */

}


