//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list move undo command (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEUNODATAPOOLLISTMOVECOMMAND_H
#define C_SDNDEUNODATAPOOLLISTMOVECOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include "stwtypes.h"
#include "C_SdNdeUnoDataPoolListAddDeleteBaseCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeUnoDataPoolListMoveCommand :
   public C_SdNdeUnoDataPoolListAddDeleteBaseCommand
{
public:
   C_SdNdeUnoDataPoolListMoveCommand(const stw_types::uint32 & oru32_NodeIndex,
                                     const stw_types::uint32 & oru32_DataPoolIndex,
                                     stw_opensyde_gui::C_SdNdeDataPoolListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                     const std::vector<stw_types::uint32> & oru32_SourceRows,
                                     const std::vector<stw_types::uint32> & oru32_TargetRows,
                                     QUndoCommand * const opc_Parent = NULL);
   virtual void redo(void) override;
   virtual void undo(void) override;

private:
   std::vector<stw_types::uint32> mc_SourceRows;
   std::vector<stw_types::uint32> mc_TargetRows;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
