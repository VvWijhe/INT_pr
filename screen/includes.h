/*
 * Standard file for all the includes of the program.
 * IF YOU ADD AN INCLUDE PLEASE EXPLAIN WHY / WHERE YOU USE IT!
 *
 * If you create a new file, just include "../includes.h".
 *
 * includes.h
 *
 *  Created on: 21 Mar 2016
 *      Author: Mark
 */

#ifndef INCLUDES_H_
#define INCLUDES_H_

//USART driver for the screen.
#include <../STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/STM32F0xx_StdPeriph_Driver/inc/stm32f0xx_usart.h>

// Normal includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>	// USED FOR uint8_t
#include <string.h>
#include "screen.h"




//DMA periphial drivers for memory
#include <stm32f0xx_gpio.h>



#endif /* INCLUDES_H_ */
