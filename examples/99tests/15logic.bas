10 REM "Logical expressions"
20 REM ""
100 A=10 
110 B=20
120 PRINT "A="; A, "; B=";B, "; C=";C
130 IF A=10 THEN PRINT "A is 10"
140 IF NOT A=20 THEN PRINT "A is not 20"
150 IF A=10 AND B=20 THEN PRINT "A is 10 and B is 20"
160 IF A=10 OR C=10 THEN PRINT "A is 10 or C is 10"
200 PRINT "Expressions"
210 L=(A=10 AND B=20)
220 PRINT "L=(A=10 AND B=20)"
230 PRINT "L =",L
240 K=NOT L
250 PRINT "NOT L =",K
260 K=NOT NOT L
270 PRINT "NOT NOT L =",K
280 END
