#include "Communication.h"
#include <msp430.h>



//Configure the UART transmission GPIO PORT
void config_GPIO_Tx_UART(){
    P2SEL1 |= BIT0 | BIT1;                    // Configure UART pins
    P2SEL0 &= ~(BIT0 | BIT1);
    PJSEL0 |= BIT4 | BIT5;                    // Configure XT1 pins

}

//Configure the Setup for UART
void config_setup_UART(){
    //unfortunately this clock adjustment is necessary
         // XT1 Setup
         CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
         CSCTL1 = DCOFSEL_0;                       // Set DCO to 1MHz
         CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
         CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
         CSCTL4 &= ~LFXTOFF;                       // Enable LFXT1
         do
         {
           CSCTL5 &= ~LFXTOFFG;                    // Clear XT1 fault flag
           SFRIFG1 &= ~OFIFG;
         }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
         CSCTL0_H = 0;                             // Lock CS registers
}


//Setting the UART
void config_UART(){
    //UART

        UCA0CTL1 = (UCSSEL__ACLK | UCSWRST); //all the standard configuration
        //No parity bit, 8 bit data, LSB First, one Stop Bit.
        //Clock ACLK (32.768 hz) and start held in reset state (UCSWRST)

        //BOUD RATE
        //Ps: For higher clock frequencies (8Mz) we need to Oversampling, but in this case we don't need it.
        //Clock=32.768 hz and Baud rate = 9600
        // N=clock/baud rate = 32768/9600 = 3.4133  as N is lower than 16, so  we follow the calc N = (N - int(N))
        // N = 3.4133 - int(3.4133), N= 3.4133 - 3 = 0.4133.   N=0.4133
        // According to 30-4 table (page 776) - the closest N value is 0.4286, Therefore UCBRSx=0x53

        UCA0MCTLW = 0x5300; //set the UCBRSx = 0x53

        //without oversampling, the USBRx = int(N), so int(3.41). USBRx = 3.
        UCA0BR0 = 3; //UDBRx (Clock Pre-scale)
        UCA0BR1 = 0x00;

        //Rx or Tx interrupts
        UCA0CTL1 &= ~(UCSWRST); // Initialize eUSCI (clear the reset)
        UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
        //UCA0IE = UCTXIE; //Transmit Interrupt Enable


}


//Tx data
void Tx_UART(volatile unsigned char Data){
    while(!(UCA0IFG & UCTXIFG));
           UCA0TXBUF = Data;                     // Load data onto buffer

    _delay_cycles(5);
}
