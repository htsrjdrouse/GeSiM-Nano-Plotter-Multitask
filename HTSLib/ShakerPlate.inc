function ShakerPlate(inputcommand:string):string;

var port:integer;
    outputstring:string;
    readchar,character:char;
    i,temp,readbytes,length:integer;
    s:string;
    text:string;
        
begin
 inputcommand=inputcommand + "\r";
 port = 7;
 text="G";
 length := StringLength( s=inputcommand);
 
 character=Chr(1);
 
 temp=RS232_Open( comPort=port, baudrate=9600, parity=0, dataBits=8, stopBits=1, timeout=2);
 
 if temp =0 then
  begin
    temp = RS232_WrtBuffer( comPort=port, buffer=inputcommand, num=length);
    TimerDelay( t=1,1 );

    readbytes := RS232_GetInQueueBytes( comPort=port);

    outputstring = "";
    for i:=0 to readbytes-1 do
        begin
        readchar := RS232_RdByte( comPort=port);
        outputstring = outputstring + readchar;
    end;
     
    RS232_Close( comPort=port);
  end;
  else
   begin
    Message("Com Port Error", "Com Port Error");
    outputstring = "Error";
   end;

 ShakerPlate = outputstring;

end;
 