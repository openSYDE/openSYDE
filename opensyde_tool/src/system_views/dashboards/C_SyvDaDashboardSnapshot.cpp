//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Snapshot of dashboard items

   Snapshot of dashboard items

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SyvDaDashboardSnapshot.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaDashboardSnapshot::C_SyvDaDashboardSnapshot(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply position offset

   \param[in] orc_NewPos Offset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSnapshot::SetDataPositionOffset(const QPointF & orc_NewPos)
{
   C_PuiBsElements::SetDataPositionOffset(orc_NewPos);
   //Labels
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_Labels.size(); ++u32_ItItem)
   {
      C_PuiSvDbLabel & rc_Data = this->mc_Labels[u32_ItItem];
      rc_Data.c_UIPosition += orc_NewPos;
   }
   //Charts
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_Charts.size(); ++u32_ItItem)
   {
      C_PuiSvDbChart & rc_Data = this->mc_Charts[u32_ItItem];
      rc_Data.c_UIPosition += orc_NewPos;
   }
   //Param widgets
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_ParamWidgets.size(); ++u32_ItItem)
   {
      C_PuiSvDbParam & rc_Data = this->mc_ParamWidgets[u32_ItItem];
      rc_Data.c_UIPosition += orc_NewPos;
   }
   //Pie charts
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_PieCharts.size(); ++u32_ItItem)
   {
      C_PuiSvDbPieChart & rc_Data = this->mc_PieCharts[u32_ItItem];
      rc_Data.c_UIPosition += orc_NewPos;
   }
   //Progress bars
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_ProgressBars.size(); ++u32_ItItem)
   {
      C_PuiSvDbProgressBar & rc_Data = this->mc_ProgressBars[u32_ItItem];
      rc_Data.c_UIPosition += orc_NewPos;
   }
   //Sliders
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_Sliders.size(); ++u32_ItItem)
   {
      C_PuiSvDbSlider & rc_Data = this->mc_Sliders[u32_ItItem];
      rc_Data.c_UIPosition += orc_NewPos;
   }
   //Spin boxes
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_SpinBoxes.size(); ++u32_ItItem)
   {
      C_PuiSvDbSpinBox & rc_Data = this->mc_SpinBoxes[u32_ItItem];
      rc_Data.c_UIPosition += orc_NewPos;
   }
   //Tables
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_Tables.size(); ++u32_ItItem)
   {
      C_PuiSvDbTable & rc_Data = this->mc_Tables[u32_ItItem];
      rc_Data.c_UIPosition += orc_NewPos;
   }
   //Toggles
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_Toggles.size(); ++u32_ItItem)
   {
      C_PuiSvDbToggle & rc_Data = this->mc_Toggles[u32_ItItem];
      rc_Data.c_UIPosition += orc_NewPos;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply Z value offset

   \param[in] of64_HighestUsedZValue Highest used Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSnapshot::SetDataZOffset(const float64 of64_HighestUsedZValue)
{
   C_PuiBsElements::SetDataZOffset(of64_HighestUsedZValue);
   //Labels
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_Labels.size(); ++u32_ItItem)
   {
      C_PuiSvDbLabel & rc_Data = this->mc_Labels[u32_ItItem];
      rc_Data.f64_ZOrder += of64_HighestUsedZValue;
   }
   //Charts
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_Charts.size(); ++u32_ItItem)
   {
      C_PuiSvDbChart & rc_Data = this->mc_Charts[u32_ItItem];
      rc_Data.f64_ZOrder += of64_HighestUsedZValue;
   }
   //Param widgets
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_ParamWidgets.size(); ++u32_ItItem)
   {
      C_PuiSvDbParam & rc_Data = this->mc_ParamWidgets[u32_ItItem];
      rc_Data.f64_ZOrder += of64_HighestUsedZValue;
   }
   //Pie charts
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_PieCharts.size(); ++u32_ItItem)
   {
      C_PuiSvDbPieChart & rc_Data = this->mc_PieCharts[u32_ItItem];
      rc_Data.f64_ZOrder += of64_HighestUsedZValue;
   }
   //Progress bars
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_ProgressBars.size(); ++u32_ItItem)
   {
      C_PuiSvDbProgressBar & rc_Data = this->mc_ProgressBars[u32_ItItem];
      rc_Data.f64_ZOrder += of64_HighestUsedZValue;
   }
   //Sliders
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_Sliders.size(); ++u32_ItItem)
   {
      C_PuiSvDbSlider & rc_Data = this->mc_Sliders[u32_ItItem];
      rc_Data.f64_ZOrder += of64_HighestUsedZValue;
   }
   //Spin boxes
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_SpinBoxes.size(); ++u32_ItItem)
   {
      C_PuiSvDbSpinBox & rc_Data = this->mc_SpinBoxes[u32_ItItem];
      rc_Data.f64_ZOrder += of64_HighestUsedZValue;
   }
   //Tables
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_Tables.size(); ++u32_ItItem)
   {
      C_PuiSvDbTable & rc_Data = this->mc_Tables[u32_ItItem];
      rc_Data.f64_ZOrder += of64_HighestUsedZValue;
   }
   //Toggles
   for (uint32 u32_ItItem = 0UL; u32_ItItem < this->mc_Toggles.size(); ++u32_ItItem)
   {
      C_PuiSvDbToggle & rc_Data = this->mc_Toggles[u32_ItItem];
      rc_Data.f64_ZOrder += of64_HighestUsedZValue;
   }
}
