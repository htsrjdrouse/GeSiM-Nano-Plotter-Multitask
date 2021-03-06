function FindWell(TPLT:integer,x:real,y:real,tarname:string,path:string,DateTime:string):tPoint;

var position: tPoint;
    imagename:string;

begin

    //Take pictures of target origin with External camera (video microscope)
    PositionAbsoluteXY( tip=-1, x=x, y=y );// tip = -1 is for video microscope. 1,2,3 is for real tips
    SetVideoSource( source=8);      
       
    Grab(TPLT); 
    imagename = tarname + ".bmp";
      
    if FindDots(TPLT) then begin  
        SetLiveVideo( on=false);
        SaveBitmap( filename=path + DateTime + imagename, , bBitmapOnly=false );       
        position = GetFoundedDot(TPLT, 1);
        //Message("Success", "The well position is x = ", position.x:0:3, ", y = ", position.y:0:3, "!");   
    end;        
    else begin 
        SetLiveVideo( on=false);
        SaveBitmap( filename=path + DateTime + imagename, , bBitmapOnly=false );        
        Message("Error", "No well found!");            
    end;
 Grab(0);
 SetLiveVideo( on=true);   
 FindWell = position;                                            
end; 