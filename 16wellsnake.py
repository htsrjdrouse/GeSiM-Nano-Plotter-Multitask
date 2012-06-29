print "<DAF>0.200000</DAF><DAV>3.000000</DAV><DADT>1.000000</DADT><DADDT>0.000000</DADDT><DADBDT>0.100000</DADBDT>"
print "<DAPZ>false</DAPZ><DAPZV>100</DAPZV><DAPZP>50</DAPZP><DAPZF>100</DAPZF>"
print "<DSDT>0.500000</DSDT><DSCHC>4</DSCHC><DSBTDT>0.100000</DSBTDT><DSCV>15.000000</DSCV>"
print "<DZOFF>0.500000</DZOFF><DZMOFF>0.000000</DZMOFF>"
print "<DWDT>30.000000</DWDT><DDT>0.100000</DDT><DEWV>0.000000</DEWV><DEWF>0.500000</DEWF><DEPZ>false</DEPZ><DEWW>extrawash</DEWW>"

#change for your number of samples

samples = 79
#change for number of tips

tips = 1

tbc = 1
tec = 3
bbc = 1
bec = 2

#array for target row and block row
tbra = [1]
bbra = [1,2,3,4,5,6,7,8]


if tips == 2:
    tl = "1,1"
    tipx = 3
elif tips == 1:
    tl = "1"
    tipx = 1

dis = tl

for i in range(0,samples):
        sr = int(i/24)*tipx + 1
        sc = i+1 - (24 * int((i+1)/24.01))
        pos = (i+5) % 5
        print "<A>1</A><TL>"+tl+"</TL><P>MTP_Sys</P><B>1</B><SR>"+str(sr)+"</SR><SC>"+str(sc)+"</SC>"
        if pos == 0:
                colstart = 1
        elif pos == 1:
                colstart = 4
        elif pos == 2:
                colstart = 7
        elif pos == 3:
                colstart = 10
        elif pos == 4:
                colstart = 13
        cbr = int((i+5)/5)
        cer = int((i+5)/5)
        cbc = colstart
        cec = colstart + 2

        for k in range(0,len(tbra)):
                for j in range(0,len(bbra)):
                        if (bbra[j] == 1) or (bbra[j] == 3)or (bbra[j] == 5)or (bbra[j] == 7):
                                print "<D>"+dis+"</D><ST>2</ST><A>4</A><B>1</B><TL>"+tl+"</TL><G>1</G><TBR>"+str(tbra[k])+"</TBR><TER>"+str(tbra[k])+"</TER><TBC>"+str(tec)+"</TBC><TEC>"+str(tbc)+"</TEC><BBR>"+str(bbra[j])+"</BBR><BER>"+str(bbra[j])+"</BER><BBC>"+str(bec)+"</BBC><BEC>"+str(bbc)+"</BEC><CBR>"+str(cbr)+"</CBR><CER>"+str(cer)+"</CER><CBC>"+str(cec)+"</CBC><CEC>"+str(cbc)+"</CEC>"
                        if (bbra[j] == 2) or (bbra[j] == 4) or (bbra[j] == 6) or (bbra[j] == 8):
                                print "<D>"+dis+"</D><ST>2</ST><A>4</A><B>1</B><TL>"+tl+"</TL><G>1</G><TBR>"+str(tbra[k])+"</TBR><TER>"+str(tbra[k])+"</TER><TBC>"+str(tbc)+"</TBC><TEC>"+str(tec)+"</TEC><BBR>"+str(bbra[j])+"</BBR><BER>"+str(bbra[j])+"</BER><BBC>"+str(bbc)+"</BBC><BEC>"+str(bec)+"</BEC><CBR>"+str(cbr)+"</CBR><CER>"+str(cer)+"</CER><CBC>"+str(cbc)+"</CBC><CEC>"+str(cec)+"</CEC>"



