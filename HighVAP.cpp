//   Copyright 2019, by Ed Carp. All rights reserved. Distribution without permission is prohibited.

#include "sierrachart.h"
// #include "scstudyfunctions.h"
#include <math.h>
#define TRIGGER 10

#define OPEN sc.Open[sc.Index]
#define HIGH sc.High[sc.Index]
#define LOW sc.Low[sc.Index]
#define CLOSE sc.Close[sc.Index]
// #define InHighPin(PRICE) (CLOSE>OPEN&&HIGH>CLOSE&&PRICE>CLOSE&&PRICE<HIGH)
// #define InLowPin(PRICE) (CLOSE<OPEN&&LOW<CLOSE&&PRICE<CLOSE&&PRICE>LOW)

/*
int InHighPin(float PRICE)
{
	if(CLOSE>OPEN && HIGH>CLOSE && PRICE>CLOSE && PRICE<HIGH)
		return 1;
	return 0;
}

int InLowPin(float PRICE)
{
	if(CLOSE<OPEN && LOW<CLOSE && PRICE<CLOSE && PRICE>LOW)
		return 1;
	return 0;
}
*/

SCDLLName("High Volume At Price v0.4g") 
const char EXPIRATION_DATE[] = "2022-01-01";
char scratchmsg[255];
int sgctr;
FILE *fout;

// Friendly Names for Colors
const unsigned int RGB_Red = RGB (255, 0, 0);
const unsigned int RGB_Red210 = RGB (210, 0, 0);
const unsigned int RGB_Green = RGB (0, 255, 0);
const unsigned int RGB_Green210 = RGB (0, 210, 0);
const unsigned int RGB_Blue = RGB (0, 0, 255);
const unsigned int RGB_Magenta = RGB (255, 0, 255);
const unsigned int RGB_Yellow = RGB (255, 255, 0);
const unsigned int RGB_LightYellow = RGB (255, 255, 128);
const unsigned int RGB_Cyan = RGB (0, 255, 255);
const unsigned int RGB_LightCyan = RGB (128, 255, 255);
const unsigned int RGB_Cyan210 = RGB (0, 210, 210);
const unsigned int RGB_White = RGB (255, 255, 255);
const unsigned int RGB_Black = RGB (0, 0, 0);
const unsigned int RGB_Pink = RGB (255, 128, 192);
const unsigned int RGB_Purple = RGB (128, 128, 192);
const unsigned int RGB_LightGreen = RGB (128, 255, 0);
const unsigned int RGB_HotPink = RGB (255, 0, 128);
const unsigned int RGB_LightRed = RGB (255, 128, 128);
const unsigned int RGB_Orange = RGB (255, 128, 64);
const unsigned int RGB_Sand = RGB (255, 227, 150);
const unsigned int RGB_LightTeal = RGB (150, 255, 227);

SCDateTime GetNow(SCStudyInterfaceRef sc) 
{
    //https://www.sierrachart.com/index.php?page=doc/ACSIL_Members_Functions.html#scGetCurrentDateTime
    if (sc.IsReplayRunning()) {
        return sc.CurrentDateTimeForReplay; 
    } else {
        return sc.CurrentSystemDateTime;
    }
}

bool IsExpired(SCStudyInterfaceRef sc) 
{
    SCString DateString (EXPIRATION_DATE);
    SCDateTime futureDate;
    futureDate = sc.DateStringToSCDateTime(DateString);

    return GetNow(sc) >= futureDate;
}

SCSFExport scsf_HighVAP(SCStudyInterfaceRef sc)
{

	if (sc.HideStudy == 1)
		return;

	sgctr = 0;
	SCInputRef ImbalanceRatio = sc.Input[0];
	SCInputRef MinimumBarSize = sc.Input[1];
	SCInputRef CompareLevels = sc.Input[2];
	SCInputRef DebugLog = sc.Input[3];
	SCInputRef MarketDepthLimit = sc.Input[4];
	SCInputRef ExhaustRatio = sc.Input[5];

	// SCSubgraphRef MaxBidVAP = sc.Subgraph[0];
	// SCSubgraphRef MaxAskVAP = sc.Subgraph[1];
	SCSubgraphRef MaxVAP = sc.Subgraph[sgctr++];
	SCSubgraphRef MaxVAPVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef TotalBidVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef TotalAskVolume = sc.Subgraph[sgctr++];
	// SCSubgraphRef TotalBidVolumePercent = sc.Subgraph[sgctr++];
	// SCSubgraphRef TotalAskVolumePercent = sc.Subgraph[sgctr++];
    SCSubgraphRef MaxBidVAP = sc.Subgraph[sgctr++];
    SCSubgraphRef MaxAskVAP = sc.Subgraph[sgctr++];
	SCSubgraphRef TotalBidVolumePerTick = sc.Subgraph[sgctr++];
	SCSubgraphRef TotalAskVolumePerTick = sc.Subgraph[sgctr++];
	SCSubgraphRef ZPB = sc.Subgraph[sgctr++];
	SCSubgraphRef ZPA = sc.Subgraph[sgctr++];
	SCSubgraphRef BuyImbalanceHi = sc.Subgraph[sgctr++];
	SCSubgraphRef SellImbalanceHi = sc.Subgraph[sgctr++];
	SCSubgraphRef BuyImbalanceLo = sc.Subgraph[sgctr++];
	SCSubgraphRef SellImbalanceLo = sc.Subgraph[sgctr++];
	SCSubgraphRef AverageVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef VolumePerTick = sc.Subgraph[sgctr++];
	SCSubgraphRef DeltaVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef Candle_Size = sc.Subgraph[sgctr++];
	SCSubgraphRef ReducingVolumeBidTop = sc.Subgraph[sgctr++];
	SCSubgraphRef ReducingVolumeBidBot = sc.Subgraph[sgctr++];
	SCSubgraphRef ReducingVolumeAskTop = sc.Subgraph[sgctr++];
	SCSubgraphRef ReducingVolumeAskBot = sc.Subgraph[sgctr++];
	
	SCSubgraphRef LOBMinBidVAP = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBMinAskVAP = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBMaxBidVAP = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBMaxAskVAP = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBTotalBidVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBTotalAskVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBTotalBidVolumePercent = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBTotalAskVolumePercent = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBDeltaVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBMinBidVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBMinAskVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBMaxBidVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBMaxAskVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBValidFlag = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBAvgBidVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBAvgAskVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBAvgVolume = sc.Subgraph[sgctr++];

	SCSubgraphRef MaxBidVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef MaxAskVolume = sc.Subgraph[sgctr++];
	
	SCSubgraphRef LOBCurBidVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef LOBCurAskVolume = sc.Subgraph[sgctr++];
	SCSubgraphRef CurPrice = sc.Subgraph[sgctr++];
	SCSubgraphRef VWAPPrice = sc.Subgraph[sgctr++];
	
	SCSubgraphRef UBHi = sc.Subgraph[sgctr++];
	SCSubgraphRef UBLo = sc.Subgraph[sgctr++];
	SCSubgraphRef NumBuyImb = sc.Subgraph[sgctr++];
	SCSubgraphRef NumSellImb = sc.Subgraph[sgctr++];
	SCSubgraphRef NumBuyZP = sc.Subgraph[sgctr++];
	SCSubgraphRef NumSellZP = sc.Subgraph[sgctr++];
	SCSubgraphRef BuyImbalanceRatioHi = sc.Subgraph[sgctr++];
	SCSubgraphRef SellImbalanceRatioHi = sc.Subgraph[sgctr++];
	SCSubgraphRef BuyImbalanceRatioLo = sc.Subgraph[sgctr++];
	SCSubgraphRef SellImbalanceRatioLo = sc.Subgraph[sgctr++];
	SCSubgraphRef BuyExhaust = sc.Subgraph[sgctr++];
	SCSubgraphRef SellExhaust = sc.Subgraph[sgctr++];

	SCSubgraphRef BuyExhRatio = sc.Subgraph[sgctr++];
	SCSubgraphRef SellExhRatio = sc.Subgraph[sgctr++];
//	SCSubgraphRef SignalsInTopTail = sc.Subgraph[61];
//	SCSubgraphRef SignalsInBotTail = sc.Subgraph[62];
	// SCSubgraphRef SCIndex = sc.Subgraph[63];

   /*
    if (IsExpired(sc))
    {
        sprintf(scratchmsg, "DLL expired on %s\n", EXPIRATION_DATE);
        sc.AddMessageToLog(scratchmsg, 1);
        return;
    }
    */

   if (sc.SetDefaults)
   {
      
      // During development set this flag to 1, so the DLL can be modified. When development is done, set it to 0 to improve performance.
      sc.FreeDLL = 0;
      sc.DrawStudyUnderneathMainPriceGraph = 0;
      sc.MaintainHistoricalMarketDepthData = 1;
      sc.MaintainVolumeAtPriceData = 1;

      sc.GraphName = "High Volume At Price";
      sc.StudyDescription = "Display various statistics for each bar.";
      sc.AutoLoop = 1;
      sc.GraphRegion = 0;
      sc.ScaleRangeType = SCALE_SAMEASREGION;

	  MaxBidVAP.Name = "MaxBidPrice";
      MaxBidVAP.DrawStyle = DRAWSTYLE_LEFT_PRICE_BAR_DASH; // left dash
      MaxBidVAP.LineWidth = 1;
      MaxBidVAP.PrimaryColor = RGB_LightRed; // light red

      MaxAskVAP.Name = "MaxAskPrice";
      MaxAskVAP.DrawStyle = DRAWSTYLE_RIGHT_PRICE_BAR_DASH; // right dash
      MaxAskVAP.LineWidth = 1;
      MaxAskVAP.PrimaryColor = COLOR_GREEN;

      MaxVAP.Name = "MaxVAP";
      MaxVAP.DrawStyle = DRAWSTYLE_DASH;
      MaxVAP.LineWidth = 1;
      MaxVAP.PrimaryColor = COLOR_YELLOW;
      MaxVAP.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;
      MaxVAP.ShortName = "VPOC";

      MaxVAPVolume.Name = "MaxVAPVolume";
      MaxVAPVolume.DrawStyle = DRAWSTYLE_IGNORE;
      MaxVAPVolume.LineWidth = 2;
      MaxVAPVolume.PrimaryColor = COLOR_YELLOW;

      TotalBidVolume.Name = "TotalBidVolume";
      TotalBidVolume.DrawStyle = DRAWSTYLE_IGNORE;
      TotalBidVolume.LineWidth = 2;
      TotalBidVolume.PrimaryColor = COLOR_YELLOW;

      TotalAskVolume.Name = "TotalAskVolume";
      TotalAskVolume.DrawStyle = DRAWSTYLE_IGNORE;
      TotalAskVolume.LineWidth = 2;
      TotalAskVolume.PrimaryColor = COLOR_YELLOW;

      /*
      TotalBidVolumePercent.Name = "TotalBidVolumePercent";
      TotalBidVolumePercent.DrawStyle = DRAWSTYLE_IGNORE;
      TotalBidVolumePercent.LineWidth = 2;
      TotalBidVolumePercent.PrimaryColor = COLOR_YELLOW;

      TotalAskVolumePercent.Name = "TotalAskVolumePercent";
      TotalAskVolumePercent.DrawStyle = DRAWSTYLE_IGNORE;
      TotalAskVolumePercent.LineWidth = 2;
      TotalAskVolumePercent.PrimaryColor = COLOR_YELLOW;
      */

      TotalBidVolumePerTick.Name = "TotalBidVolumePerTick";
      TotalBidVolumePerTick.DrawStyle = DRAWSTYLE_IGNORE;
      TotalBidVolumePerTick.LineWidth = 2;
      TotalBidVolumePerTick.PrimaryColor = COLOR_YELLOW;

      TotalAskVolumePerTick.Name = "TotalAskVolumePerTick";
      TotalAskVolumePerTick.DrawStyle = DRAWSTYLE_IGNORE;
      TotalAskVolumePerTick.LineWidth = 2;
      TotalAskVolumePerTick.PrimaryColor = COLOR_YELLOW;

      AverageVolume.Name = "AverageVolume";
      AverageVolume.DrawStyle = DRAWSTYLE_IGNORE;
      AverageVolume.LineWidth = 2;
      AverageVolume.PrimaryColor = COLOR_YELLOW;

      MaxBidVolume.Name = "MaxBidVolume";
      MaxBidVolume.DrawStyle = DRAWSTYLE_IGNORE;
      MaxBidVolume.LineWidth = 2;
      MaxBidVolume.PrimaryColor = RGB_LightRed; // light red

      MaxAskVolume.Name = "MaxAskVolume";
      MaxAskVolume.DrawStyle = DRAWSTYLE_IGNORE;
      MaxAskVolume.LineWidth = 2;
      MaxAskVolume.PrimaryColor = COLOR_GREEN;

      ZPB.Name = "Zero-Print Bid";
      ZPB.DrawStyle = DRAWSTYLE_IGNORE;
      ZPB.LineWidth = 2;
      ZPB.PrimaryColor = COLOR_GREEN;

      ZPA.Name = "Zero-Print Ask";
      ZPA.DrawStyle = DRAWSTYLE_IGNORE;
      ZPA.LineWidth = 2;
      ZPA.PrimaryColor = RGB_LightRed; // light red;

      BuyImbalanceHi.Name = "Buy Absorption Top";
      BuyImbalanceHi.DrawStyle = DRAWSTYLE_DASH;
      BuyImbalanceHi.LineWidth = 2;
      BuyImbalanceHi.PrimaryColor = RGB_LightRed;
      BuyImbalanceHi.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;
      BuyImbalanceHi.ShortName = "BUYABS";

      SellImbalanceHi.Name = "Sell Imbalance Top";
      SellImbalanceHi.DrawStyle = DRAWSTYLE_DASH;
      SellImbalanceHi.LineWidth = 1;
      SellImbalanceHi.PrimaryColor = RGB_LightRed; // light red;
      SellImbalanceHi.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;
      SellImbalanceHi.ShortName = "SIMBT";
	  
      BuyImbalanceLo.Name = "Buy Imbalance Bottom";
      BuyImbalanceLo.DrawStyle = DRAWSTYLE_DASH;
      BuyImbalanceLo.LineWidth = 1;
      BuyImbalanceLo.PrimaryColor = RGB_LightGreen;
      BuyImbalanceLo.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;
      BuyImbalanceLo.ShortName = "BIMBB";

      SellImbalanceLo.Name = "Sell Absorption Bottom";
      SellImbalanceLo.DrawStyle = DRAWSTYLE_DASH;
      SellImbalanceLo.LineWidth = 2;
      SellImbalanceLo.PrimaryColor = RGB_LightGreen;
      SellImbalanceLo.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;
      SellImbalanceLo.ShortName = "SELLABS";
	  
      BuyImbalanceRatioHi.Name = "Buy Absorption Ratio Top";
      BuyImbalanceRatioHi.DrawStyle = DRAWSTYLE_IGNORE;
      BuyImbalanceRatioHi.LineWidth = 2;
      BuyImbalanceRatioHi.PrimaryColor = RGB_White; // white;

      BuyImbalanceRatioLo.Name = "Buy Imbalance Ratio Bottom";
      BuyImbalanceRatioLo.DrawStyle = DRAWSTYLE_IGNORE;
      BuyImbalanceRatioLo.LineWidth = 2;
      BuyImbalanceRatioLo.PrimaryColor = RGB_White; // white;

      SellImbalanceRatioHi.Name = "Sell Imbalance Ratio Top";
      SellImbalanceRatioHi.DrawStyle = DRAWSTYLE_IGNORE;
      SellImbalanceRatioHi.LineWidth = 2;
      SellImbalanceRatioHi.PrimaryColor = RGB_White; // white;

      SellImbalanceRatioLo.Name = "Sell Absorption Ratio Bottom";
      SellImbalanceRatioLo.DrawStyle = DRAWSTYLE_IGNORE;
      SellImbalanceRatioLo.LineWidth = 2;
      SellImbalanceRatioLo.PrimaryColor = RGB_White; // white;

	  ImbalanceRatio.Name = "Imbalance Ratio";
	  ImbalanceRatio.SetInt(4);
	  ImbalanceRatio.SetIntLimits(1,1000);
	  
	  MinimumBarSize.Name = "Minimum Size For Bar";
	  MinimumBarSize.SetInt(3);
	  MinimumBarSize.SetIntLimits(3,1000);
	  
	  CompareLevels.Name = "Minimum Compare Levels At Bar Extreme";
	  CompareLevels.SetInt(3);
	  CompareLevels.SetIntLimits(2,1000);
	  
	  DebugLog.Name = "Log Debug Data (1=Yes, 0=No)";
	  DebugLog.SetInt(0);
	  DebugLog.SetIntLimits(0,1);
	  
	  MarketDepthLimit.Name = "Limit To Levels Of Market Depth Analysis";
	  MarketDepthLimit.SetInt(21);
	  MarketDepthLimit.SetIntLimits(1,999);

	  ExhaustRatio.Name = "Exhaustion Ratio";
	  ExhaustRatio.SetInt(3);
	  ExhaustRatio.SetIntLimits(2, 1000);
	  
      VolumePerTick.Name = "Volume Per Tick";
      VolumePerTick.DrawStyle = DRAWSTYLE_IGNORE;
      VolumePerTick.LineWidth = 2;
      VolumePerTick.PrimaryColor = RGB_Cyan; // cyan

      DeltaVolume.Name = "Delta Volume";
      DeltaVolume.DrawStyle = DRAWSTYLE_IGNORE;
      DeltaVolume.LineWidth = 2;
      DeltaVolume.PrimaryColor = RGB_Cyan; // cyan

      VWAPPrice.Name = "VWAP Price";
      VWAPPrice.DrawStyle = DRAWSTYLE_IGNORE;
      VWAPPrice.LineWidth = 1;
      VWAPPrice.PrimaryColor = RGB_Magenta; // purple/magenta
      VWAPPrice.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;
      VWAPPrice.ShortName = "VWAP";

      Candle_Size.Name = "Candle Size";
      Candle_Size.DrawStyle = DRAWSTYLE_IGNORE;
      Candle_Size.LineWidth = 2;
      Candle_Size.PrimaryColor = RGB_Cyan; // cyan
	  
	  ReducingVolumeBidTop.Name = "Reducing Volume Bid Top";
      ReducingVolumeBidTop.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_TOP;
      ReducingVolumeBidTop.LineWidth = 2;
      ReducingVolumeBidTop.PrimaryColor = RGB_Green; // dark green

	  ReducingVolumeBidBot.Name = "Reducing Volume Bid Bottom";
      ReducingVolumeBidBot.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_BOTTOM;
      ReducingVolumeBidBot.LineWidth = 2;
      ReducingVolumeBidBot.PrimaryColor = RGB_Green; // dark green

	  ReducingVolumeAskTop.Name = "Reducing Volume Ask Top";
      ReducingVolumeAskTop.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_TOP;
      ReducingVolumeAskTop.LineWidth = 2;
      ReducingVolumeAskTop.PrimaryColor = RGB_Red; // dark red

	  ReducingVolumeAskBot.Name = "Reducing Volume Ask Bottom";
      ReducingVolumeAskBot.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_BOTTOM;
      ReducingVolumeAskBot.LineWidth = 2;
      ReducingVolumeAskBot.PrimaryColor = RGB_Red; // dark red

      LOBMinBidVAP.Name = "LOBMinAskVAP";
      LOBMinBidVAP.DrawStyle = DRAWSTYLE_IGNORE;
      LOBMinBidVAP.LineWidth = 2;
      LOBMinBidVAP.PrimaryColor = RGB_LightCyan; // light cyan
	  LOBMinBidVAP.ShortName = "LOBMinAsk";
	  LOBMinBidVAP.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;

      LOBMinAskVAP.Name = "LOBMinBidVAP";
      LOBMinAskVAP.DrawStyle = DRAWSTYLE_IGNORE;
      LOBMinAskVAP.LineWidth = 2;
      LOBMinAskVAP.PrimaryColor = RGB_LightRed; // light red
      LOBMinAskVAP.ShortName = "LOBMinBid";
	  LOBMinAskVAP.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;

      LOBMaxBidVAP.Name = "LOBMaxAskVAP";
      LOBMaxBidVAP.DrawStyle = DRAWSTYLE_IGNORE;
      LOBMaxBidVAP.LineWidth = 4;
      LOBMaxBidVAP.PrimaryColor = RGB_LightCyan; // light cyan
      LOBMaxBidVAP.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;
      LOBMaxBidVAP.ShortName = "LOBMaxAsk";

      LOBMaxAskVAP.Name = "LOBMaxBidVAP";
      LOBMaxAskVAP.DrawStyle = DRAWSTYLE_IGNORE;
      LOBMaxAskVAP.LineWidth = 4;
      LOBMaxAskVAP.PrimaryColor = RGB_LightRed; // light red
      LOBMaxAskVAP.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;
      LOBMaxAskVAP.ShortName = "LOBMaxBid";

      LOBMinBidVolume.Name = "LOBMinAskVolume";
      LOBMinBidVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBMinBidVolume.LineWidth = 2;
      LOBMinBidVolume.PrimaryColor = RGB_LightRed; // light red

      LOBMinAskVolume.Name = "LOBMinBidVolume";
      LOBMinAskVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBMinAskVolume.LineWidth = 2;
      LOBMinAskVolume.PrimaryColor = COLOR_GREEN;

      LOBMaxBidVolume.Name = "LOBMaxAskVolume";
      LOBMaxBidVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBMaxBidVolume.LineWidth = 5;
      LOBMaxBidVolume.PrimaryColor = RGB_LightCyan; // light cyan

      LOBMaxAskVolume.Name = "LOBMaxBidVolume";
      LOBMaxAskVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBMaxAskVolume.LineWidth = 5;
      LOBMaxAskVolume.PrimaryColor = RGB_LightRed; // light red

      LOBAvgBidVolume.Name = "LOBAvgAskVolume";
      LOBAvgBidVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBAvgBidVolume.LineWidth = 5;
      LOBAvgBidVolume.PrimaryColor = RGB_LightCyan; // light cyan

      LOBAvgAskVolume.Name = "LOBAvgBidVolume";
      LOBAvgAskVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBAvgAskVolume.LineWidth = 5;
      LOBAvgAskVolume.PrimaryColor = RGB_LightRed; // light red

      LOBAvgVolume.Name = "LOBAvgVolume";
      LOBAvgVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBAvgVolume.LineWidth = 5;
      LOBAvgVolume.PrimaryColor = RGB_Cyan; // light red

      LOBTotalBidVolume.Name = "LOBTotalAskVolume";
      LOBTotalBidVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBTotalBidVolume.LineWidth = 2;
      LOBTotalBidVolume.PrimaryColor = COLOR_YELLOW;

      LOBTotalAskVolume.Name = "LOBTotalBidVolume";
      LOBTotalAskVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBTotalAskVolume.LineWidth = 2;
      LOBTotalAskVolume.PrimaryColor = COLOR_YELLOW;

      LOBTotalBidVolumePercent.Name = "LOBTotalAskVolumePercent";
      LOBTotalBidVolumePercent.DrawStyle = DRAWSTYLE_IGNORE;
      LOBTotalBidVolumePercent.LineWidth = 2;
      LOBTotalBidVolumePercent.PrimaryColor = COLOR_YELLOW;

      LOBTotalAskVolumePercent.Name = "LOBTotalBidVolumePercent";
      LOBTotalAskVolumePercent.DrawStyle = DRAWSTYLE_IGNORE;
      LOBTotalAskVolumePercent.LineWidth = 2;
      LOBTotalAskVolumePercent.PrimaryColor = COLOR_YELLOW;

	  LOBDeltaVolume.Name = "LOBDelta Volume";
      LOBDeltaVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBDeltaVolume.LineWidth = 2;
      LOBDeltaVolume.PrimaryColor = RGB_Cyan; // cyan

      LOBCurBidVolume.Name = "LOBCurAskVolume";
      LOBCurBidVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBCurBidVolume.LineWidth = 5;
      LOBCurBidVolume.PrimaryColor = RGB_LightCyan; // light cyan

      LOBCurAskVolume.Name = "LOBCurBidVolume";
      LOBCurAskVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBCurAskVolume.LineWidth = 5;
      LOBCurAskVolume.PrimaryColor = RGB_LightRed; // light red

      CurPrice.Name = "Current Price";
      CurPrice.DrawStyle = DRAWSTYLE_IGNORE;
      CurPrice.LineWidth = 5;
      CurPrice.PrimaryColor = COLOR_WHITE;

	  LOBValidFlag.Name = "LOB Data Valid";
      LOBValidFlag.DrawStyle = DRAWSTYLE_IGNORE;
      LOBValidFlag.LineWidth = 1;
      LOBValidFlag.PrimaryColor = COLOR_BLACK;

      UBHi.Name = "Unfinished Business Hi";
      UBHi.DrawStyle = DRAWSTYLE_IGNORE;
      UBHi.LineWidth = 1;
      UBHi.PrimaryColor = RGB_Orange; // orange
	  UBHi.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;
      UBHi.ShortName = "UFBT";
	  
      UBLo.Name = "Unfinished Business Low";
      UBLo.DrawStyle = DRAWSTYLE_IGNORE;
      UBLo.LineWidth = 1;
      UBLo.PrimaryColor = RGB_Orange; // orange
	  UBLo.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;
      UBLo.ShortName = "UFBB";
	  
	  NumBuyImb.Name = "Number of Buy Imbalances";
      NumBuyImb.DrawStyle = DRAWSTYLE_IGNORE;
      NumBuyImb.LineWidth = 2;
      NumBuyImb.PrimaryColor = COLOR_GREEN;

	  NumSellImb.Name = "Number of Sell Imbalances";
      NumSellImb.DrawStyle = DRAWSTYLE_IGNORE;
      NumSellImb.LineWidth = 2;
      NumSellImb.PrimaryColor = RGB_LightRed; // light red

	  NumBuyZP.Name = "Number of Buy Small Prints";
      NumBuyZP.DrawStyle = DRAWSTYLE_IGNORE;
      NumBuyZP.LineWidth = 2;
      NumBuyZP.PrimaryColor = COLOR_GREEN;

	  NumSellZP.Name = "Number of Sell Small Prints";
      NumSellZP.DrawStyle = DRAWSTYLE_IGNORE;
      NumSellZP.LineWidth = 2;
      NumSellZP.PrimaryColor = RGB_LightRed; // light red

	  /*
	  BuyExhaust.Name = "Buy Exhaustion";
	  BuyExhaust.DrawStyle = DRAWSTYLE_DASH;
	  BuyExhaust.LineWidth = 2;
	  BuyExhaust.PrimaryColor = RGB_Sand; // sand
	  BuyExhaust.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;
	  BuyExhaust.ShortName = "BUYEX";

	  SellExhaust.Name = "Sell Exhaustion";
	  SellExhaust.DrawStyle = DRAWSTYLE_DASH;
	  SellExhaust.LineWidth = 2;
	  SellExhaust.PrimaryColor = RGB_LightTeal; // light teal
	  SellExhaust.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;
	  SellExhaust.ShortName = "SELLEX";
	  */

	  BuyExhaust.Name = "Buy Exhaustion";
	  BuyExhaust.DrawStyle = DRAWSTYLE_DASH;
	  BuyExhaust.LineWidth = 2;
	  BuyExhaust.PrimaryColor = RGB_Sand; // sand
	  BuyExhaust.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;
	  BuyExhaust.ShortName = "BUYEX";

	  SellExhaust.Name = "Sell Exhaustion";
	  SellExhaust.DrawStyle = DRAWSTYLE_DASH;
	  SellExhaust.LineWidth = 2;
	  SellExhaust.PrimaryColor = RGB_LightTeal; // light teal
	  SellExhaust.LineLabel = LL_DISPLAY_NAME | LL_NAME_REVERSE_COLORS | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_RIGHT;
	  SellExhaust.ShortName = "SELLEX";


	  BuyExhRatio.Name = "Buy Exhaustion Ratio";
	  BuyExhRatio.DrawStyle = DRAWSTYLE_IGNORE;
	  BuyExhRatio.LineWidth = 2;
	  BuyExhRatio.PrimaryColor = COLOR_GREEN;

	  SellExhRatio.Name = "Sell Exhaustion Ratio";
	  SellExhRatio.DrawStyle = DRAWSTYLE_IGNORE;
	  SellExhRatio.LineWidth = 2;
	  SellExhRatio.PrimaryColor = RGB_LightRed; // light red

	  /*
	  SignalsInTopTail.Name = "Number of signals in top tail";
	  SignalsInTopTail.DrawStyle = DRAWSTYLE_IGNORE;
	  SignalsInTopTail.LineWidth = 2;
	  SignalsInTopTail.PrimaryColor = COLOR_GREEN;

	  SignalsInBotTail.Name = "Number of signals in bottom tail";
	  SignalsInBotTail.DrawStyle = DRAWSTYLE_IGNORE;
	  SignalsInBotTail.LineWidth = 2;
	  SignalsInBotTail.PrimaryColor = RGB_LightRed; // light red
	  */

/*
	  SCIndex.Name = "Current Index";
	  SCIndex.DrawStyle = DRAWSTYLE_IGNORE;
	  SCIndex.LineWidth = 2;
	  SCIndex.PrimaryColor = COLOR_WHITE;
*/

	  return;
   }

   if ((int)sc.VolumeAtPriceForBars->GetNumberOfBars() < sc.ArraySize)
      return;

   unsigned int xMaxBidVolume = 0, xMaxAskVolume = 0, MaxVolume = 0;
   unsigned int TBV = 0, TAV = 0; // per bar
   unsigned long TV = 0L; // Total volume for the bar
   float PTV = 0.0; // Price x total volume, used for VWAP calculation
   float xVWAPPrice;
   int DV = 0;
   float MaxBidVolumePrice = 0.0, MaxAskVolumePrice = 0.0, MaxVolumePrice = 0.0, TOT = 0.0;
   float ZPBPrice = 0.0, ZPAPrice = 0.0;
   float BIMBPriceHi = 0.0, AIMBPriceHi = 0.0;
   float BIMBPriceLo = 0.0, AIMBPriceLo = 0.0;
   float BIMBRatioHi = 0.0, AIMBRatioHi = 0.0;
   float BIMBRatioLo = 0.0, AIMBRatioLo = 0.0;
   float RVBTPrice = 0.0, RVATPrice = 0.0; // Reducing Volume Bid/Ask Top Price
   float RVBBPrice = 0.0, RVABPrice = 0.0; // Reducing Volume Bid/Ask Bottom Price
   unsigned int i;

   unsigned int BidArray[1000], AskArray[1000];
   float PriceArray[1000];
   unsigned int LOBBidArray[1000], LOBAskArray[1000];
   float LOBBidPriceArray[1000], LOBAskPriceArray[1000];

   int ImbRatio = ImbalanceRatio.GetInt();
   int MinBarSize = MinimumBarSize.GetInt();
   unsigned int CompLevels = CompareLevels.GetInt();
   unsigned int CompCount = 0;
   unsigned int TotalVolume = 0;
   int scratch = 0, InHighPinFlag = 0, InLowPinFlag = 0;
   float MyOpen = 0.0, MyHigh = 0.0, MyLow = 0.0, MyClose = 0.0;
   int xLOBAvgBidVolume = 0, xLOBAvgAskVolume = 0, xLOBAvgVolume = 0;

   int Count = sc.VolumeAtPriceForBars->GetSizeAtBarIndex(sc.Index);
   int LOBCount = min(sc.GetBidMarketDepthNumberOfLevels(), sc.GetAskMarketDepthNumberOfLevels());
   float xLOBMinBidVAP = 0.0;
   float xLOBMinAskVAP = 0.0;
   float xLOBMaxBidVAP = 0.0;
   float xLOBMaxAskVAP = 0.0;
   int xLOBMinBidVolume = 65530;
   int xLOBMinAskVolume = 65530;
   int xLOBMaxBidVolume = 0;
   int xLOBMaxAskVolume = 0;
   int xLOBTotalBidVolume = 0;
   int xLOBTotalAskVolume = 0;
   int xLOBTotalBidVolumePercent = 0;
   int xLOBTotalAskVolumePercent = 0;
   int xLOBDeltaVolume = 0;
   int xLOBCurBidVolume = 0;
   int xLOBCurAskVolume = 0;
   float xCurPrice = 0.0;
   int CountBuyImb = 0, CountSellImb = 0;
   int CountBuyZP = 0, CountSellZP = 0;
   int DebugFlag = 0;
   
   s_VolumeAtPriceV2* p_VolumeAtPriceAtIndex = 0;
   s_MarketDepthEntry DepthEntry;
   
   if (DebugLog.GetInt() == 1)
   {
	   DebugFlag = 1;
	   DebugLog.SetInt(0);
   }

   if (DebugLog.GetInt() == 1)
  {
	  sprintf(scratchmsg, "LOBCount=%d, MarketDepthLimit=%d, GetBidMarketDepthNumberOfLevels=%d, GetAskMarketDepthNumberOfLevels=%d\n",
		LOBCount, MarketDepthLimit.GetInt(), sc.GetBidMarketDepthNumberOfLevels(), sc.GetAskMarketDepthNumberOfLevels());
	  sc.AddMessageToLog(scratchmsg, 1);
  }
   
   LOBValidFlag[sc.Index] = 1;
   // If we have incomplete data, set LOBValidFlag = 0
   if (sc.GetBidMarketDepthNumberOfLevels() != sc.GetAskMarketDepthNumberOfLevels()) LOBValidFlag[sc.Index] = 0;
   if (LOBCount < MarketDepthLimit.GetInt()) LOBValidFlag[sc.Index] = 0;
   if (LOBCount > MarketDepthLimit.GetInt()) LOBCount = MarketDepthLimit.GetInt();
   
	/*
	for (i=0; i<1000; i++)
	{
		BidArray[i] = AskArray[i] = LOBBidArray[i] = LOBAskArray[i] = 0;
		PriceArray[i] = LOBBidPriceArray[i] = LOBAskPriceArray[i] = 0.0;
	}
	*/

	// for (i=0; i<sc.ArraySize; i++) TotalVolume =+ sc.Volume[i];
	// AverageVolume[sc.Index] = TotalVolume / sc.ArraySize;
	
	/* Get active filled order data */

	/*
    Count = 4, MinBarSize = 3, Index = 3, BidVolume = 0, AskVolume = 51, Price = 4508.50
	Count = 4, MinBarSize = 3, Index = 2, BidVolume = 83, AskVolume = 153, Price = 4508.25
	Count = 4, MinBarSize = 3, Index = 1, BidVolume = 183, AskVolume = 141, Price = 4508.00
	Count = 4, MinBarSize = 3, Index = 0, BidVolume = 33, AskVolume = 0, Price = 4507.75
	*/

	
	TV = 0L;
	PTV = 0.0;
	for (int ElementIndex = Count - 1; ElementIndex > -1; ElementIndex--)
	{
	  // s_VolumeAtPriceV2* p_VolumeAtPriceAtIndex = 0;
      sc.VolumeAtPriceForBars->GetVAPElementAtIndex(sc.Index, ElementIndex, &p_VolumeAtPriceAtIndex);

      if (p_VolumeAtPriceAtIndex)
      {
		  BidArray[ElementIndex] = p_VolumeAtPriceAtIndex->BidVolume;
		  AskArray[ElementIndex] = p_VolumeAtPriceAtIndex->AskVolume;
		  PriceArray[ElementIndex] = p_VolumeAtPriceAtIndex->PriceInTicks * sc.TickSize;
		  PTV += (p_VolumeAtPriceAtIndex->PriceInTicks * sc.TickSize) * (p_VolumeAtPriceAtIndex->BidVolume + p_VolumeAtPriceAtIndex->AskVolume);
		  TV += p_VolumeAtPriceAtIndex->BidVolume + p_VolumeAtPriceAtIndex->AskVolume;
		  if (DebugLog.GetInt() == 1)
		  {
		  sprintf(scratchmsg, "Count=%d, MinBarSize=%d, Index=%d, BidVolume=%d, AskVolume=%d, Price=%.02f\n",
			Count, MinBarSize, ElementIndex, BidArray[ElementIndex], AskArray[ElementIndex], PriceArray[ElementIndex]);
		  sc.AddMessageToLog(scratchmsg, 1);
		  }
	  }
	}
	xVWAPPrice = PTV / TV;
	xCurPrice = PriceArray[0];

	/* Get LOB resting order data */
	for (int Level = LOBCount - 1; Level > -1; Level--)
	{
		sc.GetBidMarketDepthEntryAtLevel(DepthEntry, Level);
		LOBBidArray[Level] = DepthEntry.Quantity;
		LOBBidPriceArray[Level] = DepthEntry.Price;
		if (xLOBMaxBidVolume < DepthEntry.Quantity)
		{
			xLOBMaxBidVolume = DepthEntry.Quantity;
			xLOBMaxBidVAP = DepthEntry.Price;
		}
		if (xLOBMinBidVolume > DepthEntry.Quantity)
		{
			xLOBMinBidVolume = DepthEntry.Quantity;
			xLOBMinBidVAP = DepthEntry.Price;
		}
		xLOBTotalBidVolume += DepthEntry.Quantity;

		sc.GetAskMarketDepthEntryAtLevel(DepthEntry, Level);
		LOBAskArray[Level] = DepthEntry.Quantity;
		LOBAskPriceArray[Level] = DepthEntry.Price;
		if (xLOBMaxAskVolume < DepthEntry.Quantity)
		{
			xLOBMaxAskVolume = DepthEntry.Quantity;
			xLOBMaxAskVAP = DepthEntry.Price;
		}
		if (xLOBMinAskVolume > DepthEntry.Quantity)
		{
			xLOBMinAskVolume = DepthEntry.Quantity;
			xLOBMinAskVAP = DepthEntry.Price;
		}
		xLOBTotalAskVolume += DepthEntry.Quantity;

		  if (DebugLog.GetInt() == 1)
		  {
		  sprintf(scratchmsg, "LOB: Count=%d, Index=%d, BidVolume=%d, AskVolume=%d, BidPrice=%.02f, AskPrice=%.02f\n",
			LOBCount, Level, LOBBidArray[Level], LOBAskArray[Level], LOBBidPriceArray[Level], LOBAskPriceArray[Level]);
		  sc.AddMessageToLog(scratchmsg, 1);
		  }
	}
	xLOBAvgBidVolume = xLOBTotalBidVolume / LOBCount;
	xLOBAvgAskVolume = xLOBTotalAskVolume / LOBCount;
	xLOBAvgVolume = (xLOBAvgBidVolume + xLOBAvgAskVolume) / 2;
	xLOBCurBidVolume = LOBBidArray[0];
	xLOBCurAskVolume = LOBAskArray[0];

	ReducingVolumeBidBot[sc.Index] = ReducingVolumeBidTop[sc.Index] = ReducingVolumeAskBot[sc.Index] = ReducingVolumeAskTop[sc.Index] = 0.0;
	
	/*********************************************************
	 *  R E D U C I N G   V O L U M E   C O D E   S T A R T  *
	 *********************************************************/

	if (Count >= MinBarSize)
	{
		// if (BidArray[0] <= BidArray[1] && BidArray[1] <= BidArray[2]) // && AskArray[0] <= AskArray[1] && AskArray[1] <= AskArray[2])
		
		/*
		   Work from the bottom of the candle up, look for increasing volume as we go up. Make sure we're in a pin.
		   Each level up must have equal or increasing volume
		 */
		MyHigh = HIGH;
		MyLow = LOW;
		MyOpen = OPEN;
		MyClose = CLOSE;
		
		CompCount = 0;
		for (i=0; i<CompLevels-1; i++)
			if (BidArray[i] <= BidArray[i + 1] || AskArray[i] <= AskArray[i + 1]) CompCount++;

		InLowPinFlag = 0;
		if (((CLOSE <= OPEN && LOW < CLOSE && PriceArray[CompLevels-1] < CLOSE) || (OPEN <= CLOSE && LOW < OPEN && PriceArray[CompLevels-1] < OPEN))) InLowPinFlag = 1;
		
		if (DebugLog.GetInt() == 1)
		{
			sprintf(scratchmsg, "Working Orders Bid: Count=%d, MinBarSize=%d, CompCount=%d, CompLevels=%d, sc.Open=%f, sc.High=%f, sc.Low=%f, sc.Close=%f, PriceArray=%f, InHighPin=%d\n",
				Count, MinBarSize, CompCount, CompLevels, OPEN, HIGH, LOW, CLOSE, PriceArray[0],
				InLowPinFlag);
			sc.AddMessageToLog(scratchmsg, 1);
		}

		if (CompCount == CompLevels-1 /* && InLowPinFlag == 1 */)
		{
			ReducingVolumeBidBot[sc.Index] = PriceArray[0];
			ReducingVolumeBidTop[sc.Index] = PriceArray[CompLevels - 1];
		}
		
		// if (AskArray[Count - 1] <= AskArray[Count - 2] && AskArray[Count - 2] <= AskArray[Count - 3]) // && BidArray[Count - 1] <= BidArray[Count - 2] && BidArray[Count - 2] <= BidArray[Count - 3])

		/*
		   Work from the top of the candle down, look for increasing volume as we go down. Make sure we're in a pin.
		   Each level down must have equal or increasing volume
		 */
		CompCount = 0;
		for (i=Count-1; i>Count-CompLevels; i--)
			if (AskArray[i] <= AskArray[i-1] || BidArray[i] <= BidArray[i-1]) CompCount++;

		InHighPinFlag = 0;
		if (((CLOSE <= OPEN && HIGH > OPEN && PriceArray[Count-CompLevels] > OPEN) || (OPEN <= CLOSE && HIGH > CLOSE && PriceArray[Count-CompLevels] > CLOSE))) InHighPinFlag = 1;
		
		if (DebugLog.GetInt() == 1)
		{
			sprintf(scratchmsg, "Working Orders Ask: Count=%d, MinBarSize=%d, CompCount=%d, CompLevels=%d, sc.Open=%f, sc.High=%f, sc.Low=%f, sc.Close=%f, PriceArray=%f, InLowPin=%d\n",
				Count, MinBarSize, CompCount, CompLevels, OPEN, HIGH, LOW, CLOSE, PriceArray[Count - CompLevels],
				InHighPinFlag);
			sc.AddMessageToLog(scratchmsg, 1);
		}

		if (CompCount == CompLevels-1 /* && InHighPinFlag == 1 */)
		{
			ReducingVolumeAskBot[sc.Index] = PriceArray[Count - CompLevels];
			ReducingVolumeAskTop[sc.Index] = PriceArray[Count - 1];
		}

		if (DebugLog.GetInt() == 1)
		{
			sprintf(scratchmsg, "RVAT=%f, RVBT=%f\n", ReducingVolumeAskTop[sc.Index], ReducingVolumeBidTop[sc.Index]);
			sc.AddMessageToLog(scratchmsg, 1);
		}
	/*****************************************************
	 *  R E D U C I N G   V O L U M E   C O D E   E N D  *
	 *****************************************************/

	}
	
   /**********************************************************
    * Zero print and buy/sell imbalance detection code START *
	**********************************************************/
   
   AIMBPriceHi = BIMBPriceHi = AIMBPriceLo = BIMBPriceLo = 0.0;
   CountBuyImb = CountSellImb = 0;
   CountBuyZP = CountSellZP = 0;
   for (int ElementIndex = 0; ElementIndex < Count; ElementIndex++)
   {
	  // s_VolumeAtPriceV2* p_VolumeAtPriceAtIndex = 0;
      sc.VolumeAtPriceForBars->GetVAPElementAtIndex(sc.Index, ElementIndex, &p_VolumeAtPriceAtIndex);
	  ZPA[ElementIndex] = ZPB[ElementIndex] = 0; // Clear out zero prints

      if (p_VolumeAtPriceAtIndex && Count > 2)
      {
		  // sprintf(scratchmsg, "ElementIndex=%d, Price=%.02f\n", ElementIndex, p_VolumeAtPriceAtIndex->PriceInTicks * sc.TickSize);
		  // sc.AddMessageToLog(scratchmsg, 1);

		  if (p_VolumeAtPriceAtIndex->BidVolume > xMaxBidVolume)
		 {
			xMaxBidVolume = p_VolumeAtPriceAtIndex->BidVolume;
			MaxBidVolumePrice = p_VolumeAtPriceAtIndex->PriceInTicks * sc.TickSize;
		 }
         if (p_VolumeAtPriceAtIndex->AskVolume > xMaxAskVolume)
		 {
			xMaxAskVolume = p_VolumeAtPriceAtIndex->AskVolume;
			MaxAskVolumePrice = p_VolumeAtPriceAtIndex->PriceInTicks * sc.TickSize;
		 }
		 
         if (p_VolumeAtPriceAtIndex->Volume > MaxVolume)
		 {
			 MaxVolume = p_VolumeAtPriceAtIndex->Volume;
			 MaxVolumePrice = p_VolumeAtPriceAtIndex->PriceInTicks * sc.TickSize;
		 }
		 
		 // Flag zero prints. We only want to flag the first entry, but don't flag the zero prints at the end of the bar.
		 if (ElementIndex > 0 && ElementIndex < Count - 1 )
		 {
			 //AIMBPrice = 0.0;
			 //BIMBPrice = 0.0;
			 if (p_VolumeAtPriceAtIndex->BidVolume < TRIGGER && p_VolumeAtPriceAtIndex->AskVolume > 0)
			 {
				 if (ZPBPrice == 0.0) ZPB[sc.Index] = p_VolumeAtPriceAtIndex->PriceInTicks * sc.TickSize;
				 CountSellZP++;
			 }
			 if (p_VolumeAtPriceAtIndex->BidVolume > 0 && p_VolumeAtPriceAtIndex->AskVolume < TRIGGER)
			 {
				 if (ZPBPrice == 0.0) ZPA[sc.Index] = p_VolumeAtPriceAtIndex->PriceInTicks * sc.TickSize;
				 CountBuyZP++;
			 }
		 }
		 
		 // Buy Imbalance
		 // if (AskArray[ElementIndex] > (BidArray[ElementIndex-1] * ImbRatio) && AskArray[ElementIndex-1] == 0 && AskArray[ElementIndex] > BidArray[ElementIndex])
		 if (DebugLog.GetInt() == 1)
		 {
			 sprintf(scratchmsg, "IMB: Count=%d, ElementIndex=%d, AskArray+1=%d, BidArray=%d\n", Count, ElementIndex, AskArray[ElementIndex+1], BidArray[ElementIndex]);
			 sc.AddMessageToLog(scratchmsg, 1);
		 }
		 if (AskArray[ElementIndex+1] >= (BidArray[ElementIndex] * ImbRatio) && ElementIndex+1 < Count) // && AskArray[ElementIndex] > BidArray[ElementIndex])
		 {
			 if (DebugLog.GetInt() == 1)
			 {
				 sprintf(scratchmsg, "Buy Imbalance: AskVolume[%d] (%d) > BidVolume[%d] (%d) * %d (%d) | ElementIndex=%d, Count=%d\n",
				 ElementIndex+1, AskArray[ElementIndex+1], ElementIndex, BidArray[ElementIndex], ImbRatio, BidArray[ElementIndex] * ImbRatio, ElementIndex, Count);
				 sc.AddMessageToLog(scratchmsg, 1);
			 }
			 CountBuyImb++;
		 }
		 
		 // Sell Imbalance
		 // if ((AskArray[ElementIndex+1] * ImbRatio) < BidArray[ElementIndex] && BidArray[ElementIndex+1] == 0 && BidArray[ElementIndex] > AskArray[ElementIndex])
		 if ((BidArray[ElementIndex]) >= (AskArray[ElementIndex+1] * ImbRatio) && ElementIndex+1 < Count) // && AskArray[ElementIndex] < BidArray[ElementIndex])
		 {
			 if (DebugLog.GetInt() == 1)
			 {
				 sprintf(scratchmsg, "Sell Imbalance: (BidVolume[%d]) (%d) > AskVolume[%d] (%d) * %d (%d) | ElementIndex=%d, Count=%d\n",
				 ElementIndex, BidArray[ElementIndex], ElementIndex+1, AskArray[ElementIndex+1], ImbRatio, AskArray[ElementIndex+1] * ImbRatio, ElementIndex, Count);
				 sc.AddMessageToLog(scratchmsg, 1);
			 }
			 CountSellImb++;
		 }
		 // if (AIMBPriceHi == BIMBPriceHi) AIMBPriceHi = BIMBPriceHi = 0.0; // If imbalances at the same price, set to 0		 
		 // if (AIMBPriceLo == BIMBPriceLo) AIMBPriceLo = BIMBPriceLo = 0.0; // If imbalances at the same price, set to 0		 

		 TBV = TBV + p_VolumeAtPriceAtIndex->BidVolume;
		 TAV = TAV + p_VolumeAtPriceAtIndex->AskVolume;
      }
   }
   NumBuyZP[sc.Index] = CountBuyZP;
   NumSellZP[sc.Index] = CountSellZP;
   NumBuyImb[sc.Index] = CountBuyImb;
   NumSellImb[sc.Index] = CountSellImb;
   
   /* New code to calculate buy/sell imbalances at the top or bottom of a bar */
   
	AIMBPriceHi = BIMBPriceHi = AIMBPriceLo = BIMBPriceLo = AIMBRatioHi = BIMBRatioHi = AIMBRatioLo = BIMBRatioLo = 0.0;
   if (Count >= MinBarSize)
   {
		/* Check buy/sell imbalance at the top of the bar */
		for (int ElementIndex = Count - 3; ElementIndex < Count; ElementIndex++)
		{
			if (AskArray[ElementIndex] >= ((BidArray[ElementIndex-1] == 0 ? 1 : BidArray[ElementIndex - 1]) * ImbRatio))
			{
				AIMBPriceHi = PriceArray[ElementIndex];
				if (AskArray[ElementIndex] > 0 && BidArray[ElementIndex-1] > 0) AIMBRatioHi = AskArray[ElementIndex] / BidArray[ElementIndex-1];
			}
			if ((BidArray[ElementIndex-1]) >= ((AskArray[ElementIndex - 1] == 0 ? 1 : AskArray[ElementIndex - 1]) * ImbRatio))
			{
				BIMBPriceHi = PriceArray[ElementIndex-1];
				if (BidArray[ElementIndex-1] > 0 && AskArray[ElementIndex] > 0) BIMBRatioHi = BidArray[ElementIndex-1] / AskArray[ElementIndex];
			}
		}
		
		/* Now do it at the bottom of the bar */
		// Do we neeed to check for 0 at the bottom of the bar? FIXME
		for (int ElementIndex = 2; ElementIndex >= 0; ElementIndex--)
		{
			if (AskArray[ElementIndex+1] >= (BidArray[ElementIndex] * ImbRatio))
			{
				AIMBPriceLo = PriceArray[ElementIndex+1];
				if (AskArray[ElementIndex+1] > 0 && BidArray[ElementIndex] > 0) AIMBRatioLo = AskArray[ElementIndex+1] / BidArray[ElementIndex];
			}
			if ((BidArray[ElementIndex]) >= (AskArray[ElementIndex+1] * ImbRatio))
			{
				BIMBPriceLo = PriceArray[ElementIndex];
				if (BidArray[ElementIndex] > 0 && AskArray[ElementIndex+1] > 0) BIMBRatioLo = BidArray[ElementIndex] / AskArray[ElementIndex+1];
			}
		}
			
   }
   
   /********************************************************
	* Zero print and buy/sell imbalance detection code END *
	********************************************************/

	/* Unfinished Business */
   
   UBHi[sc.Index] = UBLo[sc.Index] = 0;
   if (AskArray[0] > 0 && BidArray[0] > 0) UBLo[sc.Index] = PriceArray[0];
   if (AskArray[Count - 1] > 0 && BidArray[Count - 1] > 0) UBHi[sc.Index] = PriceArray[Count - 1];
   /*
   if (UBHi[sc.Index] > 0 || UBLo[sc.Index] > 0)
   {
	   sprintf(scratchmsg, "UB: AA[0] = %d, BA[0] = %d, AA[%d] = %d, BA[%d] = %d\n", AskArray[0], BidArray[0], Count - 1, AskArray[Count - 1], Count - 1, BidArray[Count - 1]);
	   sc.AddMessageToLog(scratchmsg, 1);
   }
   */
   
   if (DebugLog.GetInt() == 1 && (CountBuyImb > 0 || CountSellImb > 0))
   {
	   sprintf(scratchmsg, "COUNTIMB: Buy Imb=%d, Sell Imb=%d, Buy ZP=%d, Sell ZP=%d\n", CountBuyImb, CountSellImb, CountBuyZP, CountSellZP);
	   sc.AddMessageToLog(scratchmsg, 1);
   }

   /**************************************
    * Buy/sell exhaustion detection code *
    **************************************/

   BuyExhaust[sc.Index] = SellExhaust[sc.Index] = BuyExhRatio[sc.Index] = SellExhRatio[sc.Index] = 0;
   if (Count > 1)
   {
	   BuyExhRatio[sc.Index] = ((float)AskArray[Count - 2] / (float)(AskArray[Count - 1] == 0 ? 1 : AskArray[Count - 1]));
	   SellExhRatio[sc.Index] = ((float)BidArray[1] / (float)(BidArray[0] == 0 ? 1 : BidArray[0]));

	   if (BuyExhRatio[sc.Index] >= ExhaustRatio.GetInt())
		   BuyExhaust[sc.Index] = PriceArray[Count - 1];
	   if (SellExhRatio[sc.Index] >= ExhaustRatio.GetInt())
		   SellExhaust[sc.Index] = PriceArray[0];
   }
   
   MaxVAP[sc.Index] = MaxVolumePrice;
   MaxBidVAP[sc.Index] = MaxBidVolumePrice;
   MaxAskVAP[sc.Index] = MaxAskVolumePrice;
//    MaxVAPPercent[sc.Index] = ((MaxVolumePrice - sc.Low[sc.Index]) / (sc.High[sc.Index] - sc.Low[sc.Index])) * 100.0;
   MaxVAPVolume[sc.Index] = MaxVolume;
   TotalBidVolume[sc.Index] = TBV;
   TotalAskVolume[sc.Index] = TAV;

   DV = (int)TAV - (int)TBV;
   DeltaVolume[sc.Index] = DV;
   //if (DV == 0) DeltaVolume.PrimaryColor = COLOR_YELLOW;
   //if (DV < 0) DeltaVolume.PrimaryColor = RGB_LightRed; // light red
   //if (DV > 0) DeltaVolume.PrimaryColor = COLOR_GREEN;
   
   TOT = TAV + TBV;
   /*
   TotalBidVolumePercent[sc.Index] = (TBV / TOT) * 100.0;
   TotalAskVolumePercent[sc.Index] = (TAV / TOT) * 100.0;
   */
   TotalBidVolumePerTick[sc.Index] = TBV / Count;
   TotalAskVolumePerTick[sc.Index] = TAV / Count;
   if (ZPAPrice > 0.0) ZPA[sc.Index] = ZPAPrice;
   if (ZPBPrice > 0.0) ZPB[sc.Index] = ZPBPrice;

   BuyImbalanceHi[sc.Index] = AIMBPriceHi;
   SellImbalanceHi[sc.Index] = BIMBPriceHi;
   BuyImbalanceLo[sc.Index] = AIMBPriceLo;
   SellImbalanceLo[sc.Index] = BIMBPriceLo;
   BuyImbalanceRatioHi[sc.Index] = AIMBRatioHi;
   SellImbalanceRatioHi[sc.Index] = BIMBRatioHi;
   BuyImbalanceRatioLo[sc.Index] = AIMBRatioLo;
   SellImbalanceRatioLo[sc.Index] = BIMBRatioLo;

   VolumePerTick[sc.Index] = TOT / Count;
   Candle_Size[sc.Index] = (sc.High[sc.Index] - sc.Low[sc.Index]) / sc.TickSize;
   AverageVolume[sc.Index] = (TBV + TAV) / Candle_Size[sc.Index];
   VWAPPrice[sc.Index] = xVWAPPrice;
   
   LOBMinBidVAP[sc.Index] = xLOBMinBidVAP;
   LOBMinAskVAP[sc.Index] = xLOBMinAskVAP;
   LOBMaxBidVAP[sc.Index] = xLOBMaxBidVAP;
   LOBMaxAskVAP[sc.Index] = xLOBMaxAskVAP;
   
   LOBMinBidVolume[sc.Index] = xLOBMinBidVolume;
   LOBMinAskVolume[sc.Index] = xLOBMinAskVolume;
   LOBMaxBidVolume[sc.Index] = xLOBMaxBidVolume;
   LOBMaxAskVolume[sc.Index] = xLOBMaxAskVolume;
   
   LOBTotalBidVolume[sc.Index] = xLOBTotalBidVolume;
   LOBTotalAskVolume[sc.Index] = xLOBTotalAskVolume;
   
   LOBAvgBidVolume[sc.Index] = xLOBAvgBidVolume;
   LOBAvgAskVolume[sc.Index] = xLOBAvgAskVolume;
   LOBAvgVolume[sc.Index] = xLOBAvgVolume;
   
   LOBTotalBidVolumePercent[sc.Index] = ((float)xLOBTotalBidVolume / (float)(xLOBTotalBidVolume + xLOBTotalAskVolume)) * 100.0;
   LOBTotalAskVolumePercent[sc.Index] = ((float)xLOBTotalAskVolume / (float)(xLOBTotalBidVolume + xLOBTotalAskVolume)) * 100.0;
   LOBDeltaVolume[sc.Index] = xLOBTotalBidVolume - xLOBTotalAskVolume; // Mislabeled
   
   MaxBidVolume[sc.Index] = xMaxBidVolume;
   MaxAskVolume[sc.Index] = xMaxAskVolume;
   
   LOBCurBidVolume[sc.Index] = xLOBCurBidVolume;
   LOBCurAskVolume[sc.Index] = xLOBCurAskVolume;
   CurPrice[sc.Index] = sc.Close[sc.Index];
//    SignalsInTopTail[sc.Index] = 0;
//    SignalsInBotTail[sc.Index] = 0;
//    SCIndex[sc.Index] = sc.Index;
   /*
   if (MaxBidVolume > MaxAskVolume)
	   MaxVAP.PrimaryColor[sc.Index] = COLOR_RED;
   if (MaxBidVolume < MaxAskVolume)
	   MaxVAP.PrimaryColor[sc.Index] = COLOR_GREEN;
   */
   /*
   if (DebugFlag = 1)
   {
	   DebugFlag = 0;
	   fopen("OrderFlow.csv", "w");
	   fprintf(fout, "Subgraph,Name,Value\n);");
	   for (i = 0; i < sgctr; i++)
		   fprintf(fout, "SG%d,\"%s\", \"%s\"\n", i+1, SCSubgraphRef[i][sc.Index].Name, SCSubgraphRef[i][sc.Index]);
	   fclose(fout);
   }
   */
}
