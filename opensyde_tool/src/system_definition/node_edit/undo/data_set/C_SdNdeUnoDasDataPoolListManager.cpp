//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set command stack (implementation)

   Data pool list data set command stack

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_SdNdeUnoDasDataPoolListManager.hpp"
#include "C_SdNdeUnoDasDataPoolListMoveCommand.hpp"
#include "C_SdNdeUnoDasDataPoolListDeleteCommand.hpp"
#include "C_SdNdeUnoDasDataPoolListAddCommand.hpp"
#include "C_SdNdeUnoDasDataPoolListAddSpecificCommand.hpp"
#include "C_SdNdeUnoDasDataPoolListDataChangeCommand.hpp"
#include "C_SdNdeDpListTableView.hpp"
#include "C_SdNdeDpListTableModel.hpp"
#include "C_SdClipBoardHelper.hpp"

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
C_SdNdeUnoDasDataPoolListManager::C_SdNdeUnoDasDataPoolListManager(void) :
   mpc_UndoCommand(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register move command

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in]     orc_StartIndex                   Source row index
   \param[in]     orc_TargetIndex                  Target row index
   \param[in]     orq_AdaptIndices                 Flag, if target columns should be adapted to source column deletion
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListManager::DoMoveElements(const uint32_t & oru32_NodeIndex,
                                                      const uint32_t & oru32_DataPoolIndex,
                                                      const uint32_t & oru32_DataPoolListIndex,
                                                      C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager, const std::vector<uint32_t> & orc_StartIndex, const std::vector<uint32_t> & orc_TargetIndex,
                                                      const bool & orq_AdaptIndices)
{
   if (((orc_StartIndex.size() > 0) && (orc_TargetIndex.size() > 0)) &&
       (orc_StartIndex.size() == orc_TargetIndex.size()))
   {
      QUndoCommand * pc_Command;
      m_InitUndoCommand();
      pc_Command = new C_SdNdeUnoDasDataPoolListMoveCommand(
         oru32_NodeIndex,
         oru32_DataPoolIndex, oru32_DataPoolListIndex,
         opc_DataPoolListModelViewManager,
         orc_StartIndex,
         orc_TargetIndex, orq_AdaptIndices, this->mpc_UndoCommand);
      //Do action once
      pc_Command->redo();
   } //lint !e429  //no memory leak because of the parent of pc_Command and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do delete

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in]     orc_Indices                      Node data pool list indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListManager::DoDeleteElements(const uint32_t & oru32_NodeIndex,
                                                        const uint32_t & oru32_DataPoolIndex,
                                                        const uint32_t & oru32_DataPoolListIndex,
                                                        C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                                        const std::vector<uint32_t> & orc_Indices)
{
   if (orc_Indices.size() > 0)
   {
      QUndoCommand * pc_Command;
      m_InitUndoCommand();
      pc_Command = new C_SdNdeUnoDasDataPoolListDeleteCommand(
         oru32_NodeIndex,
         oru32_DataPoolIndex, oru32_DataPoolListIndex,
         opc_DataPoolListModelViewManager,
         orc_Indices, this->mpc_UndoCommand);

      //Do action once
      pc_Command->redo();
   } //lint !e429  //no memory leak because of the parent of pc_Command and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do paste

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in]     oru32_InsertListIndex            Node data pool list index to insert new elements at
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListManager::DoPaste(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                               const uint32_t & oru32_DataPoolListIndex,
                                               C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                               const uint32_t & oru32_InsertListIndex)
{
   std::vector<stw::opensyde_core::C_OscNodeDataPoolDataSet> c_OscNames;
   std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> > c_OscDataSetValues;
   if (C_SdClipBoardHelper::h_LoadToDataPoolListDataSetsFromClipBoard(c_OscNames,
                                                                      c_OscDataSetValues) == stw::errors::C_NO_ERR)
   {
      std::vector<uint32_t> c_Indices;

      if (c_OscNames.size() == c_OscDataSetValues.size())
      {
         QUndoCommand * pc_Command;
         c_Indices.reserve(c_OscNames.size());
         for (uint32_t u32_NewIndices = 0; u32_NewIndices < c_OscNames.size(); ++u32_NewIndices)
         {
            c_Indices.push_back(oru32_InsertListIndex + u32_NewIndices);
         }
         m_InitUndoCommand();
         pc_Command = new C_SdNdeUnoDasDataPoolListAddSpecificCommand(oru32_NodeIndex,
                                                                      oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                                      opc_DataPoolListModelViewManager,
                                                                      c_Indices, c_OscNames,
                                                                      c_OscDataSetValues, this->mpc_UndoCommand);
         //Do action once
         pc_Command->redo();
      } //lint !e429  //no memory leak because of the parent of pc_Command and the Qt memory management
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do add

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in]     orc_Indices                      Node data pool list indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListManager::DoAddElements(const uint32_t & oru32_NodeIndex,
                                                     const uint32_t & oru32_DataPoolIndex,
                                                     const uint32_t & oru32_DataPoolListIndex,
                                                     C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                                     const std::vector<uint32_t> & orc_Indices)
{
   if (orc_Indices.size() > 0)
   {
      QUndoCommand * pc_Command;
      m_InitUndoCommand();
      pc_Command = new C_SdNdeUnoDasDataPoolListAddCommand(
         oru32_NodeIndex,
         oru32_DataPoolIndex, oru32_DataPoolListIndex,
         opc_DataPoolListModelViewManager,
         orc_Indices, this->mpc_UndoCommand);
      //Do action once
      pc_Command->redo();
   } //lint !e429  //no memory leak because of the parent of pc_Command and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do add specific

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in]     orc_Indices                      Node data pool list indices
   \param[in]     orc_OscNames                     Initial Data set names
   \param[in]     orc_OscDataSetValues             Initial Data set values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListManager::DoAddSpecificElements(const uint32_t & oru32_NodeIndex,
                                                             const uint32_t & oru32_DataPoolIndex,
                                                             const uint32_t & oru32_DataPoolListIndex,
                                                             C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager, const std::vector<uint32_t> & orc_Indices, const std::vector<stw::opensyde_core::C_OscNodeDataPoolDataSet> & orc_OscNames,
                                                             const std::vector<std::vector<C_OscNodeDataPoolContent> > & orc_OscDataSetValues)
{
   if (orc_Indices.size() > 0)
   {
      QUndoCommand * pc_Command;
      m_InitUndoCommand();
      pc_Command = new C_SdNdeUnoDasDataPoolListAddSpecificCommand(
         oru32_NodeIndex,
         oru32_DataPoolIndex, oru32_DataPoolListIndex,
         opc_DataPoolListModelViewManager,
         orc_Indices, orc_OscNames, orc_OscDataSetValues, this->mpc_UndoCommand);
      //Do action once
      pc_Command->redo();
   } //lint !e429  //no memory leak because of the parent of pc_Command and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do data change

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in]     oru32_DataPoolListDataSetIndex   Node data pool list data set index
   \param[in]     orc_NewData                      New data
   \param[in]     ore_DataChangeType               Data change type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListManager::DoDataChangeElements(const uint32_t & oru32_NodeIndex,
                                                            const uint32_t & oru32_DataPoolIndex,
                                                            const uint32_t & oru32_DataPoolListIndex,
                                                            C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager, const uint32_t & oru32_DataPoolListDataSetIndex, const QVariant & orc_NewData,
                                                            const C_SdNdeDpUtil::E_DataSetDataChangeType & ore_DataChangeType)
{
   QUndoCommand * pc_Command;

   m_InitUndoCommand();
   pc_Command = new C_SdNdeUnoDasDataPoolListDataChangeCommand(
      oru32_NodeIndex,
      oru32_DataPoolIndex, oru32_DataPoolListIndex,
      opc_DataPoolListModelViewManager,
      oru32_DataPoolListDataSetIndex, orc_NewData, ore_DataChangeType, this->mpc_UndoCommand);
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
QUndoCommand * C_SdNdeUnoDasDataPoolListManager::TakeUndoCommand(void)
{
   QUndoCommand * const pc_Retval = this->mpc_UndoCommand;

   this->mpc_UndoCommand = NULL;
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init data set undo command
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListManager::m_InitUndoCommand(void)
{
   if (this->mpc_UndoCommand == NULL)
   {
      this->mpc_UndoCommand = new QUndoCommand("Change Datasets");
   }
}
