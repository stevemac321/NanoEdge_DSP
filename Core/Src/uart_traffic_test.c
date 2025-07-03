#include "dsp_test.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "arm_math.h"

#define UART_LOG_CAPACITY 256
#define UART_PACKET_MAX_LEN 32
#define HIST_NUM_BINS 10

typedef struct {
    uint32_t delta_us;
    uint16_t packet_size;
    uint8_t packet_data[UART_PACKET_MAX_LEN];
} uart_tx_log_entry_t;

uart_tx_log_entry_t uart_tx_log[UART_LOG_CAPACITY];
volatile uint16_t uart_tx_log_index = 0;

extern TIM_HandleTypeDef htim5;
static uint32_t last_uart_tx_timestamp = 0;

void gather_tx_stats(const char *ptr, int len)
{
    if (uart_tx_log_index < UART_LOG_CAPACITY) {
        uint32_t now = __HAL_TIM_GET_COUNTER(&htim5);
        uint32_t delta = now - last_uart_tx_timestamp;
        last_uart_tx_timestamp = now;

        uart_tx_log[uart_tx_log_index].delta_us = delta;
        uart_tx_log[uart_tx_log_index].packet_size = len;

        uint16_t copy_len = (len > UART_PACKET_MAX_LEN) ? UART_PACKET_MAX_LEN : len;
        memcpy(uart_tx_log[uart_tx_log_index].packet_data, ptr, copy_len);

        uart_tx_log_index++;
    }
}

void test_uart_traffic(void)
{
    printf("=== UART Transmit Traffic Log (%u entries) ===\r\n", uart_tx_log_index);
    printf("Idx\tDelta_us\tSize\tData (hex)\r\n");

    float deltas_f32[UART_LOG_CAPACITY];

    for (uint16_t i = 0; i < uart_tx_log_index; i++) {
        printf("%3u\t%8lu\t%4u\t", i,
               uart_tx_log[i].delta_us,
               uart_tx_log[i].packet_size);

        for (uint16_t j = 0; j < uart_tx_log[i].packet_size && j < UART_PACKET_MAX_LEN; j++) {
            printf("%02X ", uart_tx_log[i].packet_data[j]);
        }
        printf("\r\n");

        deltas_f32[i] = (float)uart_tx_log[i].delta_us;
    }

    if (uart_tx_log_index == 0) {
        printf("No UART TX entries logged.\r\n");
        return;
    }

    // CMSIS-DSP statistics
    float32_t min_val, max_val, mean_val;
    uint32_t min_index, max_index;

    arm_min_f32(deltas_f32, uart_tx_log_index, &min_val, &min_index);
    arm_max_f32(deltas_f32, uart_tx_log_index, &max_val, &max_index);
    arm_mean_f32(deltas_f32, uart_tx_log_index, &mean_val);

    printf("\r\nTiming Stats (CMSIS-DSP):\r\n");
    printf("  Min delta: %.1f us (at idx %lu)\r\n", min_val, min_index);
    printf("  Max delta: %.1f us (at idx %lu)\r\n", max_val, max_index);
    printf("  Mean delta: %.2f us\r\n", mean_val);

    // Manual histogram
    printf("\r\nTiming Histogram:\r\n");
    float bin_width = (max_val - min_val) / HIST_NUM_BINS;
    uint32_t bin_count[HIST_NUM_BINS] = {0};

    for (uint16_t i = 0; i < uart_tx_log_index; i++) {
        int bin = (int)((deltas_f32[i] - min_val) / bin_width);
        if (bin >= HIST_NUM_BINS) bin = HIST_NUM_BINS - 1;
        if (bin < 0) bin = 0;
        bin_count[bin]++;
    }

    for (int i = 0; i < HIST_NUM_BINS; i++) {
        float bin_start = min_val + i * bin_width;
        float bin_end = bin_start + bin_width;
        printf("  [%6.1f - %6.1f] us : %lu\r\n", bin_start, bin_end, bin_count[i]);
    }
}
