
/*Note this is just a prototype specially designed to measure 1 drop using 1 tip */

const SPOT =      2;    //  Spot on water sensitive paper
const TPLT =      1024;    //  Spot on water sensitive paper


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
          tSS.postr = 'pos_' + postr;   
 	end; //end switch loop
        
        spotgrabber = tSS;
end;


function tpltgrabber(delay,tSS:tSpotGrabData):tTPLTGrabData;

var	tSP:tTPLTGrabData;
        x,y,dm,ang,count,height,width,wetting:real;

begin
    	TimerDelay(t = delay);
    	Grab(TPLT);
    	tSP.ok=FindDots(TPLT); 

	if tSP.ok == true then begin
  	 tSP.x=GetGrabberValue( finderId=TPLT, index=POS_X);
         tSP.y=GetGrabberValue( finderId=TPLT, index=POS_Y);             
         tSP.dm=GetGrabberValue( finderId=TPLT, index=DIAMETER);
         tSP.count=GetGrabberValue( finderId=TPLT, index=COUNT);
         tSP.ang=GetGrabberValue( finderId=TPLT, index=ANGLE);
         tSP.height=GetGrabberValue( finderId=TPLT, index=HEIGHT);
         tSP.width=GetGrabberValue( finderId=TPLT, index=WIDTH);
         tSP.area=GetGrabberValue( finderId=TPLT, index=AREA);
 	end; //end switch loop

	tpltgrabber = tSP;
end;


function SpotQuant(sr:integer,sc:integer,br:integer,bc:integer,spr:integer,spc:integer,path:string,DateTime:string,anot:string,volt:real,pulse:real,ti:integer,anotdes:string,tarwsp:integer):void;

var i:integer;
    tSS:tSpotGrabData;
    tSP:tTPLTGrabData;
    spotimage,spotimagedata,datastring:string;
    d,t,JustTime:string;
    x,y,dp:real;
    sFile:file;
    filename,spotfile:string;
    delay:integer;


begin
    SetVideoSource( source=8);
    SetLiveVideo(on=true);
    PositionAbsoluteZ(z = 0.0);
    delay = 2;

    /*
    SetLiveVideo(on=false);
    PositionToBlock( tip=-1, object=WSP, blockRow=br, blockColumn=bc, spotRow=spr, spotColumn=spc );
    spotimagedata = anot+"_"+anotdes+"_srsc"+sr+sc+"brbc"+br+bc+"crcc"+spr+spc+"V"+StringOf(Int(volt))+"P"+StringOf(Int(pulse));
    spotimage = spotimagedata + "mp";
    SaveBitmap( filename=path + spotimage + ".bmp", bUseFileDialog=false, bBitmapOnly=true );
    SetLiveVideo(on=true);
    */


    PositionToBlock( tip=1, object=tarwsp, blockRow=br, blockColumn=bc, spotRow=spr, spotColumn=spc );
    PositionAbsoluteZ( z=(GetZ( object=tarwsp)-3),40 );
    PositionAbsoluteZ( z=(GetZ( object=tarwsp)-0.5),3 );
    PipetteDispense( tip_list=1, droplets=1 );
    PositionAbsoluteZ( z=0 );    
    PositionToBlock( tip=-1, object=tarwsp, blockRow=br, blockColumn=bc, spotRow=spr, spotColumn=spc );    

    /*
    SetLiveVideo(on=false);
    tSS = spotgrabber(delay,1);
    SetLiveVideo(on=true);
    */

    SetLiveVideo(on=false);
    tSP = tpltgrabber(delay,tSS);
    SetLiveVideo(on=true);
    
    spotfile = "spotimagedata";
    sFile = OpenRW( filename=path+spotfile+".txt", , append=true);

    spotimagedata = anot+"_"+anotdes+"_srsc"+sr+sc+"brbc"+br+bc+"crcc"+spr+spc+"V"+StringOf(Int(volt))+"P"+StringOf(Int(pulse));
    spotimage = spotimagedata + "my";
    SaveBitmap( filename=path + spotimage + ".bmp", bUseFileDialog=false, bBitmapOnly=false );
    spotimage = spotimagedata + "mn";
    SaveBitmap( filename=path + spotimage + ".bmp", bUseFileDialog=false, bBitmapOnly=true );


    
    if tSP.ok == true then begin
       	datastring = "<SpotData>";
        datastring = datastring + "<I>" + StringOf(ti) + "</I>";
        datastring = datastring + "<SR>" + StringOf(sr) + "</SR>";
        datastring = datastring + "<SC>" + StringOf(sc) + "</SC>";
        datastring = datastring + "<I>" + StringOf(ti) + "</I>";
        datastring = datastring + "<V>" + StringOf(volt:0:3) + "</V>";
        datastring = datastring + "<L>" + StringOf(pulse:0:3) + "</L>";
        datastring = datastring + "<AT>" + anot + "</AT>";         
        datastring = datastring + "<DP>" + StringOf(tSP.dm:0:3) + "</DP>";
        datastring = datastring + "<X>" + StringOf(tSP.x:0:3) + "</X>";
        datastring = datastring + "<Y>" + StringOf(tSP.y:0:3) + "</Y>";
        datastring = datastring + "<CT>" + StringOf(tSP.count:0:3) + "</CT>";
        datastring = datastring + "<HT>" + StringOf(tSP.height:0:3) + "</HT>";
        datastring = datastring + "<WD>" + StringOf(tSP.width:0:3) + "</WD>";
        datastring = datastring + "<AR>" + StringOf(tSP.area:0:3) + "</AR>";
        //datastring = datastring + "<POS>" + tSP.postr + "</POS>";
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
        datastring = datastring + "<DP>" + StringOf(0) + "</DP>";
        datastring = datastring + "<CT>" + StringOf(0) + "</CT>";
        datastring = datastring + "</SpotData>";
    end;
    Putln( f=sFile, s=datastring );
    if FileState(sFile) > -1 then Close(sFile);


end;

 