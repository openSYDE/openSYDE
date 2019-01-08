//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Signal move undo command (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDBUEUNOSIGNALMOVECOMMAND_H
#define C_SDBUEUNOSIGNALMOVECOMMAND_H

/* -- Includes ------------------------------------------------------------- */
#include "C_SdBueUnoSignalAddDeleteBaseCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdBueUnoSignalMoveCommand :
   public C_SdBueUnoSignalAddDeleteBaseCommand
{
public:
   C_SdBueUnoSignalMoveCommand(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_SourceMessageId,
                               const stw_types::uint32 & oru32_SourceSignalIndex,
                               const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_TargetMessageId,
                               const stw_types::uint32 & oru32_TargetSignalIndex,
                               C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
                               stw_opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
                               QUndoCommand * const opc_Parent = NULL);

   virtual void redo(void);
   virtual void undo(void);

private:
   void m_Move(const stw_types::uint64 & orc_SourceUniqueId, const stw_types::uint32 & oru32_SourceSignalIndex,
               const stw_types::uint64 & orc_TargetUniqueId, const stw_types::uint32 & oru32_TargetSignalIndex);

   stw_types::uint64 mu64_SourceUniqueId;
   const stw_types::uint32 mu32_SourceSignalIndex;
   stw_types::uint64 mu64_TargetUniqueId;
   const stw_types::uint32 mu32_TargetSignalIndex;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
