10 OPEN &8,"13040",0
20 OPEN &8,"13041",1
25 J=0
30 A$="RD"
40 A$(3)=32+J
45 PRINT "sending message"
50 PRINT &8,A$
60 DELAY 10
70 IF AVAIL (8)=0 THEN 60
75 PRINT "data available"
80 INPUT &8,B$
90 PRINT B$(1,2)
95 PRINT B$
100 FOR I=4 TO 8
110 D(I-3)=B$(I,I)-32
115 PRINT I,D(I-3)
120 NEXT 
130 PRINT "data received"
140 PRINT "Records stored :",D(1)
150 PRINT "Humidity % ",D(2)
160 PRINT "Time",D(4),D(5)
170 PRINT "Temp ",D(3)*4
175 GET &8,A:IF AVAIL (8)<>0 THEN 175
180 J=J+1:IF D(1)>J THEN 40