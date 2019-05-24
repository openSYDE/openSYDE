//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Informational Line / Arrow for user in system definition (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISDARROW_H
#define C_GISDARROW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiBiArrow.h"
#include "C_PuiSdDataElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSdArrow :
   public C_GiBiArrow,
   public stw_opensyde_gui_logic::C_PuiSdDataElement
{
public:
   C_GiSdArrow(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
               const std::vector<QPointF> * const opc_Points = NULL, QGraphicsItem * const opc_Parent = NULL);
   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
