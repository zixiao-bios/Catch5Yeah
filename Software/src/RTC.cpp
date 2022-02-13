#include "RTC.h"

RTC_DS3231 rtc;

void rtc_init() {
    Wire.setPins(PIN_RTC_SDA, PIN_RTC_SCL);
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
    }
    if (rtc.lostPower()) {
        RTC_DS3231::adjust(DateTime(DATETIME_BEGIN_YEAR, DATETIME_BEGIN_MONTH, DATETIME_BEGIN_DAY, DATETIME_BEGIN_HOUR,
                            DATETIME_BEGIN_MIN, 0));
    }
}

DateTime rtc_get_time() {
    return RTC_DS3231::now();
}

void rtc_set_time(uint32_t timestamp) {
    // convert unix timestamp to GMT+8
    RTC_DS3231::adjust(DateTime(timestamp + 28800));
}

[[noreturn]] void get_time_task(void *pv) {
    DateTime now;
    while (true) {
        now = rtc_get_time();
        Serial.print(now.year(), DEC);
        Serial.print('/');
        Serial.print(now.month(), DEC);
        Serial.print('/');
        Serial.print(now.day(), DEC);
        Serial.print("\t");
        Serial.print(now.hour(), DEC);
        Serial.print(':');
        Serial.print(now.minute(), DEC);
        Serial.print(':');
        Serial.print(now.second(), DEC);
        Serial.println();
        delay(1000);
    }
}

void run_get_time_task() {
    xTaskCreatePinnedToCore(get_time_task, "GetTime", 1024, nullptr, 2, nullptr, 1);
}

uint32_t get_timestamp() {
    return RTC_DS3231::now().unixtime();
}

bool is_same_day(uint32_t ts1, uint32_t ts2) {
    DateTime dt1(ts1);
    DateTime dt2(ts2);
    return dt1.year() == dt2.year() and dt1.month() == dt2.month() and dt1.day() == dt2.day();
}
