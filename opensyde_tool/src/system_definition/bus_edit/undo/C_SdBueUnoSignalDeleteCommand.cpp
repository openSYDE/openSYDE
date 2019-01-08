//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Signal delete undo command (implementation)

   Signal delete undo command

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
#include "C_SdBueUnoSignalDeleteCommand.h"

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

   \param[in]     orc_MessageId          Message identification indices
   \param[in]     oru32_SignalIndex      Signal index
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on
   \param[in,out] opc_Parent             Optional pointer to parent

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdBueUnoSignalDeleteCommand::C_SdBueUnoSignalDeleteCommand(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                             const uint32 & oru32_SignalIndex,
                                                             C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager, C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
                                                             QUndoCommand * const opc_Parent) :
   C_SdBueUnoSignalAddDeleteBaseCommand(orc_MessageId, oru32_SignalIndex, 0U, opc_MessageSyncManager,
                                        opc_MessageTreeWidget,
                                        "Delete Signal",
                                        opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo delete

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoSignalDeleteCommand::redo(void)
{
   this->Delete();
   C_SdBueUnoSignalAddDeleteBaseCommand::redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo delete

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoSignalDeleteCommand::undo(void)
{
   C_SdBueUnoSignalAddDeleteBaseCommand::undo();
   this->Add();
}
