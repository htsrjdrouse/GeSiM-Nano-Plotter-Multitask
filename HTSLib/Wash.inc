
//________________________________________________________//

function extrawash(extrawashvol:real,extrawashfl:real,extrapiezo:boolean,extrawashwell:integer,dwelltime:real):void;

begin


    if extrapiezo = true then begin
        PipetteStart( tip_list=[]);   
    end;
    PositionToObject( tip=1, object=extrawashwell );         
    PositionAbsoluteZ( z=GetZ( object=extrawashwell)-2, 10 );
    PositionAbsoluteZ( z=GetZ( object=extrawashwell), 3 );
    DiluterSetValvePipette( tip_list=[] );
    DiluterLoad( tip_list=[], volume=extrawashvol, flow=extrawashfl );
    PositionAbsoluteZ( z=GetZ( object=extrawashwell)-2, 3 );
    PositionAbsoluteZ( z=0, 10 );
    TimerDelay( t=dwelltime );
    if extrapiezo = true then begin
        PipetteStop( tip_list=[]);
    end;

end;


//________________________________________________________//

function Wash(washtime:real,drytime:real,
              extrawashvol:real,extrawashfl:real,extrapiezo:boolean,
              extrawashwell:integer,dwelltime:real,tiplist:integer):void;

begin

if tiplist > 0 then begin

 ///This was setting needs to be put in a function 
 if drytime = 0 and extrawashvol > 0 then begin
    PipetteWashDry( tip_list=tiplist, washTime=10, dryTime=drytime);   
    extrawash(extrawashvol,extrawashfl,extrapiezo,extrawashwell,dwelltime);
    PipetteWash( tip_list=tiplist, washTime=washtime);   
 end;
 if drytime = 0 and extrawashvol = 0 then begin
    PipetteWash( tip_list=tiplist, washTime=washtime);   
 end; 
 if drytime > 0 and extrawashvol = 0 then begin
    PipetteWashDry( tip_list=tiplist, washTime=washtime, dryTime=drytime);  
 end; 
 if drytime > 0 and extrawashvol > 0 then begin
    PipetteWashDry( tip_list=tiplist, washTime=10, dryTime=drytime);
    extrawash(extrawashvol,extrawashfl,extrapiezo,extrawashwell,dwelltime);
    PipetteWashDry( tip_list=tiplist, washTime=washtime, dryTime=drytime); 
 end; 

end; 
   
end; 