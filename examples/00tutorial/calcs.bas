10 REM "Pocket calculator in BASIC"
20 REM "This version uses the structured language set"
100 REM "Definitions - keywords and tokens"
110 DATA "+", 2, "-", 3, "*", 4, "/", 5, "p", 6, "d", 7, "q", 8
111 DATA "sqr", 9, "dup", 10, "swap", 11, "", 12, "", 13, "", 14, "", 15, "", 16
120 REM "The stack"
130 S=0
140 N=16
150 DIM S(N)
160 REM "Errors"
170 E=0
200 REM "Input loop"
205 WHILE 1
210 PRINT "<";S;">:"; :INPUT A$
220 @S=0
230 A=VAL(A$)
240 IF @S=0 THEN T=1 : GOTO 500
300 T=-1
310 RESTORE
330 @S=0
340 FOR I
350 READ K$
360 READ X
370 IF K$=A$ THEN T=X : BREAK
380 IF @S<>0 THEN BREAK
390 NEXT 
400 IF T=-1 THEN E=1
500 REM "The token switch, with an evil expression GOSUB"
505 REM "Line numbers 1000, 1100, ... are targeted"
510 IF T>0 GOSUB 1000
600 IF E<>0 THEN PRINT "Error" : E=0 : CONT
610 IF S>0 THEN PRINT S(S)
620 WEND
1000 SWITCH T
1010 CASE 1
1020 REM "Enter a number"
1030 IF S=N THEN E=0: RETURN 
1040 S=S+1 : S(S)=A
1050 CASE 2
1060 REM "Addition"
1070 IF S<2 THEN E=1: RETURN
1080 S(S-1)=S(S-1)+S(S)
1090 S=S-1
1100 CASE 3
1110 REM "Subtraction"
1120 IF S<2 THEN E=1: RETURN
1130 S(S-1)=S(S-1)-S(S)
1140 S=S-1
1150 CASE 4
1160 REM "Multiplication"
1170 IF S<2 THEN E=1: RETURN
1180 S(S-1)=S(S-1)*S(S)
1190 S=S-1
1200 CASE 5
1210 REM "Division"
1220 IF S<2 OR S(S)=0 THEN E=1: RETURN
1230 S(S-1)=S(S-1)/S(S)
1240 S=S-1
1250 CASE 6
1260 REM "Display a number"
1270 IF S=0 THEN E=1
1280 CASE 7
1290 REM "Drop"
1300 IF S=0 THEN E=1 ELSE S=S-1
1310 CASE 8
1320 REM "Quit"
1330 END
1340 CASE 9
1350 REM "Square root"
1360 IF S(S)<0 OR S=0 THEN E=1 ELSE S(S)=SQR(S(S))
1370 CASE 10
1380 REM "Dup"
1390 IF S>N-1 OR S=0 THEN E=1 ELSE S=S+1: S(S)=S(S-1)
1400 CASE 11
1410 REM "Swap"
1420 IF S=N OR S<2 THEN E=1 ELSE A=S(S) : S(S)=S(S-1) : S(S-1)=A
1430 CASE 12
1440 REM ""
1450 CASE 13
1460 REM ""
1470 CASE 14
1480 REM ""
1490 CASE 15
1500 REM ""
1510 CASE 16
1520 REM ""
1530 SWEND
1600 RETURN

