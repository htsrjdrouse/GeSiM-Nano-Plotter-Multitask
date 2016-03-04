
type tTransParse=
    record
        //Default values
        headcamimg:string; //This is going to be the image name of downloaded raspi head camera images
        dpath,dfilename:string;  //This is your default filenames and paths
        dzoff,dzmoveoff:real; //The is your default z height values
        dwdt,ddt,dewv,dewf:real; //Default wash variables
        pdepz:string; //This is a qualifier that sets the depz value since its a boolean datatype
        depz:boolean; //Do you want to turn the piezo on during the extrawash?
        deww:string; //This is the name of the default extrawash reservoir
        daf,dav,dadt,daddt,dadbdt:real; //These are your default aspiration volumes
        pdapz:string; //This is a qualifier that sets the dapz value since its a boolean datatype
        dapz:boolean; //Do you want to turn on the piezo during aspiration?
        dapzv,dapzp,dapzf:integer;//These are your default piezo values for when you aspiration assuming you turn the dapz on
        dsdt,dsbtdt,dscv:real; //These are your default stroboscope settings
        dschc:integer; //How many checks do you want to do per strob event
        //End default values
        asp:integer; //This is a flag to indicate whether to aspirate or not
        tl:array[1..1000] of real; //tiplist array of integers to control specific tips in cor file(missing spots)
        tlstring:string; //tiplist string that is put in transferlist for running cor files (missing spots)
        tgrstring:string; //target group        
        tgr:array[1..1000] of real; //target group        
        tbrstring,terstring,tbcstring,tecstring:string; //target position coordinates string for array
        tbr,ter,tbc,tec:array[1..1000] of real; //target position arrays
        bbrstring,berstring,bbcstring,becstring:string; //block position coordinates string for arrays
        bbr,ber,bbc,bec:array[1..1000] of real; //block position arrays
        cbrstring,cerstring,cbcstring,cecstring:string; //spot position coordinate string for arrays
        cbr,cer,cbc,cec:array[1..1000] of real; //spot position arrays
        s:string; //source plate
        sr,sc:integer; //source plate positions
        drops:integer; //drops per position
        volt,freq,pulse,drop:array[1..1000] of real; //nozzle parameters
        voltstring,freqstring,pulsestring,dropstring:string;
        spotzh:real;
        anot:string; //This is the anotation key
        anotdes:string; //This is the anotation description
        ztrav:real;
	z:real;
        adp:integer; //This are various ADP combinations in DriveOption.inc
        x_offset,y_offset,dx,dy:array[1..1000] of real; //ADP Spot 
        numX,numY:array[1..1000] of real; //ADP Spot
        x_offsetstr,y_offsetstr,dxstr,dystr,numXstr,numYstr:string;//ADP Spot
        bx,ex,by,ey,lx,ly,speed,xpos,ypos:array[1..1000] of real; //ADP Line	
        bxstring,exstring,bystring,eystring,lxstring,lystring,speedstring,xposstring,yposstring:string; //ADP Line
        batch:integer;
        sptypstring:string; //whether to use real coordinates or array spots
        sptyp:array[1..1000] of real; //whether to use real coordinates or array spots
       	tippos:integer; //This is tip position that can be used as an alternative to max tip. Note handling this incorrectly can destroy tips
        zhsubstrate:integer; //Just in case your target is irregular heights
        ssbct,ssbrt:integer; //This is for the spotstate to indicate whether you want multiple tips spotting in a block or designate a tip per block
        da:integer;
        ptindexstring:string;
        ptindex:array[1..1000] of real;
        hsnap:integer;

	//For python parser
	i:integer;
	pynum:integer;
	pypath,pyfile:string;
	pypathar:array[1..100] of string;
	
	gc:string; //general gcode command
	gcp,gco,gcf,gcd:integer; //digital  output gcode arduino + gcode feed rate
	gcx,gcy,gcz:real; //gcode positioning



    end;

 //_______________________________________________________________//

function BooleanQual(pboo:string):boolean;

var boo:boolean;

begin
if FindString(sToSearch=pboo, sToFind="true") = 0 then boo = true else boo = false; 
 BooleanQual = boo;

end;
 //_______________________________________________________________//

function CommaDelimited(lstr:string):array[1..1000] of real;

var len,ij,il,i:integer;
    z,snamestr:string;
    pos:array[1..1000] of integer;
    lar:array[1..1000] of real;

begin
 //Message("lstr", lstr);
 if FindString(sToSearch=lstr, sToFind=",") == -1 then lar[1] = StringToReal(lstr);
 else begin
    len = StringLength(lstr);
    ij = 0;
    for il=0 to len do begin
        z=Substring(lstr,il,il+1);
        if z = "," then begin
         ij = ij + 1;
         pos[ij] = il;
         snamestr = snamestr + z;
        end;
    end;
    for i=1 to (ij+1) do begin
        if i = 1 then begin
         lar[i] = StringToReal(Substring(lstr,0,pos[i]));
        end;
        if i > 1 and i < (ij+1) then begin
         lar[i] = StringToReal(Substring(lstr,pos[i-1]+1,pos[i]));   
        end;        
        if i = (ij+1) and i > 1 then begin
         lar[i] = StringToReal(Substring(lstr,pos[i-1]+1,-1));
        end;
    end;
 end;
 
 CommaDelimited = lar;
   
end;


//________________________________MAIN___________________________________________//


function ParserXML(line:string):tTransParse;

var tT:tTransParse;
    func:integer; //Comma Delimited data type set parameter
    i:integer;
begin

    //Spotting Locations
    tT.asp=StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<A>")+3, FindString(sToSearch=line, sToFind="</A>")));

    //Target Coordinates
    //First make strings
    tT.tgrstring=Substring(line, FindString(sToSearch=line, sToFind="<G>")+3, FindString(sToSearch=line, sToFind="</G>"));

    if FindString(sToSearch=line, sToFind="</TBR>") > -1 then tT.tbrstring=Substring(line, FindString(sToSearch=line, sToFind="<TBR>")+5, FindString(sToSearch=line, sToFind="</TBR>"));
    else tT.tbrstring = "";
 
    if FindString(sToSearch=line, sToFind="</TER>") > -1 then tT.terstring=Substring(line, FindString(sToSearch=line, sToFind="<TER>")+5, FindString(sToSearch=line, sToFind="</TER>"));
    else tT.terstring = "";

    if FindString(sToSearch=line, sToFind="</TBC>") > -1 then tT.tbcstring=Substring(line, FindString(sToSearch=line, sToFind="<TBC>")+5, FindString(sToSearch=line, sToFind="</TBC>"));
    else tT.tbcstring = "";

    if FindString(sToSearch=line, sToFind="</TEC>") > -1 then tT.tecstring=Substring(line, FindString(sToSearch=line, sToFind="<TEC>")+5, FindString(sToSearch=line, sToFind="</TEC>"));
    else tT.tecstring = "";

    //Now populate arrays
    if StringLength(tT.tgrstring) > 0 then tT.tgr=CommaDelimited(tT.tgrstring);
    if StringLength(tT.tbrstring) > 0 then tT.tbr=CommaDelimited(tT.tbrstring);
    if StringLength(tT.terstring) > 0 then tT.ter=CommaDelimited(tT.terstring);
    if StringLength(tT.tbcstring) > 0 then tT.tbc=CommaDelimited(tT.tbcstring);
    if StringLength(tT.tecstring) > 0 then tT.tec=CommaDelimited(tT.tecstring);

    //Block Coordinates
    //First make strings
    if FindString(sToSearch=line, sToFind="</BBR>") > -1 then tT.bbrstring=Substring(line, FindString(sToSearch=line, sToFind="<BBR>")+5, FindString(sToSearch=line, sToFind="</BBR>"));
    else tT.bbrstring = "";

    if FindString(sToSearch=line, sToFind="</BER>") > -1 then tT.berstring=Substring(line, FindString(sToSearch=line, sToFind="<BER>")+5, FindString(sToSearch=line, sToFind="</BER>"));
    else tT.berstring = "";


    if FindString(sToSearch=line, sToFind="</BBC>") > -1 then tT.bbcstring=Substring(line, FindString(sToSearch=line, sToFind="<BBC>")+5, FindString(sToSearch=line, sToFind="</BBC>"));
    else tT.bbcstring = "";

    if FindString(sToSearch=line, sToFind="</BEC>") > -1 then tT.becstring=Substring(line, FindString(sToSearch=line, sToFind="<BEC>")+5, FindString(sToSearch=line, sToFind="</BEC>"));
    else tT.becstring = "";

    //Now populate arrays
    if StringLength(tT.bbrstring) > 0 then tT.bbr=CommaDelimited(tT.bbrstring);
    if StringLength(tT.berstring) > 0 then tT.ber=CommaDelimited(tT.berstring);
    if StringLength(tT.bbcstring) > 0 then tT.bbc=CommaDelimited(tT.bbcstring);
    if StringLength(tT.becstring) > 0 then tT.bec=CommaDelimited(tT.becstring);

    //Spot Coordinates
    //First make strings
    if FindString(sToSearch=line, sToFind="</CBR>") > -1 then tT.cbrstring=Substring(line, FindString(sToSearch=line, sToFind="<CBR>")+5, FindString(sToSearch=line, sToFind="</CBR>"));
    else tT.cbrstring = "";

    if FindString(sToSearch=line, sToFind="</CER>") > -1 then tT.cerstring=Substring(line, FindString(sToSearch=line, sToFind="<CER>")+5, FindString(sToSearch=line, sToFind="</CER>"));
    else tT.cerstring = "";

    if FindString(sToSearch=line, sToFind="</CBC>") > -1 then tT.cbcstring=Substring(line, FindString(sToSearch=line, sToFind="<CBC>")+5, FindString(sToSearch=line, sToFind="</CBC>"));
    else tT.cbcstring = "";

    if FindString(sToSearch=line, sToFind="</CEC>") > -1 then tT.cecstring=Substring(line, FindString(sToSearch=line, sToFind="<CEC>")+5, FindString(sToSearch=line, sToFind="</CEC>"));
    else tT.cecstring = "";

    //Now populate arrays
    if StringLength(tT.cbrstring) > 0 then tT.cbr=CommaDelimited(tT.cbrstring);
    if StringLength(tT.cerstring) > 0 then tT.cer=CommaDelimited(tT.cerstring); 
    if StringLength(tT.cbcstring) > 0 then tT.cbc=CommaDelimited(tT.cbcstring);
    if StringLength(tT.cecstring) > 0 then tT.cec=CommaDelimited(tT.cecstring);
 
    //Aspirations
    if FindString(sToSearch=line, sToFind="<P>") > -1 then tT.s=Substring(line, FindString(sToSearch=line, sToFind="<P>")+3, FindString(sToSearch=line, sToFind="</P>"));
    if FindString(sToSearch=line, sToFind="<SR>") > -1 then tT.sr=StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<SR>")+4, FindString(sToSearch=line, sToFind="</SR>")));
    if FindString(sToSearch=line, sToFind="<SC>") > -1 then tT.sc=StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<SC>")+4, FindString(sToSearch=line, sToFind="</SC>")));

   //TipList and Drops
    if FindString(sToSearch=line, sToFind="<TL>") > -1 then tT.tlstring=Substring(line, FindString(sToSearch=line, sToFind="<TL>")+4, FindString(sToSearch=line, sToFind="</TL>"));
    if FindString(sToSearch=line, sToFind="<D>") > -1 then tT.dropstring=Substring(line, FindString(sToSearch=line, sToFind="<D>")+3, FindString(sToSearch=line, sToFind="</D>"));
    if FindString(sToSearch=line, sToFind="<V>") > -1 then tT.voltstring=Substring(line, FindString(sToSearch=line, sToFind="<V>")+3, FindString(sToSearch=line, sToFind="</V>"));
    if FindString(sToSearch=line, sToFind="<L>") > -1 then tT.pulsestring=Substring(line, FindString(sToSearch=line, sToFind="<L>")+3, FindString(sToSearch=line, sToFind="</L>"));
    if FindString(sToSearch=line, sToFind="<F>") > -1 then tT.freqstring=Substring(line, FindString(sToSearch=line, sToFind="<F>")+3, FindString(sToSearch=line, sToFind="</F>"));

    //Annotations
    if FindString(sToSearch=line, sToFind="<AT>") > -1 then tT.anot = Substring(line, FindString(sToSearch=line, sToFind="<AT>")+4, FindString(sToSearch=line, sToFind="</AT>"));
    if FindString(sToSearch=line, sToFind="<AD>") > -1 then tT.anotdes = Substring(line, FindString(sToSearch=line, sToFind="<AD>")+4, FindString(sToSearch=line, sToFind="</AD>"));
    if FindString(sToSearch=line, sToFind="<HD>") > -1 then tT.headcamimg = Substring(line, FindString(sToSearch=line, sToFind="<HD>")+4, FindString(sToSearch=line, sToFind="</HD>"));

    //ADP switch
    tT.adp = StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<ADP>")+5, FindString(sToSearch=line, sToFind="</ADP>")));
 
    //Positioning for ADP Array
    if FindString(sToSearch=line, sToFind="<XOFF>") > -1 then tT.x_offsetstr = Substring(line, FindString(sToSearch=line, sToFind="<XOFF>")+6, FindString(sToSearch=line, sToFind="</XOFF>"));
    if FindString(sToSearch=line, sToFind="<YOFF>") > -1 then tT.y_offsetstr = Substring(line, FindString(sToSearch=line, sToFind="<YOFF>")+6, FindString(sToSearch=line, sToFind="</YOFF>"));
    if FindString(sToSearch=line, sToFind="<DX>") > -1 then tT.dxstr = Substring(line, FindString(sToSearch=line, sToFind="<DX>")+4, FindString(sToSearch=line, sToFind="</DX>"));
    if FindString(sToSearch=line, sToFind="<DY>") > -1 then tT.dystr = Substring(line, FindString(sToSearch=line, sToFind="<DY>")+4, FindString(sToSearch=line, sToFind="</DY>"));
    if FindString(sToSearch=line, sToFind="<NUMX>") > -1 then tT.numXstr = Substring(line, FindString(sToSearch=line, sToFind="<NUMX>")+6, FindString(sToSearch=line, sToFind="</NUMX>"));
    if FindString(sToSearch=line, sToFind="<NUMY>") > -1 then tT.numYstr = Substring(line, FindString(sToSearch=line, sToFind="<NUMY>")+6, FindString(sToSearch=line, sToFind="</NUMY>"));

    //Positioning for ADP Line
    if FindString(sToSearch=line, sToFind="<BX>") > -1 then tT.bxstring = Substring(line, FindString(sToSearch=line, sToFind="<BX>")+4, FindString(sToSearch=line, sToFind="</BX>"));
    if FindString(sToSearch=line, sToFind="<BY>") > -1 then tT.bystring = Substring(line, FindString(sToSearch=line, sToFind="<BY>")+4, FindString(sToSearch=line, sToFind="</BY>"));
    if FindString(sToSearch=line, sToFind="<EX>") > -1 then tT.exstring = Substring(line, FindString(sToSearch=line, sToFind="<EX>")+4, FindString(sToSearch=line, sToFind="</EX>"));
    if FindString(sToSearch=line, sToFind="<EY>") > -1 then tT.eystring = Substring(line, FindString(sToSearch=line, sToFind="<EY>")+4, FindString(sToSearch=line, sToFind="</EY>"));
    if FindString(sToSearch=line, sToFind="<LX>") > -1 then tT.lxstring = Substring(line, FindString(sToSearch=line, sToFind="<LX>")+4, FindString(sToSearch=line, sToFind="</LX>")); //Length of line (which would substitute for the ex position if you use offsets for video targeting
    if FindString(sToSearch=line, sToFind="<LY>") > -1 then tT.lystring = Substring(line, FindString(sToSearch=line, sToFind="<LY>")+4, FindString(sToSearch=line, sToFind="</LY>")); //Length of line (which would substitute for the ex position if you use offsets for video targeting
    if FindString(sToSearch=line, sToFind="<SPEED>") > -1 then tT.speedstring = Substring(line, FindString(sToSearch=line, sToFind="<SPEED>")+7, FindString(sToSearch=line, sToFind="</SPEED>"));
    if FindString(sToSearch=line, sToFind="<ST>") > -1 then tT.sptypstring = Substring(line, FindString(sToSearch=line, sToFind="<ST>")+4, FindString(sToSearch=line, sToFind="</ST>")); //whether to use real coordinates or array spots

    //ST Points PT
    if FindString(sToSearch=line, sToFind="<PT>") > -1 then tT.ptindexstring = Substring(line, FindString(sToSearch=line, sToFind="<PT>")+4, FindString(sToSearch=line, sToFind="</PT>")); 
    if FindString(sToSearch=line, sToFind="<X>") > -1 then tT.xposstring = Substring(line, FindString(sToSearch=line, sToFind="<X>")+3, FindString(sToSearch=line, sToFind="</X>"));
    if FindString(sToSearch=line, sToFind="<Y>") > -1 then tT.yposstring = Substring(line, FindString(sToSearch=line, sToFind="<Y>")+3, FindString(sToSearch=line, sToFind="</Y>"));
    

    //Now populate arrays
    tT.numX=CommaDelimited(tT.numXstr);
    tT.numY=CommaDelimited(tT.numYstr);    
    tT.x_offset=CommaDelimited(tT.x_offsetstr);
    tT.y_offset=CommaDelimited(tT.y_offsetstr);    
    tT.dx=CommaDelimited(tT.dxstr);
    tT.dy=CommaDelimited(tT.dystr);
    tT.bx=CommaDelimited(tT.bxstring);
    tT.by=CommaDelimited(tT.bystring);
    tT.ex=CommaDelimited(tT.exstring);
    tT.ey=CommaDelimited(tT.eystring);
    tT.lx=CommaDelimited(tT.lxstring);
    tT.ly=CommaDelimited(tT.lystring);
    tT.speed=CommaDelimited(tT.speedstring);   
    tT.tl=CommaDelimited(tT.tlstring);
    tT.drop=CommaDelimited(tT.dropstring);
    tT.volt=CommaDelimited(tT.voltstring);
    tT.pulse=CommaDelimited(tT.pulsestring);
    tT.freq=CommaDelimited(tT.freqstring);   
    tT.sptyp=CommaDelimited(tT.sptypstring);
    tT.ptindex=CommaDelimited(tT.ptindexstring);
    tT.xpos=CommaDelimited(tT.xposstring);
    tT.ypos=CommaDelimited(tT.yposstring);
        
    if FindString(sToSearch=line, sToFind="<B>") > -1 then tT.batch=StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<B>")+3, FindString(sToSearch=line, sToFind="</B>")));
    if FindString(sToSearch=line, sToFind="<SP>") > -1 then tT.tippos=StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<SP>")+4, FindString(sToSearch=line, sToFind="</SP>")));
    if FindString(sToSearch=line, sToFind="<SH>") > -1 then tT.zhsubstrate=StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<SH>")+4, FindString(sToSearch=line, sToFind="</SH>")));
 

    //Default values parsing
    tT.dpath = Substring(line, FindString(sToSearch=line, sToFind="<DP>")+4, FindString(sToSearch=line, sToFind="</DP>"));
    tT.dfilename = Substring(line, FindString(sToSearch=line, sToFind="<DF>")+4, FindString(sToSearch=line, sToFind="</DF>"));
    tT.dzoff = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<DZOFF>")+7, FindString(sToSearch=line, sToFind="</DZOFF>")));
    tT.dzmoveoff = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<DZMOFF>")+8, FindString(sToSearch=line, sToFind="</DZMOFF>")));
    tT.dwdt = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<DWDT>")+6, FindString(sToSearch=line, sToFind="</DWDT>")));
    tT.ddt = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<DDT>")+5, FindString(sToSearch=line, sToFind="</DDT>")));
    tT.dewv = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<DEWV>")+6, FindString(sToSearch=line, sToFind="</DEWV>")));
    tT.dewf = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<DEWF>")+6, FindString(sToSearch=line, sToFind="</DEWF>")));
    tT.pdepz = Substring(line, FindString(sToSearch=line, sToFind="<DEPZ>")+6, FindString(sToSearch=line, sToFind="</DEPZ>"));
    tT.depz = BooleanQual(tT.pdepz);
    tT.deww = Substring(line, FindString(sToSearch=line, sToFind="<DEWW>")+6, FindString(sToSearch=line, sToFind="</DEWW>"));
    tT.daf = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<DAF>")+5, FindString(sToSearch=line, sToFind="</DAF>")));
    tT.dav = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<DAV>")+5, FindString(sToSearch=line, sToFind="</DAV>")));
    tT.dadt = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<DADT>")+6, FindString(sToSearch=line, sToFind="</DADT>")));
    tT.daddt = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<DADDT>")+7, FindString(sToSearch=line, sToFind="</DADDT>")));
    tT.dadbdt = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<DADBDT>")+8, FindString(sToSearch=line, sToFind="</DADBDT>")));
    tT.pdapz = Substring(line, FindString(sToSearch=line, sToFind="<PDAPZ>")+7, FindString(sToSearch=line, sToFind="</PDAPZ>"));
    tT.dapz = BooleanQual(tT.pdapz);
    tT.dapzv = StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<DAPZV>")+7, FindString(sToSearch=line, sToFind="</DAPZV>")));
    tT.dapzp = StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<DAPZP>")+7, FindString(sToSearch=line, sToFind="</DAPZP>")));
    tT.dapzf = StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<DAPZF>")+7, FindString(sToSearch=line, sToFind="</DAPZF>")));
    tT.dsdt = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<DSDT>")+6, FindString(sToSearch=line, sToFind="</DSDT>")));
    tT.dschc = StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<DSCHC>")+7, FindString(sToSearch=line, sToFind="</DSCHC>")));
    tT.dsbtdt = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<DSBTDT>")+8, FindString(sToSearch=line, sToFind="</DSBTDT>")));
    tT.dscv = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<DSCV>")+6, FindString(sToSearch=line, sToFind="</DSCV>")));
    
    
    //Spot state blocks and columns switch
    if FindString(sToSearch=line, sToFind="<SSBR>") > -1 then tT.ssbrt = StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<SSBR>")+6, FindString(sToSearch=line, sToFind="</SSBR>")));
    if FindString(sToSearch=line, sToFind="<SSBC>") > -1 then tT.ssbct = StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<SSBC>")+6, FindString(sToSearch=line, sToFind="</SSBC>")));
    if FindString(sToSearch=line, sToFind="<DA>") > -1 then tT.da = StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<DA>")+4, FindString(sToSearch=line, sToFind="</DA>")));
    if FindString(sToSearch=line, sToFind="<C>") > -1 then tT.hsnap = StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<C>")+3, FindString(sToSearch=line, sToFind="</C>")));

    if FindString(sToSearch=line, sToFind="<Z>") > -1 then tT.z = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<Z>")+3, FindString(sToSearch=line, sToFind="</Z>")));


    if FindString(sToSearch=line, sToFind="<num>") > -1 then begin
       tT.pynum = Int(StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<num>")+5, FindString(sToSearch=line, sToFind="</num>"))));

   for i=1 to tT.pynum do begin
     if i < 10 then begin
      if FindString(sToSearch=line, sToFind="<dir"+i+">") > -1 then tT.pypath = Substring(line, FindString(sToSearch=line, sToFind="<dir"+i+">")+6, FindString(sToSearch=line, sToFind="</dir"+i+">"));
       tT.pypathar[i] = tT.pypath;
      end;
     if i > 9 then begin
      if FindString(sToSearch=line, sToFind="<dir"+i+">") > -1 then tT.pypath = Substring(line, FindString(sToSearch=line, sToFind="<dir"+i+">")+7, FindString(sToSearch=line, sToFind="</dir"+i+">"));
       tT.pypathar[i] = tT.pypath;
     end;
   end;

   if FindString(sToSearch=line, sToFind="<fle>") > -1 then tT.pyfile = Substring(line, FindString(sToSearch=line, sToFind="<fle>")+5, FindString(sToSearch=line, sToFind="</fle>"));
   
   

 end; //searching the num tag

   if FindString(sToSearch=line, sToFind="<GCP>") > -1 then tT.gcp = StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<GCP>")+5, FindString(sToSearch=line, sToFind="</GCP>")));
   if FindString(sToSearch=line, sToFind="<GCO>") > -1 then tT.gco = StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<GCO>")+5, FindString(sToSearch=line, sToFind="</GCO>")));
   if FindString(sToSearch=line, sToFind="<GCX>") > -1 then tT.gcx = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<GCX>")+5, FindString(sToSearch=line, sToFind="</GCX>")));
   if FindString(sToSearch=line, sToFind="<GCY>") > -1 then tT.gcy = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<GCY>")+5, FindString(sToSearch=line, sToFind="</GCY>")));
   if FindString(sToSearch=line, sToFind="<GCZ>") > -1 then tT.gcz = StringToReal(Substring(line, FindString(sToSearch=line, sToFind="<GCZ>")+5, FindString(sToSearch=line, sToFind="</GCZ>")));
   if FindString(sToSearch=line, sToFind="<GCF>") > -1 then tT.gcf = StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<GCF>")+5, FindString(sToSearch=line, sToFind="</GCF>")));
   if FindString(sToSearch=line, sToFind="<GCD>") > -1 then tT.gcd = StringToInteger(Substring(line, FindString(sToSearch=line, sToFind="<GCD>")+5, FindString(sToSearch=line, sToFind="</GCD>")));
   if FindString(sToSearch=line, sToFind="<GC>") > -1 then tT.gc = Substring(line, FindString(sToSearch=line, sToFind="<GC>")+4, FindString(sToSearch=line, sToFind="</GC>"));



 ParserXML = tT;

end;
 
