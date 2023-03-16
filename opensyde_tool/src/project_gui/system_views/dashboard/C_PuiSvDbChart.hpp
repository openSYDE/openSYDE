//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard chart data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBCHART_H
#define C_PUISVDBCHART_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbWidgetBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbChart :
   public C_PuiSvDbWidgetBase
{
public:
   enum E_SettingZoomMode
   {
      eSETTING_ZM_XY,
      eSETTING_ZM_X,
      eSETTING_ZM_Y
   };

   C_PuiSvDbChart();

   void CalcHash(uint32_t & oru32_HashValue) const override;
   bool IsReadElement(void) const override;
   int32_t RemoveElement(const uint32_t & oru32_Index) override;

   std::vector<bool> c_DataPoolElementsActive;
   E_SettingZoomMode e_SettingZoomMode;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
