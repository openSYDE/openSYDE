//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Signal move undo command (implementation)

   Signal move undo command

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdBueUnoSignalMoveCommand.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in]     orc_SourceMessageId     Source message identification indices
   \param[in]     oru32_SourceSignalIndex Source signal index
   \param[in]     orc_TargetMessageId     Target message identification indices
   \param[in]     oru32_TargetSignalIndex Target signal index
   \param[in,out] opc_MessageSyncManager  Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget   Message tree widget to perform actions on
   \param[in,out] opc_Parent              Optional pointer to parent

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdBueUnoSignalMoveCommand::C_SdBueUnoSignalMoveCommand(
   const C_OSCCanMessageIdentificationIndices & orc_SourceMessageId, const uint32 & oru32_SourceSignalIndex,
   const C_OSCCanMessageIdentificationIndices & orc_TargetMessageId, const uint32 & oru32_TargetSignalIndex,
   C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
   C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, QUndoCommand * const opc_Parent) :
   C_SdBueUnoSignalAddDeleteBaseCommand(orc_SourceMessageId, oru32_SourceSignalIndex, 0U, opc_MessageSyncManager,
                                        opc_MessageTreeWidget,
                                        "Move Signal",
                                        opc_Parent),
   mu64_SourceUniqueId(0ULL),
   mu32_SourceSignalIndex(oru32_SourceSignalIndex),
   mu64_TargetUniqueId(0ULL),
   mu32_TargetSignalIndex(oru32_TargetSignalIndex)
{
   if (opc_MessageSyncManager != NULL)
   {
      this->mu64_SourceUniqueId = opc_MessageSyncManager->GetUniqueId(orc_SourceMessageId);
      this->mu64_TargetUniqueId = opc_MessageSyncManager->GetUniqueId(orc_TargetMessageId);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo move

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoSignalMoveCommand::redo(void)
{
   this->m_Move(mu64_SourceUniqueId, mu32_SourceSignalIndex, mu64_TargetUniqueId, mu32_TargetSignalIndex);
   C_SdBueUnoSignalAddDeleteBaseCommand::redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo move

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoSignalMoveCommand::undo(void)
{
   C_SdBueUnoSignalAddDeleteBaseCommand::undo();
   this->m_Move(mu64_TargetUniqueId, mu32_TargetSignalIndex, mu64_SourceUniqueId, mu32_SourceSignalIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move signal data

   \param[in] orc_SourceUniqueId      Source message identification indices (referenced by unique ID)
   \param[in] oru32_SourceSignalIndex Source signal index
   \param[in] orc_TargetUniqueId      Target message identification indices (referenced by unique ID)
   \param[in] oru32_TargetSignalIndex Target signal index

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoSignalMoveCommand::m_Move(const uint64 & orc_SourceUniqueId, const uint32 & oru32_SourceSignalIndex,
                                         const uint64 & orc_TargetUniqueId, const uint32 & oru32_TargetSignalIndex)
{
   //Step 1: delete source (Internal save of data)
   this->mu64_UniqueId = orc_SourceUniqueId;
   this->mu32_SignalIndex = oru32_SourceSignalIndex;
   this->Delete();
   //Reset color in all cases
   this->mc_UISignal.u8_ColorIndex = 0;
   //Step 2: add target (Use internally stored data)
   this->mu64_UniqueId = orc_TargetUniqueId;
   this->mu32_SignalIndex = oru32_TargetSignalIndex;
   this->Add();
}
