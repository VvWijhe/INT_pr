/*
 * screen.h
 *
 *  Created on: 16 Mar 2016
 *      Author: Mark
 */

#ifndef SCREEN_SCREEN_H_
#define SCREEN_SCREEN_H_

#include "includes.h"


// ********************************** FUNCTIONS FOR THE USART ***********************************
/**
 * Init Usart 2 @TODO Move to global function for GPS etc.
 */
void USART2Init(void);

/**
 * Send a whole string through USART1
 * @param pdata: the string
 */
void USART2SendString(const void * pdata, uint8_t length);

/**
 * Send a single char through USART1
 * @param data: The data to send
 */
void USART2SendChar(uint8_t data);



// ********************************** FUNCTIONS FOR THE SCREEN ***********************************
/**
 * Clear the entire screen
 */
void screenClear(void);

/**
 * Refresh the screen after a change of display
 */
void screenRefresh(void);

/**
 * Initialise the screen, includes a screenclear and a handshake
 */
void screenInit(void);

/**
 * Sets the size of the English font
 * @param fontsize: 0 = 32 dots, 1 = 48 dots, 2 = 64 dots
 */
void screenSelectFontSize(uint8_t fontsize);

/**
 * Display text on pixel x and y
 * @param text: The text to display
 * @param x: x pixel start coordination, from left to right
 * @param y: y pixel start coordination
 */
void screenDisplayText(char text[], uint16_t x, uint16_t y);

/**
 * Set the storage area of the screen, For normal operation must be set to external TF SD card
 * @param storageplace: 0 = Internal NAND, 1 = External Flash card
 */
void screenSetStorage(uint8_t storageplace);

/**
 * Display picture on the screen, the coordinates x and y are the starting coords of the pic
 * @param picturename, string of the name of the picture
 * @param x: x pixel start coordination, from left to right
 * @param y: y pixel start coordination
 */
void screenDisplayPicture(char picturename[], uint16_t x, uint16_t y);

/**
 * Draws a line from x to y
 * @param Xstart: Starting X position of the line
 * @param Ystart: Starting Y position of the line
 * @param Xend: End X position of the line
 * @param Yend: End Y position of the line
 */
void screenDisplayLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend);

/**
 * Draws the debug menu
 * @param menunumber: The number of the menu,
 * 						0 = Debug menu
 * 						1 = Relocate Home Position
 * 						2 = Radius
 * 						3 = Connect to computer
 */
void screenDisplayMenu(uint8_t menunumber);

/**
 * Display arrow on screen based on the amount of degrees from the front of the device
 * @param degrees: Degrees from 0 to 360
 */
void screenDisplayArrow(uint16_t degrees);

/**
 * Calculate XOR from every bit of the frame
 * @param frame: The frame that must be send to the screen
 * @return: Returns the calculated parity byte for the end of the frame
 */
uint8_t screenCalcParity(const void * pframe, uint8_t sizeofframe);

#endif /* SCREEN_SCREEN_H_ */
