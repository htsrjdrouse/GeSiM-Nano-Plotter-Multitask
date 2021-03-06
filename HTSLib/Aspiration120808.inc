//_______________________________________________________________//

function Aspiration(maxtip:integer,
                    delay:real,
                    row:integer,
                    col:integer,
                    aspfl:real,
                    aspvol:real,
                    delaytime:real,
                    bfdrypadtime:real,
                    drypadtime:real,
                    piezo:boolean,
                    piezopulse:real,
                    piezovolt:real,
                    piezofreq:real,
                    DateTime:string,
                    tiplist:integer,
                    path:string,
                    filename:string):void;
                 
var datastring:string;
    i:integer;
    tpl:integer;
    aspimage:string;
    fFile:file;
    d,t,JustTime:string;
                                        
begin


 d = Date();
 t = Time();
 JustTime=Substring(t,0, 2)+Substring(t,3,5)+Substring(t,6,8);

 if drypadtime > 0 then begin
     PipetteDry( dryTime=bfdrypadtime);
 end;  
 PositionToObject( tip=maxtip, object=MTP_Sys, row=row, column=col );
 PositionAbsoluteZ( z=GetZ( object=MTP_Sys)-2, 10 );
 PositionAbsoluteZ( z=GetZ( object=MTP_Sys), 3 );

 DiluterSetValveSystemFluid( tip_list=tiplist );
 DiluterEmpty( tip_list=tiplist, flow=50 );   
 DiluterSetValvePipette( tip_list=tiplist );
 
    if piezo=true then begin
        PipetteSetVolt( tip_list=tiplist, volt=piezovolt);
        PipetteSetFrequency( tip_list=tiplist, f=piezofreq);
        PipetteSetTPulse( tip_list=tiplist, t=piezopulse);        
        PipetteStart( tip_list=tiplist);
        DiluterLoad( tip_list=tiplist, volume=aspvol, flow=aspfl );
        PipetteStop( tip_list=tiplist);
    end;
    else DiluterLoad( tip_list=tiplist, volume=aspvol, flow=aspfl );
    
 TimerDelay( t=delaytime );

 DiluterSetValveBypass( tip_list=tiplist );
 
 PositionAbsoluteZ( z=GetZ( object=MTP_Sys)-2, 3 );
 PositionAbsoluteZ( z=0, 10 );

 if drypadtime > 0 then begin
     PipetteDry( dryTime=drypadtime);
 end;  
  
end;
 