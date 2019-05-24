//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard table data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBTABLE_H
#define C_PUISVDBTABLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbWidgetBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbTable :
   public C_PuiSvDbWidgetBase
{
public:
   C_PuiSvDbTable(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const override;
   virtual bool IsReadElement(void) const;

   std::vector<stw_types::sint32> c_ColumnWidth;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
