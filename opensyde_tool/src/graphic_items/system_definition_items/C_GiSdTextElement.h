//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom text element graphics item in system definition (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISDTEXTELEMENT_H
#define C_GISDTEXTELEMENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiBiTextElement.h"
#include "C_PuiSdDataElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSdTextElement :
   public C_GiBiTextElement,
   public stw_opensyde_gui_logic::C_PuiSdDataElement
{
public:
   C_GiSdTextElement(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID, const bool oq_Editable,
                     QGraphicsItem * const opc_Parent = NULL);
   C_GiSdTextElement(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                     QGraphicsItem * const opc_Parent = NULL);
   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
