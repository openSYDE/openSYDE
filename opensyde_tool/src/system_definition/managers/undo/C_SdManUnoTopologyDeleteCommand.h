//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Delete undo command (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYDELETECOMMAND_H
#define C_SDMANUNOTOPOLOGYDELETECOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include "C_SdManUnoTopologyAddDeleteBaseCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdManUnoTopologyDeleteCommand :
   public C_SdManUnoTopologyAddDeleteBaseCommand
{
public:
   C_SdManUnoTopologyDeleteCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                   QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologyDeleteCommand(void);
   virtual void undo(void) override;
   virtual void redo(void) override;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
