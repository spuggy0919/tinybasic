# Arduino examples 01.Basic ported to basic

## Programs 

### Palo Alto BASIC language set

This is the minimal BASIC language set also found in many Tinybasic adaptions. The orginal language was described in a Dr. Dobbs Article. It has the commands PRINT, LET, INPUT, GOTO, GOSUB, RETURN, IF, FOR, TO, STEP, NEXT, STOP, LIST, NEW, RUN, ABS, INT, RND, SIZE, REM. Note that there is no THEN in Palo Alto BASIC. This is what you get when compiling with #define BASICMINIMAL. INT, LOAD, and SAVE were not present in Palo Alto BASIC but added here nevertheless.

- Hello World: hello.bas - say hello.
- Table: table.bas - print a formatted table of values.
- Random: random.bas - create random number and show the distribution of them.
- Sort: sort.bas - sort a list of random numbers
- TicTacToe: tictac.bas - play tictactoe against the computer who moves first.
- EEPROM: eeprom.bas - store data to the EEPROM array and access it later

### Apple 1 Integer BASIC language set

The Apple 1 language set has arrays, strings and a number of additional commands. It is compatible to Apple Integer BASIC. Additional commands are NOT, AND, OR, LEN, SGN, PEEK, POKE, DIM, THEN, TAB, END, CLR, HIMEM. 

- Memory: memory.bas - access BASIC memory with PEEK and POKE, find memory layout

### Stefan's extension

These are a set of useful commands and features missing in Apple Integer BASIC. Some of then I took from other BASIC interpreters, some I missed in the past and added with different features. Commands in this language set are CONT, SQR, POW, MAP, DUMP, BREAK, GET, PUT, SET, CLS, USR, CALL, ELSE.

### Dartmouth extension

For compatibility with the Dartmouth style BASIC interpreters, the following commands have been added const char DATA, READ, RESTORE, DEF, FN, ON.

### File I/O functions

File I/O is supported with the command set CATALOG, OPEN, CLOSE, FDISK, DELETE. 

### Floating point functions 

The classical BASIC floating point functions SIN, COS, TAN, ATAN, LOG, EXP are part of the floating point laguage set.

### The Dark Arts

This language set has a few functions for low level access to the interpreter mechanism. Currently MALLOC, FIND and EVAL are implemented.

In addition to these language sets there are the Arduino IO functions. These features are explained in 01basics-10starter.

Graphics functions are ommited here, they will be explained in a special section of the examples code as well.

IoT and networking functions are also explained in a special section.

## BASIC language features by program 

hello.bas: printing with PRINT, string constants in ""

table.bas: inputting data with INPUT, FOR NEXT loops, formatted printing using #8

random.bas: use of the memory array @(), conditional expressions IF, random numbers RND

sort.bas: use of the memory array @(), conditional expression IF GOTO 

tictac.bas: GOSUB RETURN subroutines, % as a modulo operator

eeprom.bas: use of the EEPROM array @E() and the size of the EEPROM @E

memory.bas: use of SIZE and HIMEM, PEEK and POKE, memory as signed char bytes






