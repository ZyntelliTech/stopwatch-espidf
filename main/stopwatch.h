#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_LAPS 10   // configurable

typedef struct {
    uint32_t split_10ms;  // absolute time
    uint32_t lap_10ms;    // delta since previous lap
} lap_entry_t;

void stopwatch_start(void);
void stopwatch_stop(void);
void stopwatch_reset(void);
uint8_t get_stopwatch_state();

/* formatted string MM:SS.hh */
void stopwatch_get_str_10ms(char *buf, size_t len);

/* Lap / split functions */
bool stopwatch_lap(void);
uint8_t stopwatch_get_lap_count(void);
bool stopwatch_get_lap(uint8_t index, lap_entry_t *out);

/* OLED string helpers */
bool stopwatch_get_lap_str(uint8_t index, char *buf, size_t len);
bool stopwatch_get_split_str(uint8_t index, char *buf, size_t len);
#endif
/* STOPWATCH_H */   