#ifndef DSP_H
#define DSP_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "arm_math.h"
#include "stdio.h"
#include "main.h"

// Constants
#define ADC_SIZE       128                  // Number of real ADC samples
#define FFT_BIN_COUNT  ADC_SIZE             // Number of complex frequency bins
#define FFT_BUFFER_SIZE (2 * ADC_SIZE)      // Interleaved real + imag
#define FIR_TAP_NUM    5

#define REUSABLE_CAPACITY 1024
#define DSP_TEST_COUNT 10
#define DSP_TEST_ALL 15

void reset_dsp_buffers(void);

struct ru_vec {
	float * pbuf;
	uint16_t len;
};

void ru_vec_init(struct ru_vec *v, float *pbuf, uint16_t length); // zero initialize
void ru_vec_attach(struct ru_vec *v, const struct ru_vec* old); // attaches with existing data

typedef void (*dsp_fn)(void);

// Declare each test runner
// Forward declarations (void return, no params)
void test_voltage(void);
void test_temperature(void);
void test_cpu_load(void);
void test_ram_usage(void);
void test_power_draw(void);
void test_mcu_core_temp(void);
void test_loop_jitter(void);
void test_uart_traffic(void);
void test_adc_noise_floor(void);
void test_clock_drift(void);

//helpers
float ADCToTemperature(uint32_t adc_val);
void Temp_ADC1_Init(void);
void Reset_ADC1_Init(void);

// array indexes
// Constants (array indexes)
#define DSP_TEST_VOLTAGE          0
#define DSP_TEST_TEMPERATURE      1
#define DSP_TEST_CPU_LOAD         2
#define DSP_TEST_RAM_USAGE        3
#define DSP_TEST_POWER_DRAW       4
#define DSP_TEST_MCU_CORE_TEMP    5
#define DSP_TEST_LOOP_JITTER      6
#define DSP_TEST_UART_TRAFFIC     7
#define DSP_TEST_ADC_NOISE_FLOOR  8
#define DSP_TEST_CLOCK_DRIFT      9

#endif // DSP_H

