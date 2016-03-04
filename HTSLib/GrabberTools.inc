/**
GrabberTools: Functions, supporting the frame grabber.
Copyright © 2003, 2004 GeSiM mbH
**/
///PH
#include "../Lib/GetHolderInfo_2.inc"
#include "../Lib/WorkplateObjects.inc"

//
//  Findertypes
//
const CHIP =      1;    //  "Old" chip type 1
const SPOT =      2;    //  Spot on water sensitive paper
const RECT =      4;    //  Rectangle
const FLEX =      8;    //  User defined chip
const TRSN =     16;    //  Camera torsion on NP1.2
const DROP =     32;    //  Droplets in the stroboscope
const DEMO =     64;    //  GeSiM demo chip
const MANU =    128;    //  Manually set spotting positions
const PCAM =    256;    //  Cross hairs for camera positioning
const CANT =    512;    //  Cantilevers
const TPLT =   1024;    //  Dot Array Top Left
const BMRT =   2048;    //  Dot Array Bottom Right
const TPRT =  32768;    //  Dot Array Top Right
const BMLT =  65536;    //  Dot Array Bottom Left
const QYPF =  16384;    //  Quality proof
const ALL  = 249855;    //  All finder types

//
//  Video sources for SetVideoSource()
//
const OFF           = 0;
const HEADCAMERA    = 1;
const EXTERN        = 8;
const STROBOSCOPE   = 4;

//
//  Information types for GetGrabberValue()
//
const DIAMETER      = 0;
const RADIUS        = 1;
const ANGLE         = 2;
const COUNT         = 3;
const POS_X         = 4;
const POS_Y         = 5;
const WIDTH         = 6;
const HEIGHT        = 7;
const AREA          = 8;


var gt_CheckParameters : record

    stroboscope: record

        enabled: boolean;
        delay: real;

    end;

    wsp: record

        enabled: boolean;
        id: integer;
        z: real;
        droplets: integer;
        delay: real;

    end;

    tries: integer;

end;


//
//  Error correction on NP 1.2
//
//  Parameters:  dx, dy: displacement.
//
function MoveCorr(dx: real = 5.0, dy: real = 5.0): void;

begin

    PositionRelativeXY(dx, dy);
    PositionRelativeXY(-dx, -dy);

end;


//
//  Save tip check parameters to file.
//  (2004/06)
//
//  Return:     true on success, false on error
//  Parameters: filename
//
function SaveCheckParameters(filename: string = "TipCheck.cfg"): boolean;

var handle: file;
line: string;

begin

    handle = OpenRW(filename, useDialog = false, append = false);
    
    if FileState(handle) > 0 then begin
    
/*var gt_CheckParameters : record
    stroboscope: record
        enabled: boolean;
        delay: real;
    end;
    wsp: record
        enabled: boolean;
        id: integer;
        z: real;
        droplets: integer;
        delay: real;
    end;
    tries: integer;
end;*/
    
        Putln(handle, "This is an automatic generated file. Do not edit!");
        if gt_CheckParameters.stroboscope.enabled then Putln(handle, "1"); 
            else Putln(handle, "0");
        line = StringOf(gt_CheckParameters.stroboscope.delay); Putln(handle, line);
        
        Close(handle);
        
    end;
    else begin

        SaveCheckParameters = false;
   
    end;

end;


//
//  Clear workplate target
//
//  Parameter: objectId: Id of a workplate target with spot layout "points".
//
function ClearTarget(objectId: integer): void;

var empty: tPoint;

begin

    empty.x = 0.0;
    empty.y = 0.0;

    SetPoint(object = objectId, index = 0, point = empty);

end;


//
//  Move camera to middle of a target.
//
//  Parameters: objectId:   target / chip,
//              zUp:        if false, the tips not moved up (attention!)
//
function CamToObject(objectId: integer, zUp: boolean = true): void;

var x, y: real;
begin

    if zUp then PositionAbsoluteZ(0.0);

    // TG 2004-06-17: Points have no size
    
    if GetKind(objectId) = 4 then begin
    
        x = GetObjValue(objectId, 4);
        y = GetObjValue(objectId, 5);
        
    end;
    else begin
    
        x = GetObjValue(objectId, 4)+GetObjValue(objectId, 1)/2.0;
        y = GetObjValue(objectId, 5)+GetObjValue(objectId, 2)/2.0;

    end;    

    PositionAbsoluteXY(-1, x, y);

end;

//
//  Searches an object on the current position and stores the spotting
//  positions on a workplate target.
//
//  Return:     Number of spotting positions (0 on error).
//
//  Parameters: objectId: target,
//              finderId: search algorithm, default: user defined chip
//
function GrabToObject(objectId: integer, finderId: integer = FLEX, maxTries: integer = 1): integer;

var dots, dot:  integer;
    point:      tPoint;
    ok:         boolean;

begin

    if (maxTries > 0) then begin

        ClearTarget(objectId);
        ok = Grab(finderId);

        if not ok then Message("Fatal error", "Grab fails.");

        ok = FindDots(finderId);

        if ok then begin

            dots = GetDotCount(finderId);

            for dot = 1 to dots do begin

                point = GetFoundedDot(finderId, dotNumber = dot);
                SetPoint(object = objectId, index = dot, point);

            end;

            GrabToObject = dots;

        end;
        else begin

            GrabToObject = GrabToObject(objectId, finderId, maxTries-1);

        end;

    end;
    else begin

        GrabToObject = 0;

    end;

end;


//
//  Spot on a workplate target.
//
//  Parameters: tip:        tip number,
//              objectId:   target,
//              dot:        number(index) of spotting position,
//              z:          z-level of the tips when dispensing (Attention!),
//              drops:      number of droplets to dispense,
//              moveUp:     Move the tips to z = 0 on x-y-movement(Attention!) Default: yes.
//
function SetSpot(tip: integer, objectId: integer, dot: integer, z: real,
                 drops: integer, moveUp: boolean = true): void;

var point:  tPoint;

begin

    if moveUp then PositionAbsoluteZ(z = 0.0);

    point = GetPoint(objectId, index = dot);
    PositionAbsoluteXY(tip, point.x, point.y);
    PositionAbsoluteZ(z);
    PipetteDispense([tip], droplets = drops);

    if moveUp then PositionAbsoluteZ(z = 0.0);

end;


//
//  Set the parameters for the tip test on water sensitive paper.
//
//  Return:                 true on success, false on error (target has no x-y-layout)
//
//  Parameters: enabled:    use watersensitive paper or not,
//              objectId:   worplate id of the water sensitive paper,
//              tries:      how often to repeat on error, also used in stroboscope
//              z:          if positive, the z-level of the tips; if negative,
//                          the distance of the tip above the z-level of the target
//              droplets:   number of droplets per spot, default: 1,
//              delay:      dry time between dispensing and grabbing
//
function SetWaterSensitivePaper(enabled: boolean, objectId: integer = -1,
                                tries: integer = 3, z: real = 0.0,
                                droplets: integer = 1, delay: real = 0.0): boolean;

begin

    if enabled then begin

        if WPO_IsTarget(wpo = objectId) then begin

            if WPO_HasArrayLayout(wpo = objectId) then begin

                gt_CheckParameters.wsp.enabled = true;
                gt_CheckParameters.wsp.id = objectId;
                gt_CheckParameters.wsp.droplets = droplets;
                gt_CheckParameters.wsp.delay = delay;
                gt_CheckParameters.tries = tries;

                if (z < 0.0) then
                    gt_CheckParameters.wsp.z = GetZ(objectId)+z;
                else
                    gt_CheckParameters.wsp.z = z;

                SetWaterSensitivePaper = true;

                exit;

            end;

        end;

        SetWaterSensitivePaper = false;

    end;
    else begin

        gt_CheckParameters.wsp.enabled = false;
        SetWaterSensitivePaper = true;

    end;

end;


//
//  Sets the parameters for the tip test in stroboscope.
//
//  Parameters: enabled:    use the stroboscope or not,
//              tries:      how often to repeat on error, also used on water sensitive paper
//              delay:      time between dispensing start and grabbing.
//
function SetStroboscope(enabled: boolean, tries: integer = 3, delay: real = 1.0): void;

begin

    gt_CheckParameters.stroboscope.enabled = enabled;
    gt_CheckParameters.tries = tries;
    gt_CheckParameters.stroboscope.delay = delay;

end;


//
//  Combination of SetWaterSensitivePaper and SetStroboscope.
//
//  Parameters: Explained above.
//
function SetCheckParameters(wspEnabled: boolean, stroboscopeEnabled: boolean,
                            tries: integer = 3, wspId: integer = -1, wspZ: real = 0.0,
                            wspDroplets: integer = 1, wspDelay: real = 0.0,
                            stroboscopeDelay: real = 1.0): boolean;

begin

    SetStroboscope(enabled = stroboscopeEnabled, tries, delay = stroboscopeDelay);
    SetCheckParameters = SetWaterSensitivePaper(enabled = wspEnabled, objectId = wspId,
                                                tries, z = wspZ, droplets = wspDroplets,
                                                delay = wspDelay);

end;

//
//  Getting sample from a micro titer plate
//
//  Parameters: mtp:        workplate id of the titer plate,
//              row:        well row (ALLWAYS FOR HOLDER 1),
//              col:        well column,
//              volume:     volume to take,
//              dry:        dry after getting samle?        Default:    no,
//              tipList:    tips,                                       all,
//              flow:       volume flow when getting samlple,           1.0 µl/s
//              speed:      speed when going up,                        2.0 mm/s
//              wait:       waiting time between getting and moving,    2.0 s,
//              zGet:       height immersed,                            take from workplate,
//              zOut:       height not immersed,                        10 mm above zGet
//
//  Return:     false, if parameters invalid.
//
function GetSample(mtp: integer, row: integer, col: integer,
                   volume: real, dry: boolean = false, tipList: integer = 0,
                   flow: real = 1.0, speed: real = 3.0, wait: real = 2.0,
                   zGet: real = 0.0, zOut: real = 0.0): boolean;
///PH (headCol)
var rows, cols, headCol:integer;
var tip, holder:        integer;
var valid:              boolean;
///PH (tHolderInfo_2)
var holderInfo:     tHolderInfo_2;
var yWell, yTip:    real;

begin

//  Validation:

    valid = (GetKind(object = mtp) = 1);    // Object is titer plate?

    if valid then begin

        rows = Int(GetObjValue(object = mtp, valueIndex = 7));
        cols = Int(GetObjValue(object = mtp, valueIndex = 8));
///PH (GetHolderInfo_2)
        holderInfo = GetHolderInfo_2();
///PH (1+2)
        if holderInfo.numberOfTips[1]+holderInfo.numberOfTips[2] < 1 then valid = false;
        if (col < 1) or (col > cols) then valid = false;

        if valid then begin
///PH (gleicher Test fuer zweite Spalte, falls vorhanden)
            if holderInfo.headCols=2 then
             begin
              holder = Int(GetSystemValue(infoIndex = 1,
                           partIndex = holderInfo.lowestTip[2], partInfoIndex = 4));

              yTip = 4.5*(holder-1)+(rows-row)*GetObjValue(object = mtp, valueIndex = 13);
              yWell = 0.0;

              if yTip < yWell then valid = false;
             end;
///

            holder = Int(GetSystemValue(infoIndex = 1,
///PH (.lowestTip[1])
                partIndex = holderInfo.lowestTip[1], partInfoIndex = 4));

            yTip = 4.5*(holder-1)+(rows-row)*GetObjValue(object = mtp, valueIndex = 13);
            yWell = 0.0;

            if yTip < yWell then valid = false;
///PH
            if holderInfo.headCols=2 then
             begin
              holder = Int(GetSystemValue(infoIndex = 1,
                           partIndex = holderInfo.highestTip[2], partInfoIndex = 4));

              yTip = 4.5*(holder-1)+(rows-row)*GetObjValue(object = mtp, valueIndex = 13);
              yWell = (rows-1)*GetObjValue(object = mtp, valueIndex = 13);

              if yTip > yWell then valid = false;
             end;
///
            holder = Int(GetSystemValue(infoIndex = 1,
///PH (.highestTip[1])
                partIndex = holderInfo.highestTip[1], partInfoIndex = 4));

            yTip = 4.5*(holder-1)+(rows-row)*GetObjValue(object = mtp, valueIndex = 13);
            yWell = (rows-1)*GetObjValue(object = mtp, valueIndex = 13);

            if yTip > yWell then valid = false;

///PH
            for headCol=1 to holderInfo.headCols do
             for tip = holderInfo.lowestTip[headCol] to holderInfo.highestTip[headCol] do begin

                holder = Int(GetSystemValue(infoIndex = 1, partIndex = tip, partInfoIndex = 4));

                if (rows <= 8) and ((holder mod 2) != 1) then valid = false;

             end;    // next tip
///
        end;

    end;

    if valid then begin

        if (zGet <= 0.0) then zGet = GetZ(mtp);
        if (zOut <= 0.0) then zOut = zGet-10.0;
        if (zOut <= 0.0) then zOut = 0.0;

        PositionAbsoluteZ(z = 0.0);
        PositionToObject(0, object = mtp, row = row, column = col);
        DiluterSetValveSystemFluid(tipList);
        DiluterEmpty(tipList, flow = 250);
        PositionAbsoluteZ(z = zGet);
        DiluterSetValvePipette(tipList);
        DiluterLoad(tipList, volume, flow);
        TimerDelay(t = wait);
        PositionAbsoluteZ(z = zOut, speed);
        PositionAbsoluteZ(0.0);

        if dry then begin

            PipetteDry();
            DiluterSetValveBypass(tipList);

        end;
        else begin

            DiluterSetValveBypass(tipList);
            PositionAbsoluteXY(tip = 1, x = GetSystemValue(infoIndex = 4, partIndex = 5),
                               y = GetSystemValue(infoIndex = 4, partIndex = 6));

            PipetteDispense(tipList, droplets = 25);

        end;

        GetSample = true;

    end;    // if valid
    else begin

        GetSample = false;

    end;

end;


//
//  Checks tip(s) in stroboscope and calibrates then on watersenive paper.
//  Controlled by SetCheckParameters (see above).
//
//  Parameters: tipList:    tips, default: all,
//              _maxTries:   for internal use, do not change,
//              wash:       use the WashDryFunction between retries (normally not used)
//  Return:     true = success, false = error.
//

function CheckTips(tipList: integer = 0, _maxTries: integer = -1, wash: boolean = false): boolean;

var ok: boolean;
var strobo: array[1..16] of boolean;
///PH
var holderInfo: tHolderInfo_2;
///PH (headCol)
var i, tip, headCol: integer;
///PH var z: real;

begin

    if _maxTries = -1 then _maxTries = gt_CheckParameters.tries;

    if _maxTries < 1 then begin

        ok = false;

    end;
    else begin

        if gt_CheckParameters.stroboscope.enabled then begin

            //SetVideoSource(source = STROBOSCOPE);
            SetLiveVideo(on = true);
            Stroboscope();

            SetLiveVideo(on = false);
            strobo = CheckStroboscope(tip_list = tipList,
                                      delay = gt_CheckParameters.stroboscope.delay);

            //SetVideoSource(source = HEADCAMERA);
///PH
            holderInfo = GetHolderInfo_2();
            ok = true;
///PH
            for headCol=1 to holderInfo.headCols do
             for i = 1 to holderInfo.numberOfTips[headCol] do begin

                 tip = holderInfo.tipsTopDown[i,headCol];

                 if (tipList = []) or (GetBit(bitmask = tipList, bitpos = tip-1)) then
                     ok = ok and strobo[tip];

             end;
///
        end;
        else begin

            ok = true;

        end;

        if ok then begin

            if gt_CheckParameters.wsp.enabled then begin

                SetLiveVideo(on = false);
                ok = CalibratePipettes(tip_list = tipList,
                                       tar_paper = gt_CheckParameters.wsp.id,
                                       z_dispense = gt_CheckParameters.wsp.z,
                                       droplets = gt_CheckParameters.wsp.droplets,
                                       delay = gt_CheckParameters.wsp.delay);

            end;
            else begin

                ok = true;  // If nothing was check, we assume everything works.

            end;

        end;

        if !ok then begin

            if wash then PipetteWashDry(tipList);
            DiluterSetValveBypass([]);
            ok = CheckTips(tipList, _maxTries-1);

        end;

    end;

    CheckTips = ok;
    SetLiveVideo(on = true);

end;


//
//  As GetSample, but with tip check at the end and retry on error.
//
//  Parameters: See GetSample +
//              maxTries:   maximum tries on error, defaults to 3.
//  Return:     true = ok, false = error.
//
function GetSampleCheckTips(mtp: integer, row: integer, col: integer,
                   volume: real, dry: boolean = false, tipList: integer = 0,
                   flow: real = 1.0, speed: real = 2.0, wait: real = 2.0,
                   zGet: real = 0.0, zOut: real = 0.0, maxTries: integer = 3,
                   dryAfterWash: boolean = true): boolean;

begin

    if maxTries < 1 then begin

        GetSampleCheckTips = false;

    end;
    else begin

        if GetSample(mtp, row, col, volume, dry, tipList, flow, speed, wait, zGet, zOut) then begin

            if CheckTips(tipList) then begin

                GetSampleCheckTips = true;

            end;
            else begin

                if dryAfterWash then PipetteWashDry(); else PipetteWash();
                
                GetSampleCheckTips = GetSampleCheckTips(mtp, row, col, volume, dry, tipList,
                                                        flow, speed, wait, zGet, zOut,
                                                        maxTries-1);

            end;

        end;
        else begin

            GetSampleCheckTips = false;

        end;

    end;

end;

//
// Interactive version of SetWaterSensitivePaper.
//
function ConfigWaterSensitivePaper(enabled: boolean = true, objectId: integer = -1, 
        z: real = 0.0, tries: integer = 3, droplets: integer = 1, delay: real = 0.0,
        title: string = "Water Sensitive Paper"): boolean;
            
begin

    Window(title, 
           "Enable Function Test:", enabled,
           "Maximal Tries:", tries,
           "Number of Droplets", droplets,
           "Dry Time (Seconds)", delay);
               
    ConfigWaterSensitivePaper = SetWaterSensitivePaper(enabled, objectId, tries,
                                                       z, droplets, delay);
end;


//
// Interactive version of SetStroboscope.
//
function ConfigStroboscope(enabled: boolean = true, tries: integer = 3, delay: real = 1.0,
        title: string = "Stroboscope"): void;

begin

    Window(title,
           "Enable Function Test:", enabled,
           "Maximal Tries:", tries,
           "Delay Time (Seconds):", delay);
           
    SetStroboscope(enabled, tries, delay);

end;


//
// Combination of ConfigWaterSensitivePaper and ConfigStroboscope.
//
function ConfigCheckParameters(wspEnabled: boolean = true, stroboscopeEnabled: boolean = true,
        tries: integer = 3, wspId: integer = -1, wspZ: real = 0.0, wspDroplets: integer = 1, 
        wspDelay: real = 0.0, stroboscopeDelay: real = 1.0, 
        title: string = "Tip Check Configuration"): boolean;

begin

    Window(title,
           "Maximal Tries:", tries,
           "Enable Stroboscope Test:", stroboscopeEnabled,
           "Delay Time:", stroboscopeDelay,
           "Enable Water Sensitive Paper:", wspEnabled,
           "Number of Droplets:", wspDroplets,
           "Dry Time:", wspDelay);
           
    ConfigCheckParameters = SetCheckParameters(wspEnabled, stroboscopeEnabled, tries, 
        wspId, wspZ, wspDroplets, wspDelay, stroboscopeDelay);
        
end;


function CrossHairs(enable: boolean = true): void;

begin

    if enable then 
        FindDots(finderId = PCAM);
    else
        Grab(0);
        
end;



function ClearGrabWindow(): void;

begin

    Grab(0);
    
end;
 
