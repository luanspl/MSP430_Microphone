/*
*
*
*Project: Read the ADC12 Value (Microphone-Sparkfun) and transmit it through Serial Port
*
*Author: Luan Pena
*
*
*/
//****************************************************************************
//
//  ACLK = 32768Hz, MCLK = DCO = ~1MHz, SMCLK = 1Mhz/32
//
//                MSP430FR5969
//  Vin= 5V    -----------------
//     |      |                 |
//    220     |                 |
//     |      |                 |
//     |      |                 |
//     -      |                 |
//     |      |                 |
//    390     |     P2.0/UCA0TXD|--> TX UART (COM9)
//     |      |                 |
//     --3.1V | A2=P1.2         |
//     |      |                 |
//     1K     |             P1.0|--> LED
//     |      |                 |
//    GND
//
//****************************************************************************
#include <msp430.h>
#include <stdint.h>

//ADC values
volatile unsigned int ADCvalue = 0;
volatile uint8_t ADCvH = 0;                 // 8-bit MSB value
volatile uint8_t ADCvL = 0;                 // 8-bit LSB value



int main(void)
{
  WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

  // Configure GPIO
  //UART
    P2SEL1 = BIT0 | BIT1;                    // Configure UART pins
    P2SEL0 &= ~(BIT0 | BIT1);
    PJSEL0 = BIT4 | BIT5;                    // Configure XT1 pins

  //ADC (P1.2)
  P1SEL0 |= BIT2;
  P1SEL1 |= BIT2;
  //LED
  P1DIR = BIT0;
  P1OUT = 0x00;                             // Clear Output

  // Disable the GPIO power-on default high-impedance mode to activate
  // previously configured port settings
  PM5CTL0 &= ~LOCKLPM5;


  // CLOCK

  // MASTER CLOCK (MCLK)                    // CPU CLOCK
  CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
  CSCTL1 = DCOFSEL_0;                       // DCO = 1Mhz
  CSCTL2 = SELM__DCOCLK;                    // MCLK -> DCO
  CSCTL3 = DIVM__1;                         // Div / 1 (therefore the MCLK clock remains the same)

  // AUXILIARY CLOCK (ACLK)                 // TIMER CLOCK
  CSCTL2 |= SELA__LFXTCLK;                  // ACLK -> LFXTCLK (~32Khz) = 32.768
  CSCTL3 |= DIVA__1;                        // Div / 1

  // SUB-MASTER CLOCK (SMCLK)                // ADC CLOCK
  CSCTL2 |= SELS__DCOCLK;                    // SMCLK -> DCO
  CSCTL3 |= DIVS__32;                        // Div /32 (1Mhz / 32) = 31.250 hz

  CSCTL4 &= ~LFXTOFF;                       // Enable LFXT1
        do
        {
          CSCTL5 &= ~LFXTOFFG;                    // Clear XT1 fault flag
          SFRIFG1 &= ~OFIFG;
        }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
        CSCTL0_H = 0;                             // Lock CS registers


  // UART
  // Configure USCI_A0 for UART mode
    UCA0CTL1 = UCSSEL__ACLK | UCSWRST;                  // Set ACLK = 32768 as UCBRCLK
    UCA0BR0 = 3;                              // 9600 baud
    UCA0BR1 = 0x00;
    UCA0MCTLW = 0x5300;                      // 32768/9600 - INT(32768/9600)=0.41
                                              // UCBRSx value = 0x53 (See UG)
    UCA0BR1 = 0;
    UCA0CTL1 &= ~UCSWRST;                     // release from reset
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt


  // Configure ADC12
  ADC12CTL0 = ADC12SHT0_2 | ADC12ON;                               //  16 cycles
  ADC12CTL1 = ADC12SSEL_3 | ADC12SHP;                              //  SMCLK
  ADC12CTL2 = ADC12RES__12BIT;                                     //  12 bit resolution
  ADC12MCTL0 = ADC12INCH_2 | ADC12DIF;                             // Channel2 ADC input select; Vref=AVCC
  ADC12IER0 = ADC12IE0;                                            // Enable ADC conv complete interrupt

  //1.953 Sps

  __bis_SR_register(GIE);                                          // Interrupt Enable

  while (1)
  {
    __delay_cycles(5000);
    ADC12CTL0 |= ADC12ENC | ADC12SC;                               // Start sampling/conversion
    //while (!(ADC12IFGR0));                                       // Wait the ADC time sampling


    //__bis_SR_register(GIE);                                        // Interrupt Enable
    __no_operation();                                              // For debugger
  }
}

//ADC Interrupt
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void){

    ADCvalue = ADC12MEM0;
    ADCvL = ADC12MEM0_L;
    ADCvH = ADC12MEM0_H;

    while(!(UCA0IFG & UCTXIFG));
                UCA0TXBUF = ADCvL;
    while(!(UCA0IFG & UCTXIFG));
                UCA0TXBUF = ADCvH;

    P1OUT |= BIT0;                              //ADC interrupt is working (turn LED on)

   // printf("ADC12MEM0 = %d");
   // fflush (stdout);

}
