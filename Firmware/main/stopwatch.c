#include "stopwatch.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <stdio.h>

static int64_t start_us = 0;
static int64_t pause_us = 0;
static int64_t accumulated_us = 0;
static bool running = false;
static uint8_t is_initialized = 1;

/* Lap handling */
static uint32_t last_lap_time_10ms = 0;
static lap_entry_t laps[MAX_LAPS];
static uint8_t lap_count = 0;

static uint32_t get_elapsed_10ms(void)
{
    int64_t total_us = accumulated_us;

    if (running) {
        total_us += (esp_timer_get_time() - start_us);
    }

    if (total_us < 0) total_us = 0;
    return (uint32_t)(total_us / 10000);
}

static void format_10ms(uint32_t t, char *buf, size_t len)
{
    uint32_t hundredths = t % 100;
    uint32_t seconds = (t / 100) % 60;
    uint32_t minutes = (t / 6000) % 60;
    uint32_t hours   = (t / 360000);

    snprintf(buf, len, "%02lu:%02lu:%02lu.%02lu",
             hours, minutes, seconds, hundredths); //HH:MM:SS.hh\0
}

void stopwatch_start(void)
{
    if (running) return;

    start_us = esp_timer_get_time();
    running = true;
    is_initialized = 0;
}

void stopwatch_stop(void)
{
    if (!running) return;

    pause_us = esp_timer_get_time();
    accumulated_us += (pause_us - start_us);
    running = false;
    is_initialized = 0;
}

uint8_t get_stopwatch_state()
{
    return (uint8_t)running;
}

void stopwatch_reset(void)
{
    start_us = 0;
    pause_us = 0;
    accumulated_us = 0;
    running = false;

    lap_count = 0;
    last_lap_time_10ms = 0;
    is_initialized = 1;
}

uint8_t stopwatch_is_initialized(void)
{
    return is_initialized;
}

void stopwatch_get_str_10ms(char *buf, size_t len)
{
    
    if (!buf || len == 0) return;
    format_10ms(get_elapsed_10ms(), buf, len);
    
    //ESP_LOGI("STOPWATCH", "Stopwatch time: %s", buf);
}

/* Record LAP + SPLIT */
bool stopwatch_lap(void)
{
    if (!running) return false;
    if (lap_count >= MAX_LAPS) return false;

    uint32_t now_10ms = get_elapsed_10ms();

    laps[lap_count].split_10ms = now_10ms;
    laps[lap_count].lap_10ms   = now_10ms - last_lap_time_10ms;

    last_lap_time_10ms = now_10ms;
    lap_count++;

    return true;
}

/* Number of laps */
uint8_t stopwatch_get_lap_count(void)
{
    return lap_count;
}

/* Get a lap entry */
bool stopwatch_get_lap(uint8_t index, lap_entry_t *lap)
{
    if (!lap || index >= lap_count) return false;
    *lap = laps[index];
    return true;
}

bool stopwatch_get_lap_str(uint8_t index, char *buf, size_t len)
{
    if (!buf || index >= lap_count) return false;
    format_10ms(laps[index].lap_10ms, buf, len);
    return true;
}

bool stopwatch_get_split_str(uint8_t index, char *buf, size_t len)
{
    if (!buf || index >= lap_count) return false;
    format_10ms(laps[index].split_10ms, buf, len);
    return true;
}