#ifndef RTC_TIME_H
#define RTC_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set RTC date & time
 *
 * @param year  Full year (e.g. 2026)
 * @param month Month (1–12)
 * @param day   Day (1–31)
 * @param hour  Hour (0–23)
 * @param min   Minute (0–59)
 * @param sec   Second (0–59)
 */
void rtc_set_time(int year, int month, int day,
                  int hour, int min, int sec);

/**
 * @brief Get and print current RTC date & time
 *
 * Output format:
 * YYYY-MM-DD HH:MM:SS
 */
void rtc_get_time(char *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* RTC_TIME_H */
