/*

	$Id: basic.c,v 1.128 2022/02/06 06:26:03 stefan Exp stefan $

	Stefan's basic interpreter 

	Playing around with frugal programming. See the licence file on 
	https://github.com/slviajero/tinybasic for copyright/left.
    (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)

	Author: Stefan Lenz, sl001@serverfabrik.de

	Hardware definition file coming with TinybasicArduino.ino aka basic.c

	- ARDUINOLCD, ARDUINOTFT and LCDSHIELD active the LCD code, 
		LCDSHIELD automatically defines the right settings for 
		the classical shield modules
	- ARDUINOPS2 activates the PS2 code. Default pins are 2 and 3.
		If you use other pins the respective changes have to be made 
			below. 
	- _if_  and PS2 are both activated STANDALONE cause the Arduino
			to start with keyboard and lcd as standard devices.
	- ARDUINOEEPROM includes the EEPROM access code
	- ARDUINOSD and ESPSPIFFS activate filesystem code 
	- activating Picoserial is not compatible with keyboard code
		Picoserial doesn't work on MEGA

	Architectures and the definitions from the Arduino IDE

	 	ARDUINO_ARCH_SAM: no tone command, dtostrf
	 	ARDUINO_ARCH_RP2040: dtostrf (for ARDUINO_NANO_RP2040_CONNECT)
	 	ARDUINO_ARCH_SAMD: dtostrf (for ARDUINO_SAMD_MKRWIFI1010, ARDUINO_SEEED_XIAO_M0)
		ARDUINO_ARCH_ESP8266: SPIFFS, dtostrf (ESP8266)
 		ARDUINO_AVR_MEGA2560, ARDUARDUINO_SAM_DUE: second serial port is Serial1 - no software serial
 		ARDUARDUINO_SAM_DUE: hardware heuristics
 		ARDUINO_ARCH_AVR: nothing
 		ARDUINO_ARCH_EXP32 and ARDUINO_TTGO_T7_V14_Mini32, no tone, no analogWrite, avr/xyz obsolete

 	The code still contains hardware hueristics from my own projects, 
 	will be removed in the future


*/

#if defined(ARDUINO) && ! defined(__HARDWAREH__)
#define __HARDWAREH__ 

/* 
	Arduino hardware settings , set here what you need or
	use one of the predefined configurations below

	input/output methods USERPICOSERIAL, ARDUINOPS2
		ARDUINOPRT, DISPLAYCANSCROLL, ARDUINOLCDI2C,
		ARDUINOTFT
	storage ARDUINOEEPROM, ARDUINOSD, ESPSPIFFS
	sensors ARDUINORTC, ARDUINOWIRE
	network ARDUINORF24

	leave this unset if you use the definitions below
*/
#undef USESPICOSERIAL 
#undef ARDUINOPS2
#undef ARDUINOPRT
#undef DISPLAYCANSCROLL
#undef ARDUINOLCDI2C
#undef LCDSHIELD
#undef ARDUINOTFT
#undef ARDUINOVGA
#undef ARDUINOEEPROM
#undef ARDUINOEEPROMI2C
#undef ARDUINOSD
#undef ESPSPIFFS
#undef RP2040LITTLEFS
#undef ARDUINORTC
#undef ARDUINOWIRE
#undef ARDUINORF24
#undef ARDUINOMQTT
#undef STANDALONE

/* 
	Predefined hardware configurations, this assumes that all of the 
	above are undef

	UNOPLAIN: 
		a plain UNO with no peripherals
	AVRLCD: 
		a AVR system with an LCD shield
	WEMOSSHIELD: 
		a Wemos D1 with a modified simple datalogger shield
		optional keyboard and i2c display
	MEGASHIELD: 
		an Arduino Mega with Ethernet Shield
		optional keyboard and i2c display
	TTGOVGA: 
		TTGO VGA1.4 system with PS2 keyboard, standalone
  	MEGATFT, DUETFT
    	TFT 7inch screen systems, standalone
*/

#undef UNOPLAIN
#undef AVRLCD
#undef WEMOSSHIELD
#undef MEGASHIELD
#undef TTGOVGA
#undef DUETFT
#undef MEGATFT

/* 
	PIN settings and I2C addresses for various hardware configurations
	used a few heuristics and then the hardware definitions above 

	#define SDPIN sets the SD CS pin - can be left as a default for most HW configs
    	TTGO needs it as default definitions in the board file are broken
	#define PS2DATAPIN, PS2IRQPIN sets PS2 pin
*/

// PS2 Keyboard 
#define PS2DATAPIN 3
#define PS2IRQPIN 2


// list of default i2c addresses
#define EEPROMI2CADDR 0x057
#define RTCI2CADDR 0x068

// the hardware models

// a Arduino with nothing else 
#if defined(UNOPLAIN)
#define USESPICOSERIAL
#define ARDUINOEEPROM
#endif

// a AVR ARDUINO (UNO or MEGA) with the classical LCD shield
#if defined(AVRLCD)
#define ARDUINOEEPROM
#define DISPLAYCANSCROLL
#define LCDSHIELD
#endif

// a Wemos ESP8266 with a mdified datalogger shield 
// standalone capable
#if defined(WEMOSSHIELD)
#define ARDUINOPS2
#define DISPLAYCANSCROLL
#define ARDUINOLCDI2C
#define ARDUINOSD
#define ARDUINORTC
#define RTCEEPROMSIZE 0
#define ARDUINOWIRE
#define SDPIN 		D8
#define PS2DATAPIN	D2
#define PS2IRQPIN	D9
#define ARDUINOMQTT
#define MEMSIZE 32000
#endif

// mega with a Ethernet shield 
// standalone capable
#if defined(MEGASHIELD)
#define ARDUINOEEPROM
#define ARDUINOPS2
#define DISPLAYCANSCROLL
#define ARDUINOLCDI2C
#define ARDUINOSD
#define ARDUINOWIRE
#define ARDUINOPRT
#define SDPIN 	4
#define MEMSIZE 5120
#endif

// VGA system with SD card, standalone by default
#if defined(TTGOVGA)
#define ARDUINOPS2
#define ARDUINOVGA
#define ARDUINOSD
#define SDPIN   13
#define STANDALONE
#endif

// MEGA with a TFT shield, standalone by default
#if defined(MEGATFT)
#define ARDUINOEEPROM
#define ARDUINOPS2
#define DISPLAYCANSCROLL
#define ARDUINOTFT
#define ARDUINOSD
#define ARDUINOWIRE
#define ARDUINOPRT
#define PS2DATAPIN 18
#define PS2IRQPIN  19
#define STANDALONE
#endif

// DUE with a TFT shield, standalone by default
#if defined(DUETFT)
#define ARDUINOPS2
#define DISPLAYCANSCROLL
#define ARDUINOTFT
#define ARDUINOSD
#define ARDUINOWIRE
#define ARDUINOPRT
#define PS2DATAPIN 9
#define PS2IRQPIN  8
#define STANDALONE
#endif

/* 
	the non AVR arcitectures - this is somehow raw
*/
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_RP2040) || defined (ARDUINO_ARCH_ESP32)
#ifndef ARDUINO_ARCH_ESP32
#include <avr/dtostrf.h>
#endif
#define ARDUINO 100
#undef ARDUINOEEPROM
#endif

/* 
	all microcontrollers, their libraries and dependencies
*/
// a clock needs wire
#ifdef ARDUINORTC
#define ARDUINOWIRE
#endif

// a display needs wire
#ifdef ARDUINOLCDI2C
#define ARDUINOWIRE
#endif

// a radio needs SPI
#ifdef ARDUINORF24
#define ARDUINOSPI
#endif

// a filesystem needs SPI
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
#define ARDUINOSPI
#endif

// graphics adapter
#if !defined(ARDUINOTFT) && !defined(ARDUINOVGA)
#undef HASGRAPH
#endif

#ifdef ARDUINOPS2
#include <PS2Keyboard.h>
#endif

#ifdef ARDUINOPROGMEM
#ifdef ARDUINO_ARCH_ESP32
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif
#endif

#ifdef ARDUINOEEPROM
#include <EEPROM.h>
#endif

#ifdef USESPICOSERIAL
#include <PicoSerial.h>
#endif

#ifdef ARDUINOSPI
#include <SPI.h>
#endif

#ifdef ARDUINOSD
#define FILESYSTEMDRIVER
#include <SD.h>
#endif

#ifdef ESPSPIFFS
#define FILESYSTEMDRIVER
#ifdef ARDUINO_ARCH_ESP8266
#include <FS.h>
#endif
#ifdef ARDUINO_ARCH_ESP32
#include <FS.h>
#include <SPIFFS.h>
#endif
#endif

#ifdef RP2040LITTLEFS
#define FILESYSTEMDRIVER
#define LFS_MBED_RP2040_VERSION_MIN_TARGET      "LittleFS_Mbed_RP2040 v1.1.0"
#define LFS_MBED_RP2040_VERSION_MIN             1001000
#define _LFS_LOGLEVEL_          1
#define RP2040_FS_SIZE_KB       64
#define FORCE_REFORMAT          false
#include <LittleFS_Mbed_RP2040.h>
#endif


// failsafe - the code only supports one filesystem right now
#if defined(ARDUINOSD)
#undef ESPSPIFFS
#undef RP2040LITTLEFS
#endif

#ifdef ARDUINOWIRE
#include <Wire.h>
#endif

#ifdef ARDUINOTFT
#include <memorysaver.h>
#include <UTFT.h>
#endif

// experimental - only implemented in ESP8266
#ifdef ARDUINOMQTT
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#endif
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#endif
#include <PubSubClient.h>
#endif

// VGA only implemented on one platform
#if defined(ARDUINOVGA) && defined(ARDUINO_TTGO_T7_V14_Mini32)
#include <WiFi.h> 
#include <fabgl.h> 
#endif

// real time clock support
#ifdef ARDUINORTC
#include <uRTCLib.h>
#endif

// support for external EEPROMs
#ifdef ARDUINOEEPROMI2C
#include <uEEPROMLib.h>
#endif


/* 
	Arduino default serial baudrate 
*/
const int serial_baudrate = 9600;
char sendcr = 0;
short blockmode = 0;

#ifdef ARDUINOPRT
const int serial1_baudrate = 9600;
#else 
const int serial1_baudrate = 0;
#endif

/* 
	handling time - part of the Arduino core - only needed on POSIX OSes
*/
void timeinit() {}


/* 
	start the SPI bus - this is a little mean as some libraries also 
	try to start the SPI which may lead to on override of the PIN settings
	if the library code is not clean - currenty no conflict known
*/
void spibegin() {
#ifdef ARDUINOSPI
#ifdef ARDUINO_TTGO_T7_V14_Mini32
	// this fixes the wrong board definition in the ESP32 
	// core of the particular platform, the definition uses
	// the default ESP32 MISO/MOSi/SCLK/SS pins of the ESP32
	// instead of the correct ones of the VGA box
 	SPI.begin(14, 2, 12, 13);
#else 
 	SPI.begin();
#endif
#endif
}

/*
	global variables for a standard LCD shield.
*/
#ifdef LCDSHIELD 
#define DISPLAYDRIVER
#include <LiquidCrystal.h>
// LCD shield pins to Arduino
//  RS, EN, d4, d5, d6, d7; 
// backlight on pin 10;
const int dsp_rows=2;
const int dsp_columns=16;
LiquidCrystal lcd( 8,  9,  4,  5,  6,  7);
void dspbegin() { 	lcd.begin(dsp_columns, dsp_rows); dspsetscrollmode(1, 1);  }
void dspprintchar(char c, short col, short row) { lcd.setCursor(col, row); lcd.write(c);}
void dspclear() { lcd.clear(); }
short keypadread(){
	short a=analogRead(A0);
	if (a > 850) return 0;
	else if (a>600 && a<800) return 's';
	else if (a>400 && a<600) return 'l';
	else if (a>200 && a<400) return 'd';
	else if (a>60  && a<200) return 'u';
	else if (a<60)           return 'r';
	return 0;
}
#endif

/* 
	global variables for a LCD display connnected
	 via i2c.
*/ 
#ifdef ARDUINOLCDI2C
#define DISPLAYDRIVER
#include <LiquidCrystal_I2C.h>
const int dsp_rows=4;
const int dsp_columns=20;
LiquidCrystal_I2C lcd(0x27, dsp_columns, dsp_rows);
void dspbegin() {   lcd.init(); lcd.backlight(); dspsetscrollmode(1, 1); }
void dspprintchar(char c, short col, short row) { lcd.setCursor(col, row); lcd.write(c);}
void dspclear() { lcd.clear(); }
#endif

/*
	global variables for a TFT
	this is code for a SD1963 800*480 board using the UTFT library
	it is mainly intended for a MEGA or DUE as a all in one system
	this is for a MEGA shield and the CTE DUE shield, for the due 
	you need to read the comment in Arduino/libraries/UTFT/hardware/arm
	HW_ARM_defines.h -> uncomment the DUE shield
*/
#ifdef ARDUINOTFT
#define DISPLAYDRIVER
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
#ifdef ARDUINO_SAM_DUE
UTFT tft(CTE70,25,26,27,28);
#else 
UTFT tft(CTE70,38,39,40,41);
#endif
const int dsp_rows=30;
const int dsp_columns=50;
char dspfontsize = 16;
void dspbegin() { tft.InitLCD(); tft.setFont(BigFont); tft.clrScr(); dspsetscrollmode(1, 4); }
void dspprintchar(char c, short col, short row) { tft.printChar(c, col*dspfontsize, row*dspfontsize); }
void dspclear() { tft.clrScr(); }
//experimental graphics code 
#ifdef HASGRAPH
void rgbcolor(int r, int g, int b) { tft.setColor(r,g,b); }
void vgacolor(short c) {
	short base=128;
	if (c==8) { rgbcolor(64, 64, 64); return; }
	if (c>8) base=255;
	rgbcolor(base*(c&1), base*((c&2)/2), base*((c&4)/4));  
}
void plot(int x, int y) { tft.drawPixel(x, y); }
void line(int x0, int y0, int x1, int y1)   { tft.drawLine(x0, y0, x1, y1); }
void rect(int x0, int y0, int x1, int y1)   { tft.drawRect(x0, y0, x1, y1); }
void frect(int x0, int y0, int x1, int y1)  { tft.fillRect(x0, y0, x1, y1); }
void circle(int x0, int y0, int r) { tft.drawCircle(x0, y0, r); }
void fcircle(int x0, int y0, int r) { tft.fillCircle(x0, y0, r); }
#endif
#endif

/* 
	this is the VGA code for fablib - first attempt to do this now
*/
#if defined(ARDUINOVGA) && defined(ARDUINO_TTGO_T7_V14_Mini32) 
fabgl::VGAController VGAController;
fabgl::Terminal      Terminal;
Canvas cv(&VGAController);
TerminalController tc(&Terminal);
Color vga_graph_pen = Color::BrightWhite;
Color vga_graph_brush = Color::Black;
Color vga_txt_pen = Color::BrightGreen;
Color vga_txt_background = Color::Black;
// this starts the vga controller and the terminal right now
void vgabegin() {
	VGAController.begin(GPIO_NUM_22, GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5, GPIO_NUM_4, GPIO_NUM_23, GPIO_NUM_15);
	VGAController.setResolution(VGA_640x200_70Hz);
  	Canvas cv(&VGAController);
	Terminal.begin(&VGAController);
	Terminal.setBackgroundColor(vga_txt_background);
	Terminal.setForegroundColor(vga_txt_pen);
  	Terminal.connectLocally();
  	Terminal.clear();
  	Terminal.enableCursor(true);
  	Terminal.setTerminalType(TermType::VT52);
}
void vgascale(int* x, int* y) {
	*y=*y*10/24;
}
void rgbcolor(int r, int g, int b) {
	short vga;
	if (r>191 || g>191 || b>191) vga=8; else vga=0;
	vga=vga+r/128+g/128*2+b/128*4;
	vga_graph_pen=fabgl::Color(vga);

}
void vgacolor(short c) { vga_graph_pen = fabgl::Color(c%16); }
void plot(int x, int y) { 
	vgascale(&x, &y);
	cv.setPenColor(vga_graph_pen);
	cv.setPixel(x,y);
	cv.setPenColor(vga_txt_pen);
}
void line(int x0, int y0, int x1, int y1) {
	vgascale(&x0, &y0);
	vgascale(&x1, &y1);
    cv.setPenColor(vga_graph_pen);
    cv.setPenWidth(1);
    cv.drawLine(x0, y0, x1, y1);
    cv.setPenColor(vga_txt_pen);
}
void rect(int x0, int y0, int x1, int y1) { 
	vgascale(&x0, &y0);
	vgascale(&x1, &y1);
	cv.setPenColor(vga_graph_pen);
    cv.setPenWidth(1);
    cv.drawRectangle(x0, y0, x1, y1);
    cv.setPenColor(vga_txt_pen);
}
void frect(int x0, int y0, int x1, int y1) {  
	vgascale(&x0, &y0);
	vgascale(&x1, &y1);
	cv.setBrushColor(vga_graph_pen);
    cv.fillRectangle(x0, y0, x1, y1);
    cv.setBrushColor(vga_txt_background);
}
void circle(int x0, int y0, int r) {  
	int rx = r;
	int ry = r;
	vgascale(&x0, &y0);
	vgascale(&rx, &ry);
	cv.setPenColor(vga_graph_pen);
    cv.setPenWidth(1);
    cv.drawEllipse(x0, y0, rx, ry);
    cv.setPenColor(vga_txt_pen);
}
void fcircle(int x0, int y0, int r) {  
	int rx = r;
	int ry = r;
	vgascale(&x0, &y0);
	vgascale(&rx, &ry);
	cv.setBrushColor(vga_graph_pen);
    cv.fillEllipse(x0, y0, rx, ry);
    cv.setBrushColor(vga_txt_background);	
}

void vgawrite(char c){
	switch(c) {
  		case 12: // form feed is clear screen
  			Terminal.write(27); Terminal.write('H');
  			Terminal.write(27); Terminal.write('J');
    		return;
  		case 10: // this is LF Unix style doing also a CR
  			Terminal.write(10); Terminal.write(13);
    		return;
  	}
  	Terminal.write(c);
}
#else 
void vgabegin(){}
void vgawrite(char c){}
#endif


/* 
	Keyboard code for either the Fablib Terminal class or 
	PS2Keyboard - please note that you need the ESP patched 
	version here
*/

#ifdef ARDUINO_TTGO_T7_V14_Mini32
#define PS2FABLIB
fabgl::PS2Controller PS2Controller;
#else
#if defined(ARDUINO) && defined(ARDUINOPS2)
#define PS2KEYBOARD
PS2Keyboard keyboard;
#endif
#endif

void kbdbegin() {
#ifdef PS2KEYBOARD
	keyboard.begin(PS2DATAPIN, PS2IRQPIN, PS2Keymap_German);
#else
#ifdef PS2FABLIB
	PS2Controller.begin(PS2Preset::KeyboardPort0);
	PS2Controller.keyboard()->setLayout(&fabgl::GermanLayout);
#endif
#endif
}

char kbdavailable(){
#ifdef PS2KEYBOARD
	return keyboard.available();
#else
#ifdef PS2FABLIB
	return Terminal.available();
#endif
#endif
	return 0;
}

char kbdread() {
#ifdef PS2KEYBOARD
	return keyboard.read();
#else
#ifdef PS2FABLIB
	return Terminal.read();
#endif
#endif
	return 0;
}

/*
	this is a generic display code 
	it combines the functions of LCD and TFT drivers
	if this code is active 
	dspprintchar(char c, short col, short row)
	dspclear()
	dspbegin()
	dspclear()
	have to be defined before in a hardware dependent section
*/

#ifdef DISPLAYDRIVER
short dspmycol = 0;
short dspmyrow = 0;
char esc = 0;

void dspsetcursor(short c, short r) {
	dspmyrow=r;
	dspmycol=c;
}

char dspactive() {
	return od & ODSP;
}

#ifdef HASVT52
/* nano vt52 state engine, vt52s is the state variable */
char vt52s = 0;

void dspvt52(char* c){
  
	switch (vt52s) {
		case 'Y':
			if (esc == 2) { dspmyrow=(*c-31)%dsp_rows; esc=1; return;}
			if (esc == 1) { dspmycol=(*c-31)%dsp_columns; *c=0; }
      		vt52s=0; 
			break;
	}
 
	switch (*c) {
		case 'A': // cursor up
			if (dspmyrow>0) dspmyrow--;
			break;
		case 'B': // cursor down
		    dspmyrow=(dspmyrow++) % dsp_rows;
			break;
		case 'C': // cursor right
			dspmycol=(dspmycol++) % dsp_columns;
			break; 
		case 'D': // cursor left
			if (dspmycol>0) dspmycol--;
			break;
		case 'E': // GEMDOS / TOS extension clear screen
		    dspbufferclear();
    		dspclear();
			break;
		case 'H': // cursor home
		    dspmyrow=0;
    		dspmycol=0;
			break;	
		case 'Y': // Set cursor position
			vt52s='Y';
			esc=2;
      		*c=0;
			return;
		case 'F': // enter graphics mode
		case 'G': // exit graphics mode
		case 'I': // reverse line feed
		case 'J': // clear to end of screen
		case 'K': // clear to end of line
		case 'L': // Insert line
		case 'M': // Delete line
		case 'Z': // Ident
		case '=': // alternate keypad on
		case '>': // alternate keypad off
		case 'b': // GEMDOS / TOS extension text color
		case 'c': // GEMDOS / TOS extension background color
		case 'd': // GEMDOS / TOS extension clear to start of screen
		case 'e': // GEMDOS / TOS extension enable cursor
		case 'f': // GEMDOS / TOS extension disable cursor
		case 'j': // GEMDOS / TOS extension restore cursor
		case 'k': // GEMDOS / TOS extension save cursor
		case 'l': // GEMDOS / TOS extension clear line
		case 'o': // GEMDOS / TOS extension clear to start of line		
		case 'p': // GEMDOS / TOS extension reverse video
		case 'q': // GEMDOS / TOS extension normal video
		case 'v': // GEMDOS / TOS extension enable wrap
		case 'w': // GEMDOS / TOS extension disable wrap
		case '^': // Printer extensions - print on
		case '_': // Printer extensions - print off
		case 'W': // Printer extensions - print without display on
		case 'X': // Printer extensions - print without display off
		case 'V': // Printer extensions - print cursor line
		case ']': // Printer extension - print screen 
			break;
	}
      esc=0;
      *c=0;
}
#endif

#ifdef DISPLAYCANSCROLL
char dspbuffer[dsp_rows][dsp_columns];
char  dspscrollmode = 0;
short dsp_scroll_rows = 1; 

// 0 normal scroll
// 1 enable waitonscroll function
// ... more to come
void dspsetscrollmode(char c, short l) {
	dspscrollmode = c;
	dsp_scroll_rows = l;
}

void dspbufferclear() {
	short r,c;
	for (r=0; r<dsp_rows; r++)
		for (c=0; c<dsp_columns; c++)
      		dspbuffer[r][c]=0;
  	dspmyrow=0;
  	dspmycol=0;
}

void dspscroll(){
	short r,c;
	int i;
  	char a,b;

  	// shift dsp_scroll_rows up
  	for (r=0; r<dsp_rows-dsp_scroll_rows; r++) { 
    	for (c=0; c<dsp_columns; c++) {
        	a=dspbuffer[r][c];
        	b=dspbuffer[r+dsp_scroll_rows][c];
        	if ( a != b ) {
            	if (b >= 32) dspprintchar(b, c, r); else dspprintchar(' ', c, r);
        	}      
        	dspbuffer[r][c]=b;
    	} 
    }

    // delete the characters in the remaining lines
  	for (r=dsp_rows-dsp_scroll_rows; r<dsp_rows; r++) {
    	for (c=0; c<dsp_columns; c++) {
			if (dspbuffer[r][c] > 32) dspprintchar(' ', c, r); 
        	dspbuffer[r][c]=0;     
    	}
    }
  
	// set the cursor to the fist free line	    
  	dspmycol=0;
    dspmyrow=dsp_rows-dsp_scroll_rows;
}

void dspwrite(char c){

#ifdef HASVT52
    // escape character received - we switch to vt52 mode
    // the character is modified and then handed over to the
    // internal pipeline
    if (esc) dspvt52(&c);
#endif

    // will be reworked soon - not super good 
  	switch(c) {
  		case 10: // this is LF Unix style doing also a CR
    		dspmyrow=(dspmyrow + 1);
    		if (dspmyrow >= dsp_rows) dspscroll(); 
    		dspmycol=0;
    		return;
    	case 12: // form feed is clear screen - deprecated
    		dspbufferclear();
    		dspclear();
    		return;
    	case 13: // classical carriage return 
    		dspmycol=0;
    		return;
        case 27: // escape - initiate vtxxx mode
        	esc=1;
        	return;
    	case 127: // delete
    		if (dspmycol > 0) {
      			dspmycol--;
      			dspbuffer[dspmyrow][dspmycol]=0;
      			dspprintchar(' ', dspmycol, dspmyrow);
      			return;
    		}
  	}

	if (c < 32 ) return; 

	dspprintchar(c, dspmycol, dspmyrow);
	dspbuffer[dspmyrow][dspmycol++]=c;
	if (dspmycol == dsp_columns) {
		dspmycol=0;
		dspmyrow=(dspmyrow + 1);
    	if (dspmyrow >= dsp_rows) dspscroll(); 
	}
}

char dspwaitonscroll() {
  	char c;

	if ( dspscrollmode == 1 ) {
		if (dspmyrow == dsp_rows-1) {
        	c=inch();
        	if (c == ' ') outch(12);
		    return c;
		}
	}
	return 0;
}

// code for low memory simple display access - can still be simplified
#else 

void dspbufferclear() {}

char dspwaitonscroll() {
	return 0;
}

void dspwrite(char c){

#ifdef HASVT52
	// escape character received - we switch to vt52 mode
    // the character is modified and then handed over to the
    // internal pipeline
    if (esc) { dspvt52(&c); }
#endif

	switch(c) {
  		case 12: // form feed is clear screen
    		dspclear();
    		return;
  		case 10: // this is LF Unix style doing also a CR
    		dspmyrow=(dspmyrow + 1)%dsp_rows;
    		dspmycol=0;
    		return;
    	case 11: // one char down 
    		dspmyrow=(dspmyrow+1) % dsp_rows;
    		return;
    	case 13: // classical carriage return 
    		dspmycol=0;
    		return;
        case 27: // escape - initiate vtxxx mode
        	esc=1;
        	return;
    	case 127: // delete
    		if (dspmycol > 0) {
      			dspmycol--;
      			dspprintchar(' ', dspmycol, dspmyrow);
    		}
    		return;
  	}

	if (c < 32 ) return; 

	dspprintchar(c, dspmycol++, dspmyrow);
	dspmycol=dspmycol%dsp_columns;
}

void dspsetscrollmode(char c, short l) {}
#endif
#else
const int dsp_rows=0;
const int dsp_columns=0;
void dspwrite(char c){};
void dspbegin() {};
char dspwaitonscroll() { return 0; };
char dspactive() {return FALSE; }
void dspsetscrollmode(char c, short l) {}
void dspsetcursor(short c, short r) {}
#endif

/* 
	Arduino Real Time clock code based on uRTC 
	this is a minimalistic library 
*/

#ifdef ARDUINORTC
uRTCLib rtc(RTCI2CADDR);

char rtcstring[18] = { 0 }; 

char* rtcmkstr() {
	int cc = 1;
	short t;
	char ch;
	t=rtcget(2);
	rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]=':';
	t=rtcread(1);
	rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]=':';
	t=rtcread(0);
	rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]='-';
	t=rtcread(3);
	if (t/10 > 0) rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]='/';
	t=rtcread(4);
	if (t/10 > 0) rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]='/';
	t=rtcread(5);
	if (t/10 > 0) rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc]=0;
	rtcstring[0]=cc-1;

	return rtcstring;
}

short rtcread(char i) {
	switch (i) {
		case 0: 
			return rtc.second();
		case 1:
			return rtc.minute();
		case 2:
			return rtc.hour();
		case 3:
			return rtc.day();
		case 4:
			return rtc.month();
		case 5:
			return rtc.year();
		case 6:
			return rtc.dayOfWeek();
		case 7:
			return rtc.temp();
		default:
			return 0;
	}
}

short rtcget(char i) {
	rtc.refresh();
	return rtcread(i);
}

void rtcset(char i, short v) {
	uint8_t tv[7];
	char j;
	rtc.refresh();
	for (j=0; j<7; j++) tv[j]=rtcread(j);
	tv[i]=v;
	rtc.set(tv[0], tv[1], tv[2], tv[6], tv[3], tv[4], tv[5]);
}
#endif

/* 
	External EEPROM   
*/
#ifdef ARDUINOEEPROMI2C
#ifndef RTCEEPROMSIZE
#define RTCEEPROMSIZE 4096 
#endif
uEEPROMLib c_eeprom(EEPROMI2CADDR);
#else
#define RTCEEPROMSIZE 0
#endif

/* 
	Arduino Sensor library code - very experimental 
*/
#ifdef ARDUINOSENSORS
#include "DHT.h"
#define DHTTYPE DHT11
#define DHTPIN 22
DHT dht(DHTPIN, DHTTYPE);
void sensorbegin(){
  dht.begin();
}
number_t sensorread(short i) {
  switch (i) {
    case 1:
      return dht.readHumidity();
    case 2:
      return dht.readTemperature();
    default:
      return 0;
  }
}
#else
void sensorbegin() {}
number_t sensorread(short i) {return 0;};
#endif


/* 
	start a secondary serial port for printing and/or networking 
*/
#ifdef ARDUINOPRT
#if !defined(ARDUINO_AVR_MEGA2560) && !defined(ARDUINO_SAM_DUE)
#include <SoftwareSerial.h>
/* definition of the serial port pins from "pretzel board"
for UNO 11 is not good for rx */
const int software_serial_rx = 11;
const int software_serial_tx = 12;
SoftwareSerial Serial1(software_serial_rx, software_serial_tx);
#endif

// second serial port
void prtbegin() {
	Serial1.begin(serial1_baudrate);
}

void prtwrite(char c) {
#ifdef ARDUINOPRT
	Serial1.write(c);
#endif
}

char prtread() {
	while (!Serial1.available()) byield();
	return Serial1.read();
}

char prtcheckch() {
	if (Serial1.available()) return Serial1.peek(); else return 0;
}

short prtavailable() {
	return Serial1.available();
}
#endif

/*
	 definitions for the nearfield module, still very experimental
*/
#ifdef ARDUINORF24
const char rf24_ce = 8;
const char rf24_csn = 9;
#include <nRF24L01.h>
#include <RF24.h>
rf24_pa_dbm_e rf24_pa = RF24_PA_MAX;
RF24 radio(rf24_ce, rf24_csn);
#endif


/*
	definitions for ESP Wifi and MQTT, super experimental
	all in one again
*/
#ifdef ARDUINOMQTT
#include "wifisettings.h"
const char* mqtt_server = "test.mosquitto.org";
const short mqtt_port = 1883;
WiFiClient bwifi;
PubSubClient bmqtt(bwifi);


// the length of the outgoing and incomming topic 
#define MQTTLENGTH 32
char mqtt_otopic[MQTTLENGTH];
char mqtt_itopic[MQTTLENGTH];

// the buffer for incoming MQTT messages 
// this is static and currently short
// later a string should be applicable here 
#define MQTTBLENGTH 128
volatile char mqtt_buffer[MQTTBLENGTH];
volatile short mqtt_messagelength;

// the name of the client
char mqttname[12] = "iotbasicxxx";

// new client name
void mqttsetname() {
	long m = millis();
	mqttname[8]=(char)(65+m%26);
	m=m/26;
	mqttname[9]=(char)(65+m%26);
	m=m/26;
	mqttname[10]=(char)(65+m%26);
}

// the begin method 
// needs the settings from wifisettings.h
void netbegin() {
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	WiFi.setAutoReconnect(true);
  	WiFi.persistent(true);
}

// the connected method
char netconnected() {
	return(WiFi.status() == WL_CONNECTED);
}

// the mqtt callback function, using the byte type here
// because payload can be binary - interface to BASIC 
// strings need to be done
void mqttcallback(char* t, byte* p, unsigned int l) {
	short i;
	mqtt_messagelength=l;
	for(i=0; i<l; i++) mqtt_buffer[i]=(char)p[i];
	// here hook to register an MQTT event to basic
}

// starting mqtt 
void mqttbegin() {
	bmqtt.setServer(mqtt_server, mqtt_port);
	bmqtt.setCallback(mqttcallback);
	*mqtt_itopic=0;
	*mqtt_otopic=0;
}

// reconnecting mqtt - exponential backoff here 
char mqttreconnect() {
	
	// exponental backoff reconnect in 10 ms * 2^n intervals
	short timer=10;
    char reconnect=0;
    mqttsetname();
	while (!bmqtt.connected() && timer < 400) {
		bmqtt.connect(mqttname);
		delay(timer);
		timer=timer*2;
    	reconnect=1;
	}

	// after reconnect resubscribe if there is a valid topic
	if (*mqtt_itopic && bmqtt.connected() && reconnect) bmqtt.subscribe(mqtt_itopic);
 
	return bmqtt.connected();
}

// mqtt state 
int mqttstate() {
	return bmqtt.state();
}

// subscribing to a topic
void mqttsubscribe(char *t) {
	short i;
	for (i=0; i<MQTTLENGTH; i++) {
		if ((mqtt_itopic[i]=t[i]) == 0 ) break;
	}
	if (!mqttreconnect()) {ert=1; return;};
	if (!bmqtt.subscribe(mqtt_itopic)) ert=1;
}

void mqttunsubscribe() {
	if (!mqttreconnect()) {ert=1; return;};
	if (!bmqtt.unsubscribe(mqtt_itopic)) ert=1;
	*mqtt_itopic=0;
}

// set the topic we pushlish, comming from print
// basic can do only one topic 
void mqttsettopic(char *t) {
	short i;
	for (i=0; i<MQTTLENGTH; i++) {
		if ((mqtt_otopic[i]=t[i]) == 0 ) break;
	}
}

// print a mqtt message
void mqttouts(char *m, short l) {
	if (!mqttreconnect()) {ert=1; return;};
	if (!bmqtt.publish(mqtt_otopic, (uint8_t*) m, (unsigned int) l, false)) ert=1;
} 

// ins copies the buffer into a basic string - behold the jabberwock - length gynmastics
void mqttins(char *b, short nb) {
	for (z.a=0; z.a<nb && z.a<mqtt_messagelength; z.a++) b[z.a+1]=mqtt_buffer[z.a];
 	b[0]=z.a;
 	mqtt_messagelength=0;
 	*mqtt_buffer=0;
}

// just one character to emulate basic get
char mqttinch() {
	char ch=0;
	short i;
	if (mqtt_messagelength>0) {
		ch=mqtt_buffer[0];
		for (i=0; i<mqtt_messagelength-1; i++) mqtt_buffer[i]=mqtt_buffer[i+1];
	}
	mqtt_messagelength--;
	return ch;
}

#else 
void netbegin() {}
char netconnected() { return 0; }
void mqttbegin() {}
int  mqttstate() {return 0;}
void mqttsubscribe(char *t) {}
void mqttsettopic(char *t) {}
void mqttouts(char *m, short l) {}
void mqttins(char *b, short nb) { z.a=0; };
char mqttinch() {return 0;};
#endif

/* 
	EEPROM handling, these function enable the @E array and 
	loading and saving to EEPROM, they depend on the 
	hardware and wire settings in ARDUINORTC

*/ 

void ebegin(){}
void eflush(){}

// the external module EEPROM is the only EEPROM of the system
#if defined(ARDUINOEEPROMI2C) && ! defined(ARDUINOEEPROM)
address_t elength() { return RTCEEPROMSIZE; }
short eread(address_t a) { return (signed char) c_eeprom.eeprom_read(a); }
void eupdate(address_t a, short c) { 
	short b = eread(a);
	if (b != c) c_eeprom.eeprom_write(a, (signed char) c);
}
#endif

// only the internal Arduino EEPROM, no external EEPROM
#if ! defined(ARDUINOEEPROMI2C) && defined(ARDUINOEEPROM)
address_t elength() { return EEPROM.length(); }
void eupdate(address_t a, short c) { EEPROM.update(a, c); }
short eread(address_t a) { return (signed char) EEPROM.read(a); }
#endif

// the RTC EEPROM extends the internal EEPROM / internal is always first
#if defined(ARDUINOEEPROMI2C) && defined(ARDUINOEEPROM)
address_t elength() { return EEPROM.length()+RTCEEPROMSIZE; }
void eupdate(address_t a, short c) { 
	if (a<EEPROM.length()) 
		EEPROM.update(a, c); 
	else {
		short b = c_eeprom.eeprom_read(a-EEPROM.length());
		if (b != c) c_eeprom.eeprom_write(a-EEPROM.length(), (signed char) c);
	}	
}
short eread(address_t a) { 
	if (a<EEPROM.length())
		return (signed char) EEPROM.read(a); 
	else
		return (signed char) c_eeprom.eeprom_read(a-EEPROM.length()); 
}
#endif

// no EEPROM present
#if ! defined(ARDUINOEEPROMI2C) && ! defined(ARDUINOEEPROM)
address_t elength() { return 0; }
void eupdate(address_t a, short c) { return; }
short eread(address_t a) { return 0; }
#endif

// save a file to EEPROM
void esave() {
	address_t a=0;
	if (top+eheadersize < elength()) {
		a=0;
		eupdate(a++, 0); // EEPROM per default is 255, 0 indicates that there is a program

		// store the size of the program in byte 1,2 of the EEPROM	
		z.a=top;
		esetnumber(a, addrsize);
		a+=addrsize;

		while (a < top+eheadersize){
			eupdate(a, mem[a-eheadersize]);
			a++;
		}
		eupdate(a++,0);
	} else {
		error(EOUTOFMEMORY);
		er=0; //oh oh! check this.
	}
}

// load a file from EEPROM
void eload() {
	address_t a=0;
	if (elength()>0 && (eread(a) == 0 || eread(a) == 1)) { // have we stored a program
		a++;

		// how long is it?
		egetnumber(a, addrsize);
		top=z.a;
		a+=addrsize;

		while (a < top+eheadersize){
			mem[a-eheadersize]=eread(a);
			a++;
		}
	} else { // no valid program data is stored 
		error(EEEPROM);
	}
}

void autorun() {
#ifdef ARDUINOEEPROM
  	if (eread(0) == 1){ // autorun from the EEPROM
  		egetnumber(1, addrsize);
  		top=z.a;
  		st=SERUN;
  		return;    // EEPROM autorun overrule filesystem autorun
  	} 
#endif
// here filesystem autorun, ugly still
#if defined(ESPSPIFFS)
  	if (ifileopen("/autoexec.bas")) {
  		xload("/autoexec.bas");
  		st=SRUN;
  	}
  	ifileclose();
#endif	
#if defined(ARDUINOSD) 
  	if (ifileopen("autoexec.bas")) {
  		xload("autoexec.bas");
  		st=SRUN;
  	}
  	ifileclose();
#endif
}

/* the wrappers of the arduino io functions, to avoid 
   spreading arduino code in the interpreter code 
   also, this would be the place to insert the Wiring code
   for raspberry */
#ifdef ARDUINO_ARCH_ESP32
void analogWrite(int a, int b){}
#endif

void aread(){ push(analogRead(pop())); }

void dread(){ push(digitalRead(pop())); }

void awrite(number_t p, number_t v){
	if (v >= 0 && v<256) analogWrite(p, v);
	else error(ERANGE);
}

void dwrite(number_t p, number_t v){
	if (v == 0) digitalWrite(p, LOW);
	else if (v == 1) digitalWrite(p, HIGH);
	else error(ERANGE);
}

void pinm(number_t p, number_t m){
	if (m>=0 && m<=2)  pinMode(p, m);
	else error(ERANGE); 
}

void bmillis() {
	number_t m;
	// millis is processed as integer and is cyclic mod maxnumber and not cast to float!!
	m=(number_t) (millis()/(unsigned long)pop() % (unsigned long)maxnum);
	push(m); 
};

void bpulsein() { 
  unsigned long t, pt;
  t=((unsigned long) pop())*1000;
  y=pop(); 
  x=pop(); 
  pt=pulseIn(x, y, t)/10; 
  push(pt);
}


/* 
	the yield function is called after every statement
	if allows background tasks on OSes like FreeRTOS 
	On an ESP8266 this function is called every 100 microseconds
	(after each statement) in RUN mode. BASIC DELAY calls 
	this every YIELDTIME ms. 
*/
void byield() {	
#ifdef ARDUINOMQTT
	if ( millis()-lastyield > YIELDINTERVAL-1 ) {
      bmqtt.loop();
      lastyield=millis();
      delay(YIELDTIME); 
  	}
#endif
  	delay(0);
}

/* 

	Platform dependend IO functions, implemented models are
		- Arduino Serial 
		- Arduino Picoserial
		- SD filesystems
		- SPIFFS filesystems

	Wrappers around the OS functions for file and dir access

*/

/* 
	The file system driver - all methods needed to support BASIC fs access
*/

#if defined(ARDUINOSD) || defined(ESPSPIFFS)
File ifile;
File ofile;
char tempname[SBUFSIZE];
#ifdef ARDUINOSD
File root;
File file;
#endif
#ifdef ESPSPIFFS
#ifdef ARDUINO_ARCH_ESP8266
Dir root;
File file;
#endif
#ifdef ARDUINO_ARCH_ESP32
File root;
File file;
#endif
#endif
#ifdef ARDUINO_ARCH_ESP8266
#define FILE_OWRITE (sdfat::O_READ | sdfat::O_WRITE | sdfat::O_CREAT | sdfat::O_TRUNC)
#else 
#ifdef ARDUINO_ARCH_ESP32
#define FILE_OWRITE FILE_WRITE
#else 
#define FILE_OWRITE (O_READ | O_WRITE | O_CREAT | O_TRUNC)
#endif
#endif
#endif

// using the POSIX API in LittleFS
#ifdef RP2040LITTLEFS
// file I/O and dirs
#define FILESYSTEMDRIVER
FILE* ifile;
FILE* ofile;
DIR* root;
struct dirent* file;
LittleFS_MBED *myFS;
const char rootfs[10] = MBED_LITTLEFS_FILE_PREFIX;
char tmpfilename[10+SBUFSIZE];
// add the prefix
char* mkfilename(char* filename) {
	short i,j;
	for(i=0; i<10 && rootfs[i]!=0; i++) tmpfilename[i]=rootfs[i];
	tmpfilename[i++]='/';
	for(j=0; j<SBUFSIZE && filename[j]!=0; j++) tmpfilename[i++]=filename[j];
	return tmpfilename;
}
#endif

/* 
	filesystem starter for SPIFFS and SD on ESP, ESP32 and Arduino plus LittleFS
*/
void fsbegin() {
#ifdef ARDUINOSD 
#ifndef SDPIN
#define SDPIN
#endif
 	if (SD.begin(SDPIN)) { outsc("SDcard ok \n"); }	
#endif
#if defined(ESPSPIFFS) && defined(ARDUINO_ARCH_ESP8266) 
 	if (SPIFFS.begin()) {
		outsc("SPIFFS ok \n");
		FSInfo fs_info;
		SPIFFS.info(fs_info);
		outsc("File system size "); outnumber(fs_info.totalBytes); outcr();
		outsc("File system used "); outnumber(fs_info.usedBytes); outcr();
 	}
#endif
#if defined(ESPSPIFFS) && defined(ARDUINO_ARCH_ESP32) 
 	if (SPIFFS.begin()) {
		outsc("SPIFFS ok \n"); outcr();
 	}
#endif
#ifdef RP2040LITTLEFS
	myFS = new LittleFS_MBED();
	if (myFS->init()) outsc("LittleFS ok \n");
#endif
}

/*
	File I/O function on an Arduino
*/
void filewrite(char c) {
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
	if (ofile) ofile.write(c); else ert=1;
#endif
#if defined(RP2040LITTLEFS)
	if (ofile) fputc(c, ofile); else ert=1;
#endif
}

char fileread(){
	char c;
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
	if (ifile) c=ifile.read(); else { ert=1; return 0; }
	if (c == -1 ) ert=-1;
	return c;
#endif
#ifdef RP2040LITTLEFS
	if (ifile) c=fgetc(ifile); else { ert=1; return 0; }
	if (c == -1 ) ert=-1;
	return c;
#endif
	return 0;
}


char ifileopen(char* filename){
#ifdef ARDUINOSD
	ifile=SD.open(filename, FILE_READ);
	return (int) ifile;
#endif
#ifdef ESPSPIFFS
	ifile=SPIFFS.open(filename, "r");
	return (int) ifile;
#endif
#ifdef RP2040LITTLEFS
	ifile=fopen(mkfilename(filename), "r");
	return (int) ifile;
#endif
	return 0;
}

void ifileclose(){
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
	ifile.close();
#endif	
#ifdef RP2040LITTLEFS
	if (ifile) fclose(ifile);
	ifile=NULL;	
#endif
}

char ofileopen(char* filename){
#ifdef ARDUINOSD
	ofile=SD.open(filename, FILE_OWRITE);
	return (int) ofile;
#endif
#ifdef ESPSPIFFS
	ofile=SPIFFS.open(filename, "w");
	return (int) ofile;
#endif
#ifdef RP2040LITTLEFS
	ofile=fopen(mkfilename(filename), "w");
	return (int) ofile; 
#endif
	return 0;
}

void ofileclose(){
#ifndef ARDUINO
	if (ofile) fclose(ofile);
#endif
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
    ofile.close();
#endif
#ifdef RP2040LITTLEFS
	if (ofile) fclose(ofile); 
#endif
}

int fileavailable(){
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
	return ifile.available();
#endif
#ifdef RP2040LITTLEFS
	return !feof(ifile);
#endif
	return 0;
}

void rootopen() {
#ifdef ARDUINOSD
	root=SD.open("/");
#endif
#ifdef ESPSPIFFS
#ifdef ARDUINO_ARCH_ESP8266
	root=SPIFFS.openDir("/");
#endif
#ifdef ARDUINO_ARCH_ESP32
	root=SPIFFS.open("/");
#endif
#endif
#ifdef RP2040LITTLEFS
	root=opendir(rootfs);
#endif
}

int rootnextfile() {
#ifdef ARDUINOSD
	file=root.openNextFile();
	return (file != 0);
#endif
#ifdef ESPSPIFFS
#ifdef ARDUINO_ARCH_ESP8266
  	if (root.next()) {
    	file=root.openFile("r");
    	return 1;
  	} else {
    	return 0;
	}
#endif
#ifdef ARDUINO_ARCH_ESP32
	file=root.openNextFile();
	return (file != 0);
#endif
#endif
#ifdef RP2040LITTLEFS
	file = readdir(root);
	return (file != 0);
#endif
  return FALSE;
}

int rootisfile() {
#ifdef ARDUINOSD
	return (! file.isDirectory());
#endif
#ifdef ESPSPIFFS
#ifdef ARDUINO_ARCH_ESP8266
	return TRUE;
#endif
#ifdef ARDUINO_ARCH_ESP32
	return (! file.isDirectory());
#endif
#endif
#ifdef RP2040LITTLEFS
	return (file->d_type == DT_REG);
#endif
	return FALSE;
}


char* rootfilename() {
#ifdef ARDUINOSD
	return (char*) file.name();
#endif
#ifdef ESPSPIFFS
#ifdef ARDUINO_ARCH_ESP8266
   	// c_str() sometimes broken
  	int i=0;
	String s=root.fileName();
 	for (i=0; i<s.length(); i++) { tempname[i]=s[i]; }
 	tempname[i]=0;
	return tempname;
#endif
#ifdef ARDUINO_ARCH_ESP32
	return (char*) file.name();
#endif
#endif
#ifdef RP2040LITTLEFS
	return (file->d_name);
#endif
	return FALSE; 
}

int rootfilesize() {
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
  return file.size();
#endif  
#ifdef RP2040LITTLEFS
#endif
  return 0;
}

void rootfileclose() {
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
  file.close();
#endif 
#ifdef RP2040LITTLEFS
#endif
}

void rootclose(){
#ifdef ARDUINOSD
  root.close();
#endif
#ifdef ESPSPIFFS
#ifdef ARDUINO_ARCH_ESP8266
  return;
#endif
#ifdef ARDUINO_ARCH_ESP32
  root.close();
#endif
#endif
#ifdef RP2040LITTLEFS
#endif
}


void removefile(char *filename) {
#ifdef ARDUINOSD	
	SD.remove(filename);
	return;
#endif
#ifdef ESPSPIFFS
	SPIFFS.remove(filename);
	return;
#endif
#ifdef RP2040LITTLEFS
	remove(mkfilename(filename));
	return;
#endif
}

/*

	Picoserial allows to define an own input buffer and an 
	interrupt function. This is used to fill the input buffer 
	directly on read. Write is standard like in the Serial code.

*/

#ifdef USESPICOSERIAL
volatile static char picochar;
volatile static char picoa = FALSE;
volatile static char* picob = NULL;
static short picobsize = 0;
volatile static short picoi = 1;

void picogetchar(int c){
	if (picob && (! picoa) ) {
    	picochar=c;
		if (picochar != '\n' && picochar != '\r' && picoi<picobsize-1) {
			picob[picoi++]=picochar;
			outch(picochar);
		} else {
			picoa = TRUE;
			picob[picoi]=0;
			picob[0]=picoi;
			z.a=picoi;
			picoi=1;
		}
		picochar=0; // every buffered byte is deleted
	} else {
    	if (c != 10) picochar=c;
	}
}
#endif


char serialread() {
#ifdef USESPICOSERIAL
	char c;
	c=picochar;
	picochar=0;
	return c;	
#else
	while (!Serial.available()) byield();
	return Serial.read();
#endif
}


// serial begin code
void serialbegin() {
#ifdef USESPICOSERIAL
	(void) PicoSerial.begin(serial_baudrate, picogetchar); 
#else
	Serial.begin(serial_baudrate);
#endif
}


// write to a serial stream
void serialwrite(char c) {
#ifdef USESPICOSERIAL
	PicoSerial.print(c);
#else
	Serial.write(c);	
#endif
}


// check on a character
short serialcheckch() {
#ifdef USESPICOSERIAL
	return picochar;
#else
	if (Serial.available()) return Serial.peek(); else return 0;
#endif	
}

// check on a character
short serialavailable() {
#ifdef USESPICOSERIAL
	if (id == ISERIAL) return picoi;
#else
	return Serial.available();
#endif	
}



// reading from the console with inch or the picoserial callback
void consins(char *b, short nb) {
	char c;
	short i = 1;
#ifdef USESPICOSERIAL
	picob=b;
	picobsize=nb;
	picoa=FALSE;
	while (! picoa);
	//outsc(b+1); 
	outcr();
	return;
#else
	while(i < nb) {
  	c=inch();
  	if (id == ISERIAL || id == IKEYBOARD) outch(c);
  	if (c == '\r') c=inch(); /* skip carriage return */
  	if (c == '\n') {
    	break;
  	} else if ( (c == 127 || c == 8) && i>1) {
   	 i--;
  	} else {
   	 b[i++]=c;
  	} 
	}
	b[i]=0;
  b[0]=(unsigned char)i-1;
  z.a=i-1; 
#endif
}

/* 

 this can result in multiple wire begin calls
 no protection here for user errors from BASIC 

 */ 
#ifdef ARDUINOWIRE
uint8_t wire_slaveid = 0;
uint8_t wire_myid = 0;
#endif

// default begin is as a master
void wirebegin() {
#ifdef ARDUINOWIRE
	Wire.begin();
#endif
}

// as a master open sets the slave id for the communication
// no extra begin while we stay master
void wireopen(char* s) {
#ifdef ARDUINOWIRE
	if (s[0] == 'm') {
		wire_slaveid=s[1];
		wire_myid=0;
	} else if ( s[0] == 's' ) { 
		wire_myid=s[1];
		wire_slaveid=0;
		Wire.begin(wire_myid);
		// here the slave code if this Arduino is a slave
		// to be done
	} else 
		error(ERANGE);
#endif
}

void wireins(char *b, uint8_t l) {
#ifdef ARDUINOWIRE
	z.a=0;
	Wire.requestFrom(wire_slaveid, l);
	while (Wire.available() && z.a<l) b[++z.a]=Wire.read();
	b[0]=z.a;
#else 
	b[0]=0;
	z.a=0;
#endif
}

void wireouts(char *b, uint8_t l) {
#ifdef ARDUINOWIRE
	  Wire.beginTransmission(wire_slaveid); 
	  for(z.a=0; z.a<l; z.a++) Wire.write(b[z.a]);     
	  ert=Wire.endTransmission(); 
#endif
}

/* 
	read from the radio interface, radio is always block 
	oriented. This function is called from ins for an entire 
	line.

	In blockmode the entire message is returned in the 
	receiving string while in line mode the length of the 
	string is adapted. Blockmode can be used to transfer
	binary data.

*/

#ifdef ARDUINORF24
/*
	generate a uint64_t pipe address from the filename string
	for RF64, horner schema to be on the save side
*/
uint64_t pipeaddr(char * f){
	uint64_t t = 0;
	t=(uint8_t)f[0];
	for(short i=1; i<=4; i++) t=t*256+(uint8_t)f[i];
	return t;
} 
#endif

void radioins(char *b, short nb) {
#ifdef ARDUINORF24
    if (radio.available()) {
    	radio.read(b+1, nb);
    	if (!blockmode) {
        	for (z.a=0; z.a<nb; z.a++) if (b[z.a+1]==0) break;		
    	} else {
    		z.a=radio.getPayloadSize();
      		if (z.a > nb) z.a=nb;
    	}
      	b[0]=z.a;
	} else {
      	b[0]=0; 
      	b[1]=0;
      	z.a=0;
	}
#else 
      b[0]=0; 
      b[1]=0;
      z.a=0;
#endif
}

/* write to radio, no character mode here */
void radioouts(char *b, short l) {
#ifdef ARDUINORF24
	radio.stopListening();
	if (radio.write(b, l)) ert=0; else ert=1;
	radio.startListening();
#endif
}

/* 
	we always read from pipe 1 and use pipe 0 for writing, 
	the filename is the pipe address, by default the radio 
	goes to reading mode after open and is only stopped for 
	write
*/

void iradioopen(char *filename) {
#ifdef ARDUINORF24
	radio.begin();
	radio.openReadingPipe(1, pipeaddr(filename));
	radio.startListening();
#endif
}

void oradioopen(char *filename) {
#ifdef ARDUINORF24
	radio.begin();
	radio.openWritingPipe(pipeaddr(filename));
#endif
}

#endif
