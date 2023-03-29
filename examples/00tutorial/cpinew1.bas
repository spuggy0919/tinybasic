100 REM "CALCULATING PI FOR HUNDREDS OF DIGITS"
110 REM "THX TO ROSETTACODE.ORG FOR THE BASE OF THE PROGRAM"
120 REM "SHOULD BE A BENCHMARK FOR THE BASIC INTERPRETER/COMPILER"
130 REM "WRITTEN IN 2023 FOR FORUM.CLASSIC-COMPUTING.DE BY PETER DASSOW"
135 REM "Ported to IoT BASIC by Guido Lehwalder, 2023"
137 CLS
140 PRINT "Calculating Pi as a BASIC benchmark."
150 PRINT "Enter number of digits (below or equal 1000): ";
160 INPUT S$
170 IF S$="" THEN PRINT "Nothing done.": END
180 N=VAL(S$): IF N>1000 OR N<1 THEN PRINT "Not a valid number.":  GOTO 150
190 REM "N DETERMINES ALSO THE ARRAY (ABOUT 3-4 TIMES BIGGER)"
200 LN=INT(10*N/3)+16
210 ND=1
220 SM=MILLIS(1)
230 REM "DIM STATEMENT COULD HAVE LN AS SIZE PARAMETER, BUT COMPATIBILITY..."
240 DIM A(3350)
250 N9=0
260 PD=0
270 REM
280 FOR J=1 TO LN
290 A(J)=2
300 NEXT J
310 REM
320 FOR J=1 TO N
330 Q=0
340 FOR I=LN TO 1 STEP -1
350 X=10*A(I)+Q*I
360 A(I)=X-(2*I-1)*INT(X/(2*I-1))
370 Q=INT(X/(2*I-1))
380 NEXT I
390 A(1)=Q-10*INT(Q/10)
400 Q=INT(Q/10)
410 IF Q=9 THEN N9=N9+1:  GOTO 610
420 IF Q<>10 THEN  GOTO 540
430 REM "Q==10"
440 D=PD+1:  GOSUB 670
450 IF N9<=0 THEN  GOTO 500
460 FOR K=1 TO N9
470 D=0:  GOSUB 670
480 NEXT K
490 REM "END IF"
500 PD=0
510 N9=0
520 GOTO 610
530 REM "Q<>10"
540 D=PD:  GOSUB 670
550 PD=Q
560 IF N9=0 THEN  GOTO 610
570 FOR K=1 TO N9
580 D=9:  GOSUB 670
590 NEXT K
600 N9=0
610 NEXT J
620 C$=STR(PD): Z=LEN(C$)
621 PRINT C$(Z,Z)
630 EM=MILLIS(1)-SM
635 PRINT "Calculation time ";EM/1000;" seconds."
640 END
650 REM
660 REM "OUTPUT DIGITS"
670 C$=STR(D): Z=LEN(C$)
671 IF ND=0 THEN PRINT C$(Z,Z);: RETURN
680 IF D=0 THEN RETURN
690 C$=STR(D): Z=LEN(C$)
691 PRINT C$(Z,Z);".";
700 ND=0
710 RETURN