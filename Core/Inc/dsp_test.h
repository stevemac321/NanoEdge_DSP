#ifndef DSP_H
#define DSP_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "arm_math.h"
#include "main.h"

//#define SIGNAL_FORMAT  // for signal creation mode
#define INFERENCE_MODE

// for signal formatting
#ifdef SIGNAL_FORMAT
  #define printf(...)        do {} while(0)            // silence all verbose/debug logging
#endif

void sigprintf(const char *fmt, ...);
// Constants
#define ADC_SIZE       128                  // Number of real ADC samples
#define FFT_BIN_COUNT  ADC_SIZE             // Number of complex frequency bins
#define FFT_BUFFER_SIZE (2 * ADC_SIZE)      // Interleaved real + imag
#define FIR_TAP_NUM    5

#define CPU_LOAD_ITERATIONS 128

#define REUSABLE_CAPACITY 1024
#define DSP_TEST_COUNT 10
#define DSP_TEST_ALL 15

// Helper: Convert ADC raw to temperature (STM32 internal sensor formula)
// Typical STM32 formula: Temp = ((V_SENSE - V_25)/Avg_Slope) + 25
// Adjust these constants based on your STM32 datasheet or calibration
#define VREF 3.3f
#define ADC_RESOLUTION 4096.0f
#define V_25 0.76f     // Voltage at 25°C, typical
#define AVG_SLOPE 0.0025f  // Volts per °C, typical

struct ru_vec {
	float * pbuf;
	uint16_t len;
	uint16_t cap;
};

struct ru_u32_vec {
	uint32_t * pbuf;
	uint16_t len;
	uint16_t cap;
};

void ru_vec_init(struct ru_vec *v, float *pbuf, const uint16_t length, const uint16_t capacity); // zero initialize
void ru_u32_vec_init(struct ru_u32_vec *v, uint32_t *pbuf, const uint16_t length, const uint16_t capacity); // zero initialize


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
void Vrefint_ADC1_Init(void);
void Reset_ADC1_Init(void);
void fill_stack_pattern(void);
size_t get_stack_usage_bytes(void);
uint16_t read_adc_raw(void);
float read_core_temp_celsius(void);
float ADCToTemperature(uint32_t adc_val);
void gather_tx_stats(const char *ptr, int len);
uint32_t RTC_Get_Seconds(void);


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

#if 0
const char* dsp_signal_ids[] = {
    "VOLTAGE",
    "TEMP",
    "CPU",
    "RAM",
    "PWR",
    "CORE",
    "LOOP",
    "UART",
    "NOISE",
    "CLK"
};
#endif


#endif // DSP_H

