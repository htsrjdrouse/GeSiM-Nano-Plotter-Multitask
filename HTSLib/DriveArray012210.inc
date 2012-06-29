
#include "../HTSLib/adpArrayDriver.inc"


type tTipSet=
    record
     volt,pulse,freq:array[1..16] of real;
     drop:array[1..16] of integer;    
    end;

//__________________________________________________________//

function DriveArrayCommaCounter(line:string):integer;

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
 DriveArrayCommaCounter = count;
end;

//__________________________________________________________//


function SpotState(o:integer,br:integer,bc:integer,sr:integer,sc:integer,maxtip:integer,tpl:array[1..16] of boolean,spr:integer,spc:integer,tTP:tTransParse):void;

var tipspace:real;
    dx,dy:real;
    difx,dify,posblrow,posblcol,tl:integer;
    well:string;
    maxx,maxy,tx,ty,spdiv:real;
    wx,wy,bx,by,poscol,posrow,msr,msc:integer;
begin

 maxx = GetTipX( tip=maxtip);
 maxy = GetTipY( tip=maxtip);

 if maxtip > 1 then begin
  tipspace = GetSystemValue( infoIndex=1, partIndex=2, partInfoIndex=2) - GetSystemValue( infoIndex=1, partIndex=1, partInfoIndex=2);  
 end;
 
 if tipspace > 4.5 then tipspace = 9;
 if tipspace < 6 and tipspace > 3 then tipspace =4.5; 
 if tipspace < 3 then tipspace = 1;

 for tl=1 to maxtip do begin
 
    tx = GetTipX( tip=tl) - maxx;
    ty = maxy - GetTipY( tip=tl);

    if tTP.ssbct = 1 then begin 
        posblcol = bc;
        msc = Int(sc + ((tx / tipspace)*tTP.cec[1]));  
    end;
    else begin    
        posblcol = Int(bc + (tx / tipspace));
        msc = sc;
    end;
  
    if tTP.ssbrt = 1 then begin  
        posblrow = br;
        msr = Int(sr + ((ty / tipspace)*tTP.cer[1])); 
    end;
    else begin
        posblrow = Int(br + (ty / tipspace));
        msr = sr;    
    end;
    
    if GetColumns( object=MTP_Sys)==24 then spdiv = 4.5;
    if GetColumns( object=MTP_Sys)==12 then spdiv = 9;    
    
    poscol = Int(spr + (tx / spdiv));
    posrow = Int(spc + (ty / spdiv));  
    well = "S" + StringOf(posrow) + "," + StringOf(poscol);   
                                                                                                                                                       
    if tpl[tl] = true then begin
     SetState( object=o, state=2, row=msr, column=msc, blockRow=posblrow, blockCol=posblcol );
     SetText( object=o, text=well, row=msr, column=msc, blockRow=posblrow, blockCol=posblcol);                                                                        
    end;                                    
    else begin
     SetState( object=o, state=6, row=msr, column=msc, blockRow=posblrow, blockCol=posblcol );
     SetText( object=o, text=well, row=msr, column=msc, blockRow=posblrow, blockCol=posblcol);                 
    end;                                                                                                                        
 end;

end;


//__________________________________________________________//

function DriveArraySetTips(maxtip:integer,tTP:tTransParse):tTipSet;

var i:integer;
    tTS:tTipSet;

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


DriveArraySetTips = tTS;

end;

//__________________________________________________________//


function DriveArray(tarname:string,maxtip:integer,tiplist:integer,tTP:tTransParse,zdispoffset:real,zmoveoffset:real,SCD:array[1..16] of boolean,spr:integer,spc:integer):void;

var tarid:integer;
    br,tr,bc,tc,sr,sc:integer;
    mtc,mbc,msc:integer;
    zheightmove:real;
    tTS:tTipSet;
    dx,dy,x,y:real;
    numX,numY:integer;
    totalcoords,ti:integer;
begin

 tTS=DriveArraySetTips(maxtip,tTP);       
 zheightmove = 0;

if tTP.da = 0 then begin

//Message("tTP.tec[1]", tTP.tec[1]);
//Message("tTP.tbc[1]", tTP.tbc[1]);

if (tTP.bec[1] < tTP.bbc[1]) or (tTP.tec[1] < tTP.tbc[1]) then begin
 for br=Int(tTP.bbr[1]) to Int(tTP.ber[1]) do begin
 for tr=Int(tTP.tbr[1]) to Int(tTP.ter[1]) do begin
 for tc=Int(tTP.tec[1]) to Int(tTP.tbc[1]) do begin
   if tTP.tec[1] < tTP.tbc[1] then begin
     mtc = Int(tTP.tbc[1])-(tc-Int(tTP.tec[1]));
     //Message("tTP.tbc[1]", tTP.tbc[1]);
     //Message("tc", tc);
     //Message("mtc", mtc);
   end;
   else mtc = tc;
   tarid = GetTargetId( target_group_id=GetObjId( name=tarname), row=tr, column=mtc);
 for bc=Int(tTP.bec[1]) to Int(tTP.bbc[1]) do begin
   if tTP.bec[1] = tTP.bbc[1] then mbc = bc;
   else mbc = Int(tTP.bbc[1])-Int(bc-tTP.bec[1]);
 for sr=Int(tTP.cbr[1]) to Int(tTP.cer[1]) do begin
 for sc=Int(tTP.cec[1]) to Int(tTP.cbc[1]) do begin
   msc = Int(tTP.cbc[1])-(sc-Int(tTP.cec[1]));
   PositionToBlock( tip=maxtip, object=tarid, blockRow=br, blockColumn=mbc, spotRow=sr, spotColumn=msc );
   PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset, 12);   
   PipetteDispense( tip_list=tiplist, droplets=0 );
   SpotState(tarid,br,mbc,sr,msc,maxtip,SCD,spr,spc,tTP);
 end;
 end;
 PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset-zmoveoffset, 12);
 end;
 end; 
 end;
 end;

end; //End left to right direction 

else begin //Begin right to left - "standard direction"

 for br=Int(tTP.bbr[1]) to Int(tTP.ber[1]) do begin
 for tr=Int(tTP.tbr[1]) to Int(tTP.ter[1]) do begin
 for tc=Int(tTP.tbc[1]) to Int(tTP.tec[1]) do begin
 for bc=Int(tTP.bbc[1]) to Int(tTP.bec[1]) do begin
 for sr=Int(tTP.cbr[1]) to Int(tTP.cer[1]) do begin
 for sc=Int(tTP.cbc[1]) to Int(tTP.cec[1]) do begin
    tarid = GetTargetId( target_group_id=GetObjId( name=tarname), row=tr, column=tc);
    PositionToBlock( tip=maxtip, object=tarid, blockRow=br, blockColumn=bc, spotRow=sr, spotColumn=sc );
    PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset, 12);   
    PipetteDispense( tip_list=tiplist, droplets=0 );
    SpotState(tarid,br,bc,sr,sc,maxtip,SCD,spr,spc,tTP);
 end;
 end; 
 PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset-zmoveoffset, 12);
 end;
 end; 
 end;
 end;

end; //End direction switch logic
end; //End if da=0 condition


if tTP.da = 1 then begin

if (tTP.bec[1] < tTP.bbc[1]) or (tTP.tec[1] < tTP.tbc[1]) then begin
 for br=Int(tTP.bbr[1]) to Int(tTP.ber[1]) do begin
 for tr=Int(tTP.tbr[1]) to Int(tTP.ter[1]) do begin
 for tc=Int(tTP.tec[1]) to Int(tTP.tbc[1]) do begin
 for bc=Int(tTP.bec[1]) to Int(tTP.bbc[1]) do begin
   if tTP.bec[1] = tTP.bbc[1] then mbc = bc;
   else mbc = Int(tTP.bbc[1])-Int(bc-tTP.bec[1]);
   //else mbc = Int(tTP.bbc[1])-(bc-1);

   //mtc = Int(tTP.tbc[1])-(tc-1);
   mtc = Int(tTP.tbc[1])-(tc-Int(tTP.tec[1]));
   tarid = GetTargetId( target_group_id=GetObjId( name=tarname), row=tr, column=mtc);
   dx=GetObjValue( object=tarid, valueIndex=14); 
   dy=GetObjValue( object=tarid, valueIndex=13);   
   numX=(Int(tTP.cbc[1]-tTP.cec[1]))+1;
   numY=(Int(tTP.cer[1]-tTP.cbr[1]))+1; 
   
   PositionToBlock( tip=maxtip, object=tarid, blockRow=br, blockColumn=mbc, spotRow=Int(tTP.cbr[1]), spotColumn=Int(tTP.cec[1] ));
   PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset, 12); 
   adpArrayDriver( -dx,numX,-dy,numY,0,tiplist);   
   

 for sr=Int(tTP.cbr[1]) to Int(tTP.cer[1]) do begin
 for sc=Int(tTP.cec[1]) to Int(tTP.cbc[1]) do begin
   msc = Int(tTP.cbc[1])-(sc-Int(tTP.cec[1]));
   SpotState(tarid,br,mbc,sr,msc,maxtip,SCD,spr,spc,tTP);
 end;
 end;
 PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset-zmoveoffset, 12);
 end;
 end; 
 end;
 end;

end; //End left to right direction 

else begin //Begin right to left - "standard direction"

 for br=Int(tTP.bbr[1]) to Int(tTP.ber[1]) do begin
 for tr=Int(tTP.tbr[1]) to Int(tTP.ter[1]) do begin
 for tc=Int(tTP.tbc[1]) to Int(tTP.tec[1]) do begin
 for bc=Int(tTP.bbc[1]) to Int(tTP.bec[1]) do begin 
 
    tarid = GetTargetId( target_group_id=GetObjId( name=tarname), row=tr, column=tc);
    dx=GetObjValue( object=tarid, valueIndex=14); 
    dy=GetObjValue( object=tarid, valueIndex=13); 
    numX=(Int(tTP.cec[1]-tTP.cbc[1]))+1;
    numY=(Int(tTP.cer[1]-tTP.cbr[1]))+1;
    PositionToBlock( tip=maxtip, object=tarid, blockRow=br, blockColumn=bc, spotRow=Int(tTP.cbr[1]), spotColumn=Int(tTP.cbc[1] ));
    PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset, 12); 
    adpArrayDriver( dx,numX,-dy,numY,0,tiplist);


 for sr=Int(tTP.cbr[1]) to Int(tTP.cer[1]) do begin
 for sc=Int(tTP.cbc[1]) to Int(tTP.cec[1]) do begin
    SpotState(tarid,br,bc,sr,sc,maxtip,SCD,spr,spc,tTP);

 end;
 end; 
 PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset-zmoveoffset, 12);
 end;
 end; 
 end;
 end;

end; //End direction switch logic
end; //End if da=1 condition


if tTP.da = 2 then begin

if (tTP.bec[1] < tTP.bbc[1]) then begin
 for br=Int(tTP.bbr[1]) to Int(tTP.ber[1]) do begin
 for tr=Int(tTP.tbr[1]) to Int(tTP.ter[1]) do begin
 for tc=Int(tTP.tec[1]) to Int(tTP.tbc[1]) do begin
   if tTP.tec[1] < tTP.tbc[1] then mtc = Int(tTP.tbc[1])-Int(tc-tTP.tec[1]);
   else mtc = tc;
   tarid = GetTargetId( target_group_id=GetObjId( name=tarname), row=tr, column=mtc);
 for bc=Int(tTP.bec[1]) to Int(tTP.bbc[1]) do begin
   if tTP.bec[1] = tTP.bbc[1] then mbc = bc;
   else mbc = Int(tTP.bbc[1])-Int(bc-tTP.bec[1]);

   
 totalcoords = DriveOptionCommaCounter(tTP.cbcstring);    
 for ti=1 to totalcoords do begin                          
 for sr=Int(tTP.cbr[ti]) to Int(tTP.cer[ti]) do begin
 for sc=Int(tTP.cec[ti]) to Int(tTP.cbc[ti]) do begin
   msc = Int(tTP.cbc[ti])-(sc-Int(tTP.cec[ti]));
   PositionToBlock( tip=maxtip, object=tarid, blockRow=br, blockColumn=mbc, spotRow=sr, spotColumn=msc );
   PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset, 12);   
   PipetteDispense( tip_list=tiplist, droplets=0 );
   SpotState(tarid,br,mbc,sr,msc,maxtip,SCD,spr,spc,tTP);
 end;
 end;
 end; 
 
 PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset-zmoveoffset, 12);
 end;
 end; 
 end;
 end;

end; //End left to right direction 

else begin //Begin right to left - "standard direction"

 for br=Int(tTP.bbr[1]) to Int(tTP.ber[1]) do begin
 for tr=Int(tTP.tbr[1]) to Int(tTP.ter[1]) do begin
 for tc=Int(tTP.tbc[1]) to Int(tTP.tec[1]) do begin
 for bc=Int(tTP.bbc[1]) to Int(tTP.bec[1]) do begin
 
 totalcoords = DriveOptionCommaCounter(tTP.cbcstring);  
 for ti=1 to totalcoords do begin  
 for sr=Int(tTP.cbr[ti]) to Int(tTP.cer[ti]) do begin
 for sc=Int(tTP.cbc[ti]) to Int(tTP.cec[ti]) do begin
    tarid = GetTargetId( target_group_id=GetObjId( name=tarname), row=tr, column=tc);
    PositionToBlock( tip=maxtip, object=tarid, blockRow=br, blockColumn=bc, spotRow=sr, spotColumn=sc );
    PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset, 12);   
    PipetteDispense( tip_list=tiplist, droplets=0 );
    SpotState(tarid,br,bc,sr,sc,maxtip,SCD,spr,spc,tTP);
 end;
 end; 
 end;
 
 PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset-zmoveoffset, 12);
 end;
 end; 
 end;
 end;

end; //End direction switch logic
end; //End if da=2 condition


//Begin da=3 
//spotting on the fly
if tTP.da = 3 then begin

if (tTP.bec[1] < tTP.bbc[1]) or (tTP.tec[1] < tTP.tbc[1]) then begin
 for br=Int(tTP.bbr[1]) to Int(tTP.ber[1]) do begin
 for tr=Int(tTP.tbr[1]) to Int(tTP.ter[1]) do begin
 for tc=Int(tTP.tec[1]) to Int(tTP.tbc[1]) do begin
 for bc=Int(tTP.bec[1]) to Int(tTP.bbc[1]) do begin
   if tTP.bec[1] = tTP.bbc[1] then mbc = bc;
   else mbc = Int(tTP.bbc[1])-Int(bc-tTP.bec[1]);

   mtc = Int(tTP.tbc[1])-Int(tc-tTP.tec[1]);
   tarid = GetTargetId( target_group_id=GetObjId( name=tarname), row=tr, column=mtc);
   dx=GetObjValue( object=tarid, valueIndex=14); 
   dy=GetObjValue( object=tarid, valueIndex=13);   
   numX=(Int(tTP.cbc[1]-tTP.cec[1]))+1;
   numY=(Int(tTP.cer[1]-tTP.cbr[1]))+1; 
   
   PositionToBlock( tip=maxtip, object=tarid, blockRow=br, blockColumn=mbc, spotRow=Int(tTP.cbr[1]), spotColumn=Int(tTP.cec[1] ));
   PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset, 12); 
   x=GetSystemValue( infoIndex=1, partIndex=maxtip, partInfoIndex=1);
   y=GetSystemValue( infoIndex=1, partIndex=maxtip, partInfoIndex=2);

   sofArrayDriver(maxtip,tiplist,0,x,y,dx,-dy,numX,numY,10,0);
   //adpArrayDriver( -dx,numX,-dy,numY,0,tiplist);   
   

 for sr=Int(tTP.cbr[1]) to Int(tTP.cer[1]) do begin
 for sc=Int(tTP.cec[1]) to Int(tTP.cbc[1]) do begin
   msc = Int(tTP.cbc[1])-(sc-Int(tTP.cec[1]));
   SpotState(tarid,br,mbc,sr,msc,maxtip,SCD,spr,spc,tTP);
 end;
 end;
 PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset-zmoveoffset, 12);
 end;
 end; 
 end;
 end;

end; //End left to right direction 

else begin //Begin right to left - "standard direction"

 for br=Int(tTP.bbr[1]) to Int(tTP.ber[1]) do begin
 for tr=Int(tTP.tbr[1]) to Int(tTP.ter[1]) do begin
 for tc=Int(tTP.tbc[1]) to Int(tTP.tec[1]) do begin
 for bc=Int(tTP.bbc[1]) to Int(tTP.bec[1]) do begin 
 
    tarid = GetTargetId( target_group_id=GetObjId( name=tarname), row=tr, column=tc);
    dx=GetObjValue( object=tarid, valueIndex=14); 
    dy=GetObjValue( object=tarid, valueIndex=13); 
    numX=(Int(tTP.cec[1]-tTP.cbc[1]))+1;
    numY=(Int(tTP.cer[1]-tTP.cbr[1]))+1;
    PositionToBlock( tip=maxtip, object=tarid, blockRow=br, blockColumn=bc, spotRow=Int(tTP.cbr[1]), spotColumn=Int(tTP.cbc[1] ));
    PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset, 12); 
    
    x=GetSystemValue( infoIndex=1, partIndex=maxtip, partInfoIndex=1);
    y=GetSystemValue( infoIndex=1, partIndex=maxtip, partInfoIndex=2);    
    //adpArrayDriver( dx,numX,-dy,numY,0,tiplist);
    sofArrayDriver(maxtip,tiplist,0,x,y,dx,-dy,numX,numY,10,1);

 for sr=Int(tTP.cbr[1]) to Int(tTP.cer[1]) do begin
 for sc=Int(tTP.cbc[1]) to Int(tTP.cec[1]) do begin
    SpotState(tarid,br,bc,sr,sc,maxtip,SCD,spr,spc,tTP);

 end;
 end; 
 PositionAbsoluteZ( z=GetZ( object=tarid)-zdispoffset-zmoveoffset, 12);
 end;
 end; 
 end;
 end;

end; //End direction switch logic
end; //End if da=3 condition







end;
 