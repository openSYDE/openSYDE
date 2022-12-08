//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard tab chart data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBTABCHART_HPP
#define C_PUISVDBTABCHART_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbWidgetBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
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

   enum E_SettingVerticalAxisMode
   {
      eSETTING_YA_ONE_VISIBLE,
      eSETTING_YA_ALL_VISIBLE
   };

   C_PuiSvDbTabChart();

   void CalcHash(uint32_t & oru32_HashValue) const override;
   bool IsReadElement(void) const override;
   int32_t RemoveElement(const uint32_t & oru32_Index) override;

   std::vector<bool> c_DataPoolElementsActive;
   std::vector<uint8_t> c_DataPoolElementsColorIndex;
   int32_t s32_SplitterLeftWidth;
   E_SettingZoomMode e_SettingZoomMode;
   E_SettingVerticalAxisMode e_SettingVerticalAxisMode;
   bool q_IsZoomModeActive;
   bool q_IsPaused;
   bool q_AreSamplePointsShown;
   std::vector<std::array<float64_t, 4> > c_VisibleScreen; ///< Store range values for visible screen for
   // each
   // axis
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
