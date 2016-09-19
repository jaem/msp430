#include "stringProcessingLib.h"
#include <msp430.h>

void initRxTxBuffer(struct rxTxBuffer * rxtx) {
	rxtx->maxsize = MAX_RXTX_BUFFERSIZE;
	rxtx->lastPosition = MAX_RXTX_BUFFERSIZE_minusONE;
	rxtx->position = 0;
	rxtx->cursize = 0;
}

/******************************************************************************
 * Send a string. We can pass the MAX string size to prevent oversend
 *
 * */
void fillBufferWithOverrunProtection(struct rxTxBuffer * rxtx,
		unsigned char * inString) {
	rxtx->position = 0;
	rxtx->cursize = 0;
	while (rxtx->position != rxtx->maxsize) {
		if (*(inString + rxtx->cursize) != 0) {
			rxtx->buffer[rxtx->position] = *(inString + rxtx->cursize);
			rxtx->position++;
			rxtx->cursize++;
		} else {
			// Initialise the rest of the buffer with 0
			rxtx->buffer[rxtx->position] = 0;
			rxtx->position++;
		}
	}
	rxtx->buffer[rxtx->maxsize - 1] = 0; // Force the final char to be a NULL
}

/******************************************************************************
 * Send a string. We can pass the MAX string size to prevent oversend
 *
 * */
void addToNullTermBuffersCurrPos(struct rxTxBuffer * rxtx,
		unsigned char * inString) {
	rxtx->position = rxtx->cursize;
	while (rxtx->position != rxtx->maxsize) {
		if (*inString != 0) {
			rxtx->buffer[rxtx->position] = *inString++;
			rxtx->position++;
			rxtx->cursize++;
		} else {
			// Buffer should be NULL terminated, so just exit by forcing position to MAX.
			rxtx->position = rxtx->maxsize;
		}
	}
	rxtx->buffer[rxtx->lastPosition] = 0; // Force the final char to be a NULL
}

/* This function has no protection built in to check that buffers are not over-running.
 * Keep this in mind when passing value. Ints on the MSP430 should be 16 bit so there should
 * be a requirement for only 6 chars including the NULL terminator.
 * */
int loadBufferWithDecimalStringRetLength(struct rxTxBuffer * rxtx,
		uint16_t integerInput) {

	// The 10 is the number base, 10 being decimal, see itoa C++ reference for more detail
	return myItoaU16(integerInput, rxtx->buffer, 0xffff, 1);

}

/* Convert a HEX string to 16 bit integer on an MSP platform
 * */
uint16_t charHexStringToUint(char * string, int length) {
	uint16_t value = 0;
	uint8_t tmp = 0;

	while (length-- > 0) {
		tmp = *string;
		if (tmp > 0x60) {
			tmp -= 'a';
			tmp += 10;
		} else {
			if (tmp > 0x40) {
				tmp -= 'A';
				tmp += 10;
			} else {
				tmp -= '0';
			}
		}
		value += tmp << (length * 4);
		*string++;
	}

	return value;
}

/*
 * Search for 2 commas in a string. Use this as position indication of data
 * */
int16_t searchForTwoCommas(char * string, int max) {
	uint16_t count = 0;
	uint16_t commas = 0;
	int16_t returnValue = -1;

	while ((*string != 0) & (count < max)) {
		count++; // Inc straight away

		if (*string == ',') {
			commas++; // Count the commas
		}

		if (commas == 2) {
			// we have 2 commas, exit the while loop by forcing count to max
			returnValue = count;
			count = max;
		}

		*string++; // next char to search!
	}

	return returnValue;
}

/*
 * Simple method to convert a char buffer to a 16 bit integer on an MSP platform
 *
 * */
int charBufferStringToUint16(char * string, int length) {
	int value = 0;
	uint16_t myArray[5] = { 1, 10, 100, 1000, 10000 };

	while (length-- > 0) {
		value += (*string++ - '0') * myArray[length];
	}

	return value;
}

/*
 * Simple method to convert a char buffer to a 16 bit integer on an MSP platform
 *
 * */
int uintToHexString(char * buffer, uint16_t value, int bytes) {
	uint8_t tmp = 0;
	while (bytes-- > 0) {

		tmp = (value >> (4 * bytes)) & 0xF;
		if (tmp < 10) {
			tmp += '0';
		} else {
			tmp += 'A' - 10;
		}
		*buffer++ = tmp;
	}
	*buffer++ = 0;

	return value;
}

/*
 * Simple method to convert a char buffer to a 16 bit integer on an MSP platform
 *
 * */
int uint16ToHexString(char * buffer, uint16_t value) {
	return uintToHexString(buffer, value, 4);
}

int uint8ToHexString(char * buffer, uint16_t value) {
	return uintToHexString(buffer, value, 2);
}

/*
 * */
int fillBufferWithNulls(char * buffer, int length) {

	int count = 0;
	while (count++ != length) {
		*buffer++ = 0;
	}
	return count;
}

// base-10 itoa for positive numbers. Populates str with a null-terminated string.
// limit lets you overflow back to 1 when you hit the limit+1, 2*limit+1, ...
// make sure *str is an array that can hold the number of digits in your limit + 1.
int myItoaU16(uint16_t val, char *str, uint16_t limit, uint8_t endWithNull) {
	int temploc = 0;
	int digit = 0;
	int strloc = 0;
	char tempstr[5]; //16-bit number can be at most 5 ASCII digits;

	if (val > limit)
		val %= limit;

	do {
		digit = val % 10;
		tempstr[temploc++] = digit + '0';
		val /= 10;
	} while (val > 0);

	// reverse the digits back into the output string
	while (temploc > 0) {
		*str++ = tempstr[--temploc];
		strloc++;
	}
	if (endWithNull == 1) {
		*str = 0;
	}
	return strloc;
}

/******************************************************************************
 *
 * MSP430 Specific section. Label these with the correct UC(A/B)(0-N)
 * No not use macros to redefine register calls. Less code but confusing to
 * read. Prefer seperate file if this shows to big.
 *
 * */

/*
 * Bundelled calls
 * */
void uint16ToStringSendOn_UCA0(struct rxTxBuffer * rxtx, uint16_t value) {
	loadBufferWithDecimalStringRetLength(rxtx, value);
	sendBuffer_UCA0(rxtx);
}

void copyAndSendString_UCA0(struct rxTxBuffer * rxtx, unsigned char * inString) {
	fillBufferWithOverrunProtection(rxtx, inString);
	sendBuffer_UCA0(rxtx);
}

/*
 * Direct IO register calls
 * */
void sendBuffer_UCA0(struct rxTxBuffer * rxtx) {
	rxtx->position = 0;
	rxtx->buffer[rxtx->lastPosition] = 0; // Force this to be NULL. We will never overrun.
	// This might not protect against all, but will limit printing of string to port.
	while (*(rxtx->buffer + rxtx->position) != 0) {
		while (!(UCA0IFG & UCTXIFG))
			; // Wait until we are ready to send
		UCA0TXBUF = (unsigned int) *(rxtx->buffer + rxtx->position++);
	}
}

/*
 * Test helper...
 *
 *
 * */
int test_uint16ToHexString(struct rxTxBuffer * rxtx, uint16_t value) {
	copyAndSendString_UCA0(rxtx, (unsigned char *) " == ");
	uint16ToHexString(rxtx->buffer, value);
	sendBuffer_UCA0(rxtx);
	return 0;
}

int test_charHexStringToUintuint16ToHexString(struct rxTxBuffer * rxtx,
		char * exampleStr, uint16_t offset, uint16_t width) {
	uint16_t working;
	copyAndSendString_UCA0(rxtx, (unsigned char *) " == ");
	working = charHexStringToUint((exampleStr + offset), width);
	loadBufferWithDecimalStringRetLength(rxtx, working);
	sendBuffer_UCA0(rxtx);
	return 0;
}

int test_Search2Commas_charHexStringToUintuint16ToHexString(
		struct rxTxBuffer * rxtx, char * exampleStr, uint16_t offset,
		uint16_t width) {
	uint16_t working;
	copyAndSendString_UCA0(rxtx, (unsigned char *) " == ");
	working = searchForTwoCommas(exampleStr, 20);
	working = charHexStringToUint((exampleStr + working + offset), width);
	loadBufferWithDecimalStringRetLength(rxtx, working);
	sendBuffer_UCA0(rxtx);
	return 0;
}
