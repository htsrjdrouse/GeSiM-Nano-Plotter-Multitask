
#include "../HTSLib/adpArrayDriver.inc"
#include "../HTSLib/sofArrayDriver.inc"
#include "../HTSLib/adpLineDriver.inc"

const PCAM =    256;    //  Cross hairs for camera positioning

type tDOTipSet=
    record
     volt,pulse,freq:array[1..16] of real;
     drop:array[1..16] of integer;    
    end;

//_______________________________________________________________//

function CrossHairs(enable: boolean = true): void;

begin

    if enable then 
        FindDots(finderId = PCAM);
    else
        Grab(0);
        
end;





function DriveOptionCommaCounter(line:string):integer;

var len,i,count:integer;
    z:string;

begin
 count=1;
 for i=0 to StringLength(line) do begin
    z=Substring(line,i,i+1);
    if FindString(sToSearch=z, sToFind=",") > -1 then begin
        count=count+1;
    end;
 end;
 DriveOptionCommaCounter = count;
end;

//_______________________________________________________________//

function DriveOptionSetTips(maxtip:integer,tTP:tTransParse):tDOTipSet;

var i:integer;
    tTS:tDOTipSet;

begin

 for i=1 to maxtip do begin

    if tTP.volt[i] > 0 then begin 
        tTS.volt[i] = tTP.volt[i];
        PipetteSetVolt( tip_list=[i], volt=Int(tTS.volt[i]));
    end;
    else tTS.volt[i] = GetSystemValue( infoIndex=1, partIndex=i, partInfoIndex=7);      
    if tTP.freq[i] > 0 then begin 
        tTS.freq[i] = tTP.freq[i]; 
        PipetteSetFrequency( tip_list=[i], f=Int(tTS.freq[i]));
    end;
    else tTS.freq[i] = GetSystemValue( infoIndex=1, partIndex=i, partInfoIndex=8);        
    if tTP.pulse[i] > 0 then begin
        tTS.pulse[i] = tTP.pulse[i];
        PipetteSetTPulse( tip_list=[i], t=Int(tTS.pulse[i]));
    end;
    else tTS.pulse[i] = GetSystemValue( infoIndex=1, partIndex=i, partInfoIndex=9);
    if tTP.drop[i] > 0 then begin 
        tTS.drop[i] = Int(tTP.drop[i]);
        PipetteSetDroplets( tip_list=[i], droplets=Int(tTS.drop[i]));
    end;

 end;


DriveOptionSetTips = tTS;

end;

//__________________________________________________________//





//_______________________________________________________________//

function DriveOption(tTP:tTransParse,maxtip:integer,tiplist:integer,x:real,y:real,tarid:integer,zdispoffset:real,zmoveoffset:real,path:string):void;

var ai,ci,totalcoords:integer;
    center_x_offset,olinex,oliney,elinex,eliney:real;
    d,t,DateTimecam,pictname:string;
    tTS:tDOTipSet; 
     
begin

 //This sets the nozzle paramters
  tTS=DriveOptionSetTips(maxtip,tTP);       

    if tTP.adp = 1 then begin //for ADParray for centering array from offset location   
     totalcoords = DriveOptionCommaCounter(tTP.dxstr); 
     for ci=1 to totalcoords do begin 
       if tTP.numX[ci]>1 then center_x_offset=(tTP.numX[ci]-1)*tTP.dx[ci]/2 else center_x_offset=0;               
        //move to target group dispense position
        SetVideoSource( source=1);  // Head camera  
        PositionAbsoluteXY( tip=maxtip, x=x-center_x_offset, y=y-tTP.y_offset[ci] );
        if tTP.zhsubstrate = 0 then PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset, 12);    
        else PositionAbsoluteZ( z=tTP.zhsubstrate-zdispoffset, 12);
        //Start dispensing
        adpArrayDriver( tTP.dx[ci],Int(tTP.numX[ci]),-(tTP.dy[ci]),Int(tTP.numY[ci]),Int(tTP.drop[ci]),tiplist); 
       end; 
    end;

    if tTP.adp = 6 then begin //for ADParray using offset positioning   
     totalcoords = DriveOptionCommaCounter(tTP.dxstr); 
     for ci=1 to totalcoords do begin               
        //move to target group dispense position
        SetVideoSource( source=1);  // Head camera  
        PositionAbsoluteXY( tip=maxtip, x=x+tTP.x_offset[ci], y=y-tTP.y_offset[ci] );
        if tTP.zhsubstrate = 0 then PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset, 12);    
        else PositionAbsoluteZ( z=tTP.zhsubstrate-zdispoffset, 12);
        //Start dispensing
        //adpArrayDriver( tTP.dx[ci],Int(tTP.numX[ci]),-(tTP.dy[ci]),Int(tTP.numY[ci]),Int(tTP.drop[c1]),tiplist); 
        adpArrayDriver( tTP.dx[ci],Int(tTP.numX[ci]),-(tTP.dy[ci]),Int(tTP.numY[ci]),0,tiplist); 
       end; 
    end;
          
    if tTP.adp = 2 then begin //for ADPline
     totalcoords = DriveOptionCommaCounter(tTP.bxstring);
     for ci=1 to totalcoords do begin                   
      //move to target group dispense position
      SetVideoSource( source=1);  // Head camera   
      PositionAbsoluteXY( tip=maxtip, x=x+tTP.x_offset[ci], y=y-tTP.y_offset[ci] );
      if tTP.zhsubstrate = 0 then PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset, 12);    
      else PositionAbsoluteZ( z=tTP.zhsubstrate-zdispoffset, 12);
      olinex = x+tTP.x_offset[ci];
      oliney = y-tTP.y_offset[ci];
      elinex = (x+tTP.x_offset[ci])+tTP.lx[ci];
      eliney = (y-tTP.y_offset[ci])+tTP.ly[ci];  
//Message("lines", "olinex = "+olinex + "\noliney = "+oliney + "\nelinex = "+elinex + "\neliney = "+eliney);

      if SimulationMode() = false then begin                                   
       adpLineDriver(maxtip,(olinex),(oliney),(elinex),(eliney),Int(tTP.speed[ci]),tiplist); 
      end;           
     end;
    end;

//adp=3 does 2 distinct adp patterns for ADParray for centering array from offset location
    if tTP.adp = 3 then begin //for offset ADParray 
     totalcoords = DriveOptionCommaCounter(tTP.dxstr);
     for ci=1 to totalcoords do begin 
        if tTP.numX[ci]>1 then center_x_offset=(tTP.numX[ci]-1)*tTP.dx[ci]/2 else center_x_offset=0;           
        //move to target group dispense position
        SetVideoSource( source=1);  // Head camera  
        PositionAbsoluteXY( tip=maxtip, x=x-center_x_offset, y=y-tTP.y_offset[ci] );
        if tTP.zhsubstrate = 0 then PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset, 12);    
        else PositionAbsoluteZ( z=tTP.zhsubstrate-zdispoffset, 12);
        //Start dispensing
        adpArrayDriver( tTP.dx[ci],Int(tTP.numX[ci]),-(tTP.dy[ci]),Int(tTP.numY[ci]),Int(tTP.drop[ci]),tiplist);
        PositionAbsoluteXY( tip=maxtip, x=x-center_x_offset+tTP.dx[ci]/2, y=y-tTP.y_offset[ci]-tTP.dy[ci]/2 );
        adpArrayDriver( tTP.dx[ci],Int(tTP.numX[ci]-1),-(tTP.dy[ci]),Int(tTP.numY[ci]-1),Int(tTP.drop[ci]),tiplist);
     end;
    end;

    if tTP.adp = 4 then begin  //single spot command
     totalcoords = DriveOptionCommaCounter(tTP.x_offsetstr);
     for ci=1 to totalcoords do begin    
      //move to target group dispense position
      SetVideoSource( source=1);  // Head camera  
      PositionAbsoluteXY( tip=maxtip, x=x+tTP.x_offset[ci], y=y-tTP.y_offset[ci] ); 
      if tTP.zhsubstrate = 0 then PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset, 12);    
      else PositionAbsoluteZ( z=tTP.zhsubstrate-zdispoffset, 12);
      PipetteDispense( tip_list=tiplist, droplets=Int(tTP.drop[ci]) );    
     end;
    end;

    if tTP.adp = 40 then begin  //single spot command
     totalcoords = DriveOptionCommaCounter(tTP.x_offsetstr);
     for ci=1 to totalcoords do begin    
      //move to target group dispense position
      SetVideoSource( source=8);  // video microscope  
      CrossHairs(true);

      PositionAbsoluteZ( z=0 );  
      PositionAbsoluteXY( tip=-1, x=x+tTP.x_offset[ci], y=y-tTP.y_offset[ci] ); 
      d = Date();
      t = Time();
      DateTimecam=Substring(d,6,10)+Substring(d,0,2)+Substring(d,3,5)+ "_"+Substring(t,0, 2)+Substring(t,3,5)+Substring(t,6,8);
      pictname = path + "/" + DateTimecam + "_" + tTP.anot + "vid.bmp";
      SetLiveVideo( on=false);   
      SaveBitmap( filename=pictname, , bBitmapOnly=false );
      SetVideoSource( source=1); 
      SetLiveVideo( on=true); 
     end;
    end;

//Absolute positioning -- the way to go for video targeting

    if tTP.adp = 7 then begin
     totalcoords = DriveOptionCommaCounter(tTP.xposstring); 
     for ci=1 to totalcoords do begin   
      //move to target group dispense position
      SetVideoSource( source=1);  // Head camera 
      PositionAbsoluteXY( tip=maxtip, x=tTP.xpos[ci], y=tTP.ypos[ci], ); 
      if tTP.zhsubstrate = 0 then PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset, 40);    
      else PositionAbsoluteZ( z=tTP.zhsubstrate-zdispoffset, 40);
      PipetteDispense( tip_list=tiplist, droplets=Int(tTP.drop[ci]) );    
     end;
    end;




    if tTP.adp = 77 then begin
     totalcoords = DriveOptionCommaCounter(tTP.xposstring); 
     for ci=1 to totalcoords do begin   
      //move to target group dispense position
      SetVideoSource( source=8);  // video microscope  
      CrossHairs(true);
      PositionAbsoluteZ(0);
      PositionAbsoluteXY( tip=-1, x=tTP.xpos[ci], y=tTP.ypos[ci], ); 
      d = Date();
      t = Time();
      DateTimecam=Substring(d,6,10)+Substring(d,0,2)+Substring(d,3,5)+ "_"+Substring(t,0, 2)+Substring(t,3,5)+Substring(t,6,8);
      pictname = path + "/" + DateTimecam + "_" + tTP.anot + "vid.bmp";
      SetLiveVideo( on=false);   
      SaveBitmap( filename=pictname, , bBitmapOnly=false );
      CrossHairs(true);
      //SetVideoSource( source=1); 
      //SetLiveVideo( on=true); 
     end;
    end;





end;

 
