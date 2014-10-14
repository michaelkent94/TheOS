#include <kernel/rtc.h>
#include <asm/io.h>

#define CURRENT_YEAR        2014                            // Change this each year!

int century_register = 0x00;       // Set by ACPI table parsing code if possible

uint8_t second;
uint8_t minute;
uint8_t hour;
uint8_t day;
uint8_t month;
uint32_t year;

bool pm;

int time_zone = +4;

int days_per_month[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

enum {
	cmos_address = 0x70, cmos_data = 0x71
};

int get_update_in_progress_flag() {
	outb(cmos_address, 0x0A);
	return (inb(cmos_data) & 0x80);
}

unsigned char get_RTC_register(int reg) {
	outb(cmos_address, reg);
	return inb(cmos_data);
}

void read_rtc() {
	uint8_t century;
	uint8_t last_second;
	uint8_t last_minute;
	uint8_t last_hour;
	uint8_t last_day;
	uint8_t last_month;
	uint8_t last_year;
	uint8_t last_century;
	uint8_t registerB;

	// Note: This uses the "read registers until you get the same values twice in a row" technique
	//       to avoid getting dodgy/inconsistent values due to RTC updates

	while (get_update_in_progress_flag());                // Make sure an update isn't in progress
	second = get_RTC_register(0x00);
	minute = get_RTC_register(0x02);
	hour = get_RTC_register(0x04);
	day = get_RTC_register(0x07);
	month = get_RTC_register(0x08);
	year = get_RTC_register(0x09);
	if (century_register != 0) {
		century = get_RTC_register(century_register);
	}

	do {
		last_second = second;
		last_minute = minute;
		last_hour = hour;
		last_day = day;
		last_month = month;
		last_year = year;
		last_century = century;

		while (get_update_in_progress_flag())
			;           // Make sure an update isn't in progress
		second = get_RTC_register(0x00);
		minute = get_RTC_register(0x02);
		hour = get_RTC_register(0x04);
		day = get_RTC_register(0x07);
		month = get_RTC_register(0x08);
		year = get_RTC_register(0x09);
		if (century_register != 0) {
			century = get_RTC_register(century_register);
		}
	} while ((last_second != second) || (last_minute != minute)
			|| (last_hour != hour) || (last_day != day) || (last_month != month)
			|| (last_year != year) || (last_century != century));

	registerB = get_RTC_register(0x0B);

	// Convert BCD to binary values if necessary
	if (!(registerB & 0x04)) {
		second = (second & 0x0F) + ((second / 16) * 10);
		minute = (minute & 0x0F) + ((minute / 16) * 10);
		hour = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);
		day = (day & 0x0F) + ((day / 16) * 10);
		month = (month & 0x0F) + ((month / 16) * 10);
		year = (year & 0x0F) + ((year / 16) * 10);
		if (century_register != 0) {
			century = (century & 0x0F) + ((century / 16) * 10);
		}
	}

	// Convert hour to the correct time zone
	// the values are unsigned so we can't make them go negative
	if (hour < time_zone) {
		hour = 24 - (time_zone - hour);
		// we need to move back a day too
		if (day == 1) {
			if (month == 1) {
				month = 12;
				year--;
			} else {
				month--;
			}

			day = days_per_month[month];
		} else {
			day--;
		}
	} else {
		hour -= time_zone;
	}

	hour %= 24;

	// get am/pm
	if (hour > 12) {
		pm = true;
	} else {
		pm = false;
	}

	// Convert 12 hour clock to 24 hour clock if necessary
	if ((registerB & 0x02) && hour > 12) {
		hour = ((hour & 0x7F) + 12) % 24;
	}

	if (hour == 0) {
		hour = 12;
	}

	// Calculate the full (4-digit) year

	if (century_register != 0) {
		year += century * 100;
	} else {
		year += (CURRENT_YEAR / 100) * 100;
		if (year < CURRENT_YEAR)
			year += 100;
	}
}

void rtc_update() {
	read_rtc();
}

uint8_t rtc_getHours() {
	return hour;
}

uint8_t rtc_getMinutes() {
	return minute;
}

uint8_t rtc_getSeconds() {
	return second;
}

bool rtc_getPm() {
	return pm;
}

uint8_t rtc_getDay() {
	return day;
}

uint8_t rtc_getMonth() {
	return month;
}

uint32_t rtc_getYear() {
	return year;
}
