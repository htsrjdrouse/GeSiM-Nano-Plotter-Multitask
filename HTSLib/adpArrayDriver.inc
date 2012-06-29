
//_______________________________________________________________//

function adpArrayDriver( dx:real,
                    numX:integer,
                    dy:real,
                    numY:integer,
                    drops:integer,
                    tiplist:integer):integer;

begin

 ADP_SetPara( 0, 0 ); //set defaults
 ADP_SetPara( paraType=1, paraValue=3 ); //Move Dir: deflt=1=Uni-Dir.Y; 2=Uni-Dir.X; 3=Bi-Dir.Y; 4=Bi-Dir.X
 ADP_SetPara( paraType=2, paraValue=2 ); //PosAdjust: deflt=0=No; 1=Line begin; 2=All Positions
 ADP_DispenseArray( numX=numX, numY=numY, dx=dx, dy=dy, tip_list=tiplist, droplets=drops);
 while ADP_GetState() > 0 do ;
 adpArrayDriver = 1;
 
end;

 
 
