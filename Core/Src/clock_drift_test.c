/*
 * clock_drift_test.c
 *
 *  Created on: Jul 2, 2025
 *      Author: steve
 */
#include <dsp_test.h>

extern RTC_HandleTypeDef hrtc;

void test_clock_drift(void)
{
    printf("hello from test_clock_drift\n");

    uint32_t t1_ticks = HAL_GetTick();
    uint32_t t1_rtc = RTC_Get_Seconds();  // Stub: implement this

    HAL_Delay(3000);  // Wait ~3 seconds

    uint32_t t2_ticks = HAL_GetTick();
    uint32_t t2_rtc = RTC_Get_Seconds();

    float delta_tick_ms = (float)(t2_ticks - t1_ticks);
    float delta_rtc_sec = (float)(t2_rtc - t1_rtc);
    float expected_ms = delta_rtc_sec * 1000.0f;

    float drift_ms = delta_tick_ms - expected_ms;
    float drift_pct = 100.0f * drift_ms / expected_ms;

    printf("Clock drift test:\n");
    printf("  ΔTick = %.2f ms\n", delta_tick_ms);
    printf("  ΔRTC  = %.2f s (%.2f ms)\n", delta_rtc_sec, expected_ms);
    printf("  Drift = %.2f ms (%.2f%%)\n", drift_ms, drift_pct);
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
