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

#endif //SOFTWARE_RTC_H
