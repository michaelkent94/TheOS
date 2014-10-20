#ifndef _RTC_H
#define _RTC_H 1

#include <stdint.h>
#include <stdbool.h>

void rtc_update();
uint8_t rtc_getHours();
uint8_t rtc_getMinutes();
uint8_t rtc_getSeconds();

bool rtc_getPm();

uint8_t rtc_getDay();
uint8_t rtc_getMonth();
uint32_t rtc_getYear();

#endif
