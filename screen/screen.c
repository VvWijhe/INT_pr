/* screen.c
 *
 *  Created on: 16 Mar 2016
 *      Author: Mark
 */

#include "screen.h"


void USART2Init(void)
{
	// USART periferial initialization settings
	  USART_InitTypeDef USART_InitStructure;
	  GPIO_InitTypeDef  GPIO_InitStructure;

	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
	  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
	  //Configure USART2 pins: Rx (PA2) and Tx (PA3)
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
	  //Configure USART2 setting: ----------------------------
	  USART_InitStructure.USART_BaudRate = 115200;
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	  USART_InitStructure.USART_StopBits = USART_StopBits_1;
	  USART_InitStructure.USART_Parity = USART_Parity_No;
	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	  USART_Init(USART2, &USART_InitStructure);
	  USART_Cmd(USART2,ENABLE);
}

void USART2SendChar(uint8_t data)
{
	 while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	    USART_SendData(USART2, data);
}

void USART2SendString(const void * pdata, uint8_t length)
{
	uint8_t count;
	unsigned char * p = (unsigned char *)pdata;
	for(count = 0; count <= length; count++)
	{
		USART2SendChar(p[count]);
	}
}

void screenInit(void)
{
	uint8_t sendbuffer[] = 	// Fill the buffer with the data to send
	{0xA5, 0x00, 0x09, 0x00, 0xCC, 0x33, 0xC3, 0x3C, 0xAC};
	USART2SendString(sendbuffer, sizeof sendbuffer);

	screenClear();			// Clear Screen
	screenRefresh();
	//screenSetStorage(0x01);	// Set storage as external (SD Card)
}

void screenClear(void)
{
	uint8_t sendbuffer[] =
	{0xA5, 0x00, 0x09, 0x2E, 0xCC, 0x33, 0xC3, 0x3C, 0x82};
	USART2SendString(sendbuffer, sizeof sendbuffer);
}

void screenRefresh(void)
{
	uint8_t sendbuffer[] =
	{0xA5, 0x00, 0x09, 0x0A, 0xCC, 0x33, 0xC3, 0x3C, 0xA6};
	USART2SendString(sendbuffer, sizeof sendbuffer);
}

void screenSelectFontSize(uint8_t fontsize)
{
	uint8_t sendbuffer[10] =
	{0xA5, 0x00, 0x0A, 0x1E, 0xFF, 0xCC, 0x33, 0xC3, 0x3C};
	//0	    1	  2		3     4	    5	  6		7	  8		9
	//						  Data							Parity (seperate send)
	// Data: 01 for 32 dot....... Input:	0
	// 		 02 for 48 dot....... Input:	1
	// 		 03 for 64 dot....... Input:	2
	switch (fontsize)
	{
	case 0:
		sendbuffer[4] = 0x01;
		sendbuffer[9] = (screenCalcParity(sendbuffer, (sizeof sendbuffer)-1));
		USART2SendString(sendbuffer, sizeof sendbuffer);
		break;

	case 1:
		sendbuffer[4] = 0x02;
		sendbuffer[9] = (screenCalcParity(sendbuffer, (sizeof sendbuffer)-1));
		USART2SendString(sendbuffer, sizeof sendbuffer);
		break;

	case 2:
		sendbuffer[4] = 0x03;
		sendbuffer[9] = (screenCalcParity(sendbuffer, (sizeof sendbuffer)-1));
		USART2SendString(sendbuffer, sizeof sendbuffer);
		break;

	default:
		break; // Do not send data if input is not known
	}
}

void screenDisplayText(char text[], uint16_t x, uint16_t y)
{
	uint16_t framelength = 0x0D;		// 0x0D is the standard framelength, the size of the string should be added to it
	uint8_t currentpos 	 = 8; 			// Position of the string in the buffer.
	uint8_t count;
	uint8_t sizeoftext;

	for(sizeoftext = 0; text[sizeoftext] != 0x00; sizeoftext++); // check size of text
	char textbuffer[(sizeoftext + 1)];
	strcpy(textbuffer, text);

	textbuffer[sizeoftext + 1] = 0x00;	// Add final character to the string.
	sizeoftext = sizeof textbuffer; // Update the size of the string

	// Split 16 bit ints to two 8 bit ints by masking non important bits and shifting important bits
	uint8_t framelengthpart1 =  ((framelength + sizeoftext) & 0xFF00) >> 8;
	uint8_t framelengthpart2 =  (framelength + sizeoftext) & 0x00FF;
	uint8_t posxpart1 = (x & 0xFF00) >> 8;
	uint8_t posxpart2 =  x & 0x00FF;
	uint8_t posypart1 = (y & 0xFF00) >> 8;
	uint8_t posypart2 =  y & 0x00FF;

	uint8_t sendbuffer[30] = // set array max size
	{0xA5, framelengthpart1, framelengthpart2, 0x30, posxpart1, posxpart2, posypart1, posypart2};

	for (count = 0; count < sizeoftext; count++)
	{
		sendbuffer[currentpos] = textbuffer[count];
		currentpos++;
	}
	// Add preset final bits of the frame
	sendbuffer[currentpos] = 0xCC;
	sendbuffer[currentpos +1] = 0x33;
	sendbuffer[currentpos +2] = 0xC3;
	sendbuffer[currentpos +3] = 0x3C;
	sendbuffer[currentpos +4] = screenCalcParity(sendbuffer, currentpos +3);

	// send it out
	USART2SendString(sendbuffer, currentpos + 4);
}

void screenDisplayLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
	//A5 00 11 22 00 0A 00 0A 00 FF 00 FF CC 33 C3 3C 96

	// Shift 16 bit vars to two 8 bit vars
	uint8_t Xstartpart1 = (Xstart & 0xFF00) >> 8;
	uint8_t Xstartpart2 =  Xstart & 0x00FF;
	uint8_t Ystartpart1 = (Ystart & 0xFF00) >> 8;;
	uint8_t Ystartpart2 =  Ystart & 0x00FF;

	uint8_t Xendpart1 = (Xend & 0xFF00) >> 8;
	uint8_t Xendpart2 =  Xend & 0x00FF;
	uint8_t Yendpart1 = (Yend & 0xFF00) >> 8;
	uint8_t Yendpart2 =  Yend & 0x00FF;

	uint8_t sendbuffer[17] ={
			0xA5, 0x00, 0x11, 0x22, Xstartpart1, Xstartpart2, Ystartpart1, Ystartpart2, Xendpart1, Xendpart2,
			Yendpart1, Yendpart2, 0xCC, 0x33, 0xC3, 0x3C
	};
	sendbuffer[16] =  screenCalcParity(sendbuffer, 16);

	USART2SendString(sendbuffer, 17);
}

void screenSetStorage(uint8_t storageplace)
{
	if (storageplace || (storageplace == 0))	// Make sure storageplace is a known number
	{
		uint8_t sendbuffer[] =
		{0xA5, 0x00, 0x0A, 0x07, storageplace, 0xCC, 0x33, 0xC3, 0x3C};
		//						 01   For external SD
		//						 00   For internal NAND
		USART2SendString(sendbuffer, sizeof sendbuffer);
		USART2SendChar(screenCalcParity(sendbuffer, sizeof sendbuffer));
	}
}


void screenDisplayPicture(char picturename[], uint16_t x, uint16_t y)
{
		uint16_t framelength = 0x0D;		// 0x0D is the standard framelength, the size of the string should be added to it
		uint8_t currentpos 	 = 8; 			// Position of the string in the buffer.
		uint8_t count;
		uint8_t sizeofname;

		for(sizeofname = 0; picturename[sizeofname] != 0x00; sizeofname++); // determine size of name
		char namebuffer[(sizeofname + 1)];
		strcpy(namebuffer, picturename);

		namebuffer[sizeofname + 1] = 0x00;	// Add final character to the string.
		sizeofname = sizeof namebuffer; // Update the size of the string

		// Split 16 bit ints to two 8 bit ints by masking non important bits and shifting important bits
		uint8_t framelengthpart1 =  ((framelength + sizeofname) & 0xFF00) >> 8;
		uint8_t framelengthpart2 =  (framelength + sizeofname) & 0x00FF;
		uint8_t posxpart1 = (x & 0xFF00) >> 8;
		uint8_t posxpart2 =  x & 0x00FF;
		uint8_t posypart1 = (y & 0xFF00) >> 8;
		uint8_t posypart2 =  y & 0x00FF;

		uint8_t sendbuffer[30] = // set array max size
		{0xA5, framelengthpart1, framelengthpart2, 0x70, posxpart1, posxpart2, posypart1, posypart2};

		for (count = 0; count < sizeofname; count++)
		{
			sendbuffer[currentpos] = namebuffer[count];
			currentpos++;
		}
		// Add preset final bits of the frame
		sendbuffer[currentpos] = 0xCC;
		sendbuffer[currentpos +1] = 0x33;
		sendbuffer[currentpos +2] = 0xC3;
		sendbuffer[currentpos +3] = 0x3C;
		sendbuffer[currentpos +4] = screenCalcParity(sendbuffer, currentpos +3);

		// send it to the screen
		USART2SendString(sendbuffer, currentpos + 4);
}


uint8_t screenCalcParity(const void * pframe, uint8_t sizeofframe)
{
		uint8_t paritybyte = 0 ;
		unsigned char * p = (unsigned char *)pframe;
		uint8_t count;

		for(count = 0; count <= sizeofframe; count++)
		{
			paritybyte ^= p[count];
		}
		return paritybyte;
}

void screenDisplayMenu(uint8_t menunumber)
{
	switch (menunumber){
	case 0: // Debug main menu
		// First reset data on the screen
		screenClear();
		screenSelectFontSize(2);

		screenDisplayLine(0, 150, 800, 150);
		screenDisplayLine(0, 300, 800, 300);
		screenDisplayLine(0, 450, 800, 450);
		screenDisplayText("Relocate Home Position", 100, 75);
		screenDisplayText("Set Radius", 100, 225);
		screenDisplayText("Connect to Computer", 100, 375);
		screenDisplayText("Back", 100, 475);

		screenRefresh();
		break;

	case 1:	// Relocate home position
		// First reset data
		screenClear();
		screenSelectFontSize(2);

		screenDisplayLine(0, 150, 800, 150);
		screenDisplayLine(0, 450, 800, 450);
		screenDisplayText("Relocate Home Position", 100, 75);
		screenDisplayText("Back", 100, 475);
		screenDisplayText("Set New Home Position", 100, 275);

		screenRefresh();
		break;

	case 2: // Setting new Radius
		// First reset Data
		screenClear();
		screenSelectFontSize(2);

		screenDisplayLine(0, 150, 800, 150);
		screenDisplayLine(0, 450, 800, 450);
		screenDisplayText("Set New Radius", 100, 75);
		screenDisplayText("Back", 100, 475);
		screenSelectFontSize(1);
		screenDisplayText("500 m", 100, 200);
		screenDisplayText("1 Km", 100, 250);
		screenDisplayText("2 Km", 100, 300);

		screenRefresh();
		break;

	case 3:
		// First reset Data
		screenClear();
		screenSelectFontSize(2);

		screenDisplayLine(0, 150, 800, 150);
		screenDisplayLine(0, 450, 800, 450);
		screenDisplayText("Connect to Computer", 100, 75);
		screenDisplayText("Back", 100, 475);
		screenDisplayText("Connect", 100, 275);

		screenRefresh();
		break;

	}
}

void screenDisplayArrow(uint16_t degrees)
{
	uint8_t arrownum = 100; // init arrownum to an impossible value

	if(degrees > 360){ // Show error message when degrees is larger than 360
		screenSelectFontSize(2);
		screenDisplayText("ERROR: Graden groter dan 360?", 0, 200);
		screenRefresh();
		return;

	} else if (degrees <= 102 && degrees > 78){
		arrownum = 0;
	} else if (degrees <= 78 && degrees > 54){
		arrownum = 1;
	} else if (degrees <= 54 && degrees > 30){
		arrownum = 2;
	} else if (degrees <= 30 && degrees > 6){
		arrownum = 3;
	} else if (degrees <= 6 && degrees > 342){
		arrownum = 4;
	} else if (degrees <= 342 && degrees > 318){
		arrownum = 5;
	} else if (degrees <= 318 && degrees > 294){
		arrownum = 6;
	} else if (degrees <= 294 && degrees > 270){
		arrownum = 7;
	} else if (degrees <= 270 && degrees > 246){
		arrownum = 8;
	} else if (degrees <= 246 && degrees > 222){
		arrownum = 9;
	} else if (degrees <= 222 && degrees > 198){
		arrownum = 10;
	} else if (degrees <= 198 && degrees > 174){
		arrownum = 11;
	} else if (degrees <= 174 && degrees > 150){
		arrownum = 12;
	} else if (degrees <= 150 && degrees > 126){
		arrownum = 13;
	} else if (degrees <= 126 && degrees > 102){
		arrownum = 14;
	}

	screenClear();

	switch(arrownum){
	case 0:
		screenDisplayPicture("PIC0.BMP", 50, 100);
		break;
	case 1:
		screenDisplayPicture("PIC1.BMP", 50, 100);
		break;
	case 2:
		screenDisplayPicture("PIC2.BMP", 50, 100);
		break;
	case 3:
		screenDisplayPicture("PIC3.BMP", 50, 100);
		break;
	case 4:
		screenDisplayPicture("PIC4.BMP", 50, 100);
		break;
	case 5:
		screenDisplayPicture("PIC5.BMP", 50, 100);
		break;
	case 6:
		screenDisplayPicture("PIC6.BMP", 50, 100);
		break;
	case 7:
		screenDisplayPicture("PIC7.BMP", 50, 100);
		break;
	case 8:
		screenDisplayPicture("PIC8.BMP", 50, 100);
		break;
	case 9:
		screenDisplayPicture("PIC9.BMP", 50, 100);
		break;
	case 10:
		screenDisplayPicture("PIC10.BMP", 50, 100);
		break;
	case 11:
		screenDisplayPicture("PIC11.BMP", 50, 100);
		break;
	case 12:
		screenDisplayPicture("PIC12.BMP", 50, 100);
		break;
	case 13:
		screenDisplayPicture("PIC13.BMP", 50, 100);
		break;
	case 14:
		screenDisplayPicture("PIC14.BMP", 50, 100);
		break;
	default:
		screenSelectFontSize(2);
		screenDisplayText("ERROR: Onbekend Pijlnummer!", 0, 200);
		screenRefresh();
		return;
	}
	screenSelectFontSize(2);
	screenDisplayText("Thuis", 300, 0);
	screenRefresh();

	return;
}
