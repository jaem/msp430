/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP430FR231x Demo -  eUSCI_A0 UART echo at 9600 baud using BRCLK = 8MHz.
//
//  Description: This demo echoes back characters received via a PC serial port.
//  SMCLK/ DCO is used as a clock source and the device is put in LPM3
//  The auto-clock enable feature is used by the eUSCI and SMCLK is turned off
//  when the UART is idle and turned on when a receive edge is detected.
//  Note that level shifter hardware is needed to shift between RS232 and MSP
//  voltage levels.
//
//  The example code shows proper initialization of registers
//  and interrupts to receive and transmit data.
//  To test code in LPM3, disconnect the debugger.
//
//  ACLK = REFO = 32768Hz, MCLK = DCODIV = SMCLK = 8MHz.
//
//                MSP430FR2311
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//            |                 |
//            |     P1.7/UCA0TXD|----> PC (echo)
//            |     P1.6/UCA0RXD|<---- PC
//            |                 |
//
//   Darren Lu
//   Texas Instruments Inc.
//   Oct. 2015
//   Built with IAR Embedded Workbench v6.30 & Code Composer Studio v6.1
//******************************************************************************
#include <msp430.h>
#include <stdint.h>

void Init_GPIO();
void Init_MSP430();
#include "stringProcessingLib.h"
#include "gpReadings.h"

/******************************************************************************
 *
 * */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;                // Stop watchdog timer
    Init_MSP430();
    struct rxTxBuffer rxtx;
    struct rxTxBuffer rxtxSpare;
    struct readings readings_s;
    initRxTxBuffer(&rxtx);

    int counter = 0;

    while (1) {
        char * exampleStr = "  kjsdk,sdsd,112233445566778899aAbBcCdDeEfF";

        // process strings, thats what I need to do.....
        uint16ToStringSendOn_UCA0(&rxtx, counter++);

        test_charHexStringToUintuint16ToHexString(&rxtx, "134", 0, 3);
        test_charHexStringToUintuint16ToHexString(&rxtx, "fF", 0, 2);
        test_charHexStringToUintuint16ToHexString(&rxtx, "A", 0, 1);

        test_Search2Commas_charHexStringToUintuint16ToHexString(&rxtx, exampleStr, 0, 4);

        test_uint16ToHexString(&rxtx, 32);
        test_uint16ToHexString(&rxtx, 0xffff);
        test_uint16ToHexString(&rxtx, 0x15BF);

        copyAndSendString_UCA0(&rxtx, (unsigned char *) " == Maggie\n\r");

        readings_s.yearFrom2000 = 16;
        readings_s.month = 8;
        readings_s.day = 30;
        readings_s.hour = 23;
        readings_s.min = 59;
        readings_s.sec = 50;
        readings_s.windspeed = 3214;
        readings_s.windDirection = 12;
        readings_s.rssi = 9;
        readings_s.txPower = 5;

        test_buildingHexStringFromReadings(&rxtx, "sendReadings,thisGoat,", &readings_s);

        copyAndSendString_UCA0(&rxtxSpare, (unsigned char *) "----");

        struct readings readings2_s;
        test_unpackingHexStringFromReadings(&rxtx, &readings2_s);

        test_buildingHexStringFromReadings(&rxtx, "sendReadings,thisGoat222,", &readings2_s);
        copyAndSendString_UCA0(&rxtxSpare, (unsigned char *) "\n\r");

    }

//	__bis_SR_register(LPM3_bits | GIE);        // Enter LPM3, interrupts enabled
//	__no_operation();                         // For debugger
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch (__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG)) {
    case USCI_NONE:
        break;
    case USCI_UART_UCRXIFG:
        while (!(UCA0IFG & UCTXIFG))
            ;
        UCA0TXBUF = UCA0RXBUF;
        __no_operation();
        break;
    case USCI_UART_UCTXIFG:
        break;
    case USCI_UART_UCSTTIFG:
        break;
    case USCI_UART_UCTXCPTIFG:
        break;
    default:
        break;
    }
}

void Init_MSP430() {
#ifdef __MSP430FR2311__

    // Configure GPIO
    Init_GPIO();

    PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode
                          // to activate 1previously configured port settings

    __bis_SR_register(SCG0);                 // disable FLL
    CSCTL3 |= SELREF__REFOCLK;               // Set REFO as FLL reference source
    CSCTL0 = 0;                              // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);             // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_3;                     // Set DCO = 8MHz
    CSCTL2 = FLLD_0 + 243;                   // DCODIV = 8MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);                 // enable FLL
    while (CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1))
        ; // Poll until FLL is locked

    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK; // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                               // default DCODIV as MCLK and SMCLK source

    // Configure UART pins
    P1SEL0 |= BIT6 | BIT7;                  // set 2-UART pin as second function

    // Configure UART
    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 |= UCSSEL__SMCLK;

    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 17-4: UCBRSx = 0x49
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA0BR0 = 52;                             // 8000000/16/9600
    UCA0BR1 = 0x00;
    UCA0MCTLW = 0x4900 | UCOS16 | UCBRF_1;

    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
#else
            // Populate as required
#endif
}

void Init_GPIO() {
    P1DIR = 0xFF;
    P2DIR = 0xFF;
    P1REN = 0xFF;
    P2REN = 0xFF;
    P1OUT = 0x00;
    P2OUT = 0x00;
}
