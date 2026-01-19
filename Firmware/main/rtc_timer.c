#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "esp_log.h"
#include "rtc_timer.h"

static const char *TAG = "RTC";

/* Set RTC date & time */
void rtc_set_time(int year, int month, int day,
                  int hour, int min, int sec)
{
    struct tm tm_time = {
        .tm_year = year - 1900,  // years since 1900
        .tm_mon  = month - 1,    // 0-11
        .tm_mday = day,
        .tm_hour = hour,
        .tm_min  = min,
        .tm_sec  = sec
    };

    time_t t = mktime(&tm_time);
    struct timeval now = {
        .tv_sec = t,
        .tv_usec = 0
    };

    settimeofday(&now, NULL);
    ESP_LOGI(TAG, "RTC time set successfully");
}

/* Get RTC date & time */
void rtc_get_time(char *buf, size_t len)
{
    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);
    ESP_LOGI(TAG,
        "RTC Time: %04d-%02d-%02d %02d:%02d:%02d",
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec);
    
        snprintf(buf, len, "%02d:%02d:%02d",
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}
