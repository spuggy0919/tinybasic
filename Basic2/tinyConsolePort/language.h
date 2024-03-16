/*
 *
 *	$Id: language.h,v 1.1 2024/02/25 04:43:16 stefan Exp stefan $ 
 *
 *	Stefan's IoT BASIC interpreter 
 *
 * 	See the licence file on 
 *	https://github.com/slviajero/tinybasic for copyright/left.
 *    (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)
 *
 *	Author: Stefan Lenz, sl001@serverfabrik.de
 *
 *	This is the language definition file. Edit this to set the language 
 * 	capabilities.
 *
 *	MEMSIZE sets the BASIC main memory to a fixed value,
 *		if MEMSIZE=0 a heuristic is used based on free heap
 *		size and architecture parameters
 */
#define MEMSIZE 0 

/*
 *	DEBUG switches on compiled debug mode. Consider using runtime 
 * 		debug with SET 0,x before using this.
 */
#define DEBUG 0

/*
 * Interpreter feature sets, choose one of the predefines 
 * or undefine all predefines and set the features in custom settings
 *
 * BASICFULL: full language set, use this with flash >32kB - ESPs, MKRs, Mega2560, RP2040, UNO R4
 * BASICINTEGER: integer BASIC with full language, use this with flash >32kB
 * BASICSIMPLE: integer BASIC with reduced language set, 32kB capable - for UNOs with a lot of device drivers
 * BASICSIMPLEWITHFLOAT: a small floating point BASIC, 32kB capable, for UNOs - good for UNOs with the need of float
 * BASICTINYWITHFLOAT: a floating point tinybasic, if you have 32kB and need complex device drivers
 * BASICMINIMAL: minimal language, just Palo Alto plus Arduino I/O, works on 168 with 1kB RAM and 16kB flash
 */
#define BASICFULL
#undef	BASICINTEGER
#undef	BASICSIMPLE
#undef	BASICMINIMAL
#undef	BASICSIMPLEWITHFLOAT
#undef	BASICTINYWITHFLOAT

/*
 * custom settings undef all the the language sets 
 * when you def here
 */ 
#define HASAPPLE1
#define HASARDUINOIO
#define HASFILEIO
#define HASTONE
#define HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#define HASVT52
#define HASFLOAT
#define HASGRAPH
#define HASDARTMOUTH
#define HASDARKARTS
#define HASIOT
#define HASMULTIDIM
#define HASSTRINGARRAYS
#define HASTIMER
#define HASEVENTS
#define HASERRORHANDLING
#define HASARRAYLIMIT
#define HASSTRUCT

/* 
 * Experimental features:
 * 
 * HASMSSTRINGS is a (rudimentary) string compatibility to MS Basic.
 * HASMULTILINEFNCTIONS is a bit like the old DEC BASIC on the PDP.
 * HASEDITOR is a line editor for the console.
 * HASTINYBASICINPUT is a tinybasic like input routine using the expression
 *     parser. This allows expressions in the input statement. Does not work
 *     with comma separated input. (not recommended)
 */
#define HASMSSTRINGS
#define HASMULTILINEFUNCTIONS
#define HASEDITOR
#define HASTINYBASICINPUT

/*
 * Odd stuff - these things change the behaviour of BASIC in some aspects.
 * They can be used to make the interpreter compatible with other dialects.
 * 
 * 	POWERRIGHTTOLEFT: normally the ^ operator works from left to right 
 *		which means 2^3^2 = (2^3)^2 = 8^2 = 64. Setting this flag would 
 *		change the behaviour to 2^3^2 = 2^(3^2) = 512
 *  MSARRAYLIMITS: in BASIC arrays start at 1 and DIM A(10) creates 10 
 *      elements. With MSARRAYLIMITS defined, arrays start at 0 and have 
 *      n+1 elements.
 *  SUPPRESSSUBSTRINGS: switch off substring logic, makes only sense with 
 *      HASMSSTRINGS activated. With this, the syntax of strings and string 
 *      arrays is comaptible to MS strings. 
 *  USELONGJUMP: use the longjmp feature of C. This greatly simplifies 
 *      error handling at the cost of portability to some MCU platforms
 *      currently only experimental. It costs memory for the jump buffer. 
 *      Don't use it on very small systems. LONGJUMP must be set to 0 or 1 as 
 *      it is used in boolean expression in the code
 *  BOOLEANMODE: switch the behaviour of BASICs boolean operators. Default (0)
 *      is to cast all numbers to signed 16bit and then do bitwise arithemtic.
 *      In this mode false is 0 and -1 is true. (1) is C style boolean arithemtic.
 *      (2) is like 0 but with the default integer size as number type. This means that
 *      32 bit platforms will cast to 32 bit integers. (3) is like (0) with signed 8 bit. 
 */
#undef POWERRIGHTTOLEFT
#undef MSARRAYLIMITS
#undef SUPPRESSSUBSTRINGS
#define USELONGJUMP 0
#define BOOLEANMODE 1

/* Palo Alto plus Arduino functions */
#ifdef BASICMINIMAL
#undef HASAPPLE1
#define HASARDUINOIO
#undef HASFILEIO
#undef HASTONE
#undef HASPULSE
#undef HASSTEFANSEXT
#undef HASERRORMSG
#undef HASVT52
#undef HASFLOAT
#undef HASGRAPH
#undef HASDARTMOUTH
#undef HASDARKARTS
#undef HASIOT
#undef HASMULTIDIM
#undef HASSTRINGARRAYS
#undef HASTIMER
#undef HASEVENTS
#undef HASERRORHANDLING
#undef HASARRAYLIMIT
#undef HASSTRUCT
#undef HASMSSTRINGS
#undef HASMULTILINEFUNCTIONS
#undef HASEDITOR
#endif

/* all features minus float and tone */
#ifdef  BASICINTEGER
#define HASAPPLE1
#define HASARDUINOIO
#define HASFILEIO
#define HASTONE
#define HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#define HASVT52
#undef  HASFLOAT
#define HASGRAPH
#define HASDARTMOUTH
#define HASDARKARTS
#define HASIOT
#define HASMULTIDIM
#define HASSTRINGARRAYS
#define HASTIMER
#define HASEVENTS
#define HASERRORHANDLING
#define HASARRAYLIMIT
#define HASSTRUCT
#define HASMSSTRINGS
#define HASMULTILINEFUNCTIONS
#define HASEDITOR
#endif

/* a simple integer basic for small systems (UNO etc) */
#ifdef  BASICSIMPLE
#define HASAPPLE1
#define HASARDUINOIO
#define HASFILEIO
#define HASTONE
#define HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#define HASVT52
#undef  HASFLOAT
#undef  HASGRAPH
#define HASDARTMOUTH
#undef  HASDARKARTS
#define HASIOT
#undef  HASMULTIDIM
#undef  HASSTRINGARRAYS
#define HASTIMER
#define HASEVENTS
#define HASERRORHANDLING
#undef 	HASARRAYLIMIT
#undef 	HASSTRUCT
#undef  HASMSSTRINGS
#undef HASMULTILINEFUNCTIONS
#undef HASEDITOR
#endif

/* all features activated */
#ifdef BASICFULL
#define HASAPPLE1
#define HASARDUINOIO
#define HASFILEIO
#define HASTONE
#define HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#define HASVT52
#define HASFLOAT
#define HASGRAPH
#define HASDARTMOUTH
#define HASDARKARTS
#define HASIOT
#define HASMULTIDIM
#define HASSTRINGARRAYS
#define HASTIMER
#define HASEVENTS
#define HASERRORHANDLING
#define HASARRAYLIMIT
#define HASSTRUCT
#define HASMSSTRINGS
#define HASMULTILINEFUNCTIONS
#define HASEDITOR
#endif

/* a simple BASIC with float support */
#ifdef BASICSIMPLEWITHFLOAT
#define HASAPPLE1
#define HASARDUINOIO
#undef HASFILEIO
#undef HASTONE
#undef HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#undef HASVT52
#define HASFLOAT
#undef HASGRAPH
#define HASDARTMOUTH
#undef HASDARKARTS
#undef HASIOT
#undef HASMULTIDIM
#undef HASSTRINGARRAYS
#undef HASTIMER
#undef HASEVENTS
#undef HASERRORHANDLING
#undef HASARRAYLIMIT
#undef HASSTRUCT
#undef HASMSSTRINGS
#undef HASMULTILINEFUNCTIONS
#undef HASEDITOR
#endif

/* a Tinybasic with float support */
#ifdef BASICTINYWITHFLOAT
#define HASAPPLE1
#define HASARDUINOIO
#undef HASFILEIO
#undef HASTONE
#undef HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#undef HASVT52
#define HASFLOAT
#undef HASGRAPH
#undef HASDARTMOUTH
#undef HASDARKARTS
#undef HASIOT
#undef HASMULTIDIM
#undef HASSTRINGARRAYS
#undef HASTIMER
#undef HASEVENTS
#undef HASERRORHANDLING
#undef HASARRAYLIMIT
#undef HASSTRUCT
#undef HASMSSTRINGS
#undef HASMULTILINEFUNCTIONS
#undef HASEDITOR
#endif

/* 
 *	Language feature dependencies
 * 
 * Dartmouth and darkarts needs the heap which is in Apple 1
 * IoT needs strings and the heap, also Apple 1
 * 
 * String arrays need multi dimensional capabilities
 *
 * The structured language set needs ELSE from STEFANSEXT
 *
 */
#if defined(HASDARTMOUTH) || defined(HASDARKARTS) || defined(HASIOT)
#define HASAPPLE1
#endif

#if defined(HASSTRINGARRAYS)
#define HASMULTIDIM
#endif

#if defined(HASSTRUCT)
#define HASSTEFANSEXT
#endif

/* dependencies on the hardware */
#if !defined(DISPLAYHASGRAPH) 
#undef HASGRAPH
#endif
