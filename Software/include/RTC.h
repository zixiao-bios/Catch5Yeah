#ifndef SOFTWARE_RTC_H
#define SOFTWARE_RTC_H

#include <Wire.h>
#include <RTClib.h>
#include "../Config.h"

void rtc_init();

DateTime rtc_get_time();

void rtc_set_time(uint32_t timestamp);

void run_get_time_task();

uint32_t get_timestamp();

// return whether two timestamp is a same day or not
bool is_same_day(uint32_t ts1, uint32_t ts2);

#endif //SOFTWARE_RTC_H
