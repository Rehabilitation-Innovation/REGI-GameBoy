[![Website](https://img.shields.io/badge/Website-Link-blue.svg)](https://gavinlyonsrepo.github.io/)  [![Rss](https://img.shields.io/badge/Subscribe-RSS-yellow.svg)](https://gavinlyonsrepo.github.io//feed.xml)  [![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/paypalme/whitelight976)

# Display Library for 16-bit color graphic displays for Raspberry PI PICO

## Table of contents

  * [Overview](#overview)
  * [Documentation](#documentation)
    * [Supported devices](#supported-devices)
    * [API documentation](#api-documentation)
    * [Fonts](#fonts)
  * [Software](#software)
    * [Advanced Graphics](#advanced-graphics)
    * [Test](#test)
    * [Print](#print)
    * [Advanced buffer mode](#Advanced-buffer-mode)
    * [File-system](#file-system)
    * [Error Codes](#error-codes)
  * [Notes](#notes)
    * [Projects using library](#projects-using-library)
    * [Older versions](#older-versions)

## Overview

* Name : displaylib_16bit_PICO
* Description :

0. C++ Library to support 16-bit color graphic displays
	for the Raspberry PI PICO.
1. Graphics class included.
2. Bitmaps supported.
3. Polymorphic print class included to print many data types.
4. Multiple displays supported, see supported-devices, new components can be easily added.
5. 16 fonts included. 
6. [URL project github link](https://github.com/gavinlyonsrepo/displaylib_16bit_PICO)

* Author: Gavin Lyons

* Developed on Toolchain:
	1. Raspberry pi PICO RP2040
	2. SDK(1.4.0) C++20
	3. compiler G++ for arm-none-eabi((15:10.3-2021.07-4) 
	4. CMAKE(VERSION 3.18) , VScode(1.84.2)
	5. Linux Mint 22.1

## Documentation

### Supported devices

| Component name | Type | Interface | Readme URL link |
| -------- | ---------- | --------- | ---------- |
| ST7789 |16 bit Colour Graphic TFT LCD|SPI HW & SW| [Readme](extra/doc/st7789/README.md)|
| ST7735 |16 bit Colour Graphic TFT LCD|SPI HW & SW| [Readme](extra/doc/st7735/README.md)|
| SSD1331|16 bit Colour Graphic OLED |SPI HW & SW  | [Readme](extra/doc/ssd1331/README.md)|
| ILI9341 |16 bit Colour Graphic TFT LCD |SPI HW & SW  | [Readme](extra/doc/ili9341/README.md)|
| GC9A01 |16 bit Colour Graphic TFT LCD |SPI HW & SW  | [Readme](extra/doc/gc9a01/README.md)|

### API Documentation

The application programming interface html [documentation is at link](https://gavinlyonsrepo.github.io/misc/software_docs/displaylib_16bit_PICO/index.html) 
 hosted on github pages and generated by Doxygen. Detailed project overview images are available there.

### Fonts

The font system readme is in the 'doc' folder [at link.](extra/doc/fonts/README.md)

## Software

### Advanced Graphics

There is an advanced graphics modes in library.
Standard graphics supports drawing lines, pixels
rectangles, triangles, circles and rounded rectangles.
Advanced graphics supports drawing polygons, dot grid, quadrilaterals, 
arcs, ellipses and lines at an angle. 
It is ON by default.
If you don't want these 'advanced' functions and want to save memory space: Simply 
comment out line 19 #define ADVANCED_GRAPHICS_ENABLE in displaylib_16_graphics.hpp. 
This will disable advanced graphics mode. If this is disabled some examples and demos 
may not work fully or even compile. 

### Advanced buffer mode

Advanced buffer mode. There is advanced buffer mode where the code writes to a global screen buffer instead of the VRAM of display. It is off by default more details at readme, 
which is in the 'doc' folder [at link.](extra/doc/buffer_mode/README.md)

### Test

There are multiple examples for each device,
User picks the one they want by editing the:
CMakeLists.txt :: add_executable(${PROJECT_NAME} section. 
Comment in one path and one path ONLY. See displays readme's for more details.

### Print

The print class can print integers, floats, characters, character arrays
and C++ std::strings. It can also format floating point numbers to a number of 
decimal places. and format integers in different base number systems.
Support for other data types can be added. 



### File system

Class Hierarchy:

![image link](https://github.com/gavinlyonsrepo/displaylib_16bit_PICO/blob/main/extra/image/filesystem.png)

### Error Codes 

Most functions that return a value, return a enum. 
Zero for success and a positive number for an error code. 
Error messages are outputted to console at 38400 baud.

## Notes

### Projects using library

* [**Super-Mario-Pico**](https://github.com/tansiret/Super-Mario-Pico):  
	A Super Mario Bros demo game for the Raspberry Pi Pico that uses this library to drive a 16-bit TFT display.

### Older versions

* This repository before version 2.0.0 was originally 
	called ST7735_TFT_PICO and just supported st7735. This older version
	is available in releases as 1.7.2.


