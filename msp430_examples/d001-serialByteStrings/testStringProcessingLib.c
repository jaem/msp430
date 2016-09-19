/*
 * testStringProcessingLib.c
 *
 *  Created on: 18 Sep 2016
 *      Author: andy
 */

#include "gpReadings.h"
#include "stringProcessingLib.h"
#include <msp430.h>

void packHexStringFrom_readings(struct rxTxBuffer * rxtx, unsigned char * initStr, struct readings * readings_s) {
    char confBuffer[8];
    fillBufferWithOverrunProtection(rxtx, initStr);
    uint8ToHexString(confBuffer, readings_s->yearFrom2000);
    addToNullTermBuffersCurrPos(rxtx, confBuffer);
    uint8ToHexString(confBuffer, readings_s->month);
    addToNullTermBuffersCurrPos(rxtx, confBuffer);
    uint8ToHexString(confBuffer, readings_s->day);
    addToNullTermBuffersCurrPos(rxtx, confBuffer);
    uint8ToHexString(confBuffer, readings_s->hour);
    addToNullTermBuffersCurrPos(rxtx, confBuffer);
    uint8ToHexString(confBuffer, readings_s->min);
    addToNullTermBuffersCurrPos(rxtx, confBuffer);
    uint8ToHexString(confBuffer, readings_s->sec);
    addToNullTermBuffersCurrPos(rxtx, confBuffer);
    uint16ToHexString(confBuffer, readings_s->windspeed);
    addToNullTermBuffersCurrPos(rxtx, confBuffer);
    uint8ToHexString(confBuffer, readings_s->windDirection);
    addToNullTermBuffersCurrPos(rxtx, confBuffer);
    uint8ToHexString(confBuffer, readings_s->rssi);
    addToNullTermBuffersCurrPos(rxtx, confBuffer);
    uint8ToHexString(confBuffer, readings_s->txPower);
    addToNullTermBuffersCurrPos(rxtx, confBuffer);
    sendBuffer_UCA0(rxtx);
}

void unpackHexStringFrom_readings(struct rxTxBuffer * rxtx, struct readings * readings_s) {
    uint16_t working;
    working = searchForTwoCommas(rxtx->buffer, 30);

    readings_s->yearFrom2000 = charHexStringToUint((rxtx->buffer + working + 0), 2);
    readings_s->month = charHexStringToUint((rxtx->buffer + working + 2), 2);
    readings_s->day = charHexStringToUint((rxtx->buffer + working + 4), 2);
    readings_s->hour = charHexStringToUint((rxtx->buffer + working + 6), 2);
    readings_s->min = charHexStringToUint((rxtx->buffer + working + 8), 2);
    readings_s->sec = charHexStringToUint((rxtx->buffer + working + 10), 2);
    readings_s->windspeed = charHexStringToUint((rxtx->buffer + working + 12), 4);
    readings_s->windDirection = charHexStringToUint((rxtx->buffer + working + 16), 2);
    readings_s->rssi = charHexStringToUint((rxtx->buffer + working + 18), 2);
    readings_s->txPower = charHexStringToUint((rxtx->buffer + working + 20), 2);

}

void test_buildingHexStringFromReadings(struct rxTxBuffer * rxtx, unsigned char * initStr, struct readings * readings_s) {
    packHexStringFrom_readings(rxtx, initStr, readings_s);
}

void test_unpackingHexStringFromReadings(struct rxTxBuffer * rxtx, struct readings * readings_s) {
    unpackHexStringFrom_readings(rxtx, readings_s);
}

//sendReadings,thisGoat,00100008001E0017003B00320C8E0C000900051 == 308 == 255 == 10 == 4386 == 0020 == FFFF == 15BF == Maggie

