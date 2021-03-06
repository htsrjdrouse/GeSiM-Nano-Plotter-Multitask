
//_______________________________________________________________//

function adpLineDriver( maxtip:integer,
                        bx:real,
                        by:real,
                        ex:real,
                        ey:real,
                        speed1:integer,
                        tiplist:integer):integer;

begin
               
 ADP_SetPara( 0, 0 ); //set defaults
 ADP_SetPara( paraType=1, paraValue=3 ); //Move Dir: deflt=1=Uni-Dir.Y; 2=Uni-Dir.X; 3=Bi-Dir.Y; 4=Bi-Dir.X
 ADP_SetPara( paraType=2, paraValue=1 ); //PosAdjust: deflt=0=No; 1=Line begin; 2=All Positions
 ADP_DispenseLine( refTip=tiplist, tipList=tiplist, x0=bx, y0=by, x1=ex, y1=ey, speed=speed1); 
 while ADP_GetState() > 0 do ;
 adpLineDriver = 1;
 
end;

 
