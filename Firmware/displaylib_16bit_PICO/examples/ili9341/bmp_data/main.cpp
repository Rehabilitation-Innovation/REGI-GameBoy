/*!
	@file   main.cpp
	@author Gavin Lyons
	@brief  Example cpp file for ili9341 driver.bitmap tests + FPS bitmap test.
	@note   See USER OPTIONS 0-2 in SETUP function, Messages to serial port, baud 38400
	@test
		-# Test 300 Sprite 
		-# Test 301 "clock demo" , small bitmap,
		-# Test 302 bi-color small image
		-# Test 303 bi-color image 128x128
		-# Test 304 16 bit color image from a data array
		-# Test 305 16 bit color image data from a data array
		-# Test 306 8 bit color image data from a data array
		-# Test 603 FPS bitmap results to serial port, 38400 baud
		-# Test 802 Error checking bitmap functions, results to serial port
*/

// Section ::  libraries
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "displaylib_16/ili9341.hpp"
#include "displaylib_16/Bitmap_TEST_Data.hpp"
#include <span>
#include <vector> // for error checking test

/// @cond

// Section :: Defines
//  Test timing related defines
#define TEST_DELAY1 1000
#define TEST_DELAY2 2000
#define TEST_DELAY5 5000
#define CLOCK_DISPLAY_TIME 20 // seconds
#ifdef dislib16_ADVANCED_SCREEN_BUFFER_ENABLE
#pragma message("gll: dislib16_ADVANCED_SCREEN_BUFFER_ENABLE is defined. This example is not for that mode")
#endif
// Section :: Globals
ILI9341_TFT myTFT;
// Empty span For unit testing 
std::span<const uint8_t> emptyBitmap{};

//  Section ::  Function Headers
void Setup(void);	// setup + user options
void Test300(void); // sprite
void Test301(void); // "clock demo" , small bitmap,
void Test302(void); // bi-color small image
void Test303(void); // bi-color full screen image 128x128
void Test304(void); // 16 bit color image from a data array
void Test305(void); // 16 bit color image data from a data array
void Test306(void); // 8 bit color image data from a data array
void Test603(void); // FPS test, results to serial port
void Test802(void); // bitmap error checking, results to serial port 
void EndTests(void);

//  Section ::  MAIN

int main(void)
{
	Setup();
	Test300();
	Test301();
	Test302();
	Test303();
	Test304();
	Test305();
	Test306();
	Test802();
	Test603();
	EndTests();
}
// *** End OF MAIN **

//  Section ::  Function Space

/*!
	@brief  setup + user options
*/
void Setup(void)
{
	stdio_init_all(); // optional for error messages , Initialize chosen serial port, default 38400 baud
	MILLISEC_DELAY(1000);
	printf("Start\r\n");
//*************** USER OPTION 0 SPI_SPEED + TYPE ***********
	bool bhardwareSPI = true; // true for hardware spi, false for software
	
	if (bhardwareSPI == true) { // hw spi
		uint32_t TFT_SCLK_FREQ =  8000 ; // Spi freq in KiloHertz , 1000 = 1Mhz
		myTFT.SetupSPI(TFT_SCLK_FREQ, spi0); 
	} else { // sw spi
		uint16_t SWSPICommDelay = 0; // optional SW SPI GPIO delay in uS
		myTFT.SetupSPI(SWSPICommDelay);
	}
//*********************************************************
// ******** USER OPTION 1 GPIO *********
// NOTE if using Hardware SPI clock and data pins will be tied to 
// the chosen interface eg Spi0 CLK=18 DIN=19 MISO = 17)
	int8_t SDIN_TFT = 19; 
	int8_t SCLK_TFT = 18; 
	int8_t RST_TFT = 4;
	int8_t DC_TFT = 3;
	int8_t CS_TFT = 2;  
	myTFT.SetupGPIO(RST_TFT, DC_TFT, CS_TFT, SCLK_TFT, SDIN_TFT);
//**********************************************************

// ****** USER OPTION 2 Screen Setup ****** 
	uint16_t TFT_WIDTH = 240;// Screen width in pixels
	uint16_t TFT_HEIGHT = 320; // Screen height in pixels
	myTFT.SetupScreenSize(TFT_WIDTH , TFT_HEIGHT);
// ******************************************

	myTFT.ILI9341Initialize(); 
}

/*!
	@brief  "sprite demo" 32 x 32 with LBLUE background
*/
void Test300(void)
{
	// Test 300-A test 16-bit color Sprite 
	// Draw as sprite, without background , 32 X 32 .bakcground color = ST7375_LBLUE
	// Green bakcground screen
	myTFT.fillScreen(myTFT.C_GREEN);
	MILLISEC_DELAY(TEST_DELAY2);
	myTFT.drawSpriteData(55, 55, sSpriteTest16, 32, 32, myTFT.C_LBLUE, false);
	myTFT.drawSpriteData(100, 40, sSpriteTest16, 32, 32, myTFT.C_LBLUE, true);
	MILLISEC_DELAY(TEST_DELAY5);

	// Test 300-B test 16-bit color Sprite 
	// Draw as sprite, without background , 32 X 32 .background color = ST7375_LBLUE
	// Bitmap background screen
	myTFT.drawBitmap16Data(50, 50, sPosterImage, 80, 48);
	MILLISEC_DELAY(TEST_DELAY2);

    myTFT.drawSpriteData(80, 50, sSpriteTest16, 32, 32, myTFT.C_LBLUE, false);
	myTFT.drawSpriteData(60, 80, sSpriteTest16, 32, 32, myTFT.C_LBLUE, false);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}


/*! @brief  small bitmap,*/
void Test301(void)
{
	myTFT.fillScreen(myTFT.C_BLACK);
	char teststr1[] = "Test 301";
	myTFT.writeCharString(90, 90, teststr1);
	// TOP small bitmap box
	myTFT.drawBitmap(40,  40, 16, 8, myTFT.C_BLACK, myTFT.C_WHITE, SignalIcon);
	myTFT.drawBitmap(60,  40, 16, 8, myTFT.C_BLACK, myTFT.C_WHITE, MsgIcon);
	myTFT.drawBitmap(80,  40, 8,  8,  myTFT.C_BLACK, myTFT.C_WHITE, AlarmIcon);
	myTFT.drawBitmap(110, 40, 16, 8, myTFT.C_BLACK, myTFT.C_WHITE, BatIcon);
	// second box
	myTFT.drawBitmap(35, 70, 16, 8, myTFT.C_GREEN, myTFT.C_BLACK, powerIcon);
	myTFT.drawBitmap(55, 70, 16, 8, myTFT.C_RED, myTFT.C_YELLOW, speedIcon);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
} // end of Test 301

/*!
	@brief  Test 302 bi-color small image 20x24
*/
void Test302(void)
{
	myTFT.fillScreen(myTFT.C_BLACK);
	char teststr1[] = "Test 302";
	myTFT.writeCharString(50, 50, teststr1);
	myTFT.drawBitmap(80, 60, 40, 16, myTFT.C_CYAN, myTFT.C_BLACK, sSunTextImage);
	myTFT.drawBitmap(20, 100, 40, 16, myTFT.C_RED, myTFT.C_BLACK, sSunTextImage);
	myTFT.drawBitmap(30, 140, 40, 16, myTFT.C_YELLOW, myTFT.C_RED, sSunTextImage);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

/*!
	@brief  Test303 bi-color full screen image 128x128
*/
void Test303(void)
{
	char teststr1[] = "Test 303";
	myTFT.writeCharString(50, 50, teststr1);
	MILLISEC_DELAY(TEST_DELAY2);
	myTFT.drawBitmap(50, 65, 128, 128, myTFT.C_WHITE, myTFT.C_GREEN, sArrowImage);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

/*!
	@brief  Test304 16 bit color image from a data array
*/
void Test304(void)
{
	char teststr1[] = "Test 304";
	myTFT.writeCharString(50, 50, teststr1);
	MILLISEC_DELAY(TEST_DELAY2);
	myTFT.drawBitmap16Data(65, 65, sMotorImage, 128, 128);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

/*!
	@brief  Test305 16 bit color image data from a data array
*/
void Test305(void)
{
	char teststr1[] = "Test 305";
	myTFT.writeCharString(50, 50, teststr1);
	MILLISEC_DELAY(TEST_DELAY2);
	myTFT.drawBitmap16Data(65, 65, sPosterImage, 80, 48);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

/*!
	@brief  Test306 8 bit color image data from a data array
*/
void Test306(void)
{
	char teststr1[] = "Test 306";
	myTFT.writeCharString(50, 50, teststr1);
	MILLISEC_DELAY(TEST_DELAY2);
	myTFT.drawBitmap8Data(65, 65, sColorTestImage, 96,96);
	MILLISEC_DELAY(TEST_DELAY5);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

/*!
	@brief  Test603 frame rate per second FPS ,results to serial port
*/
void Test603(void)
{
	// Values to count frame rate per second
	long previousMillis = 0;
	long lastFramerate = 0;
	long currentFramerate = 0;
	myTFT.setFont(font_mega);
	uint16_t count = 0;
	uint16_t seconds = 0;
	uint16_t fps = 0;
	char teststr1[] = "Test 601 FPS, Output Results to USB port";
	myTFT.writeCharString(20, 50, teststr1);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.setTextColor(myTFT.C_YELLOW, myTFT.C_RED);

	while (1)
	{
		unsigned long currentMillis = to_ms_since_boot(get_absolute_time());

		if (currentMillis - previousMillis >= 1000) // every second
		{
			fps = currentFramerate - lastFramerate;
			lastFramerate = currentFramerate;
			previousMillis = currentMillis;
			seconds++;
			if (seconds == 20)
				break;
		}
		currentFramerate++;
		count++;

		//  ** Code to test **
		myTFT.drawBitmap16Data(60, 120 ,sMotorImage, 128, 128);
		myTFT.setCursor(60, 90); 
		myTFT.print(fps);
		//   **
	} // end of while
	
	// Report results to usb
	printf("Seconds :: %u \n", seconds);
	printf("Count :: %u \n", count);
	printf("FPS :: %u \n", fps);
	// Print to screen
	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.setCursor(0, 50); 
	myTFT.print("Seconds : ");
	myTFT.println(seconds);
	myTFT.print("Count : ");
	myTFT.println(count);
	myTFT.print("FPS : ");
	myTFT.print(fps);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
}

/*!
	@brief  Bitmap error checking test
*/
void Test802(void)
{
	// === Setup tests ===
	// Define the expected return values
	std::vector<uint8_t> expectedErrors = 
	{
		DisLib16::Success, 
		DisLib16::BitmapScreenBounds, DisLib16::BitmapScreenBounds, DisLib16::BitmapDataEmpty, DisLib16::BitmapSize, //sprite
		DisLib16::BitmapScreenBounds, DisLib16::BitmapScreenBounds, DisLib16::BitmapDataEmpty, DisLib16::BitmapHorizontalSize, //1-bit bitmap
		DisLib16::BitmapScreenBounds, DisLib16::BitmapScreenBounds, DisLib16::BitmapDataEmpty, DisLib16::BitmapSize,//8-bit bitmap
		DisLib16::BitmapScreenBounds, DisLib16::BitmapScreenBounds, DisLib16::BitmapDataEmpty, DisLib16::BitmapSize,//16-bit bitmap
		DisLib16::BitmapScreenBounds, DisLib16::BitmapScreenBounds, DisLib16::BitmapDataEmpty, DisLib16::BitmapSize//16-bit bitmap
	};
	// Vector to store return values
	std::vector<uint8_t> returnValues; 
	
	// test variables
	char testString5[] = "Error Check Test 802, results to usb";
	bool errorFlag = false;

	// === Tests===
	printf("=== START Error checking. Expecting errors ===\r\n");
	// Perform function calls and store return values
	myTFT.setFont(font_mega);
	returnValues.push_back(myTFT.writeCharString(5, 55, testString5)); 
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.fillScreen(myTFT.C_BLACK);
	//drawSpriteData
	returnValues.push_back(myTFT.drawSpriteData(400, 50, sSpriteTest16, 32, 32, myTFT.C_LBLUE, false));
	returnValues.push_back(myTFT.drawSpriteData(40, 400, sSpriteTest16, 32, 32, myTFT.C_LBLUE, false));
	returnValues.push_back(myTFT.drawSpriteData(40, 40, emptyBitmap, 32, 32, myTFT.C_LBLUE, false));
	returnValues.push_back(myTFT.drawSpriteData(40, 40, sSpriteTest16, 48, 32, myTFT.C_LBLUE, false));
	//drawBitmap
	returnValues.push_back(myTFT.drawBitmap(400, 65, 128, 128, myTFT.C_WHITE, myTFT.C_GREEN, sArrowImage));
	returnValues.push_back(myTFT.drawBitmap(50, 400, 128, 128, myTFT.C_WHITE, myTFT.C_GREEN, sArrowImage));
	returnValues.push_back(myTFT.drawBitmap(50, 65, 128, 128, myTFT.C_WHITE, myTFT.C_GREEN, emptyBitmap));
	returnValues.push_back(myTFT.drawBitmap(20, 20, 70, 128, myTFT.C_WHITE, myTFT.C_GREEN, sArrowImage));
	//drawBitmap8Data
	returnValues.push_back(myTFT.drawBitmap8Data(400, 50, sColorTestImage, 96, 96));
	returnValues.push_back(myTFT.drawBitmap8Data(40, 400, sColorTestImage, 96, 96));
	returnValues.push_back(myTFT.drawBitmap8Data(40, 40, emptyBitmap, 32, 32));
	returnValues.push_back(myTFT.drawBitmap8Data(40, 40, sColorTestImage, 128, 96));
	//drawBitmap16Data
	returnValues.push_back(myTFT.drawBitmap16Data(400, 50, sSpriteTest16, 32, 32));
	returnValues.push_back(myTFT.drawBitmap16Data(40, 400, sSpriteTest16, 32, 32));
	returnValues.push_back(myTFT.drawBitmap16Data(40, 40,  emptyBitmap, 32, 32));
	returnValues.push_back(myTFT.drawBitmap16Data(40, 40,  sSpriteTest16, 50, 32));
	//drawBitmap16Data
	returnValues.push_back(myTFT.drawBitmap16Data(400, 50, sPosterImage, 80, 48));
	returnValues.push_back(myTFT.drawBitmap16Data(40, 400, sPosterImage, 80, 48));
	returnValues.push_back(myTFT.drawBitmap16Data(40, 40,  emptyBitmap, 80, 48));
	returnValues.push_back(myTFT.drawBitmap16Data(40, 40,  sPosterImage, 80, 60));
	
	
	//== SUMMARY SECTION===
	printf("\nError Checking Summary.\n");
	// Check return values against expected errors
	for (size_t i = 0; i < returnValues.size(); ++i) {
		if (i >= expectedErrors.size() || returnValues[i] != expectedErrors[i]) {
			errorFlag = true;
			printf("Unexpected error code: %d at test case %zu (expected: %d)\n", 
				returnValues[i], i + 1, (i < expectedErrors.size() ? expectedErrors[i] : -1));
		}
	}

	// Print all expectedErrors for summary
	for (uint8_t value : expectedErrors ) 
	{
		printf("%d ", value);
	}
	printf("\n");
	// Print all returnValues for summary
	for (uint8_t value : returnValues) 
	{
		printf("%d ", value);
	}
	if (errorFlag == true ){
		printf("\nError Checking has FAILED.\n");
	}else{
		printf("\nError Checking has PASSED.\n");
	}
	printf("\n=== STOP Error checking. ===\r\n");
}

/*!
	@brief  Stop testing and shutdown the TFT
*/
void EndTests(void)
{
	char teststr1[] = "Tests over";
	myTFT.fillScreen(myTFT.C_BLACK);
	myTFT.writeCharString(50, 50, teststr1);
	MILLISEC_DELAY(TEST_DELAY5);
	myTFT.PowerDown();
	printf("TFT :: Tests Over\r\n");
}
// *************** EOF ****************

/// @endcond
