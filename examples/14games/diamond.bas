1 REM "DIAMOND from 101 BASIC games"
2 REM "Ported to Stefan's BASIC in 2021"
3 REM
4 PRINT "FOR A PRETTY DIAMOND PATTERN,"
5 INPUT "TYPE IN AN ODD NUMBER BETWEEN 5 AND 21";R:PRINT
6 Q=INT(60/R):A$="CC"
8 FOR L=1 TO Q
10 X=1:Y=R:Z=2
20 FOR N=X TO Y STEP Z
25 TAB((R-N)/2)
28 FOR M=1 TO Q
29 C=1
30 FOR A=1 TO N
32 IF C>LEN(A$) THEN PRINT "!";: CONT
34 PRINT A$(C,C);
36 C=C+1
50 NEXT 
53 IF M=Q THEN BREAK
55 TAB(R*M+(R-N)/2)
56 NEXT 
60 PRINT
70 NEXT 
83 IF X<>1 THEN CONT
85 X=R-2:Y=1:Z=-2
90 GOTO 20
95 NEXT 
99 END