/*
This can do one or many stroboscope checks and can calulate statistics
on the droplet profiles by calculating the .bmp images

Richard - rjdrouse@htsresources.com
*/

const DROP =     32;    //  Droplets in the stroboscope

//For GetGrabberValue
const DIAMETER      = 0;
const RADIUS        = 1;
const ANGLE         = 2;
const COUNT         = 3;
const POS_X         = 4;
const POS_Y         = 5;
const WIDTH         = 6;
const HEIGHT        = 7;
const AREA          = 8;
const WETTING       = 13;
const SATANGLE      = 20;
const VOLUME        = 19;
//const MAINX         = 21;
//const MAINY         = 22;

type tStrobData=
    record
        ok:boolean;
        x,y,d,ang,count,height,width,wetting,mainx,mainy,volume,satangle:array[1..100] of real;
        avgy,miny,maxy,stdevy,coefy:real;
    end;

function StrobCheck(linecount:integer,
		    tip:integer,
                    drop:integer,
                    chkcount:integer,
                    row:integer,
                    col:integer,
                    strobfilename:string,
                    delay:real,
                    assay:integer,
                    btdelay:real,
                    path:string,
                    anotation:string):tStrobData;

var i:integer;
    tSS:tStrobData;
    strobimage,datastring:string;
    le,gr,calc,avg:real;
    fFile:file;
    d,t,JustTime:string;

begin
 d = Date();
 t = Time();
 JustTime=Substring(t,0, 2)+Substring(t,3,5)+Substring(t,6,8);
 TimerDelay(t = delay); //initial delay

 tSS.ok=true; 

 for i=1 to chkcount do begin
  if tSS.ok=true then begin        
    tSS.x[i]=-1.0;
    tSS.y[i]=-1.0;
    tSS.d[i]=-1.0;
    tSS.ang[i]=-1.0;
    tSS.count[i]=-1.0;
               
    strobimage = path + "line"+StringOf(linecount) + "_AT" + anotation + "_"+ JustTime + "Assay_" + anotation + "Tip_" + StringOf(tip) + "_Drop_" + StringOf(drop);
    strobimage = strobimage + "_Chk_" + StringOf(i);
    
    if (SimulationMode()=true) or (TipDummyActive()=true) then begin
        tSS.ok=true;        
    end;
    else begin
        SetLiveVideo( on=false); 
        Grab(DROP);
        tSS.ok=FindDots(DROP);
        SaveBitmap( filename=strobimage + ".bmp", bUseFileDialog=false, bBitmapOnly=false );
        //Exec( command="C:/Program Files/ImageMagick-6.4.9-Q16/mogrify.exe -resize 25% " + strobimage + ".bmp");
        //SaveBitmap( filename=strobimage + ".nomask.bmp", bUseFileDialog=false, bBitmapOnly=true );
        SetLiveVideo( on=true);
    end;
    
    if (tSS.ok = true and TipDummyActive()=false and SimulationMode() = false)  then begin
                                    
        tSS.x[i]=GetGrabberValue( finderId=DROP, index=POS_X);
        tSS.y[i]=GetGrabberValue( finderId=DROP, index=POS_Y);             
        tSS.d[i]=GetGrabberValue( finderId=DROP, index=DIAMETER);
        tSS.ang[i]=GetGrabberValue( finderId=DROP, index=ANGLE);
        tSS.count[i]=GetGrabberValue( finderId=DROP, index=COUNT);
        tSS.satangle[i]=GetGrabberValue( finderId=DROP, index=SATANGLE);
        tSS.volume[i]=GetGrabberValue( finderId=DROP, index=VOLUME); 
        //tSS.mainx[i]=GetGrabberValue( finderId=DROP, index=MAINX); 
        //tSS.mainy[i]=GetGrabberValue( finderId=DROP, index=MAINY); 



        fFile=OpenRW( filename=path + strobfilename, , append=true);         
        datastring = "<StbDt Batch=" + StringOf(assay) + " TS=" + JustTime + ">";       
        datastring = datastring + "<LN>" + StringOf(linecount) + "</LN>";
        datastring = datastring + "<Tip>" + StringOf(tip) + "</Tip>";
        datastring = datastring + "<AT>" + anotation + "</AT>";
        datastring = datastring + "<StrbCh>" + StringOf(i) + "</StrobCheck>";
        datastring = datastring + "<PosX>" + StringOf(tSS.x[i]:0:3) + "</PosX>";
        datastring = datastring + "<PosY>" + StringOf(tSS.y[i]:0:3) + "</PosY>";
        datastring = datastring + "<DpDia>" + StringOf(tSS.d[i]:0:3) + "</DpDia>";
        datastring = datastring + "<Vol>" + StringOf(tSS.volume[i]:0:3) + "</Vol>";
        datastring = datastring + "<DflAng>" + StringOf(tSS.ang[i]:0:3) + "</DflAng>";
        datastring = datastring + "<SatAng>" + StringOf(tSS.satangle[i]:0:3) + "</SatAng>";
        datastring = datastring + "<DpCnt>" + StringOf(Int(tSS.count[i])) + "</DpCnt>";
        datastring = datastring + "</StrobData>";
        Putln( f=fFile, s=datastring );
    	if FileState(fFile) > -1 then Close(fFile);
    end;
    else if (tSS.ok = false and TipDummyActive()=false and SimulationMode() = false) then begin
        fFile=OpenRW( filename=path + strobfilename, , append=true); 
        datastring = "<StbDt Batch=" + StringOf(assay) + "TS=" + JustTime + ">";        
        datastring = datastring + "<LN>" + StringOf(linecount) + "</LN>";
        datastring = datastring + "<Tip>" + StringOf(tip) + "</Tip>";
        datastring = datastring + "<AT>" + anotation + "</AT>";
        datastring = datastring + "<StrbCh>" + StringOf(i) + "</StrbCh>";       
        datastring = datastring + "<PosX>0</PosX>";
        datastring = datastring + "<PosY>0</PosY>";
        datastring = datastring + "<DpDia>0</DpDia>";
        datastring = datastring + "<Vol>0</Vol>";
        datastring = datastring + "<DflAng>0</DflAng>";
        datastring = datastring + "<SatAng>0</SatAng>";
        datastring = datastring + "<DpCnt>0</DpCnt>";
        datastring = datastring + "</StrobData>"; 
        Putln( f=fFile, s=datastring );
    	if FileState(fFile) > -1 then Close(fFile);
    end;

   TimerDelay( t=btdelay );
  end;                              
 end;
 
 le = 10000;
 gr = 0;
 for i=1 to chkcount do begin
    calc = calc + tSS.y[i];
    if tSS.y[i] > gr then gr = tSS.y[i];
    if tSS.y[i] < le then le = tSS.y[i];
 end; 
 
 tSS.avgy = (calc / chkcount);
 tSS.miny = le;
 tSS.maxy = gr;
 tSS.stdevy = (tSS.avgy - le) / tSS.avgy;
 tSS.coefy = (tSS.stdevy * 100);
 
 fFile=OpenRW( filename=path + strobfilename, , append=true); 
 
 datastring = "<AssayData TS=" + JustTime + ">";     
 datastring = datastring + "<LN>" + StringOf(linecount) + "</LN>";
 datastring = datastring + "<Tip>" + StringOf(tip) + "</Tip>";
 datastring = datastring + "<AT>" + anotation + "</AT>";
 datastring = datastring + "<AverageY>" + StringOf(tSS.avgy:0:3) + "</AverageY>";
 datastring = datastring + "<MinY>" + StringOf(tSS.miny:0:3) + "</MinY>"; 
 datastring = datastring + "<MaxY>" + StringOf(tSS.maxy:0:3) + "</MaxY>"; 
 datastring = datastring + "<StDevY>" + StringOf(tSS.stdevy:0:3) + "</StDevY>";
 datastring = datastring + "<CVY>" + StringOf(tSS.coefy:0:3) + "</CVY>";
 datastring = datastring + "</AssayData>";
 Putln( f=fFile, s=datastring );
 if FileState(fFile) > -1 then Close(fFile);
     
 Grab(0);

 StrobCheck = tSS;
 
end;

 
