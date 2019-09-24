/*
*
*Author: Luan Pena
*
*
*Subject: Change the PWM output Duty Cycle according to the ADC input (Microphone Sparkfun)
*
*
*/
//****************************************************************************
//
//       MCLK -> DCO = 1MHZ, ACLK -> LFXTCLK (~32Khz) = 32.768 Hz, SMCLK -> DCO (1Mhz / 32) = 31.250 hz
//
//                MSP430FR5969
//  Vin= 5V    -----------------
//     |      |                 |
//     |      |                 |
//     1K     |                 |
//     |      |                 |
//     |      |                 |
//     -      |                 |
//     |      |                 |
//     |3.45V |                 |
//     -----> | A2= P1.2        |
//     |      |                 |
//     2.2K   |             P1.4|--> PWM Output
//     |      |                 |
//    GND
//
//
//
// Date: 08/17/2019
//
//****************************************************************************
#include <msp430.h>
#include <stdint.h>

volatile unsigned uint16_t result = 0;

int main(void)
{
  WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

  // Configure GPIO
  P1DIR = BIT0 | BIT4;
  P1SEL0 = BIT4;                            // PWM output
  //ADC (P1.2)
  P1SEL0 |= BIT2;
  P1SEL1 = BIT2;
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
  CSCTL2 |= SELA__LFXTCLK;                  // ACLK -> LFXTCLK (~32Khz) = 32.768 Hz
  CSCTL3 |= DIVA__1;                        // Div / 1

  // SUB-MASTER CLOCK (SMCLK)                // ADC CLOCK
  CSCTL2 |= SELS__DCOCLK;                    // SMCLK -> DCO
  CSCTL3 |= DIVS__32;                        // Div /32 (1Mhz / 32) = 31.250 hz



  // Configure PWM output (TIMER B)
  TB0CTL = TBSSEL__ACLK | ID__1 | MC__UP | TBCLR | TBIE;             // TIMERB -> ACLK , DIV/1 , UP_MODE, Interrupt Enable
  TB0CCTL1 = OUTMOD_7;                                               // MODE RESET/SET

  //Duty Cycle
  TB0CCR0 = 1000-1;
  TB0CCR1 = 100;                                                    // Duty Cycle = (10%) - initial duty cycle


  // Configure ADC12
  ADC12CTL0 = ADC12SHT0_2 | ADC12ON;                               //  16 cycles
  ADC12CTL1 = ADC12SSEL_3 | ADC12SHP;                              //  ADC12CLK -> SMCLK
  ADC12CTL2 = ADC12RES__12BIT;                                     //  12 bit resolution
  ADC12MCTL0 = ADC12INCH_2 | ADC12DIF;                             // Channel2 ADC input select; Vref=AVCC
  ADC12IER0 = ADC12IE0;                                            // Enable ADC conv complete interrupt

  while (1)
  {
    __delay_cycles(5000);
    ADC12CTL0 |= ADC12ENC | ADC12SC;                               // Start sampling/conversion

    __bis_SR_register(GIE);                                        // Interrupt Enable
    __no_operation();                                              // For debugger
  }
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void){
    //ADC12MEM0
    P1OUT ^= BIT0;                                                   // Output Led to verify if the ADC interrupt is OK

    result = ADC12MEM0;
    if(result>=0x0340 && result < 0x0A80){                     //  ADC >= 822 AND ADC < 2688
        TB0CCR1 = 450;                  //   PWM Duty Cycle (45%)
    }
    else if(result>=0x0A80){                                      // ADC > = 2688
        TB0CCR1 = 900;                  //   PWM Duty Cycle (90%)
    }
    else{
        TB0CCR1 = 100;                  //   PWM Duty Cycle (10%)
}

}

//TIMER B INTERRUPT (B1 VECTOR)
//PS: It's not B0 interrupt, it's B1 interrupt
#pragma vector=TIMER0_B1_VECTOR
__interrupt void Timer_B1(void){

    TB0CTL &= ~(TBIFG);                                               // Clear Flag

}
