//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delete undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAUNODELETECOMMAND_H
#define C_SYVDAUNODELETECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvDaUnoAddDeleteBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaUnoDeleteCommand :
   public C_SyvDaUnoAddDeleteBaseCommand
{
public:
   C_SyvDaUnoDeleteCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                           QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SyvDaUnoDeleteCommand(void);
   void undo(void) override;
   void redo(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
