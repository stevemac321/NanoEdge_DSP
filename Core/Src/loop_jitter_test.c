/*
 * loop_jitter_test.c
 *
 *  Created on: Jul 2, 2025
 *      Author: steve
 */
#include <dsp_test.h>

#define LOOP_COUNT 128
extern uint32_t buffer0_u32_128[128]; // for timestamps
extern float buffer0_128[LOOP_COUNT];

extern TIM_HandleTypeDef htim5;

void test_loop_jitter(void)
{
	struct ru_u32_vec vtimestamps;
	struct ru_vec vloop_deltas;
	ru_u32_vec_init(&vtimestamps, buffer0_u32_128, LOOP_COUNT, LOOP_COUNT);
	ru_vec_init(&vloop_deltas, buffer0_128, LOOP_COUNT, LOOP_COUNT);

    for (int i = 0; i < LOOP_COUNT; i++)
    {
        vtimestamps.pbuf[i] = __HAL_TIM_GET_COUNTER(&htim5);
        // Simulated work here
        for (volatile int j = 0; j < 1000; j++);
    }

    for (int i = 0; i < LOOP_COUNT - 1; i++)
    {
        vloop_deltas.pbuf[i] = (float32_t)(vtimestamps.pbuf[i + 1] - vtimestamps.pbuf[i]);
    }

    float32_t mean, stddev, variance;
    arm_mean_f32(vloop_deltas.pbuf, LOOP_COUNT - 1, &mean);
    arm_var_f32(vloop_deltas.pbuf, LOOP_COUNT - 1, &variance);
    arm_std_f32(vloop_deltas.pbuf, LOOP_COUNT - 1, &stddev);

    printf("Loop Time Mean: %.2f us\r\n", mean);
    printf("Loop Time Std Dev (Jitter): %.2f us\r\n", stddev);
    printf("Loop Time Variance: %.2f\r\n", variance);
}
