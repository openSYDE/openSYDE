//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data pool list element move undo command (implementation)

   Data pool list element move undo command

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
#include "C_SdNdeUnoLeDataPoolListElementMoveCommand.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeUnoUtil.h"

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

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in]     orc_SourceRow                    Source rows
   \param[in]     orc_TargetRow                    Target rows
   \param[in]     orq_AdaptIndices                 Flag, if target rows should be adapted to source row deletion
   \param[in,out] opc_Parent                       Optional pointer to parent

   \created     25.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeUnoLeDataPoolListElementMoveCommand::C_SdNdeUnoLeDataPoolListElementMoveCommand(const uint32 & oru32_NodeIndex,
                                                                                       const uint32 & oru32_DataPoolIndex, const uint32 & oru32_DataPoolListIndex, C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager,
                                                                                       const std::vector<uint32> & orc_SourceRow,
                                                                                       const std::vector<uint32> & orc_TargetRow, const bool & orq_AdaptIndices,
                                                                                       QUndoCommand * const opc_Parent)
   :
   C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                       opc_DataPoolListModelViewManager,  orc_SourceRow,
                                                       "Move List element", opc_Parent),
   mc_SourceRow(orc_SourceRow),
   mc_TargetRow(orc_TargetRow)
{
   if (orq_AdaptIndices == true)
   {
      C_SdNdeUnoUtil::h_AdaptTargetToDeletedSource(this->mc_SourceRow, this->mc_TargetRow);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementMoveCommand::redo(void)
{
   //DELETE
   this->SetIndices(mc_SourceRow);
   this->Delete();
   //ADD
   this->SetIndices(mc_TargetRow);
   this->Add();
   C_SdNdeUnoLeDataPoolListElementBaseCommand::redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementMoveCommand::undo(void)
{
   C_SdNdeUnoLeDataPoolListElementBaseCommand::undo();
   //DELETE
   this->SetIndices(mc_TargetRow);
   this->Delete();
   //ADD
   this->SetIndices(mc_SourceRow);
   this->Add();
}
