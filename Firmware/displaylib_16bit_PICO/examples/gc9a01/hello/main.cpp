/*!
	@file   main.cpp
	@author Gavin Lyons
	@brief  Example cpp file for gc9a01 driver. Test Hello World.
	@note   See USER OPTIONS 0-2 in SETUP function
	@test
		-# Test 101 Print out Hello world  
*/

// Section ::  libraries 
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "displaylib_16/gc9a01.hpp"

///@cond

// Section :: Defines   
//  Test timing related defines 
#define TEST_DELAY1 1000 // mS
#define TEST_DELAY2 2000 // mS
#define TEST_DELAY5 5000 // mS
#ifdef dislib16_ADVANCED_SCREEN_BUFFER_ENABLE
#pragma message("gll: dislib16_ADVANCED_SCREEN_BUFFER_ENABLE is defined. This example is not for that mode")
#endif

// Section :: Globals 
GC9A01_TFT myTFT;

//  Section ::  Function Headers 

void Setup(void);  // setup + user options
void Test100(void);  
void EndTests(void);

//  Section ::  MAIN loop

int main(void) 
{
	Setup();
	Test100();
	EndTests();
}
// *** End OF MAIN **

//  Section ::  Function Space 

void Setup(void)
{
	stdio_init_all(); // optional for error messages , Initialize chosen serial port, default 38400 baud
	MILLISEC_DELAY(TEST_DELAY1);
	printf("TFT: Start\r\n");
//*** USER OPTION 0 SPI_SPEED + TYPE ***
	bool bhardwareSPI = true; // true for hardware spi, false for software
	
	if (bhardwareSPI == true) { // hw spi
		uint32_t TFT_SCLK_FREQ =  8000 ; // Spi freq in KiloHertz , 1000 = 1Mhz
		myTFT.TFTInitSPIType(TFT_SCLK_FREQ, spi0); 
	} else { // sw spi
		uint16_t SWSPICommDelay = 0; // optional SW SPI GPIO delay in uS
		myTFT.TFTInitSPIType(SWSPICommDelay);
	}
// ***
// *** USER OPTION 1 GPIO ***
// NOTE if using Hardware SPI clock and data pins will be tied to 
// the chosen interface eg Spi0 CLK=18 DIN=19)
	int8_t SDIN_TFT = 19; 
	int8_t SCLK_TFT = 18; 
	int8_t DC_TFT = 3;
	int8_t CS_TFT = 2 ;  
	int8_t RST_TFT = 4;
	myTFT.TFTsetupGPIO(RST_TFT, DC_TFT, CS_TFT, SCLK_TFT, SDIN_TFT);
// ***
// *** USER OPTION 2 Screen Setup ***
	uint16_t TFT_WIDTH = 240;// Screen width in pixels
	uint16_t TFT_HEIGHT = 240; // Screen height in pixels
	myTFT.TFTInitScreenSize(TFT_WIDTH , TFT_HEIGHT);
// ***
	myTFT.TFTGC9A01Initialize(); 
}

void Test100(void) {
	printf("Version %u \n",DisLib16::LibraryVersion());
	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.setTextColor(myTFT.C_GREEN, myTFT.C_BLACK);
	myTFT.setCursor(15,80);
	myTFT.setFont(font_arialBold);
	myTFT.print("Hello World");
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
	MILLISEC_DELAY(TEST_DELAY1);
}

void EndTests(void)
{
	myTFT.TFTPowerDown(); 
	printf("TFT: End\n");
}
///@endcond
// *************** EOF ****************
