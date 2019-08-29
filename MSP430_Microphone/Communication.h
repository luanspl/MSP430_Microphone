/*
 * Communication.h
 *
 *  Created on: 28 de ago de 2019
 *      Author: Luan
 *
 *
 *     Description: HAL (Hardware Abstract Layer) practice
 *     Description 2: send the data through UART protocol
 *
 *
 *     UART:
 *     Baud Rate: 9600
 *     Clock: 32.768 hz
 *     Hardware Port: COM9
 *     No parity bit
 *     8 bit data
 *     LSB First
 *     one Stop Bit
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_



    void config_GPIO_Tx_UART(); //Configure the UART transmission GPIO PORT
    void config_setup_UART(); //Configure the Setup for UART
    void config_UART(); //Setting the UART
    void Tx_UART(unsigned char Data); //Tx data




#endif /* COMMUNICATION_H_ */
