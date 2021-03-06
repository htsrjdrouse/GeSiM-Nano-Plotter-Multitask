
/*Note this is just a prototype specially designed to measure 1 drop using 1 tip */

const TPLT =      1024;    //  Spot on water sensitive paper


//For GetGrabberValue


type tSpotData=
    record
        ok:boolean;
        x,y,dm,ang,count,height,width,wetting:real;
    end;

function SpotQuant(sr:integer,sc:integer,br:integer,bc:integer,spr:integer,spc:integer,path:string,DateTime:string,anot:string,volt:real,pulse:real,ti:integer):tSpotData;

var i:integer;
    tSP:tSpotData;
    spotimage,datastring:string;
    d,t,JustTime:string;
    x,y,dp:real;
    sFile:file;
    filename,spotfile:string;


begin
    SetVideoSource( source=8);
    SetLiveVideo(on=true);
    spotfile = "spotimage"+DateTime;
    PositionToBlock( tip=1, object=WSP, blockRow=br, blockColumn=bc, spotRow=spr, spotColumn=spc );
    PositionAbsoluteZ( z=(GetZ( object=WSP)-3),40 );
    PositionAbsoluteZ( z=(GetZ( object=WSP)-0.5),3 );
    PipetteDispense( tip_list=1, droplets=1 );
    PositionAbsoluteZ( z=0 );    
    PositionToBlock( tip=-1, object=WSP, blockRow=br, blockColumn=bc, spotRow=spr, spotColumn=spc );    
    SetLiveVideo(on=false);
    Grab(TPLT);
    tSP.ok=FindDots(TPLT);
    spotimage = "maskSR"+sr+"SC"+sc+"BR"+br+"BC"+bc+"SPR"+spr+"SPC"+spc;
    SaveBitmap( filename=path + spotimage + ".bmp", bUseFileDialog=false, bBitmapOnly=false );
    spotimage = "SR"+sr+"SC"+sc+"BR"+br+"BC"+bc+"SPR"+spr+"SPC"+spc;
    SaveBitmap( filename=path + spotimage + ".bmp", bUseFileDialog=false, bBitmapOnly=true );
    
    Writeln(volt);
    sFile = OpenRW( filename=path+spotfile+".txt", , append=true);

    
    if tSP.ok = true then begin
        tSP.count = GetDotCount( finderId=TPLT);
        tSP.x=GetGrabberValue( finderId=TPLT, index=POS_X);
        tSP.y=GetGrabberValue( finderId=TPLT, index=POS_Y);             
        tSP.dm=GetGrabberValue( finderId=TPLT, index=DIAMETER);
        tSP.ang=GetGrabberValue( finderId=TPLT, index=DIAMETER);
        	datastring = "<SpotData>";
        datastring = datastring + "<I>" + StringOf(ti) + "</I>";
        datastring = datastring + "<SR>" + StringOf(sr) + "</SR>";
        datastring = datastring + "<SC>" + StringOf(sc) + "</SC>";
        datastring = datastring + "<I>" + StringOf(ti) + "</I>";
        datastring = datastring + "<V>" + StringOf(volt:0:3) + "</V>";
        datastring = datastring + "<L>" + StringOf(pulse:0:3) + "</L>";
        datastring = datastring + "<AT>" + anot + "</AT>";         
        datastring = datastring + "<SX>" + StringOf(tSP.x) + "</SX>";
        datastring = datastring + "<SY>" + StringOf(tSP.y) + "</SY>";
        datastring = datastring + "<DP>" + StringOf(tSP.dm) + "</DP>";
        datastring = datastring + "<CT>" + StringOf(tSP.count) + "</CT>";
        datastring = datastring + "</SpotData>";       
    end;
    else begin
        datastring = "<SpotData>";   
        datastring = datastring + "<I>" + StringOf(ti) + "</I>";
        datastring = datastring + "<SR>" + StringOf(sr) + "</SR>";
        datastring = datastring + "<SC>" + StringOf(sc) + "</SC>";
        datastring = datastring + "<V>" + StringOf(volt:0:3) + "</V>";
        datastring = datastring + "<L>" + StringOf(pulse:0:3) + "</L>";
        datastring = datastring + "<AT>" + anot + "</AT>";     
        datastring = datastring + "<SX>" + StringOf(0) + "</SX>";
        datastring = datastring + "<SY>" + StringOf(0) + "</SY>";
        datastring = datastring + "<DP>" + StringOf(0) + "</DP>";
        datastring = datastring + "<CT>" + StringOf(0) + "</CT>";
        datastring = datastring + "</SpotData>";
    end;
    Putln( f=sFile, s=datastring );
    if FileState(sFile) > -1 then Close(sFile);

    SpotQuant = tSP;

end;

 
