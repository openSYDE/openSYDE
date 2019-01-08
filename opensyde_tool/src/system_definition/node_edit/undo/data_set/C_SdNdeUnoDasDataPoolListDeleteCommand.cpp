//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data pool list data set delete undo command (implementation)

   Data pool list data set delete undo command

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
#include "C_SdNdeUnoDasDataPoolListDeleteCommand.h"

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
   \param[in]     orc_Indices                      Node data pool list indices
   \param[in,out] opc_Parent                       Optional pointer to parent

   \created     25.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeUnoDasDataPoolListDeleteCommand::C_SdNdeUnoDasDataPoolListDeleteCommand(const uint32 & oru32_NodeIndex,
                                                                               const uint32 & oru32_DataPoolIndex,
                                                                               const uint32 & oru32_DataPoolListIndex,
                                                                               C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager,
                                                                               const std::vector<uint32> & orc_Indices,
                                                                               QUndoCommand * const opc_Parent) :
   C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                 opc_DataPoolListModelViewManager,
                                                 orc_Indices,
                                                 "Delete List Dataset", opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Redo delete

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListDeleteCommand::redo(void)
{
   this->Delete();
   C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand::redo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Undo delete

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListDeleteCommand::undo(void)
{
   C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand::undo();
   this->Add();
   this->mq_Initial = false;
}
