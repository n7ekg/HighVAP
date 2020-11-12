/* Date: 2012-05-21
   Version: 1.0
   Author: Ymmv
   http://www.bigmiketrading.com/sierra-chart-programming/20255-acsil-function-returns-price-highest-volume-within-bar.html
   
   This was the original code that only indicated where the highest volume was within a bar. I took it and heavily modified it.
   
   Copyright 2019, by Ed Carp. All rights reserved. Distribution without permission is prohibited.
*/

#include "sierrachart.h"
#include "scstudyfunctions.h"
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

char scratchmsg[255];
SCDLLName("High Volume At Price v0.4b") 

SCSFExport scsf_HighVAP(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MaxBidVAP = sc.Subgraph[0];
	SCSubgraphRef MaxAskVAP = sc.Subgraph[1];
	SCSubgraphRef MaxVAP = sc.Subgraph[2];
	SCSubgraphRef MaxVAPPercent = sc.Subgraph[3];
	SCSubgraphRef TotalBidVolume = sc.Subgraph[4];
	SCSubgraphRef TotalAskVolume = sc.Subgraph[5];
	SCSubgraphRef TotalBidVolumePercent = sc.Subgraph[6];
	SCSubgraphRef TotalAskVolumePercent = sc.Subgraph[7];
	SCSubgraphRef TotalBidVolumePerTick = sc.Subgraph[8];
	SCSubgraphRef TotalAskVolumePerTick = sc.Subgraph[9];
	SCSubgraphRef ZPB = sc.Subgraph[10];
	SCSubgraphRef ZPA = sc.Subgraph[11];
	SCSubgraphRef BuyImbalance = sc.Subgraph[12];
	SCSubgraphRef SellImbalance = sc.Subgraph[13];
	SCSubgraphRef AverageVolume = sc.Subgraph[14];
	SCSubgraphRef VolumePerTick = sc.Subgraph[15];
	SCSubgraphRef DeltaVolume = sc.Subgraph[16];
	SCSubgraphRef Candle_Size = sc.Subgraph[17];
	SCSubgraphRef ReducingVolumeBidTop = sc.Subgraph[18];
	SCSubgraphRef ReducingVolumeBidBot = sc.Subgraph[19];
	SCSubgraphRef ReducingVolumeAskTop = sc.Subgraph[20];
	SCSubgraphRef ReducingVolumeAskBot = sc.Subgraph[21];
	
	SCSubgraphRef LOBMinBidVAP = sc.Subgraph[22];
	SCSubgraphRef LOBMinAskVAP = sc.Subgraph[23];
	SCSubgraphRef LOBMaxBidVAP = sc.Subgraph[24];
	SCSubgraphRef LOBMaxAskVAP = sc.Subgraph[25];
	SCSubgraphRef LOBTotalBidVolume = sc.Subgraph[26];
	SCSubgraphRef LOBTotalAskVolume = sc.Subgraph[27];
	SCSubgraphRef LOBTotalBidVolumePercent = sc.Subgraph[28];
	SCSubgraphRef LOBTotalAskVolumePercent = sc.Subgraph[29];
	SCSubgraphRef LOBDeltaVolume = sc.Subgraph[30];
	SCSubgraphRef LOBMinBidVolume = sc.Subgraph[31];
	SCSubgraphRef LOBMinAskVolume = sc.Subgraph[32];
	SCSubgraphRef LOBMaxBidVolume = sc.Subgraph[33];
	SCSubgraphRef LOBMaxAskVolume = sc.Subgraph[34];
	SCSubgraphRef LOBValidFlag = sc.Subgraph[35];
	SCSubgraphRef LOBAvgBidVolume = sc.Subgraph[36];
	SCSubgraphRef LOBAvgAskVolume = sc.Subgraph[37];
	SCSubgraphRef LOBAvgVolume = sc.Subgraph[38];

	SCSubgraphRef MaxBidVolume = sc.Subgraph[39];
	SCSubgraphRef MaxAskVolume = sc.Subgraph[40];
	
	SCSubgraphRef LOBCurBidVolume = sc.Subgraph[41];
	SCSubgraphRef LOBCurAskVolume = sc.Subgraph[42];
	SCSubgraphRef CurPrice = sc.Subgraph[43];
	SCSubgraphRef VWAPPrice = sc.Subgraph[44];
	
	SCInputRef ImbalanceRatio = sc.Input[0];
	SCInputRef MinimumBarSize = sc.Input[1];
	SCInputRef CompareLevels = sc.Input[2];
	SCInputRef DebugLog = sc.Input[3];
	SCInputRef MarketDepthLimit = sc.Input[4];

	if (sc.HideStudy == 1)
		return;

   if (sc.SetDefaults)
   {
      // During development set this flag to 1, so the DLL can be modified. When development is done, set it to 0 to improve performance.
      sc.FreeDLL = 0;
	  sc.DrawStudyUnderneathMainPriceGraph = 1;

      sc.GraphName = "High Volume At Price v0.4b";
      sc.StudyDescription = "Display various statistics for each bar.";
      sc.AutoLoop = 1;
      sc.GraphRegion = 0;
      sc.ScaleRangeType = SCALE_SAMEASREGION;
      sc.MaintainVolumeAtPriceData = 1;

      MaxBidVAP.Name = "MaxBidVAP";
      MaxBidVAP.DrawStyle = DRAWSTYLE_DASH;
      MaxBidVAP.LineWidth = 2;
      MaxBidVAP.PrimaryColor = RGB(255,128,128); // light red;;

      MaxAskVAP.Name = "MaxAskVAP";
      MaxAskVAP.DrawStyle = DRAWSTYLE_DASH;
      MaxAskVAP.LineWidth = 2;
      MaxAskVAP.PrimaryColor = COLOR_GREEN;

      MaxVAP.Name = "MaxVAP";
      MaxVAP.DrawStyle = DRAWSTYLE_DASH;
      MaxVAP.LineWidth = 3;
      MaxVAP.PrimaryColor = COLOR_YELLOW;
	  MaxVAP.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_RIGHT;

      MaxVAPPercent.Name = "MaxVAPPercent";
      MaxVAPPercent.DrawStyle = DRAWSTYLE_IGNORE;
      MaxVAPPercent.LineWidth = 2;
      MaxVAPPercent.PrimaryColor = COLOR_YELLOW;

      TotalBidVolume.Name = "TotalBidVolume";
      TotalBidVolume.DrawStyle = DRAWSTYLE_IGNORE;
      TotalBidVolume.LineWidth = 2;
      TotalBidVolume.PrimaryColor = COLOR_YELLOW;

      TotalAskVolume.Name = "TotalAskVolume";
      TotalAskVolume.DrawStyle = DRAWSTYLE_IGNORE;
      TotalAskVolume.LineWidth = 2;
      TotalAskVolume.PrimaryColor = COLOR_YELLOW;

      TotalBidVolumePercent.Name = "TotalBidVolumePercent";
      TotalBidVolumePercent.DrawStyle = DRAWSTYLE_IGNORE;
      TotalBidVolumePercent.LineWidth = 2;
      TotalBidVolumePercent.PrimaryColor = COLOR_YELLOW;

      TotalAskVolumePercent.Name = "TotalAskVolumePercent";
      TotalAskVolumePercent.DrawStyle = DRAWSTYLE_IGNORE;
      TotalAskVolumePercent.LineWidth = 2;
      TotalAskVolumePercent.PrimaryColor = COLOR_YELLOW;

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
      MaxBidVolume.PrimaryColor = RGB(255,128,128); // light red;;

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
      ZPA.PrimaryColor = RGB(255,128,128); // light red;

      BuyImbalance.Name = "Buy Imbalance";
      BuyImbalance.DrawStyle = DRAWSTYLE_DASH;
      BuyImbalance.LineWidth = 2;
      BuyImbalance.PrimaryColor = RGB(0,255,255); // cyan
	  BuyImbalance.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_RIGHT;

      SellImbalance.Name = "Sell Imbalance";
      SellImbalance.DrawStyle = DRAWSTYLE_DASH;
      SellImbalance.LineWidth = 2;
      SellImbalance.PrimaryColor = RGB(255,128,128); // light red;
	  SellImbalance.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_RIGHT;
	  
	  ImbalanceRatio.Name = "Imbalance Ratio";
	  ImbalanceRatio.SetInt(4);
	  //ImbalanceRatio.SetIntLimits(1,8);
	  
	  MinimumBarSize.Name = "Minimum Size For Bar";
	  MinimumBarSize.SetInt(5);
	  MinimumBarSize.SetIntLimits(3,1000);
	  
	  CompareLevels.Name = "Minimum Compare Levels At Bar Extreme";
	  CompareLevels.SetInt(3);
	  CompareLevels.SetIntLimits(2,1000);
	  
	  DebugLog.Name = "Log Debug Data (1=Yes, 0=No)";
	  DebugLog.SetInt(0);
	  DebugLog.SetIntLimits(0,1);
	  
	  MarketDepthLimit.Name = "Limit To Levels Of Market Depth Analysis";
	  MarketDepthLimit.SetInt(7);
	  MarketDepthLimit.SetIntLimits(1,999);
	  
	  VolumePerTick.Name = "Volume Per Tick";
      VolumePerTick.DrawStyle = DRAWSTYLE_IGNORE;
      VolumePerTick.LineWidth = 2;
      VolumePerTick.PrimaryColor = RGB(0,255,255); // cyan

	  DeltaVolume.Name = "Delta Volume";
      DeltaVolume.DrawStyle = DRAWSTYLE_IGNORE;
      DeltaVolume.LineWidth = 2;
      DeltaVolume.PrimaryColor = RGB(0,255,255); // cyan

      VWAPPrice.Name = "VWAP Price";
      VWAPPrice.DrawStyle = DRAWSTYLE_DASH;
      VWAPPrice.LineWidth = 2;
      VWAPPrice.PrimaryColor = RGB(255,0,255); // purple/magenta

	  Candle_Size.Name = "Candle Size";
      Candle_Size.DrawStyle = DRAWSTYLE_IGNORE;
      Candle_Size.LineWidth = 2;
      Candle_Size.PrimaryColor = RGB(0,255,255); // cyan
	  
	  ReducingVolumeBidTop.Name = "Reducing Volume Bid Top";
      ReducingVolumeBidTop.DrawStyle = DRAWSTYLE_FILL_RECTANGLE_TOP;
      ReducingVolumeBidTop.LineWidth = 2;
      ReducingVolumeBidTop.PrimaryColor = RGB(0,128,0); // dark green

	  ReducingVolumeBidBot.Name = "Reducing Volume Bid Bottom";
      ReducingVolumeBidBot.DrawStyle = DRAWSTYLE_FILL_RECTANGLE_BOTTOM;
      ReducingVolumeBidBot.LineWidth = 2;
      ReducingVolumeBidBot.PrimaryColor = RGB(0,128,0); // dark green

	  ReducingVolumeAskTop.Name = "Reducing Volume Ask Top";
      ReducingVolumeAskTop.DrawStyle = DRAWSTYLE_FILL_RECTANGLE_TOP;
      ReducingVolumeAskTop.LineWidth = 2;
      ReducingVolumeAskTop.PrimaryColor = RGB(128,64,64); // dark red

	  ReducingVolumeAskBot.Name = "Reducing Volume Ask Bottom";
      ReducingVolumeAskBot.DrawStyle = DRAWSTYLE_FILL_RECTANGLE_BOTTOM;
      ReducingVolumeAskBot.LineWidth = 2;
      ReducingVolumeAskBot.PrimaryColor = RGB(128,64,64); // dark red

      LOBMinBidVAP.Name = "LOBMinBidVAP";
      LOBMinBidVAP.DrawStyle = DRAWSTYLE_IGNORE;
      LOBMinBidVAP.LineWidth = 2;
      LOBMinBidVAP.PrimaryColor = RGB(255,128,128); // light red

      LOBMinAskVAP.Name = "LOBMinAskVAP";
      LOBMinAskVAP.DrawStyle = DRAWSTYLE_IGNORE;
      LOBMinAskVAP.LineWidth = 2;
      LOBMinAskVAP.PrimaryColor = COLOR_GREEN;

      LOBMaxBidVAP.Name = "LOBMaxBidVAP";
      LOBMaxBidVAP.DrawStyle = DRAWSTYLE_LINE;
      LOBMaxBidVAP.LineWidth = 5;
      LOBMaxBidVAP.PrimaryColor = RGB(128,255,255); // light cyan

      LOBMaxAskVAP.Name = "LOBMaxAskVAP";
      LOBMaxAskVAP.DrawStyle = DRAWSTYLE_LINE;
      LOBMaxAskVAP.LineWidth = 5;
      LOBMaxAskVAP.PrimaryColor = RGB(255,128,128); // light red

      LOBMinBidVolume.Name = "LOBMinBidVolume";
      LOBMinBidVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBMinBidVolume.LineWidth = 2;
      LOBMinBidVolume.PrimaryColor = RGB(255,128,128); // light red

      LOBMinAskVolume.Name = "LOBMinAskVolume";
      LOBMinAskVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBMinAskVolume.LineWidth = 2;
      LOBMinAskVolume.PrimaryColor = COLOR_GREEN;

      LOBMaxBidVolume.Name = "LOBMaxBidVolume";
      LOBMaxBidVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBMaxBidVolume.LineWidth = 5;
      LOBMaxBidVolume.PrimaryColor = RGB(128,255,255); // light cyan

      LOBMaxAskVolume.Name = "LOBMaxAskVolume";
      LOBMaxAskVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBMaxAskVolume.LineWidth = 5;
      LOBMaxAskVolume.PrimaryColor = RGB(255,128,128); // light red

      LOBAvgBidVolume.Name = "LOBAvgBidVolume";
      LOBAvgBidVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBAvgBidVolume.LineWidth = 5;
      LOBAvgBidVolume.PrimaryColor = RGB(128,255,255); // light cyan

      LOBAvgAskVolume.Name = "LOBAvgAskVolume";
      LOBAvgAskVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBAvgAskVolume.LineWidth = 5;
      LOBAvgAskVolume.PrimaryColor = RGB(255,128,128); // light red

      LOBAvgVolume.Name = "LOBAvgVolume";
      LOBAvgVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBAvgVolume.LineWidth = 5;
      LOBAvgVolume.PrimaryColor = RGB(255,128,128); // light red

      LOBTotalBidVolume.Name = "LOBTotalBidVolume";
      LOBTotalBidVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBTotalBidVolume.LineWidth = 2;
      LOBTotalBidVolume.PrimaryColor = COLOR_YELLOW;

      LOBTotalAskVolume.Name = "LOBTotalAskVolume";
      LOBTotalAskVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBTotalAskVolume.LineWidth = 2;
      LOBTotalAskVolume.PrimaryColor = COLOR_YELLOW;

      LOBTotalBidVolumePercent.Name = "LOBTotalBidVolumePercent";
      LOBTotalBidVolumePercent.DrawStyle = DRAWSTYLE_IGNORE;
      LOBTotalBidVolumePercent.LineWidth = 2;
      LOBTotalBidVolumePercent.PrimaryColor = COLOR_YELLOW;

      LOBTotalAskVolumePercent.Name = "LOBTotalAskVolumePercent";
      LOBTotalAskVolumePercent.DrawStyle = DRAWSTYLE_IGNORE;
      LOBTotalAskVolumePercent.LineWidth = 2;
      LOBTotalAskVolumePercent.PrimaryColor = COLOR_YELLOW;

	  LOBDeltaVolume.Name = "LOBDelta Volume";
      LOBDeltaVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBDeltaVolume.LineWidth = 2;
      LOBDeltaVolume.PrimaryColor = RGB(0,255,255); // cyan

      LOBCurBidVolume.Name = "LOBCurBidVolume";
      LOBCurBidVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBCurBidVolume.LineWidth = 5;
      LOBCurBidVolume.PrimaryColor = RGB(128,255,255); // light cyan

      LOBCurAskVolume.Name = "LOBCurAskVolume";
      LOBCurAskVolume.DrawStyle = DRAWSTYLE_IGNORE;
      LOBCurAskVolume.LineWidth = 5;
      LOBCurAskVolume.PrimaryColor = RGB(255,128,128); // light red

      CurPrice.Name = "Current Price";
      CurPrice.DrawStyle = DRAWSTYLE_IGNORE;
      CurPrice.LineWidth = 5;
      CurPrice.PrimaryColor = COLOR_WHITE;

	  LOBValidFlag.Name = "LOB Data Valid";
      LOBValidFlag.DrawStyle = DRAWSTYLE_IGNORE;
      LOBValidFlag.LineWidth = 1;
      LOBValidFlag.PrimaryColor = COLOR_BLACK;

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
   float BIMBPrice = 0.0, AIMBPrice = 0.0;
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
   
   s_VolumeAtPriceV2* p_VolumeAtPriceAtIndex = 0;
   s_MarketDepthEntry DepthEntry;
   
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
   
	// sprintf(scratchmsg, "Count=%d\n", Count);
	// sc.AddMessageToLog(scratchmsg, 1);

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

// LOB: Count=10, Index=9, BidVolume=159, AskVolume=87, BidPrice=3091.75, AskPrice=3096.50
// LOB: Count=10, Index=8, BidVolume=74, AskVolume=75, BidPrice=3092.00, AskPrice=3096.25
// LOB: Count=10, Index=7, BidVolume=69, AskVolume=120, BidPrice=3092.25, AskPrice=3096.00
// LOB: Count=10, Index=6, BidVolume=70, AskVolume=77, BidPrice=3092.50, AskPrice=3095.75
// LOB: Count=10, Index=5, BidVolume=51, AskVolume=73, BidPrice=3092.75, AskPrice=3095.50
// LOB: Count=10, Index=4, BidVolume=53, AskVolume=46, BidPrice=3093.00, AskPrice=3095.25
// LOB: Count=10, Index=3, BidVolume=67, AskVolume=73, BidPrice=3093.25, AskPrice=3095.00
// LOB: Count=10, Index=2, BidVolume=106, AskVolume=88, BidPrice=3093.50, AskPrice=3094.75
// LOB: Count=10, Index=1, BidVolume=59, AskVolume=50, BidPrice=3093.75, AskPrice=3094.50
// LOB: Count=10, Index=0, BidVolume=47, AskVolume=35, BidPrice=3094.00, AskPrice=3094.25

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


// Count = 13
// Index=12, BidVolume=0, AskVolume=6, Price=3052.00
// Index=11, BidVolume=14, AskVolume=9, Price=3051.75
// Index=10, BidVolume=31, AskVolume=164, Price=3051.50
// Index=9, BidVolume=30, AskVolume=59, Price=3051.25
// Index=8, BidVolume=34, AskVolume=17, Price=3051.00
// Index=7, BidVolume=14, AskVolume=23, Price=3050.75
// Index=6, BidVolume=38, AskVolume=70, Price=3050.50
// Index=5, BidVolume=6, AskVolume=11, Price=3050.25
// Index=4, BidVolume=14, AskVolume=27, Price=3050.00
// Index=3, BidVolume=20, AskVolume=42, Price=3049.75
// Index=2, BidVolume=37, AskVolume=12, Price=3049.50
// Index=1, BidVolume=18, AskVolume=19, Price=3049.25
// Index=0, BidVolume=1, AskVolume=0, Price=3049.00

// Count = 10
// Index=9, BidVolume=0, AskVolume=259, Price=3071.75
// Index=8, BidVolume=54, AskVolume=80, Price=3071.50
// Index=7, BidVolume=64, AskVolume=61, Price=3071.25
// Index=6, BidVolume=22, AskVolume=54, Price=3071.00
// Index=5, BidVolume=29, AskVolume=14, Price=3070.75
// Index=4, BidVolume=26, AskVolume=8, Price=3070.50
// Index=3, BidVolume=22, AskVolume=24, Price=3070.25
// Index=2, BidVolume=15, AskVolume=12, Price=3070.00
// Index=1, BidVolume=0, AskVolume=3, Price=3069.75
// Index=0, BidVolume=1, AskVolume=20, Price=3069.50
	
	ReducingVolumeBidBot[sc.Index] = ReducingVolumeBidTop[sc.Index] = ReducingVolumeAskBot[sc.Index] = ReducingVolumeAskTop[sc.Index] = 0;
	
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
		/* MyOpen == 3063.50 && MyHigh == 3067.25 && MyLow == 3063.00 && MyClose == 3065.75 */
		/* MyOpen == 3066.00 && MyHigh == 3068.00 && MyLow == 3065.50 && MyClose == 3066.50 */
		/* MyOpen == 3066.25 && MyHigh == 3066.75 && MyLow == 3062.75 && MyClose == 3064.25 */
		
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
			// ReducingVolumeAskBot[sc.Index] = 0;
			// ReducingVolumeAskTop[sc.Index] = 0;
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
			// ReducingVolumeBidBot[sc.Index] = 0;
			// ReducingVolumeBidTop[sc.Index] = 0;
		}

	/*****************************************************
	 *  R E D U C I N G   V O L U M E   C O D E   E N D  *
	 *****************************************************/

	}
	
   /**********************************************************
    * Zero print and buy/sell imbalance detection code START *
	**********************************************************/
   
   AIMBPrice = BIMBPrice = 0.0;
   for (int ElementIndex = 0; ElementIndex < Count; ElementIndex++)
   {
	  // s_VolumeAtPriceV2* p_VolumeAtPriceAtIndex = 0;
      sc.VolumeAtPriceForBars->GetVAPElementAtIndex(sc.Index, ElementIndex, &p_VolumeAtPriceAtIndex);
	  ZPA[ElementIndex] = ZPB[ElementIndex] = 0; // Clear out zero prints

      if (p_VolumeAtPriceAtIndex)
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
		 // if (p_VolumeAtPriceAtIndex->BidVolume < TRIGGER && p_VolumeAtPriceAtIndex->AskVolume > 0 && ElementIndex > 0 && ElementIndex < Count - 1 && ZPBPrice == 0.0) ZPBPrice = p_VolumeAtPriceAtIndex->PriceInTicks * sc.TickSize;
		 // if (p_VolumeAtPriceAtIndex->BidVolume > 0 && p_VolumeAtPriceAtIndex->AskVolume < TRIGGER && ElementIndex > 0 && ElementIndex < Count - 1 && ZPAPrice == 0.0) ZPAPrice = p_VolumeAtPriceAtIndex->PriceInTicks * sc.TickSize;
		 if (ElementIndex > 0 && ElementIndex < Count - 1 )
		 {
			 //AIMBPrice = 0.0;
			 //BIMBPrice = 0.0;
			 if (p_VolumeAtPriceAtIndex->BidVolume < TRIGGER && p_VolumeAtPriceAtIndex->AskVolume > 0 && ZPBPrice == 0.0) ZPB[sc.Index] = p_VolumeAtPriceAtIndex->PriceInTicks * sc.TickSize;
			 if (p_VolumeAtPriceAtIndex->BidVolume > 0 && p_VolumeAtPriceAtIndex->AskVolume < TRIGGER && ZPAPrice == 0.0) ZPA[sc.Index] = p_VolumeAtPriceAtIndex->PriceInTicks * sc.TickSize;
		 }
		 // if (BIMBPrice == 0.0 && p_VolumeAtPriceAtIndex->AskVolume > (p_VolumeAtPriceAtIndex->BidVolume * ImbRatio)) BIMB[sc.Index] = p_VolumeAtPriceAtIndex->PriceInTicks * sc.TickSize;
		 
		 // Buy Imbalance
		 // if (AskArray[ElementIndex] > (BidArray[ElementIndex-1] * ImbRatio) && AskArray[ElementIndex-1] == 0 && AskArray[ElementIndex] > BidArray[ElementIndex])
		 if (AskArray[ElementIndex+1] > (BidArray[ElementIndex] * ImbRatio) && ElementIndex < 2) // && AskArray[ElementIndex] > BidArray[ElementIndex])
		 {
			 if (DebugLog.GetInt() == 1)
			 {
				 sprintf(scratchmsg, "Buy Imbalance: AskVolume[%d] (%d) > BidVolume[%d] (%d) * %d (%d)\n",
				 ElementIndex+1, AskArray[ElementIndex+1], ElementIndex, BidArray[ElementIndex], ImbRatio, BidArray[ElementIndex] * ImbRatio);
				 sc.AddMessageToLog(scratchmsg, 1);
			 }
			 AIMBPrice = PriceArray[ElementIndex+1];
			 // BIMBPrice = 0.0;
		 }
		 
		 // Sell Imbalance
		 // if ((AskArray[ElementIndex+1] * ImbRatio) < BidArray[ElementIndex] && BidArray[ElementIndex+1] == 0 && BidArray[ElementIndex] > AskArray[ElementIndex])
		 if ((BidArray[ElementIndex]) > (AskArray[ElementIndex+1] * ImbRatio) && ElementIndex < Count - 1 && ElementIndex > Count - 3) // && AskArray[ElementIndex] < BidArray[ElementIndex])
		 {
			 if (DebugLog.GetInt() == 1)
			 {
				 sprintf(scratchmsg, "Sell Imbalance: (BidVolume[%d]) (%d) > AskVolume[%d] (%d) * %d (%d)\n",
				 ElementIndex, BidArray[ElementIndex], ElementIndex+1, AskArray[ElementIndex+1], ImbRatio, ElementIndex+1, AskArray[ElementIndex+1] * ImbRatio);
				 sc.AddMessageToLog(scratchmsg, 1);
			 }
			 BIMBPrice = PriceArray[ElementIndex];
			 // AIMBPrice = 0.0;
		 }
		 if (AIMBPrice == BIMBPrice) AIMBPrice = BIMBPrice = 0.0; // If imbalances at the same price, set to 0		 

		 TBV = TBV + p_VolumeAtPriceAtIndex->BidVolume;
		 TAV = TAV + p_VolumeAtPriceAtIndex->AskVolume;
      }
   }
   /********************************************************
    * Zero print and buy/sell imbalance detection code END *
	********************************************************/
   


   	/*
	sprintf(scratchmsg, "BidV=%d, AskV=%d, MaxV=%d\n", MaxBidVolume, MaxAskVolume, MaxVolume);
	sc.AddMessageToLog(scratchmsg, 1);
   	sprintf(scratchmsg, "BidVPrice=%f, AskVPrice=%f, MaxVPrice=%f\n", MaxBidVolumePrice, MaxAskVolumePrice, MaxVolumePrice);
	sc.AddMessageToLog(scratchmsg, 1);
	*/

   MaxVAP[sc.Index] = MaxVolumePrice;
   MaxBidVAP[sc.Index] = MaxBidVolumePrice;
   MaxAskVAP[sc.Index] = MaxAskVolumePrice;
   MaxVAPPercent[sc.Index] = ((MaxVolumePrice - sc.Low[sc.Index]) / (sc.High[sc.Index] - sc.Low[sc.Index])) * 100.0;
   TotalBidVolume[sc.Index] = TBV;
   TotalAskVolume[sc.Index] = TAV;
   DV = (int)TAV - (int)TBV;
   DeltaVolume[sc.Index] = DV;
   //if (DV == 0) DeltaVolume.PrimaryColor = COLOR_YELLOW;
   //if (DV < 0) DeltaVolume.PrimaryColor = RGB(255,128,128); // light red
   //if (DV > 0) DeltaVolume.PrimaryColor = COLOR_GREEN;
   TOT = TAV + TBV;
   TotalBidVolumePercent[sc.Index] = (TBV / TOT) * 100.0;
   TotalAskVolumePercent[sc.Index] = (TAV / TOT) * 100.0;
   TotalBidVolumePerTick[sc.Index] = TBV / Count;
   TotalAskVolumePerTick[sc.Index] = TAV / Count;
   if (ZPAPrice > 0.0) ZPA[sc.Index] = ZPAPrice;
   if (ZPBPrice > 0.0) ZPB[sc.Index] = ZPBPrice;
   // BuyImbalance[sc.Index] = SellImbalance[sc.Index] = 0.0;
   BuyImbalance[sc.Index] = AIMBPrice;
   SellImbalance[sc.Index] = BIMBPrice;
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
   LOBDeltaVolume[sc.Index] = xLOBTotalAskVolume - xLOBTotalBidVolume;
   
   MaxBidVolume[sc.Index] = xMaxBidVolume;
   MaxAskVolume[sc.Index] = xMaxAskVolume;
   
   LOBCurBidVolume[sc.Index] = xLOBCurBidVolume;
   LOBCurAskVolume[sc.Index] = xLOBCurAskVolume;
   CurPrice[sc.Index] = sc.Close[sc.Index];
   /*
   if (MaxBidVolume > MaxAskVolume)
	   MaxVAP.PrimaryColor[sc.Index] = COLOR_RED;
   if (MaxBidVolume < MaxAskVolume)
	   MaxVAP.PrimaryColor[sc.Index] = COLOR_GREEN;
   */
}
