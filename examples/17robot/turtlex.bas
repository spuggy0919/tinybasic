10 REM "engine code, the pins of the Arduino motor shield"
20 DA=12: SA=3: BA=9
30 DB=13: SB=11: BB=8
40 TP=4: EP=5: SP=6
100 REM "set the pins, Dx is direction, Bx is break"
110 PINM DA,1: PINM BA,1
120 PINM DB,1: PINM BB,1
130 AWRITE SA,0: AWRITE SB,0
140 PINM TP, 1: PINM EP, 0
200 REM "some parameters, TT is step time, S is speed ie voltage"
210 TT=5
220 S=140
300 REM "The function for the servo and the echo, integer safe!"
310 DEF FNP(A)=40+A*200/180
320 DEF FNE(P)=P/29*100/2
1000 REM "Test code"
1010 INPUT A$
1020 GOSUB 8000
1030 GOTO 1000
8000 REM "The turtle code: A$ has a set of commands"
8010 REM "Commands f,b,r,l and single digits for the distance"
8020 REM "Prepare, set the speed and directions and hit the break"
8030 DWRITE BA,1: DWRITE BB,1
8040 AWRITE SA,S: AWRITE SB,S
8050 DWRITE DA,0: DWRITE DB,0
8100 REM "every character in A$ is a command"
8110 WHILE LEN(A$)>0
8120 C=A$: A$=A$(2)
8130 IF C=>"0" AND C<="9" THEN TT=C-"0": CONT
8140 SWITCH C
8150 CASE "e": END
8160 CASE "f": DWRITE DA,0: DWRITE DB,0: GOSUB 9000
8170 CASE "b": DWRITE DA,1: DWRITE DB,1: GOSUB 9000
8180 CASE "l": DWRITE DA,0: DWRITE DB,1: GOSUB 9000
8190 CASE "r": DWRITE DA,1: DWRITE DB,0: GOSUB 9000
8200 CASE "L": GOSUB 10000: FOR I=1 TO 5: PRINT I,E(I): NEXT
8390 SWEND
8400 REM "Report the distance after each command"
8410 GOSUB 11000: PRINT "Distance:",E
8500 WEND
8900 RETURN
9000 REM "drive a bit, TT says how long"
9010 DWRITE BA,0: DWRITE BB,0
9020 DELAY 100+TT*100
9030 DWRITE BA,1: DWRITE BB,1
9040 RETURN
10000 REM "The look around code"
10060 FOR J=1 TO 5
10070 A=(J-1)*45: GOSUB 12000: GOSUB 11000: E(J)=E
10120 NEXT
10130 A=90: GOSUB 12000
10190 RETURN
11000 REM "Echo once and return the value in E"
11010 DWRITE TP,0: PULSE TP,10
11020 E=FNE(PULSE(EP,1,70))
11030 RETURN
12000 REM "The servo code"
12010 DWRITE SP,0
12020 FOR I=1 TO 40: DELAY 20: PULSE SP,FNP(A): NEXT
12030 RETURN

