10 FOR I=1 TO 10
20 A=A+I
30 NEXT I
40 PRINT A
50 FOR I=1
60 IF I=5 BREAK
70 NEXT
80 PRINT I
90 FOR I=1 TO 10
100 IF I>5 CONT
110 PRINT I
120 NEXT
130 FOR I=1 TO 2
140 FOR J=1 TO 2
150 FOR K=1 TO 2
160 FOR L=1 TO 2
170 A=A+(I+J)*(K+L)
180 NEXT
190 NEXT
200 NEXT
210 NEXT
220 PRINT A 