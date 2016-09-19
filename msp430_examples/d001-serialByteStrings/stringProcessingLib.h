/*
 * stringProcessingLib.h
 *
 *  Created on: 18 Sep 2016
 *      Author: andy
 */

#ifndef STRINGPROCESSINGLIB_H_
#define STRINGPROCESSINGLIB_H_

#include <stdint.h>

/*
 * References
 *
 * http://www.asciitable.com
 *
 * */

/******************************************************************************
 *
 * */
#define MAX_RXTX_BUFFERSIZE 80
#define MAX_RXTX_BUFFERSIZE_minusONE (MAX_RXTX_BUFFERSIZE - 1)

/*
 * General purpose buffer that we can use for RXTX comms. Contains direct
 * support flags for flow control so we can use directly in Interrupt Routine
 *
 * */
struct rxTxBuffer {
	uint8_t maxsize;
	uint8_t lastPosition;
	uint8_t position;
	uint8_t cursize;
	unsigned char buffer[MAX_RXTX_BUFFERSIZE];
};

/* Buffer setup/config */
void initRxTxBuffer(struct rxTxBuffer * rxtx);
void fillBufferWithOverrunProtection(struct rxTxBuffer * rxtx,
		unsigned char * inString);
void addToNullTermBuffersCurrPos(struct rxTxBuffer * rxtx,
		unsigned char * inString);
int loadBufferWithDecimalStringRetLength(struct rxTxBuffer * rxtx,
		uint16_t integerInput);

/* String to Byte conversions */
uint16_t charHexStringToUint(char * string, int length);
int charBufferStringToUint16(char * string, int length);
int fillBufferWithNulls(char * buffer, int length);
int myItoaU16(uint16_t val, char *str, uint16_t limit, uint8_t endWithNull);

int uintToHexString(char *, uint16_t, int);
int uint16ToHexString(char *, uint16_t);
int uint8ToHexString(char *, uint16_t);

/* Visual check routines. Test over UART with external script */
int test_uint16ToHexString(struct rxTxBuffer * rxtx, uint16_t value);
int test_Search2Commas_charHexStringToUintuint16ToHexString(
		struct rxTxBuffer * rxtx, char * exampleStr, uint16_t offset,
		uint16_t width);
int test_charHexStringToUintuint16ToHexString(struct rxTxBuffer * rxtx,
		char * exampleStr, uint16_t offset, uint16_t width);

/* Custom search routines */
int16_t searchForTwoCommas(char * string, int max);

/*
 * MSP430 Specific routines that call registers directly
 *
 * */
/* Bundelled */
void copyAndSendString_UCA0(struct rxTxBuffer * rxtx, unsigned char * inString);
void uint16ToStringSendOn_UCA0(struct rxTxBuffer * rxtx, uint16_t value);

/* direct register access */
void sendBuffer_UCA0(struct rxTxBuffer * rxtx);

#endif /* STRINGPROCESSINGLIB_H_ */
