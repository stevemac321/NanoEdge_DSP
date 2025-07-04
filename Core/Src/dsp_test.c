/*
 * dsp_test.c
 *
 *  Created on: Jun 30, 2025
 *      Author: steve
 */
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
uint32_t buffer0_u32_128[128] = {0}; // for timestamps

const float firCoeffs[FIR_TAP_NUM] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f};
float firState[ADC_SIZE + FIR_TAP_NUM + 1];  // CMSIS requires: state length = numTaps + blockSize - 1

// ru_vec implementation, for float
void ru_vec_init(struct ru_vec *v, float *pbuf, const uint16_t length, const uint16_t capacity)
{
	// zero initialize
	v->pbuf = pbuf;
	memset(v->pbuf, 0, length * sizeof(float));
	v->len = length;
}
void ru_u32_vec_init(struct ru_u32_vec *v, uint32_t *pbuf, const uint16_t length, const uint16_t capacity)
{
	// zero initialize
	v->pbuf = pbuf;
	memset(v->pbuf, 0, length * sizeof(uint32_t));
	v->len = length;
}
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

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

float ADCToTemperature(uint32_t adc_val)
{
    float v_sense = (adc_val * VREF) / ADC_RESOLUTION;
    float temp = ((v_sense - V_25) / AVG_SLOPE) + 25.0f;
    return temp;
}
void sigprintf(const char *fmt, ...) {
#ifdef SIGNAL_FORMAT
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
#endif
}



