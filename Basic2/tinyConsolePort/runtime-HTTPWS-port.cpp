/*
 * This file is part of ESP32_TinyConsole.
 *
 * ESP32_TinyConsole is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ESP32_TinyConsole is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ESP32_TinyConsole.  If not, see 
 *
 *	https://github.com/spuggy0919/ESP32_WEB_CONTROL/blob/espcontrol/LICENSE
 *    GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 *
 *	Author: spuggy0919, spuggy0919@gmail.com
 */
/*
### Portting driver layer of tinyConsole
tinybasic port on tinyconsole is 
base on my driver layers for tinyconsole
1. **Keybard** input and **console out** text out, use 
    **wsSerial.cpp** serial driver communicate with basic and websocket
    extern WebSocketSerial wsSerial;
2. **Itouch.cpp** **touch** event queue driver , stream come from websocket client side
3. display **graphics** functions in one include
**#include "audiovideo.h"**
4. **File system** is littlefs, the tinybasic use old littlefsfun.h library, my new interface is 
    **fileio.h** class object, global name is **g_fsio**	
      
### Basic2 port modification:

**runtime.cpp runtime.h**
1. port runtime dependency, there are serial, vga display file system is **runtime-HTTPWS-port **
2. **runtime.cpp** modifications
 2.1 ioinit modified  add ITOUCH with device 3, and vgabegin enable
  #define ITOUCH 3 /* locate in **runtime.h**
 2.2 checkch ITOUCH
 2.3 availch ITOUCH
 2.4 ins ITOUCH
 For display VGA
 2.5  undefined below function, we will replace
      void vgabegin(){}
int vgastat(uint8_t c) {return 0; }
void vgawrite(char c){}

      netbegin... are all disable
    2.6 disable the fs functions , here will replace by below item.

### runtime driver interface
 The file runtime-HTTPWS-port contain tinybasic runtime driver which inteface with draiver layers of tinyConsole with ESP32
 
 **runtime-HTTPWS-port.h**

1. Use LITTLEFS 
1. HTTP websocket serial 
2. HTTP websocket graphics command and COLOR
3. HTTP active all wifi network so disable the network connecting check
copy from my original  port tinybasic 1.4a
put last line of runtime.cpp
*/

#include "Arduino.h"
#include "ESP32inc.h"
#include "hardware.h"
#include "runtime.h"

// already defined
// #define ESPLITTLEFS
// #define HTTPWSSERIAL
// #define HTTPWSVGA
// #define HTTPWSTOUCH

// #ifdef ESPLITTLEFS
/* 
 *	filesystem starter for SPIFFS and SD on ESP, ESP32 and Arduino plus LittleFS
 *	the verbose option needs to be checked 
 * 
 * if SDPIN is empty it is the standard SS pin of the platfrom
 * 
 */
#define FBUFSIZE 32
char tempname[FBUFSIZE]; /* this is needed for the catalog code as these platforms do not give static C strings back */
File ifile;
File ofile;
const char rootfsprefix[8] = "/basic/"; /*spuggy0919***/
File root;
File file;


/* add the prefix to the filename */
char* mkfilename(const char* filename) {
	uint8_t i,j;
	for(i=0; i<10 && rootfsprefix[i]!=0; i++) tempname[i]=rootfsprefix[i];
	tempname[i++]='/';
	for(j=0; j<FBUFSIZE && filename[j]!=0; j++) tempname[i++]=filename[j];
	tempname[i]=0;
	return tempname;
}

/* remove the prefix from the filename */
const char* rmrootfsprefix(const char* filename) {
	uint8_t i=0;
	while (filename[i] == rootfsprefix[i] && rootfsprefix[i] !=0 && filename[i] !=0 ) i++;
	if (filename[i]=='/') i++;
	return &filename[i];
}

void fsbegin() {
#define FILE_OWRITE FILE_WRITE


 /* spuggy0919 */
	Serial.println("fsbegin"); /*spuggy0919*/
 	//if (LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED) && v) {
		Serial.printf("success %d,%d\n",LittleFS.totalBytes(),LittleFS.usedBytes());
	
		// outsc("LittleFS ok \n");
		// outsc("File system size "); outnumber(LittleFS.totalBytes()); outcr();
		// outsc("File system used "); outnumber(LittleFS.usedBytes()); outcr();
 	//}

}

uint8_t fsstat(uint8_t c) { 
#if defined(FILESYSTEMDRIVER)
  if (c == 0) return 1;
#endif
  return 0;
}

/*
 *	File I/O function on an Arduino
 * 
 * filewrite(), fileread(), fileavailable() as byte access
 * open and close is handled separately by (i/o)file(open/close)
 * only one file can be open for write and read at the same time
 */
void filewrite(char c) {
	// Serial.printf("fwrite[%2x]%c\n",c,c); 
	if (ofile) ofile.write(c); else ioer=1;

}

char fileread(){
	char c;

	if (ifile) c=ifile.read(); else { ioer=1; return 0; }
	if (c == -1 ) ioer=-1;
	if (c == 0xff ) ioer=-1; /* spuggy0919 EOF BUG ? S=0*/
	// Serial.printf("fileread[%2x]%c ert=%d\n",c,c,ert);

	return c;


}

int fileavailable(){
	//  Serial.printf("fileavailable[%4x]\n",ifile.available());
	int len = ifile.available();
	if (len == 0) { ioer=-1;}  /*EOF BUG? Spuggy0919*/
	return len;

}

uint8_t ifileopen(const char* filename){
/*spuggy0919*/
	ifile=LittleFS.open(mkfilename(filename));
	Serial.printf("ifileopen[%4x]\n",ifile);

	return (int) ifile;

}

void ifileclose(){
	Serial.printf("ifileclose[%4x]\n",ifile);
	ifile.close();
}

uint8_t ofileopen(const char* filename, const char* m){

	ofile=LittleFS.open(mkfilename(filename),m,true);
	Serial.printf("ofileopen[%4x]\n",ofile);
	return (int) ofile;

}

void ofileclose(){
	Serial.printf("ofileclose[%4x]\n",ofile);
    ofile.close();

}

/*
 * directory handling for the catalog function
 * these methods are needed for a walkthtrough of 
 * one directory
 *
 * rootopen()
 * while rootnextfile()
 * 	if rootisfile() print rootfilename() rootfilesize()
 *	rootfileclose()
 * rootclose()
 */
void rootopen() {

	root=LittleFS.open("/basic/"); /*spuggy0919***/
	Serial.printf("rootopen[%4x]%s\n",root,root.name());

}

uint8_t rootnextfile() {

	file=root.openNextFile();
	// if (file) {
	// 	Serial.printf("openNextFile[%4x]\n",file);
	// 	Serial.printf("name=%s,size=%d,isDir%s\n",file.name(),file.size(),(file.isDirectory())?"Yes":"NO");
	// }
	return (file != 0);

}

uint8_t rootisfile() {

	return (! file.isDirectory());
}

const char* rootfilename() {

	// Serial.printf("name=%s,size=%d,isDir(%s)\n",file.name(),file.size(),((file.isDirectory())?"true":"false"));

	const char * fname = rmrootfsprefix(file.name());
	// Serial.printf("rmrootfsprefix(%s)\n",fname);
 return fname;

}

uint32_t rootfilesize() {
  return file.size();

}

void rootfileclose() {
		// Serial.printf("rootfileclose[%4x]name=%s\n",file,file.name());
  		file.close();

}

void rootclose(){
/*spuggy0919*/
		// Serial.printf("rootfileclose[%4x],name=%s\n",root,root.name());
  root.close();


}

/*
 * remove method for files
 */
void removefile(const char *filename) {

	LittleFS.remove(mkfilename(filename));
	return;


}

/*
 * formatting for fdisk of the internal filesystems
 */
void formatdisk(uint8_t i) {


	return ; // not support now. spuggy0919

}
// #endif
// #include "wsSerial.h" alreday include
// #include "WebServer.h"

const uint16_t serial_baudrate = 9600;

/* 
 * blocking serial single char read for Serial
 * unblocking for Picoserial because it is not 
 * character oriented -> consins.
 */
char serialread() {
    char c;
    while (!wsSerial.available())   byield(); // may be deadlock **WARNING **
	c = wsSerial.read();  	
	// Serial.printf("basic<WS[%2x]%c\n",c,c);   /*spuggy0919*/
    return c;
}

/*
 *  serial begin code with a one second delay after start
 *	this is not needed any more 
 */
void serialbegin() {

	// stdioRedirector(); //alreaedy start on main setup
		wsSerial.begin();
	///Serial.begin(115200);
  //while(!Serial) byield();
	delay(1000); // maybe not
}

/* state information on the serial port */
uint8_t serialstat(uint8_t c){
  if (c == 0) return 1;
  if (c == 1) return 0; //serial_baudrate; /*websocket default*/
  return 0;
}

/* write to a serial stream */
void serialwrite(char c) {
	// _d_PutChar(c);
    // Serial.printf("basic>>WS[%2x]%c\n",c,c); /*spuggy0919*/
	// _d_PutChar(c); stdiolib spuggy0919
 	// Serial.printf("basic>>WS[%2x]%c\n",c,c); /*spuggy0919*/
  	wsSerial.write(c);
	if (c=='\n') {yield();}
// #ifdef HASMSTAB
//   if (c > 31) charcount+=1;
//   if (c == 10) charcount=0;
// #endif
}

/* check on a character, needed for breaking */
char serialcheckch() {
	// Serial.printf("sck");  
	
	// if (_d_isrxavailable()) {
	// 	char c =  _d_GetPeek();
	// 	//   Serial.printf("peek[%2x]%c\n",c,c);  

	// 	return c;
	// }
	// Serial.printf("sck");  
  	if (wsSerial.available()) {
  		// char c =  _d_GetPeek();
 		char c = wsSerial.peek();
  		//   Serial.printf("peek[%2x]%c\n",c,c);  
		return c;
	}

	return 0;

}

/* avail method, needed for AVAIL() */ 
uint16_t serialavailable() {
	// short i = _d_isrxavailable();
	uint16_t i = wsSerial.available();
	//   Serial.printf("ava[%2x]%c\n",i,i);  
	  return i;

}

void serialflush(){/* flush serial */
	wsSerial.flush();
}


/* 
 * spuggy0919
 * this is the Touch/Mouse for HTTPTOUCHWS - experimental
 * give a websocket protocol for TOUCH input functions
 * #define ITOUCH 3, and input device function should be added
 */
// #ifdef HTTPWSTOUCH
#include "itouch.h"
// #endif

/* 
 * spuggy0919
 * this is the VGA code for httpwsVGA - experimental
 * give a websocket protocol for drawing functions
 */

#undef DISPLAYDRIVER
#define DISPLAYHASCOLOR
#define DISPLAYHASGRAPH
#include "audiovideo.h"
int vga_graph_pen=0;
int vga_graph_pen_width=1;
int vga_graph_brush = 0x000000;
int vga_txt_pen = 0x00ff00;
int vga_txt_background = 0x000000;
unsigned int vga_pallete[16]={
	0x000000,
	0x800000,
	0x008000,
	0x808000,
	0x000080,
	0x800080,
	0x008080,
	0xc0c0c0,
	0x808080,
	0xFF0000,
	0x00ff00,
	0xFFff00,
	0x0000ff,
	0xFf00ff,
	0x00ffff,
	0xFFffff
};



/* this starts the vga controller and the terminal right now */
void vgabegin() {
	vga_graph_pen=0x000000;
}

int vgastat(char c) {return 0; }

void vgascale(int* x, int* y) {
	*y=*y*10/24;
}

void rgbcolor(uint8_t r, uint8_t g, uint8_t b) {
	vga_graph_pen=((unsigned)r<<24)|((unsigned)g<<16)|((unsigned)b<<8)|0xFF;
}
void rgbacolor( uint8_t r, uint8_t g, uint8_t b, uint8_t a) { /*spuggy0919*/
	vga_graph_pen=((unsigned)r<<24)|((unsigned)g<<16)|((unsigned)b<<8)|((unsigned)a);
}
int setPenColor(int color){
	vga_graph_pen = color;
	return 0;
}
int setPenWidth(int w){
	vga_graph_pen_width = w;
	return 0;
}
void vgacolor(uint8_t c) { vga_graph_pen = (vga_pallete[c%16]<<8)|0xff;}
void plot(int x, int y) { 
		drawPenColor(vga_graph_pen);
		drawPixel( x, y);
		drawPenColor(vga_txt_pen);

}
void line(int x0, int y0, int x1, int y1) {
		drawPenColor(vga_graph_pen);
		drawLine(x0, y0, x1, y1);
		drawPenColor(vga_txt_pen);

}
void rect(int x0, int y0, int x1, int y1) { 
		drawPenColor(vga_graph_pen);
	    drawRect(x0, y0, x1, y1);
			drawPenColor(vga_txt_pen);

}
void frect(int x0, int y0, int x1, int y1) {  
		Serial.printf("Frect%d %d %d %d\n",x0,y0,x1,y1);
		drawPenColor(vga_graph_pen);
	    drawFRect(x0, y0, x1, y1);
		drawPenColor(vga_txt_pen);

}
void circle(int x0, int y0, int r) {  
		drawPenColor(vga_graph_pen);
	    drawCircle(x0, y0, r);
		drawPenColor(vga_txt_pen);

}
void fcircle(int x0, int y0, int r) {  
		drawPenColor(vga_graph_pen);
	    drawFCircle(x0, y0, r);
		drawPenColor(vga_txt_pen);
}
void vgawrite(char c){
	drawPenColor(vga_graph_pen);
	drawStrokeWidth(1);
	switch(c) {
    case 12: /* form feed is clear screen */
	    drawClearScr();
      return;
    case 10: /* this is LF Unix style doing also a CR */
        // Terminal.write(10); Terminal.write(13);
			drawChar(0xa);
			drawChar(0xd);
    	return;
  	}
	drawChar(c);
}


/* 
 *	network begin method 
 *	needs the settings from wifisettings.h
 * 
 * Default is right now that Wifi is started at boot
 * This may change in the future.
 * 
 * Ethernet begin has to be reviewed to avoid DHCP 
 * start timeout if network is not connected
 */

void netbegin() {
}

/*
 * network stop methode, needed sometime to reinit networking
 * completely or to save power
 * 
 */
void netstop() {
}

/*
 * network reconnect methode
 * 
 */
void netreconnect() {
}


/*
 * network connected method
 * on ESP Wifi try to reconnect, the delay is odd 
 * This is a partial reconnect, BASIC needs to handle 
 * repeated reconnects
 */
uint8_t netconnected() {
	return true;
}


