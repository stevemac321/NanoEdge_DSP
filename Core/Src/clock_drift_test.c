/*
 * clock_drift_test.c
 *
 *  Created on: Jul 2, 2025
 *      Author: steve
 */
#include <dsp_test.h>

#define DELAY 500

extern RTC_HandleTypeDef hrtc;
extern float buffer0_128[128];

void test_clock_drift(void)
{
#if defined(SIGNAL_FORMAT) || defined(SELF_DIAG_MODE)
    struct ru_vec vdrift;
    ru_vec_init(&vdrift, buffer0_128, 128, 128);
    sigprintf("CLK ");
    for (int i = 0; i < 128; i++) {
        uint32_t t1_ticks = HAL_GetTick();
        uint32_t t1_rtc = RTC_Get_Seconds();

        HAL_Delay(DELAY);

        uint32_t t2_ticks = HAL_GetTick();
        uint32_t t2_rtc = RTC_Get_Seconds();

        float delta_tick_ms = (float)(t2_ticks - t1_ticks);
        float delta_rtc_sec = (float)(t2_rtc - t1_rtc);
        float expected_ms = delta_rtc_sec * 1000.0f;

        float drift_ms = delta_tick_ms - expected_ms;
        float drift_ppm = (drift_ms / expected_ms) * 1e6f;

        // Clamp extreme values
        if (drift_ppm > 100.0f) drift_ppm = 100.0f;
        if (drift_ppm < -100.0f) drift_ppm = -100.0f;

        float norm = drift_ppm / 100.0f;

        sigprintf("%8.5f ", norm);
    #ifdef SELF_DIAG_MODE
        vdrift.pbuf[i] = norm;
    #endif
    }

    sigprintf("END\n");

#ifdef SELF_DIAG_MODE
    one_inference(vdrift.pbuf, "CLOCK from clock_drift_test.c");
#endif

#else
    printf("Clock Drift Test (Single Sample):\n");

    uint32_t t1_ticks = HAL_GetTick();
    uint32_t t1_rtc = RTC_Get_Seconds();

    HAL_Delay(3000);

    uint32_t t2_ticks = HAL_GetTick();
    uint32_t t2_rtc = RTC_Get_Seconds();

    float delta_tick_ms = (float)(t2_ticks - t1_ticks);
    float delta_rtc_sec = (float)(t2_rtc - t1_rtc);
    float expected_ms = delta_rtc_sec * 1000.0f;

    float drift_ms = delta_tick_ms - expected_ms;
    float drift_pct = 100.0f * drift_ms / expected_ms;

    printf("  ΔTick = %.2f ms\n", delta_tick_ms);
    printf("  ΔRTC  = %.2f s (%.2f ms)\n", delta_rtc_sec, expected_ms);
    printf("  Drift = %.2f ms (%.2f%%)\n", drift_ms, drift_pct);
#endif
}
uint32_t RTC_Get_Seconds(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    // Read RTC time (this also latches the date)
    if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }

    // Convert to seconds since midnight
    return (uint32_t)(sTime.Hours * 3600 + sTime.Minutes * 60 + sTime.Seconds);
}
