/*
 * cpu_test.c
 *
 *  Created on: Jul 2, 2025
 *      Author: steve
 */
#include <dsp_test.h>
void test_cpu_load(void)
{
    float avg_cycles = 0.0f;
    float avg_mhz = 0.0f;

    for (int i = 0; i < CPU_LOAD_ITERATIONS; i++) {
        // Enable DWT CYCCNT (if not already done)
        if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
            CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        }

        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

        uint32_t start_cycles = DWT->CYCCNT;
        uint32_t start_ms = HAL_GetTick();

        for (volatile int j = 0; j < 100000; j++); // Simulated load

        uint32_t end_cycles = DWT->CYCCNT;
        uint32_t end_ms = HAL_GetTick();

        uint32_t cycles = end_cycles - start_cycles;
        uint32_t ms = end_ms - start_ms;

        float mhz = (ms > 0) ? (float)cycles / (ms * 1000.0f) : 0.0f;

        printf("Run %d:\n", i + 1);
        printf("  Elapsed ms: %lu\n", ms);
        printf("  CPU Cycles: %lu\n", cycles);
        printf("  Approx. MHz: %.2f\n", mhz);

        avg_cycles += (float)cycles;
        avg_mhz += mhz;
    }

    avg_cycles /= CPU_LOAD_ITERATIONS;
    avg_mhz /= CPU_LOAD_ITERATIONS;

    printf("Average over %d runs:\n", CPU_LOAD_ITERATIONS);
    printf("  Avg CPU Cycles: %.2f\n", avg_cycles);
    printf("  Avg MHz: %.2f\n", avg_mhz);
}


