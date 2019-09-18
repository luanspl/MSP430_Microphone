//******************************************************************************
//
//Author: Luan Pena
//
//About: Change the Duty Cycle of the output PWM (using Timer B)
//
//
//
//
//*******************************************************************************
//  MSP430FR59x -
//  CPU Clock (Master Clock - MCLK) -> DCO (1MHZ)
//  Timer Clock (Auxiliary Clock - ACLK) -> LFXT (~32Khz)
//
//
//           MSP430FR5969
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |               |
//        |     P1.4/TB0.1|--> CCR1 - starts at 10% PWM up to 100%
//        |               |
//

//******************************************************************************
#include <msp430.h>


int main(void)
{
  WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

  //Clock

  // MASTER CLOCK (MCLK)
  CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
  CSCTL1 = DCOFSEL_0;                       // DCO = 1Mhz
  CSCTL2 = SELM__DCOCLK;                    // MCLK -> DCO
  CSCTL3 = DIVM__1;                         // Div / 1 (therefore the MCLK clock remains the same)

  //AUXILIARY CLOCK (ACLK)
  CSCTL2 |= SELA__LFXTCLK;                  // ACLK -> LFXTCLK (32Khz)
  CSCTL4 |= DIVA__1;                        // Div / 1


  // Configure GPIO
  P1DIR |= BIT4 | BIT0;                     // P1.4 output TIMER and P1.0 output
  P1SEL0 = BIT4;                            // special output for timer B0
  P1OUT = 0x00;                             // clear the output value

  // Disable the GPIO power-on default high-impedance mode to activate
  // previously configured port settings
  PM5CTL0 &= ~LOCKLPM5;


  //TIMER B

  TB0CTL = TBSSEL__ACLK | ID__1 | MC__UP | TBCLR | TBIE;             // TIMERB -> ACLK , DIV/1 , UP_MODE, Interrupt Enable
  TB0CCTL1 = OUTMOD_7;                                               // MODE RESET/SET

  //Duty Cycle
  TB0CCR0 = 1000-1;
  TB0CCR1 = 100;                                                    // Duty Cycle = (10%)


  __bis_SR_register(GIE);                                           //Enable Interrupt
  while(1){}
}

//TIMER B INTERRUPT (B1 VECTOR)
//PS: It's not B0 interrupt, it's B1 interrupt
#pragma vector=TIMER0_B1_VECTOR
__interrupt void Timer_B1(void){

    P1OUT ^= BIT0;                                                   // Output Led to verify if it is getting into the interrupt
    TB0CCR1 += 10;                                                   // Increasing 10 (1%) each (1000/32000)=0.003125 seconds

    //just a condition to not exceed the 100%
    if(TB0CCR1 > 990){
        TB0CCR1 = 100;
    }
    TB0CTL &= ~(TBIFG);                                               // Clear Flag

}

