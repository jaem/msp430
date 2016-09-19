/*
 * gpReadings.h
 *
 *  Created on: 18 Sep 2016
 *      Author: andy
 */

#ifndef GPREADINGS_H_
#define GPREADINGS_H_

#include <stdint.h>

struct readings {
    uint8_t yearFrom2000;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t windspeed;
    uint8_t windDirection;
    uint8_t rssi;
    uint8_t txPower;
};

#endif /* GPREADINGS_H_ */
