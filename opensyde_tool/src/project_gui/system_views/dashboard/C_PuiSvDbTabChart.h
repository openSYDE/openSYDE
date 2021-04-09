//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard tab chart data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBTABCHART_H
#define C_PUISVDBTABCHART_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbWidgetBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbTabChart :
   public C_PuiSvDbWidgetBase
{
public:
   enum E_SettingZoomMode
   {
      eSETTING_ZM_XY,
      eSETTING_ZM_X,
      eSETTING_ZM_Y
   };

   enum E_SettingYAxisMode
   {
      eSETTING_YA_ONE_VISIBLE,
      eSETTING_YA_ALL_VISIBLE
   };

   C_PuiSvDbTabChart();

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const override;
   virtual bool IsReadElement(void) const override;
   virtual stw_types::sint32 RemoveElement(const stw_types::uint32 & oru32_Index) override;

   std::vector<bool> c_DataPoolElementsActive;
   std::vector<stw_types::uint8> c_DataPoolElementsColorIndex;
   stw_types::sint32 s32_SplitterLeftWidth;
   E_SettingZoomMode e_SettingZoomMode;
   E_SettingYAxisMode e_SettingYAxisMode;
   bool q_IsZoomModeActive;
   bool q_IsPaused;
   bool q_AreSamplePointsShown;
   std::vector<std::array<stw_types::float64, 4> > c_VisibleScreen; ///< Store range values for visible screen for each
                                                                    // axis
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
