//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list paste undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNODATAPOOLLISTPASTECOMMAND_H
#define C_SDNDEUNODATAPOOLLISTPASTECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SdNdeUnoDataPoolListAddDeleteBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoDataPoolListPasteCommand :
   public C_SdNdeUnoDataPoolListAddDeleteBaseCommand
{
public:
   C_SdNdeUnoDataPoolListPasteCommand(const stw_types::uint32 & oru32_NodeIndex,
                                      const stw_types::uint32 & oru32_DataPoolIndex,
                                      stw_opensyde_gui::C_SdNdeDataPoolListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                      QUndoCommand * const opc_Parent = NULL);
   bool InitialSetup(const stw_types::uint32 & oru32_FirstIndex);
   virtual void redo(void) override;
   virtual void undo(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
