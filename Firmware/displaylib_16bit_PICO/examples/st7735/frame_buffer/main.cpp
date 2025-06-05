/*!
    @file   main.cpp
    @author Gavin Lyons
    @brief  Example cpp file for st7735 library. Tests  frame buffer mode
    @note   See USER OPTIONS 0-3 in SETUP function
            dislib16_ADVANCED_SCREEN_BUFFER_ENABLE must be enabled for this example
            or it will not compile
    @test
        -# Frame buffer mode.
*/

// Section ::  libraries
#include "pico/time.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "displaylib_16/Bitmap_TEST_Data.hpp"
#include "displaylib_16/st7735.hpp"
#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"
#include "pico/rand.h"
/// @cond

#ifndef dislib16_ADVANCED_SCREEN_BUFFER_ENABLE
#pragma message("gll: dislib16_ADVANCED_SCREEN_BUFFER_ENABLE is not defined. it is required for this example")
#endif
// Section :: Globals
ST7735_TFT myTFT;

//  Section ::  Function Headers
bool Setup(void);
void Test1(void);
void TestRotate(void);
void TestBitmap(void);
void TestBitmapFPS(void);
void TestTextFPS(void);
void EndTests(void);
void ScreenReset(void);

#define BALL_CNT 1

struct ball {

    double x, y;
    double dirx, diry;
    double spd;

} ball1, ball2, ball3;
struct ball balls[BALL_CNT];

void pre_init() {
    // ball1.x = 2;
    // ball1.y = 10;
    // ball1.dirx = 0.1;
    // ball1.diry = 0.5;
    // ball1.spd = 500;



    // ball2.x = 2;
    // ball2.y = 10;
    // ball2.dirx = 0.1;
    // ball2.diry = 0.5;
    // ball2.spd = 500;

    for (uint8_t i = 0; i < BALL_CNT; i++) {
        balls[i].x = 2;
        balls[i].y = 10;
        balls[i].dirx = 0.1;
        balls[i].diry = 0.5;
        balls[i].spd = 500;
    }

}

//  Section ::  MAIN loop
int main(void)
{
    DisLib16::setDebugMode(true); // turn on debug mode
    if (!Setup())
        return -1;
    Test1();
    TestRotate();
    TestBitmap();
    TestBitmapFPS();
    TestTextFPS();
    EndTests();

    // // while (1) {

    // myTFT.fillScreen(myTFT.C_CYAN);

    // // for (uint8_t i = 0; i < BALL_CNT; i++) {

    // //     switch (i % 2)
    // //     {
    // //     case 1:
    // myTFT.fillRect(10, 10, 10, 10, myTFT.C_BLUE);
    // myTFT.fillCircle(50, 50, 10, myTFT.C_BLUE);
    // //         break;
    // //     case 0:
    // //         myTFT.fillCircle(balls[i].x, balls[i].y, 10, 63488);
    // //         break;
    // //     default:
    // //         break;
    // //     }
    // // }

    // //     for (uint8_t i = 0; i < BALL_CNT; i++) {
    // //         if (balls[i].x >= 128) {
    // //             balls[i].dirx = -1 * ((double)get_rand_32() / (double)__RAND_MAX);
    // //         }
    // //         if (balls[i].y >= 128) {
    // //             balls[i].diry = -1 * ((double)get_rand_32() / (double)__RAND_MAX);
    // //         }
    // //         if (balls[i].x <= 0) {
    // //             balls[i].dirx = 1 * ((double)get_rand_32() / (double)__RAND_MAX);
    // //         }
    // //         if (balls[i].y <= 0) {
    // //             balls[i].diry = 1 * ((double)get_rand_32() / (double)__RAND_MAX);
    // //         }

    // //         balls[i].x += balls[i].spd * 0.1 * balls[i].dirx;

    // //         balls[i].y += balls[i].spd * 0.1 * balls[i].diry;
    // //     }
    // // }
    // myTFT.writeBuffer();
    // MILLISEC_DELAY(1000);


}
// *** End OF MAIN **

//  Section ::  Function Space
bool Setup(void)
{
    set_sys_clock_khz(300000, true);
    stdio_init_all(); // optional for error messages , Initialize chosen serial port, default 38400 baud
    MILLISEC_DELAY(1000);
    printf("\nTFT: Start\r\n");
    DisLib16::printMemoryUsage();
    //*************** USER OPTION 0 SPI_SPEED + TYPE ***********
    bool bhardwareSPI = true; // true for hardware spi, false for software

    if (bhardwareSPI == true)
    {								   // hw spi
        uint32_t TFT_SCLK_FREQ = 62000; // Spi freq in KiloHertz , 1000 = 1Mhz
        myTFT.TFTInitSPIType(TFT_SCLK_FREQ, spi1);
    }
    else
    {								 // sw spi
        uint16_t SWSPICommDelay = 0; // optional SW SPI GPIO delay in uS
        myTFT.TFTInitSPIType(SWSPICommDelay);
    }
    //*********************************************************
    // ******** USER OPTION 1 GPIO *********
    // NOTE if using Hardware SPI clock and data pins will be tied to
    // the chosen interface eg Spi0 CLK=18 DIN=19)
    int8_t SDIN_TFT = 11;
    int8_t SCLK_TFT = 10;
    int8_t DC_TFT = 8;
    int8_t CS_TFT = 13;
    int8_t RST_TFT = 9;
    myTFT.setupGPIO(RST_TFT, DC_TFT, CS_TFT, SCLK_TFT, SDIN_TFT);
    //**********************************************************

    // ****** USER OPTION 2 Screen Setup ******
    uint8_t OFFSET_COL = 0;	   // 2, These offsets can be adjusted for any issues->
    uint8_t OFFSET_ROW = 0;	   // 3, with screen manufacture tolerance/defects
    uint16_t TFT_WIDTH = 128;  // Screen width in pixels
    uint16_t TFT_HEIGHT = 128; // Screen height in pixels
    myTFT.TFTInitScreenSize(OFFSET_COL, OFFSET_ROW, TFT_WIDTH, TFT_HEIGHT);
    // ******************************************

    // ******** USER OPTION 3 PCB_TYPE  **************************
    myTFT.TFTInitPCBType(myTFT.TFT_ST7735R_Red); // pass enum,4 choices,see README
    //**********************************************************

    if (myTFT.setBuffer() != DisLib16::Success)
        return false;					  // set up buffer
    myTFT.setTextCharPixelOrBuffer(true); // set to use pixel mode for text
    DisLib16::setDebugMode(true);
    DisLib16::printMemoryUsage();
    myTFT.setRotation(displaylib_16_graphics::display_rotate_e::Degrees_180);
    ScreenReset();
    return true;
}

void EndTests(void)
{
    myTFT.setRotation(displaylib_16_graphics::display_rotate_e::Degrees_180);
    myTFT.fillScreen(myTFT.C_BLACK);
    myTFT.clearBuffer(myTFT.C_BLACK);
    myTFT.destroyBuffer();
    myTFT.TFTPowerDown();
    printf("TFT: Tests Over\n");
}

void ScreenReset(void)
{
    myTFT.writeBuffer();
    MILLISEC_DELAY(2500);
    myTFT.clearBuffer(myTFT.C_BLACK);
}

void Test1(void)
{
    printf("Test1\r\n");
    myTFT.clearBuffer(myTFT.C_RED);
    myTFT.writeBuffer();
    MILLISEC_DELAY(1000);
    myTFT.clearBuffer(myTFT.C_GREEN);
    myTFT.writeBuffer();
    MILLISEC_DELAY(1000);
    myTFT.clearBuffer(myTFT.C_BLUE);
    myTFT.writeBuffer();
    MILLISEC_DELAY(1000);

    myTFT.clearBuffer(myTFT.C_BLACK);
    myTFT.setTextColor(myTFT.C_GREEN, myTFT.C_BLACK);
    myTFT.setCursor(5, 5);
    myTFT.setFont(font_default);
    myTFT.print("Buffer Mode     Test");
    myTFT.writeBuffer();
    MILLISEC_DELAY(5000);
}

void TestRotate(void)
{
    printf("Rotate\r\n");
    myTFT.fillScreen(myTFT.C_BLACK);
    myTFT.clearBuffer(myTFT.C_BLACK);
    const char* rotationLabels[] = { "Rotate 0", "Rotate 90", "Rotate 180", "Rotate 270" };
    const decltype(myTFT.Degrees_0) rotations[] = {
        myTFT.Degrees_0,
        myTFT.Degrees_90,
        myTFT.Degrees_180,
        myTFT.Degrees_270 };
    for (int i = 0; i < 4; ++i)
    {
        myTFT.setRotation(rotations[i]);
        myTFT.setCursor(20, 20);
        myTFT.print(rotationLabels[i]);
        myTFT.setCursor(20, 100);
        myTFT.print("Bottom");
        ScreenReset();
    }
    myTFT.setRotation(myTFT.Degrees_0);
    myTFT.TFTchangeMode(myTFT.TFT_Normal_mode);
    ScreenReset();
}

void TestBitmap(void)
{
    printf("Sprite\n");
    // Sprite
    for (int col = 0; col < 2; ++col) // 0: no flip, 1: flip
    {
        bool flip = (col == 1);
        int x = col * 32;

        for (int row = 0; row < 3; ++row)
        {
            int y = row * 32;
            myTFT.drawSpriteData(x, y, sSpriteTest16, 32, 32, myTFT.C_LBLUE, flip);
        }
    }
    ScreenReset();
    // Bitmap 1-bit
    printf("Bitmap 1-bit\n");
    myTFT.drawBitmap(0, 0, 128, 128, myTFT.C_WHITE, myTFT.C_GREEN, sArrowImage);
    ScreenReset();
    // Bitmap 8-bit
    printf("Bitmap 8-bit\n");
    myTFT.drawBitmap8Data(20, 20, sColorTestImage, 96, 96);
    ScreenReset();
    // Bitmap 16-bit
    printf("Bitmap 16-bit\n");
    myTFT.drawBitmap16Data(0, 0, sMotorImage, 128, 128);
    ScreenReset();
}

void TestBitmapFPS(void)
{
    printf("Bitmap FPS\n");
    // Values to count frame rate per second
    long previousMillis = 0;
    long lastFramerate = 0;
    long currentFramerate = 0;

    uint16_t count = 0;
    uint16_t seconds = 0;
    uint16_t fps = 0;

    char teststr1[] = "Test bitmap FPS, Output Results to USB Serial port";
    myTFT.writeCharString(5, 5, teststr1);
    MILLISEC_DELAY(5000);
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
        myTFT.drawBitmap16Data(0, 0, sMotorImage, 128, 128);
        myTFT.setCursor(0, 0);
        myTFT.print(fps);
        myTFT.writeBuffer();
        //   **
    } // end of while
    // Report results to usb
    printf("Seconds :: %u \n", seconds);
    printf("Count :: %u \n", count);
    printf("FPS :: %u \n", fps);
    // Print to screen
    myTFT.clearBuffer(myTFT.C_BLACK);
    myTFT.setCursor(0, 0);
    myTFT.print("Seconds : ");
    myTFT.println(seconds);
    myTFT.print("Count : ");
    myTFT.println(count);
    myTFT.print("FPS : ");
    myTFT.print(fps);
    myTFT.writeBuffer();
    MILLISEC_DELAY(5000);
    myTFT.clearBuffer(myTFT.C_BLACK);
    myTFT.fillScreen(myTFT.C_BLACK);
}

void TestTextFPS(void)
{
    printf("Text FPS\n");
    myTFT.fillScreen(myTFT.C_BLACK);
    myTFT.setFont(font_pico);
    // Values to count frame rate per second
    long previousMillis = 0;
    long lastFramerate = 0;
    long currentFramerate = 0;
    uint16_t count = 0;
    uint16_t seconds = 0;
    uint16_t fps = 0;
    while (1)
    {
        unsigned long currentMillis = to_ms_since_boot(get_absolute_time());

        if (currentMillis - previousMillis >= 1000) // every second
        {
            fps = currentFramerate - lastFramerate;
            lastFramerate = currentFramerate;
            previousMillis = currentMillis;
            seconds++;
            //myTFT.clearBuffer(myTFT.C_BLACK);
            if (count >= 2000) {
                printf("FPS : %u\n", fps);
                printf("Seconds : %u\n", seconds);
                return; // end if count gets to this
            }
        }
        currentFramerate++;
        count++;
        //  ** Code to test **
        // print seconds
        myTFT.setCursor(5, 5);
        myTFT.println("Secs:");
        myTFT.println(seconds);
        // print fps
        myTFT.println("FPS:");
        myTFT.println(fps);
        // print count
        myTFT.println("Count:");
        myTFT.println(count);
        myTFT.println("Testing!");
        myTFT.println(DisLib16::LibraryVersion());
        myTFT.writeBuffer();
        //  ****
    }
}
/// @endcond
