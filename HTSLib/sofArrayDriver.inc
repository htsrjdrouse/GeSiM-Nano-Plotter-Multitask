
//_______________________________________________________________//

function sofArrayDriver(refTip:integer,
			tipMask:integer,
			droplets:integer,
			x0:real,
			y0:real,
			dx:real,
			dy:real,
			nx:integer,
			ny:integer,
			speed:integer,
			xdir:integer):integer;	
	
var moveMode:integer;

begin

 moveMode := 0;
 moveMode := moveMode + 1; //  Start from: 0=TopRight, 1=TopLeft, 2=BottomRight, 3=BottomLeft
 moveMode := moveMode + 4; //  Flag: 0=Uni-, 1=Bi-Dir.(Meander)
 //if ny > 1 then moveMode := moveMode + 16;  //  Flag: 0=X ;  +16->Y is spotting line


SOF_Array( refTip=refTip,
  tipMask=tipMask,
  droplets=droplets,
  moveMode=moveMode,
  x0=x0,
  y0=y0,
  dx=dx,
  dy=dy,
  nX=nx,
  nY=ny,
  speed=speed);


 sofArrayDriver = 1;
 TimerWaitWhileBusy();
 
end;

 
 