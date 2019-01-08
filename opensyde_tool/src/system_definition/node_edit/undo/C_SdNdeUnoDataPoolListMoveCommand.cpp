//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data pool list move undo command (implementation)

   Data pool list move undo command

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
#include "C_SdNdeUnoDataPoolListMoveCommand.h"
#include "C_PuiSdHandler.h"
#include "C_SdUtil.h"

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
   \param[in]     oru32_SourceRows            Source rows
   \param[in]     oru32_TargetRows            Target rows
   \param[in,out] opc_Parent                  Optional pointer to parent

   \created     25.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeUnoDataPoolListMoveCommand::C_SdNdeUnoDataPoolListMoveCommand(const uint32 & oru32_NodeIndex,
                                                                     const uint32 & oru32_DataPoolIndex,
                                                                     stw_opensyde_gui::C_SdNdeDataPoolListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                                                     const std::vector<uint32> & oru32_SourceRows,
                                                                     const std::vector<uint32> & oru32_TargetRows,
                                                                     QUndoCommand * const opc_Parent) :
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, opc_DataPoolListsTreeWidget,
                                              oru32_SourceRows,
                                              "Move List", opc_Parent),
   mc_SourceRows(oru32_SourceRows),
   mc_TargetRows(oru32_TargetRows)
{
   C_SdUtil::h_SortSourceDescending(this->mc_SourceRows, this->mc_TargetRows);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListMoveCommand::redo(void)
{
   //Delete
   this->SetIndices(this->mc_SourceRows);
   this->Delete();
   //Add
   this->SetIndices(this->mc_TargetRows);
   this->Add();
   //Update
   this->mpc_DataPoolListsTreeWidget->UpdateUI();
   C_SdNdeUnoDataPoolListBaseCommand::redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListMoveCommand::undo(void)
{
   C_SdNdeUnoDataPoolListBaseCommand::undo();
   //Delete
   this->SetIndices(this->mc_TargetRows);
   this->Delete();
   //Add
   this->SetIndices(this->mc_SourceRows);
   this->Add();
   //Update
   this->mpc_DataPoolListsTreeWidget->UpdateUI();
}
