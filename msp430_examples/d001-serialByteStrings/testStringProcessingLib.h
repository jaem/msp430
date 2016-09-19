/*
 * testStringProcessingLib.h
 *
 *  Created on: 18 Sep 2016
 *      Author: andy
 */

#ifndef TESTSTRINGPROCESSINGLIB_H_
#define TESTSTRINGPROCESSINGLIB_H_

#include <stdint.h>
#include "stringProcessingLib.h"
#include "gpReadings.h"

void test_buildingHexStringFromReadings(struct rxTxBuffer * rxtx, unsigned char * initStr, struct readings * readings_s);


#endif /* TESTSTRINGPROCESSINGLIB_H_ */
