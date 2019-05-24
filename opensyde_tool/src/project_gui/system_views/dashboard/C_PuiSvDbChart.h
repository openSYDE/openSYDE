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
#include "C_PuiSvDbWidgetBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbChart :
   public C_PuiSvDbWidgetBase
{
public:
   C_PuiSvDbChart();

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const override;
   virtual bool IsReadElement(void) const;
   virtual stw_types::sint32 RemoveElement(const stw_types::uint32 & oru32_Index) override;

   std::vector<bool> c_DataPoolElementsActive;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
