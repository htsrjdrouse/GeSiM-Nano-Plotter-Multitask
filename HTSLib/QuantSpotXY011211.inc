
/*Note this is just a prototype specially designed to measure 1 drop using 1 tip */

const SPOT =      2;    //  Spot on water sensitive paper
const TPLT =      1024;    //  Spot on water sensitive paper
const BMRT =   2048;    //  Dot Array Bottom Right
const TPRT =  32768;    //  Dot Array Top Right
const BMLT =  65536;    //  Dot Array Bottom Left

//const PCAM = 	  256;  //For camera positioning

//For GetGrabberValue


type tSpotGrabData=
    record
        pos:tPoint;
        postr:string;
        count:integer;
    end;

type tTPLTGrabData=
    record
        ok:boolean;
        x,y,dm,ang,count,height,width,area,wetting:real;
	postr:string;
    end;


/*
function CrossHairs(enable: boolean = true): void;
begin
    if enable then 
        FindDots(finderId = PCAM);
    else
        Grab(0);
end;
*/




//This isn't working for some reason    
function spotgrabber(delay:integer,switch:integer):tSpotGrabData;

var tSS:tSpotGrabData;
    pos:tPoint;
    j:integer;
    postr:string;
    
begin
	if switch == 0 then begin
    	  TimerDelay(t = delay);
      	  GrabBackground(finderId = SPOT);
	  tSS.postr = '';
        end;
 	if switch == 1 then begin  //begin switch loop
    	  TimerDelay(t = delay);
	  Grab(SPOT);
  
          tSS.count = Int(GetDotCount( finderId=SPOT));
	  postr = '';
          for j=1 to Int(tSS.count) do begin
            pos = GetFoundedDot(finderId = SPOT, dotNumber = j); 
            postr = postr+j+"x"+StringOf(pos.x:0:3)+"y"+StringOf(pos.y:0:3);
          end;
	  Writeln("testing " +postr);
          tSS.postr = 'pos_' + postr;   
 	end; //end switch loop
        
        spotgrabber = tSS;
end;


function tpltgrabber(delay,tSS:tSpotGrabData):tTPLTGrabData;

var	tSP:tTPLTGrabData;
        x,y,dm,ang,count,height,width,wetting:real;
	cnt,i:integer;
        pos:tPoint;
	postr:string;

begin
    	TimerDelay(t = delay);
    	Grab(BMLT);
    	tSP.ok=FindDots(BMLT); 
	postr = '';
	if tSP.ok == true then begin
	  cnt = GetDotCount( finderId=BMLT);
	  for i=1 to cnt do begin
	   pos = GetFoundedDot( finderId=BMLT, dotNumber=i);
           postr = postr+i+"x"+StringOf(pos.x:0:3)+"y"+StringOf(pos.y:0:3);
	  end;
	end;      
        tSP.postr = postr;
	tpltgrabber = tSP;
end;


function SpotQuantXY(sr:integer,sc:integer,br:integer,bc:integer,xpos:real,ypos:real,path:string,DateTime:string,anot:string,volt:real,pulse:real,ti:integer,anotdes:string,tarwsp:string):void;

var i:integer;
    tSS:tSpotGrabData;
    tSP:tTPLTGrabData;
    spotimage,spotimagedata,datastring:string;
    d,t,JustTime:string;
    x,y,dp:real;
    sFile:file;
    filename,spotfile:string;
    delay:integer;
    targpid,tarid:integer;


begin
    SetVideoSource( source=8);
    CrossHairs(true);
    SetLiveVideo(on=true);
    PositionAbsoluteZ(z = 0.0);
    delay = 2;
    
    targpid = GetObjId( name=tarwsp);
    tarid = GetTargetId( target_group_id=targpid, row=1, column=1);

    PositionAbsoluteXY( tip=1, x=xpos, y=ypos, );
    PositionAbsoluteZ( z=(GetZ( object=tarid)-3),40 );
    PositionAbsoluteZ( z=(GetZ( object=tarid)-0.5),40 );
    PipetteDispense( tip_list=1, droplets=3 );
    PositionAbsoluteXY( tip=1, x=(xpos+0.2), y=ypos, );
    PipetteDispense( tip_list=1, droplets=3 );
    PositionAbsoluteXY( tip=1, x=(xpos+0.4), y=ypos, );
    PipetteDispense( tip_list=1, droplets=3 );

    PositionAbsoluteZ( z=0 );    
    PositionAbsoluteXY( tip=-1, x=xpos, y=ypos );

    SetLiveVideo(on=false);
    tSP = tpltgrabber(delay,tSS);
    SetLiveVideo(on=true);
    
    spotfile = "spotimagedata";
    sFile = OpenRW( filename=path+spotfile+".txt", , append=true);

    spotimagedata = anot+"_"+anotdes+"_br"+br+"bc"+bc;
    spotimage = spotimagedata + "my";
    SaveBitmap( filename=path + spotimage + ".bmp", bUseFileDialog=false, bBitmapOnly=false );
    spotimage = spotimagedata + "mn";
    SaveBitmap( filename=path + spotimage + ".bmp", bUseFileDialog=false, bBitmapOnly=true );

    if tSP.ok == true then begin
       	datastring = "<SpotData>";
        datastring = datastring + "<POS>" + tSP.postr + "</POS>";
        datastring = datastring + "</SpotData>";       
    end;
    else begin
        datastring = "<SpotData>";   
        datastring = datastring + "0";
        datastring = datastring + "</SpotData>";
    end;
    Putln( f=sFile, s=datastring );
    if FileState(sFile) > -1 then Close(sFile);


end;

 
