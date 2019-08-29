#include "HAL_Microphone.h"
#include <msp430.h>


volatile unsigned int result = 0;

    // PORT SETTINGS

//Config GPIO PORT P1.0 as an output for LED
void config_GPIO_output_LED(){
    P1REN |= BIT0; //Setting the P1.0 as an output (LED)
    P1OUT = 0x00; //Clear the pins (initialization)
}

//Config GPIO PORT p1.2 as an input analog ADC
void config_GPIO_input_MIC(){
    P1SEL1 |= BIT2;                    // Configure ADC inputs A2
    P1SEL0 |= BIT2;

    PM5CTL0 &= ~LOCKLPM5;     // Disable the GPIO power-on default high-impedance mode to activate


}

    //ADC INPUT SETTINGS

//Config the ADC12
void config_ADC12(){
    //ADC
       // ADC12CTL0 &= ~(ADC12ENC); //Conversion Disabled for now
        ADC12CTL0 |= (ADC12SHT0_15 | ADC12ON); //Sampling time (16 cycles) and Turn on the ADC
        ADC12CTL1 = ADC12SHP;                     // Use sampling timer
        ADC12CTL2 = ADC12RES_2; // 12-bit conversion

        ADC12MCTL0 |= ADC12INCH_2 | ADC12DIF; // Enable A2 analog port (Channel A2)
        // Channel2 ADC input select; Vref=AVCC


}
void enable_MIC(){

    ADC12IER0 = (ADC12IE0); // Enable ADC Interrupt

    while (1)
      {
        __delay_cycles(5000);
        ADC12CTL0 |= ADC12ENC | ADC12SC;        // Start sampling/conversion

        __bis_SR_register(LPM0_bits + GIE);     // LPM0, ADC12_ISR will force exit
        __no_operation();                       // For debugger
      }

}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void){

    /* 12-bit ADC, N=2^12= 4.096
     * LSB = 3.3v/4096 = 0.0008056 = 0.8mv difference for each bit
     * if I want to trigger the output when the analog input is 0.5Vcc (50% of VRef), so it's 4096/2=2047, in HEX it's 7ff
     * ADC12MEM0 is the register that hold the ADC Value
     *
     */


    if (ADC12MEM0 >= 0x0C80){               // ADC12MEM0 = A2 > 0.5AVcc
            P1OUT |= BIT0;                      // Turn on the LED (P1.0)
    }
    else{
            P1OUT &= ~BIT0;                     // Turn off the LED (P1.0)
          __bic_SR_register_on_exit(LPM0_bits); // Exit active CPU
          }
}
