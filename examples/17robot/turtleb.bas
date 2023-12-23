10 REM "Engine code, the pins of the Arduino motor shield"
20 DA=12: SA=3: BA=9
30 DB=13: SB=11: BB=8
40 REM "Servo and sensor pins"
50 TP=4: EP=5
60 SP=6
100 REM "Set the pins and PWM"
110 PINM DA,1: PINM BA,1
120 PINM DB,1: PINM BB,1
130 AWRITE SA,0: AWRITE SB,0
140 PINM TP,1: PINM EP,0
150 PINM SP,1
200 REM "some parameters, TT is step time, S is speed ie voltage"
210 TT=5
220 S=140
300 REM "Prep the servo forward"
310 A=90:  GOSUB 9100
1000 REM "Input loop"
1010 INPUT A$
1020 GOSUB 8000: REM "call the turtle"
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
8130 IF C =>"0" AND C <="9" THEN TT=C-"0": CONT
8140 SWITCH C
8150 CASE "e": END
8160 CASE "f": DWRITE DA,0: DWRITE DB,0
8170 CASE "b": DWRITE DA,1: DWRITE DB,1
8180 CASE "l": DWRITE DA,0: DWRITE DB,1
8190 CASE "r": DWRITE DA,1: DWRITE DB,0
8200 CASE "L": A=0:  GOSUB 9100:  GOSUB 9000: A=90:  GOSUB 9100: CONT
8210 CASE "R": A=180:  GOSUB 9100:  GOSUB 9000: A=90:  GOSUB 9100: CONT
8220 CASE "F":  GOSUB 9000: CONT
8290 SWEND
8300 REM "drive a bit, TT says how long"
8310 DWRITE BA,0: DWRITE BB,0
8320 DELAY 100+TT*100
8330 DWRITE BA,1: DWRITE BB,1
8340 GOSUB 9000: PRINT "Distance:",E
8350 WEND
8900 RETURN
9000 REM "Measure the distance"
9010 DWRITE TP,0: PULSE TP,10
9020 E=PULSE(EP,1,100)/29*100/2
9030 PRINT "Distance:",E
9040 RETURN
9100 REM "Set the servo angle"
9110 DWRITE SP,0
9120 P=40+A*200/180
9130 FOR I=1 TO 40: DELAY 20: PULSE SP,P: NEXT
9140 RETURN
