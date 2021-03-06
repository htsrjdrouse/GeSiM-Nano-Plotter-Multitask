

//<A>10</A> option
//function DriveOption(tTP:tTransParse,maxtip:integer,tiplist:integer,x:real,y:real,tarid:integer,zdispoffset:real,zmoveoffset:real,path:string):void;
//This is not for piezo dispensing ... I will rewrite this for that purpose

function GDrive(gc:string,
		gcp:integer,
		gco:integer,
		gcf:integer,
		gcd:integer,
		gcx:real,
		gcy:real,
		gcz:real
		):void;


var cmd:string;
  t:real;

begin

  if FindString(sToSearch=gc, sToFind="M106") > -1 then begin
   cmd = 'python connect.py '+StringOf(gcp)+' '+StringOf(gco);
   TimerWaitWhileBusy();
   Exec(cmd,wait=1); 
  end;
  if FindString(sToSearch=gc, sToFind="M107") > -1 then begin
   cmd = 'python connect.py '+StringOf(gcp)+' '+StringOf(gco);
   TimerWaitWhileBusy();
   Exec(cmd, wait=1); 
  end;
  if FindString(sToSearch=gc, sToFind="M126") > -1 then begin
   cmd = 'python connect.py '+StringOf(gcp)+' '+StringOf(gco);
   TimerWaitWhileBusy();
   Exec(cmd, wait=1); 
  end;
  if FindString(sToSearch=gc, sToFind="M127") > -1 then begin
   cmd = 'python connect.py '+StringOf(gcp)+' '+StringOf(gco);
   TimerWaitWhileBusy();
   Exec(cmd, wait=1); 
  end;
  if FindString(sToSearch=gc, sToFind="G4") > -1 then begin
   t = Int(gcd)/1000;
   TimerWaitWhileBusy();
   TimerDelay(Int(t));
  end;
  if FindString(sToSearch=gc, sToFind="G1") > -1 then begin
   TimerWaitWhileBusy();
   PositionAbsoluteXY( tip=1, x=gcx, y=gcy, speed=gcf, wait=0);
   TimerWaitWhileBusy();
   PositionAbsoluteZ(gcz);
   TimerWaitWhileBusy();
  end;
end;

 
