//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Message delete undo command (implementation)

   Message delete undo command

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

#include "C_SdBueUnoMessageDeleteCommand.h"

/* -- Used Namespaces ------------------------------------------------------ */
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
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on
   \param[in,out] opc_Parent             Optional pointer to parent

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdBueUnoMessageDeleteCommand::C_SdBueUnoMessageDeleteCommand(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId,
   C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
   C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, QUndoCommand * const opc_Parent) :
   C_SdBueUnoMessageAddDeleteBaseCommand(orc_MessageId, opc_MessageSyncManager, opc_MessageTreeWidget, "Delete Message",
                                         opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo delete

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoMessageDeleteCommand::redo(void)
{
   this->Delete();
   C_SdBueUnoMessageAddDeleteBaseCommand::redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo delete

   \created     20.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueUnoMessageDeleteCommand::undo(void)
{
   C_SdBueUnoMessageAddDeleteBaseCommand::undo();
   this->Add();
}
