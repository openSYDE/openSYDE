//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data pool list delete undo command (implementation)

   Data pool list delete undo command

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdNdeUnoDataPoolListDeleteCommand.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in]     oru32_NodeIndex             Node index
   \param[in]     oru32_DataPoolIndex         Node data pool index
   \param[in,out] opc_DataPoolListsTreeWidget Data pool lists tree widget to perform actions on
   \param[in]     orc_Indices                 Node data pool list indices
   \param[in,out] opc_Parent                  Optional pointer to parent

   \created     25.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeUnoDataPoolListDeleteCommand::C_SdNdeUnoDataPoolListDeleteCommand(const uint32 & oru32_NodeIndex,
                                                                         const uint32 & oru32_DataPoolIndex,
                                                                         stw_opensyde_gui::C_SdNdeDataPoolListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                                                         const std::vector<uint32> & orc_Indices,
                                                                         QUndoCommand * const opc_Parent) :
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex,
                                              opc_DataPoolListsTreeWidget, orc_Indices,
                                              "Delete List", opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo delete

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListDeleteCommand::redo(void)
{
   this->Delete();
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand::redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo delete

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListDeleteCommand::undo(void)
{
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand::undo();
   this->Add();
}
