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
#include <stm32f0xx_usart.h>

// Normal includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>	// USED FOR uint8_t
#include <string.h>
#include "screen/screen.h"




//RCC, GPIO and SPI periphial drivers for memory
#include <stm32f0xx_spi.h>
#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>
#include <stm32f0xx_dma.h>



//Timer & GPIO drivers for the motor.
#include <stm32f0xx_tim.h>
#include <stm32f0xx_misc.h>
#include <stm32f0xx_gpio.h>

//Addition to create push
//Trace and Interface includes
#include "diag/Trace.h"





#endif /* INCLUDES_H_ */
