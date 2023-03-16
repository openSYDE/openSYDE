//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard pie chart data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBPIECHART_H
#define C_PUISVDBPIECHART_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbWidgetBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbPieChart :
   public C_PuiSvDbWidgetBase
{
public:
   C_PuiSvDbPieChart(void);

   void CalcHash(uint32_t & oru32_HashValue) const override;
   bool IsReadElement(void) const override;

   bool q_ShowUnit;
   bool q_ShowValue;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
