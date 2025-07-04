/*
 * clock_drift_test.c
 *
 *  Created on: Jul 2, 2025
 *      Author: steve
 */
#include <dsp_test.h>

#define DELAY 500

extern RTC_HandleTypeDef hrtc;

void test_clock_drift(void)
{
#ifdef SIGNAL_FORMAT
    sigprintf("CLK ");
    for (int i = 0; i < 128; i++) {
        uint32_t t1_ticks = HAL_GetTick();
        uint32_t t1_rtc = RTC_Get_Seconds();  // You’ll need to implement this

        HAL_Delay(DELAY);  // Wait

        uint32_t t2_ticks = HAL_GetTick();
        uint32_t t2_rtc = RTC_Get_Seconds();

        float delta_tick_ms = (float)(t2_ticks - t1_ticks);
        float delta_rtc_sec = (float)(t2_rtc - t1_rtc);
        float expected_ms = delta_rtc_sec * 1000.0f;

        float drift_ms = delta_tick_ms - expected_ms;

        sigprintf("%8.5f ", drift_ms);  // You could also use drift_pct or ppm
    }
    sigprintf("END\n");

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
