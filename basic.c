// $Id: basic.c,v 1.32 2021/07/11 05:34:31 stefan Exp stefan $
// 

#include <stdio.h>
#include <stdlib.h>

#define TRUE  1
#define FALSE 0

#define DEBUG  0
#define DEBUG1 0
#define DEBUG2 0
#define DEBUG3 0
#define DEBUG4 1

#define BUFSIZE 72
#define MEMSIZE 256
#define VARSIZE 26
#define STACKSIZE 15

/*

   	the tokens 
	all single character operators are their own tokens
	ASCII values above 0x7f are used for tokens of keywords

*/

#define EOL			0
#define NUMBER   	-127
#define LINENUMBER  -126
#define STRING   	-125
#define VARIABLE 	-124
#define GREATEREQUAL -123
#define LESSEREQUAL  -122
#define NOTEQUAL    -121
#define TIF     -120
#define TOT     -119
#define TNEW    -118
#define TRUN    -117
#define TLET    -116
#define TFOR    -115
#define TEND    -114
#define TTHEN   -113
#define TGOTO   -112
#define TSTOP   -111
#define TNEXT   -110
#define TSTEP   -109
#define TPRINT  -108
#define TINPUT  -107
#define TGOSUB  -106
#define TRETURN -105
#define TLIST	-104
#define TCLR	-103
#define TNOT    -102
#define TAND	-101
#define TOR  	-100
#define TABS 	-99
#define TRND	-98
#define TSGN	-97	
#define TPEEK	-96
#define TSQR	-95
#define TFRE	-94
#define TERROR  -3
#define UNKNOWN -2
#define NEWLINE -1

// the errors
#define EOUTOFMEMORY 1
#define ESTACK 		 2
#define ELINE        3
#define EUNKNOWN	 4
#define EVARIABLE	 5
#define EDIVIDE		 6
#define ERANGE 		 7

// the number of keywords, and the base index of the keywords
#define NKEYWORDS 30
#define BASEKEYWORD -123

// interpreter status values 
// SRUN means running from a programm
// SINT means interactibe mode
#define SINT 0
#define SRUN 1

//keywords
static char* const keyword[] = {"=>", "<=", "<>", "IF","TO", "NEW","RUN","LET","FOR", "END", "THEN" ,
                                "GOTO" , "STOP", "NEXT", "STEP", "PRINT", "INPUT", "GOSUB", "RETURN", 
                                "LIST", "CLR", "NOT", "AND", "OR" , "ABS", "RND", "SGN", "PEEK",
                                "SQR", "FRE"};

/*
	the basic interpreter is implemented as a stack machine
	with global variable for the interpreter state, the memory
	and the arithmetic during run time.

	stack and sp controll the stack
	ibuffer is the input buffer and *bi a pointer to it
	vars is a static array of 26 single character vaiables
	mem is the working memory of the basic interperter 

	x, y, xc, yc are two 16 bit and two 8 bit accumulators

	ir is a general index egister for string processing

	token contains the actually processes token

	st, here and top are the interpreter runtime controls

*/
static short stack[STACKSIZE];
static unsigned short sp=0; 

static char ibuffer[BUFSIZE] = "\0";
static char *bi;

static short vars[VARSIZE];

static char mem[MEMSIZE];

static short x, y;
static char xc, yc;

static char *ir;
static char token;
static char er;

static char st; 
static unsigned short here, here2, here3; 
static unsigned short top;

static unsigned int rd;

// goes to a header later on
void outputtoken();
void outsc(char *);
void outcr();
void outnumber(short);
void debugtoken();
void statement();
void gettoken();
void error(char);

/* 

	there two function access variables 
	in this implementation variables are a 
	static array and we simply subract 'A' 
	from the variable name to get the index
	any other way to generate an index from a 
	byte hash can be used just as well

	delvar and createvar as stubs for further 
	use

 */

short getvar(char c){
	if (c >= 65 && c<=91)
		return vars[c-65];
	error(EVARIABLE);
	return 0;
}

void setvar(char c, short v){
	if (c >= 65 && c<=91)
		vars[c-65]=v;
	else 
		error(EVARIABLE);
}

char varname(char c){
	return c;
}

void createvar(char c){
	return;
}

void delvar(char c){
	return;
}

void clrvars() {
	for (char i=0; i<VARSIZE; i++) vars[i]=0;
}

/* 

  the general error handler 

*/ 

void error(char e){
	outsc("Syntax Error ");
	er=e;
	if (e < 0) {
		outsc("in token:");
		debugtoken();
		return;
	}
	switch (e) {
		case EVARIABLE:
			outsc("Unknown Variable. \n");
		case EUNKNOWN: 
			outsc("Unknown token. \n");
			return;
		case ELINE:
			outsc("Line not found. \n");
			return;
		case ESTACK: 
			outsc("Stack error. \n");
			return;
		case EOUTOFMEMORY: 
			outsc("Out of memory. \n");
			return;
		case ERANGE:
			outsc("Out of range. \n");
			return;
	}
	return;
}

/*
	arithmetic and runtime operations are mostly done
	on a stack of 16 bit objects

*/

void push(short t){
	if (sp == STACKSIZE) {
		error(EOUTOFMEMORY);
		return;
	}
	stack[sp++]=t;
}

short pop(){
	if (sp == 0) {
		error(ESTACK);
		return 0;
	}
	else
		return stack[--sp];	
}


void drop() {
	if (sp == 0)
		error(ESTACK);
	else
		--sp;	

}

void clearst(){
	sp=0;
}

// very basic random number generator with constant seed
short rnd(int r) {
	rd = (31421*rd + 6927) % 0x10000;
	if (r>=0) 
		return (rd*r)/0x10000;
	else 
		return (rd*r)/0x10000+1;
}

// a very simple approximate square root formula
short sqr(short r){
	short t=r;
	short l=0;
	while (t > 0) {
		t>>=1;
		l++;
	}
	l=l/2;
	t=1;
	t<<=l;
	do {
		l=t;
		t=(t+r/t)/2;
	} while (abs(t-l)>1);
	return t;
}

/* 

	Functions related with input and output.
	The interpreter is implemented to use only putchar 
	and getchar. 

	These functions need to be provided by the stdio lib 
	or by any other routine like a serial terminal or keyboard 
	interface of an arduino

*/

void outch(char c) {
	putchar(c);
}

char inch(){
	return getchar();
}

void outcr() {
	outch('\n');
} 

void outspc() {
	outch(' ');
}

void outtab() {
	outch('\t');
}

// output a string of length x at index ir - basic style
void outs(char *ir, short x){
	int i;
	for(i=0; i<x; i++) outch(ir[i]);
}

// output a zero terminated string at ir - c style
void outsc(char *ir){
	while (*ir != 0) outch(*ir++);
}

// reads a line from the keybord to the input buffer
void ins(){
	char c;
	bi=ibuffer;
	while(TRUE) {
		c=inch();
		if (bi-ibuffer < BUFSIZE-1) {
			if (c == '\n') {
				*bi=0x00;
				break;
			} else 
				*bi++=c;
		} 
		if (c == 0x08 && bi>ibuffer) {
			bi--;
		}
	}
}

void outnumber(short x){
	char c, co;
	short d;
	if (x<0) {
		outch('-');
		x=-x;
	}
	d=10000;
	c=FALSE; // surpress leading 0s
	while (d > 0){
		co=x/d;
		x=x%d;
		if (co != 0 || d == 1 || c) {
			co=co+48;
			outch(co);
			c=TRUE;
		}
		d=d/10;
	}
}
/* 
	usind the stack to collect digits during input
*/  

short innumber(){
	char c;
	char nd;
	short s;
	short r;
	s=1;
	nd=0;
	c=inch();
	if (c == '-') {
		s=-1;
		c=inch();
	}
	while (TRUE){
		if (c <= '9' && c >='0' && nd<5) {
			x=c-'0';
			push(x);
			nd++;
		} 
		if (c == 0x08 && nd>0) {
			x=pop();
			nd--;
		}
		if (c == '\n')
			break;
		c=inch();
	}
	r=0;
	while(nd>0){
		r+=pop()*s;
		s=s*10;
		nd--;
	}
	return r;
}


/*

	Barebone lexical analyser - tokenizes the input line.

	nexttoken() increments the input buffer index bi and delivers values in the global 
	variable token, with arguments in the accumulator x and the index register ir
	xc is used in the routine. 

	xc, ir and x change values in nexttoken and deliver the result to the calling
	function.

	bi and ibuffer should not be changed or used for any other function in 
	interactive node as they contain the state of nexttoken(). In run mode 
	bi and ibuffer are not used as the program is fully tokenized in mem.

	debugtoken() is the debug function for the tokenizer
	whitespaces() skips whitespaces

*/ 

void debugtoken(){
	outsc("Token found: ");
	if (token != EOL) {
		outputtoken();
		outcr();
	} else {
		outsc("EOL");
		outcr();
	}
}

void whitespaces(){
	while (*bi == ' ' || *bi == '\t') bi++;
}

void nexttoken() {

	// RUN mode vs. INT mode
	if (st == SRUN) {
		gettoken();
		return;
	}

	// remove whitespaces outside strings
	whitespaces();

	// end of line token 
	if (*bi == '\0') { 
		token=EOL; 
		if (DEBUG) debugtoken();
		return; 
	}

	// unsigned numbers, value returned in x
	if (*bi <='9' && *bi>= '0'){
		x=*bi-'0';
		bi++;
		while(*bi <='9' && *bi>= '0'){
			x=x*10+*bi-'0';
			bi++;
		}
		token=NUMBER;
		if (DEBUG) debugtoken();
		return;
	}

	// strings between " " or " EOL, value returned in ir
	if (*bi == '"'){
		x=0;
		bi++;
		ir=bi;
		while(*bi != '"' && *bi !='\0') {
			x++;
			bi++;
		} 
		bi++;
		token=STRING;
		if (DEBUG) debugtoken();
		return;
	}

	// single character operators are their own tokens
	if (*bi == '+' || *bi == '-' || *bi == '*' || *bi == '/' || *bi == '%'  ||
		*bi == '\\' || *bi == ':' || *bi == ',' || *bi == '(' || *bi == ')' ) { 
			token=*bi; 
			bi++; 
			if (DEBUG) debugtoken();
			return; 
	}  

	// relations
	// single character relations are their own token
	// >=, =<, =<, =>, <> ore tokenized
	if (*bi == '=') {
		bi++;
		whitespaces();
		if (*bi == '>') {
			token=GREATEREQUAL;
			bi++;
		} else if (*bi == '<'){
			token=LESSEREQUAL;
			bi++;
		} else {
			token='=';
		}
		if (DEBUG) debugtoken();
		return;
	}

	if (*bi == '>'){
		bi++;
		whitespaces();
		if (*bi == '=') {
			token=GREATEREQUAL;
			bi++;
		} else  {
			token='>';
		}
		if (DEBUG) debugtoken();
		return;
	}

	if (*bi == '<'){
		bi++;
		whitespaces();
		if (*bi == '=') {
			token=LESSEREQUAL;
			bi++;
		} else  if (*bi == '>') {
			token=NOTEQUAL;
			bi++;
		} else {
			token='<';
		}
		if (DEBUG) debugtoken();
		return;
	}

	// keyworks and variables
	// isolate a word, bi points to the beginning, x is the length of the word
	// ir points to the end of the word after isolating
	x=0;
	ir=bi;
	while (-1) {
		// toupper 
		if (*ir >= 'a' && *ir <= 'z') {
			*ir-=32;
			ir++;
			x++;
		} else if (*ir >= 'A' && *ir <= 'Z'){
			ir++;
			x++;
		} else {
			break;
		}
	}

/* 

	ir is resused here to implement string compares
	scanning the keyword array 
	once a keyword is detected the input buffer is advanced 
	by its length, and the token value is returned 

	keywords are an arry of null terminated strings

*/

	token=0;
	while (token < NKEYWORDS){
		ir=keyword[token];
		xc=0;
		while (*(ir+xc) != 0) {
			if (*(ir+xc) != *(bi+xc)){
				token++;
				xc=0;
				break;
			} else 
				xc++;
		}
		if (xc == 0)
			continue;
		if ( *(bi+xc) < 'A' || *(bi+x) > 'Z' ) {
			bi+=xc;
			token+=BASEKEYWORD;
			if (DEBUG) debugtoken();
			return;
		} else {
			token=UNKNOWN;
			return;
		}
	}

	// a variable have length 1 in this first version
	// its literal value is returned in xc
	if ( x == 1 ){
		token=VARIABLE;
		xc=*bi;
		bi++;
		if (DEBUG) debugtoken();
		return;
	}

	token=UNKNOWN;
	bi++;
	if (DEBUG) debugtoken();
	return;
}

/* 
	Editing the program, the structure of a line is 
	LINENUMBER linenumber(2bytes) token(n bytes)

	store* stores something to memory 
	get* retrieves information

	No matter how long int is in the C implementation
	we pack into 2 bytes, this is clumsy but portable
	the store and get commands all operate on here 
	and advance it

	Storetoken operates on the variable top. 
	We always append at the end and the sort. 

	Gettoken operate on the variable here 
	which will also be the key variable in run mode.

	Tokens are stored including their payload
*/

char nomemory(short b){
	if (top >= MEMSIZE-b) return TRUE; else return FALSE;
}

void dumpmem(int i) {
	int j;
	int k=0;
	while (i>0) {
		for (j=0; j<8; j++) {
			outnumber(mem[k++]); outspc();
		}
		outcr();
		i--;
	}
	outsc("top= "); outnumber(top); outcr();
}

void storetoken() {
	short i=x;
	switch (token) {
		case NUMBER:
		case LINENUMBER:
			if ( nomemory(3) ) goto memoryerror;
			mem[top++]=token;	
			mem[top++]=x%256;
			mem[top++]=x/256;
			return;
		case VARIABLE:
			if ( nomemory(2) ) goto memoryerror;
			mem[top++]=token;
			mem[top++]=xc;
			return;
		case STRING:
			if ( nomemory(x+2) ) goto memoryerror;
			mem[top++]=token;
			mem[top++]=i;
			while (i > 0) {
				mem[top++] = *ir++;
				i--;
			}
			return;
		default:
			if ( nomemory(1) ) goto memoryerror;
			mem[top++]=token;
			return;
	}
memoryerror:
	er=EOUTOFMEMORY;
} 

void gettoken() {
	token=mem[here++];
	switch (token) {
		case NUMBER:
		case LINENUMBER:		
			x=mem[here++];
			x+=mem[here++]*256;
			break;
		case VARIABLE:
			xc=mem[here++];
			break;
		case STRING:
			x=mem[here++];
			ir=&mem[here];
			here+=x;
	}
}

void firstline() {
	if (top == 0) {
		x=0;
		return;
	}
	here=0;
	gettoken();
}

void nextline() {
	while (here < top) {
		gettoken();
		if (token == LINENUMBER)
			return;
		if (here >= top) {
			here=top;
			x=0;
			return;
		}
	}
}


void findline(short l) {
	here=0;
	while (here < top) {
		gettoken();
		if (token == LINENUMBER && x == l) {
			return;
		}
	}
	error(ELINE);
}

/*
   move a block of storage beginng at b ending at e
  to destination d

*/

void moveblock(short b, short l, short d){
	short i;
	
	if (l<1) {
		return;
	}
	if (b < d)
		for (i=l-1; i>=0; i--)
			mem[d+i]=mem[b+i]; 
	else 
		for (i=0; i<l; i++) 
			mem[d+i]=mem[b+i]; 
}

void zeroblock(short b, short l){
	short i;
	if (l<1) {
		return;
	}
	for (i=0; i<l+1; i++) mem[b+i]=0;
}

/*

	this is a prototype, no sorting of lines implemented
	stage 1: no matter what the line number is - store at the top
   				remember the location in here.
	stage 2: see if it is only an empty line - try to delete this line
	stage 3: caculate lengthes and free memory and make room at the 
				appropriate place
	stage 4: copy to the right place 
	
*/

void diag(){
	outsc("top, here, here2, here3, y and x\n");
	outnumber(top); outspc();
	outnumber(here); outspc();
	outnumber(here2); outspc();
	outnumber(here3); outspc();
	outnumber(y); outspc();	
	outnumber(x); outspc();		
	outcr();
}

void storeline() {

	short linelength;
	short newline; 

// zero is an illegal line number

	if (x == 0) {
		error(ELINE);
		return;
	}

/*
	stage 1: append the line at the end of the memory,
	remember the line number on the stack and the old top in here

*/
    push(x);			
    here=top;		
    newline=here;	 
	token=LINENUMBER;
	do {
		storetoken();
		if (er == EOUTOFMEMORY) {
			outsc("Out of memory error, line ignored.\n");
			drop();
			return;
		}
		nexttoken();
	} while (token != EOL);

	x=pop();				// recall the line number
	linelength=top-here;	// calculate the number of stored bytes

/* 
	stage 2: check if only a linenumber stored - then delete this line
	
*/

	if (linelength == 3) {  		
		zeroblock(here, linelength);
		top-=3;
		y=x;					
		findline(y);
		if (er == ELINE) {
			outsc("Line not found \n");
			return;
		}		
		y=here-3;							
		nextline();			
		here-=3;
		if (x != 0) {						
			moveblock(here, top-here, y);	
			top=top-(here-y);
			zeroblock(top, here-y);
		} else {
			zeroblock(y, top-y);
			top=y;
		}
		return;
	}
/* 
	stage 3, a nontrivial line with linenumber x is to be stored
	try to find it first by walking through all lines 
*/
	else {	
		y=x;
		here2=here;
		here=3;
		nextline();
		if (x == 0) { // there is no nextline after the first line, we are done
			return;
		}
		here=0;       // go back to the beginning
		here2=0;
		while (here < top) {
			here3=here2;
			here2=here;
			nextline();
			if (x > y) break;
		}

		// at this point y contains the number of the line to be inserted
		// x contains the number of the first line with a higher line number
		// or 0 if the line is to be inserted at the end
		// here points to the following line and here2 points to the previous line

		if (x == 0) { 
			here=here3-3;
			gettoken();
			if (token == LINENUMBER && x == y) { // we have a double line at the end
				here2-=3;
				here-=3;
				moveblock(here2, linelength, here);
				zeroblock(here+linelength, top-here-linelength);
				top=here+linelength;
			}
			return;
		}
		here-=3;
		push(here);
		here=here2-3;
		push(here);
		gettoken();
		if (x == y) {     // the line already exists and has to be replaced
			here2=pop();  // this is the line we are dealing with
			here=pop();   // this is the next line
			y=here-here2; // the length of the line as it is 
			if (linelength == y) {     // no change in line legth
				moveblock(top-linelength, linelength, here2);
				top=top-linelength;
				zeroblock(top, linelength);
			} else if (linelength > y) { // the new line is longer than the old one
				moveblock(here, top-here, here+linelength-y);
				here=here+linelength-y;
				top=top+linelength-y;
				moveblock(top-linelength, linelength, here2);
				zeroblock(top-linelength, linelength);
				top=top-linelength;
			} else {					// the new line is short than the old one
				moveblock(top-linelength, linelength, here2);
				zeroblock(top-linelength, linelength);
				top=top-linelength;
				moveblock(here, top-here, here2+linelength);
				top=top-y+linelength;
			}
		} else {         // the line has to be inserted in between
			drop();
			here=pop();
			moveblock(here, top-here, here+linelength);
			moveblock(top, linelength, here);
			zeroblock(top, linelength);
		}
	}
}

/* 
 
 calculates an expression, with a recursive descent algorithm
 using the functions term, factor and expression
 all function use the stack to pass values back. We use the 
 Backus-Naur form of basic from here https://rosettacode.org/wiki/BNF_Grammar
 implementing a C style logical expression model

*/

void expression();

void factor(){
	if (DEBUG1) { outsc("Entering factor with token: "); debugtoken(); }
	switch (token) {
		case NUMBER: 
			push(x);
			break;
		case VARIABLE: 
			push(getvar(xc));
			break;
		case '(':
			nexttoken();
			expression();
			if (token != ')')
				error(token);
			break;
		case TABS: 
			nexttoken();
			if ( token != '(') {
				error(TABS);
				break;
			}
			nexttoken();
			expression();
			if (er == 0 && token == ')') {
				x=pop();
				if (x<0) x=-x;
				push(x);
			} else {
				error(')');
				break;
			}
			nexttoken();
			break;
		case TRND: 
			nexttoken();
			if ( token != '(') {
				error(TABS);
				break;
			}
			nexttoken();
			expression();
			if (er == 0 && token == ')') {
				x=pop();
				x=rnd(x);
				push(x);
			} else {
				error(')');
				break;
			}
			break;
		case TSGN: 
			nexttoken();
			if ( token != '(') {
				error(TSGN);
				break;
			}
			nexttoken();
			expression();
			if (er == 0 && token == ')') {
				x=pop();
				if (x>0) x=1;
				if (x<0) x=-1;
				push(x);
			} else {
				error(')');
				break;
			}
			break;
		case TPEEK: 
			nexttoken();
			if ( token != '(') {
				error(TPEEK);
				break;
			}
			nexttoken();
			expression();
			if (er == 0 && token == ')') {
				x=pop();
				if (x>=0 && x<MEMSIZE) {
					push(mem[x]);
				} else 
					error(ERANGE);
			} else {
				error(')');
				break;
			}
			break;
		case TSQR: 
			nexttoken();
			if ( token != '(') {
				error(TSQR);
				break;
			}
			nexttoken();
			expression();
			if (er == 0 && token == ')') {
				x=pop();
				if (x>=0) {
					push(sqr(x));
				} 

			} else {
				error(')');
				break;
			}
			break;
		case TFRE: 
			nexttoken();
			if ( token != '(') {
				error(TSQR);
				break;
			}
			nexttoken();
			expression();
			if (er == 0 && token == ')') {
				x=pop();
				x=(MEMSIZE-top);
				push(x);
			} else {
				error(')');
				break;
			}
			break;	
		default:
			error(token);
	}
	if (DEBUG2) { outsc("Leaving factor with token: "); debugtoken(); }
}

void term(){
	if (DEBUG1) { outsc("Entering term with token: "); debugtoken(); }
	factor();

nextfactor:
	nexttoken();
	if (DEBUG) { outsc("Called nexttoken in term with token: "); debugtoken(); }
	if (token == '*'){
		nexttoken();
		factor();
		y=pop();
		x=pop();
		push(x*y);
		goto nextfactor;
	} else if (token == '/'){
		nexttoken();
		factor();
		y=pop();
		x=pop();
		if (y != 0)
			push(x/y);
		else {
			error(EDIVIDE);
			push(0);	
		}
		goto nextfactor;
	} else if (token == '%'){
		nexttoken();
		factor();
		y=pop();
		x=pop();
		if (y != 0)
			push(x%y);
		else {
			error(EDIVIDE);
			push(0);	
		}
		goto nextfactor;
	} 
	if (DEBUG2) { outsc("Leaving term with token: "); debugtoken(); }
}

void addexpression(){
	if (DEBUG1) { outsc("Entering addexpression with token: "); debugtoken(); }
	if (token != '+' && token != '-') {
		term();
	} else {
		push(0);
	}

nextterm:
	if (token == '+' ) { 
		nexttoken();
		term();
		y=pop();
		x=pop();
		push(x+y);
		goto nextterm;
	} else if (token == '-'){
		nexttoken();
		term();
		y=pop();
		x=pop();
		push(x-y);
		goto nextterm;
	}
	if (DEBUG2) { outsc("Leaving addexpression with token: "); debugtoken(); }
}

void compexpression() {
	if (DEBUG1) { outsc("Entering compexpression with token: "); debugtoken(); }
	addexpression();
	switch (token){
		case '=':
			nexttoken();
			compexpression();
			y=pop();
			x=pop();
			push(x == y);
			break;
		case NOTEQUAL:
			nexttoken();
			compexpression();
			y=pop();
			x=pop();
			push(x != y);
			break;
		case '>':
			nexttoken();
			compexpression();
			y=pop();
			x=pop();
			push(x > y);
			break;
		case '<':
			nexttoken();
			compexpression();
			y=pop();
			x=pop();
			push(x < y);
			break;
		case LESSEREQUAL:
			nexttoken();
			compexpression();
			y=pop();
			x=pop();
			push(x <= y);
			break;
		case GREATEREQUAL:
			nexttoken();
			compexpression();
			y=pop();
			x=pop();
			push(x >= y);
			break;
	}
	if (DEBUG2) { outsc("Leaving compexpression with token: "); debugtoken(); }
}

void notexpression() {
	if (DEBUG1) { outsc("Entering notexpression with token: "); debugtoken(); }
	if (token == TNOT) {
		nexttoken();
		compexpression();
		x=pop();
		push(! x);
	} else 
		compexpression();
	if (DEBUG2) { outsc("Leaving notexpression with token: "); debugtoken(); }
}

void andexpression() {
	if (DEBUG1) { outsc("Entering andexpression with token: "); debugtoken(); }
	notexpression();
	if (token == TAND) {
		nexttoken();
		expression();
		y=pop(); 
		x=pop(); 
		push(x && y);
	}
	if (DEBUG2) { outsc("Leaving andexpression with token: "); debugtoken(); }
}

void expression(){
	if (DEBUG1) { outsc("Entering expression with token: "); debugtoken(); }
	andexpression();
	if (token == TOR) {
		nexttoken();
		expression();
		y=pop(); 
		x=pop(); 
		push(x || y);
	}
	if (DEBUG2) { outsc("Leaving expression with token: "); debugtoken(); }
}


/*
   the print command 
   PRINT expressionlist ( EOL | : )
   expressionlist :== ( string | expression ) (, ( string | expression ) )* 
*/ 
void print(){
	if (DEBUG1) { outsc("Entering print with token: "); debugtoken(); }
	while (TRUE) {
		nexttoken();
		if (token == STRING) {
			outs(ir, x);	
			nexttoken();
		} else if (token != ',') {
			expression();
			if (er == 0) {
				outnumber(pop());
			} else {
				clearst();
				er=0;
				return ;
			}
		}
		if (token == EOL || token ==':' || token == LINENUMBER) {
			outcr();
			break;
		}
	}
	if (DEBUG2) { outsc("Leaving print with token: "); debugtoken(); }
}

void assignment() {
	if (DEBUG1) { outsc("Entering assignment with token: "); debugtoken(); }
	push(xc);
	nexttoken();
	if ( token == '=') {
		nexttoken();
		expression();
		if (er == 0) {
			x=pop(); 
			y=pop();
			setvar(y,x);
		} else {
			clearst();
			er=0;
			return;
		}
	} else 
		error(token);
	if (DEBUG2) { outsc("Leaving assignment with token: "); debugtoken(); }
}

void xgoto() {
	if (DEBUG1) { outsc("Entering goto with token: "); debugtoken(); }
	nexttoken();
	expression();
	if (er == 0) {
		x=pop();
		findline(x);
		nexttoken();
	} else {
		clearst();
		er=0;
		return;
	}
	if (DEBUG2) { outsc("Leaving goto with token: "); debugtoken(); }
}

void end(){
	st=SINT;
}

void xif(){
	if (DEBUG1) { outsc("Entering if with token: "); debugtoken(); }
	nexttoken();
	expression();
	if (er == 0) {
		x=pop();
		if (DEBUG1) { outsc("In if with boolean value: "); outnumber(x); outcr(); } 
		if (! x) {// on condition false skip the entire line
			do {
				nexttoken();
			} while (token != EOL && here <= top);
		}
		if (token == TTHEN) {
			nexttoken();
			if (token == NUMBER) {
				findline(x);
			}
		}
	} else {
		outsc("Error in if from expression \n");
		clearst();
		er=0;
		return;
	}
	if (DEBUG2) { outsc("Leaving if with token: "); debugtoken(); }
}

void input(){
	if (DEBUG1) { outsc("Entering input with token: "); debugtoken(); }
	nexttoken();
	if (token == STRING) {
		outs(ir, x);
		nexttoken();
		if (token != ',') {
			error(TINPUT);
			return;
		} else 
			nexttoken();
	}
	do {
		if (token == VARIABLE) {
			outsc("? ");
			x=innumber();
			setvar(xc, x);
		}
		nexttoken();
	} while (token == ',');
	if (DEBUG2) { outsc("Leaving if with token: "); debugtoken(); }
}

/* 

	control commands LIST, RUN, NEW, CLR

*/

void clr() {
	clrvars();
}

void outputtoken() {
	if (token == LINENUMBER) {
		outcr();
		outnumber(x);
		outspc();
	} else if (token == NUMBER ){
		outnumber(x); outspc();
	} else if (token == VARIABLE){
		outch(xc);
	} else if (token == STRING) {
		outch('"');
		outs(ir, x);
		outch('"');
	} else if (token < -3) {
		outsc(keyword[token-BASEKEYWORD]);
		if (token != LESSEREQUAL && token != GREATEREQUAL) 
			outspc();
	} else if (token >= 32) {
		outch(token);
	} else{
		outch(token); 
		outspc();
		outnumber(token);
	} 
}

void list(){
	here=0;
	gettoken();
	while (here <= top) {
		outputtoken();
		gettoken();
	}
	outcr();
 }


void run(){
	if (DEBUG1) { outsc("Entering run with token: "); debugtoken(); }
	here=0;
	st=SRUN;
	nexttoken();
	while (here < top && st == SRUN){
		statement();
	}
	st=SINT;
	if (DEBUG2) { outsc("Leaving run with token: "); debugtoken(); }
}

void new(){
	top=0;
	*mem=EOL;
	clrvars();
}

/* 
	statement processes an entire basic statement 

*/

void statement(){
	if (DEBUG1) { outsc("Entering statement with token: "); debugtoken(); }
	while (token != EOL) {
		switch(token){
			case TLIST:
				list();
				nexttoken();
				break;
			case TNEW:
				new();
				return;
			case TCLR:
				clr();
				nexttoken();
				break;
			case TRUN:
				run();
				return;		
			case TPRINT:
				print();
				break;
			case TLET:
				nexttoken();
				if ( token != VARIABLE ) {
					error(token);
					break;
				}
			case VARIABLE:
				assignment();
				break;
			case TGOTO:
				xgoto();
				break;
			case TEND:
				end();
				return;
			case TIF:
				xif();
				break;
			case TINPUT:
				input();
				break;
			case UNKNOWN:
				error(EUNKNOWN);
				return;
			default:
				nexttoken();
		}
	}
	if (DEBUG2) { outsc("Leaving statement with token: "); debugtoken(); }
}

int main(){

	new();
	while (TRUE) {

		// get a new line 
		ins();

		// go to the beginning of the line
		bi=ibuffer;

		// processing a new input line 
		nexttoken();

		// leading line number means edit mode - new program data is input
		// else run mode, command is input and directly executed
		if (token == NUMBER)
			storeline();
		else {
			st=SINT;
			statement();
		}
	}

}