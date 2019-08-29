#include <msp430.h> 
#include "HAL_Microphone.h"


/**
 * main.c
 */


void init_mic(){

    config_GPIO_output_LED();
    config_GPIO_input_MIC();
    config_ADC12();
    enable_MIC();
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	init_mic();

	while(1){}
	
	return 0;
}
