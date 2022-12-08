//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list array edit command stack (implementation)

   Data pool list array edit command stack

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_SdNdeUnoAedDataPoolListManager.hpp"
#include "C_SdNdeUnoAedDataPoolListDataChangeCommand.hpp"
#include "C_SdNdeDpListTableView.hpp"
#include "C_SdNdeDpListTableModel.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoAedDataPoolListManager::C_SdNdeUnoAedDataPoolListManager(void) :
   mpc_UndoCommand(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do data change

   \param[in]        oru32_NodeIndex                     Node index
   \param[in]        oru32_DataPoolIndex                 Node data pool index
   \param[in]        oru32_ListIndex                     Node data pool list index
   \param[in]        oru32_ElementIndex                  Node data pool list element index
   \param[in]        ore_ArrayEditType                   Enum for node data pool list element variable
   \param[in]        oru32_DataSetIndex                  If min or max use 0
                                                         Else use data set index
   \param[in]        oru32_ArrayElementIndex             Array index
   \param[in]        orc_NewData                         New data
   \param[in,out]    opc_DataPoolListModelViewManager    Data pool lists model view manager to get objects to perform
                                                         actions on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoAedDataPoolListManager::DoDataChangeElements(const uint32_t & oru32_NodeIndex,
                                                            const uint32_t & oru32_DataPoolIndex,
                                                            const uint32_t & oru32_ListIndex,
                                                            const uint32_t & oru32_ElementIndex,
                                                            const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                                            const uint32_t & oru32_DataSetIndex,
                                                            const uint32_t & oru32_ArrayElementIndex,
                                                            const QVariant & orc_NewData,
                                                            C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager)
{
   QUndoCommand * pc_Command;

   m_InitUndoCommand();
   pc_Command = new C_SdNdeUnoAedDataPoolListDataChangeCommand(
      oru32_NodeIndex,
      oru32_DataPoolIndex, oru32_ListIndex,
      oru32_ElementIndex,
      ore_ArrayEditType, oru32_DataSetIndex, oru32_ArrayElementIndex, orc_NewData, opc_DataPoolListModelViewManager,
      this->mpc_UndoCommand);
   //Do action once
   pc_Command->redo();
} //lint !e429  //no memory leak because of the parent of pc_Command and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   GetUndocommand and take ownership

   Internal undo command is reseted

   \return
   NULL No changes
   Else Undocommand accumulating all changes
*/
//----------------------------------------------------------------------------------------------------------------------
QUndoCommand * C_SdNdeUnoAedDataPoolListManager::TakeUndoCommand(void)
{
   QUndoCommand * const pc_Retval = this->mpc_UndoCommand;

   this->mpc_UndoCommand = NULL;
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init data set undo command
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoAedDataPoolListManager::m_InitUndoCommand(void)
{
   if (this->mpc_UndoCommand == NULL)
   {
      this->mpc_UndoCommand = new QUndoCommand("Change array element(s)");
   }
}
