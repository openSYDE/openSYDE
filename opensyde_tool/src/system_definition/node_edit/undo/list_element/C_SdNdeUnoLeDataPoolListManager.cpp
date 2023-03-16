//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list command stack (implementation)

   Data pool list command stack

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdNdeUnoLeDataPoolListManager.hpp"
#include "C_SdNdeUnoLeDataPoolListElementMoveCommand.hpp"
#include "C_SdNdeUnoLeDataPoolListElementDeleteCommand.hpp"
#include "C_SdNdeUnoLeDataPoolListElementPasteCommand.hpp"
#include "C_SdNdeUnoLeDataPoolListElementAddCommand.hpp"
#include "C_SdNdeUnoLeDataPoolListElementAddSpecificCommand.hpp"
#include "C_SdNdeUnoLeDataPoolListElementDataChangeCommand.hpp"
#include "C_SdNdeDpListTableView.hpp"
#include "C_SdNdeDpListTableModel.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;

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
C_SdNdeUnoLeDataPoolListManager::C_SdNdeUnoLeDataPoolListManager(void) :
   mpc_UndoStack(NULL)
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
   \param[in]     orq_AdaptIndices                 Flag, if target rows should be adapted to source row deletion
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListManager::DoMoveElements(const uint32_t & oru32_NodeIndex,
                                                     const uint32_t & oru32_DataPoolIndex,
                                                     const uint32_t & oru32_DataPoolListIndex,
                                                     C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager, const std::vector<uint32_t> & orc_StartIndex, const std::vector<uint32_t> & orc_TargetIndex,
                                                     const bool & orq_AdaptIndices)
{
   if (((orc_StartIndex.size() > 0) && (orc_TargetIndex.size() > 0)) &&
       (orc_StartIndex.size() == orc_TargetIndex.size()))
   {
      C_SdNdeUnoLeDataPoolListElementMoveCommand * const pc_MoveCommand =
         new C_SdNdeUnoLeDataPoolListElementMoveCommand(
            oru32_NodeIndex,
            oru32_DataPoolIndex, oru32_DataPoolListIndex,
            opc_DataPoolListModelViewManager,
            orc_StartIndex,
            orc_TargetIndex, orq_AdaptIndices);

      DoPush(pc_MoveCommand);
   }
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
void C_SdNdeUnoLeDataPoolListManager::DoDeleteElements(const uint32_t & oru32_NodeIndex,
                                                       const uint32_t & oru32_DataPoolIndex,
                                                       const uint32_t & oru32_DataPoolListIndex,
                                                       C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                                       const std::vector<uint32_t> & orc_Indices)
{
   if (orc_Indices.size() > 0)
   {
      C_SdNdeUnoLeDataPoolListElementDeleteCommand * const pc_DeleteCommand =
         new C_SdNdeUnoLeDataPoolListElementDeleteCommand(
            oru32_NodeIndex,
            oru32_DataPoolIndex, oru32_DataPoolListIndex,
            opc_DataPoolListModelViewManager,
            orc_Indices);

      //Check if last element(s) -> add replacement, so we never encounter zero data elements
      if (opc_DataPoolListModelViewManager != NULL)
      {
         C_SdNdeDpListTableModel * const pc_Model = opc_DataPoolListModelViewManager->GetElementModel(
            oru32_NodeIndex,
            oru32_DataPoolIndex,
            oru32_DataPoolListIndex);
         if (pc_Model != NULL)
         {
            if (static_cast<uint32_t>(pc_Model->rowCount()) == orc_Indices.size())
            {
               std::vector<uint32_t> c_Index;
               c_Index.push_back(0);
               new C_SdNdeUnoLeDataPoolListElementAddCommand(oru32_NodeIndex, oru32_DataPoolIndex,
                                                             oru32_DataPoolListIndex,
                                                             opc_DataPoolListModelViewManager, c_Index,
                                                             pc_DeleteCommand);
            }
         }
      }
      DoPush(pc_DeleteCommand);
   }
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
void C_SdNdeUnoLeDataPoolListManager::DoPaste(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                              const uint32_t & oru32_DataPoolListIndex,
                                              C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                              const uint32_t & oru32_InsertListIndex)
{
   C_SdNdeUnoLeDataPoolListElementPasteCommand * const pc_PasteCommand =
      new C_SdNdeUnoLeDataPoolListElementPasteCommand(oru32_NodeIndex,
                                                      oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                      opc_DataPoolListModelViewManager);

   if (pc_PasteCommand->InitialSetup(oru32_InsertListIndex) == true)
   {
      //Accept
      DoPush(pc_PasteCommand);
   }
   else
   {
      //Discard
      delete (pc_PasteCommand);
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
void C_SdNdeUnoLeDataPoolListManager::DoAddElements(const uint32_t & oru32_NodeIndex,
                                                    const uint32_t & oru32_DataPoolIndex,
                                                    const uint32_t & oru32_DataPoolListIndex,
                                                    C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                                    const std::vector<uint32_t> & orc_Indices)
{
   if (orc_Indices.size() > 0)
   {
      C_SdNdeUnoLeDataPoolListElementAddCommand * const pc_AddCommand = new C_SdNdeUnoLeDataPoolListElementAddCommand(
         oru32_NodeIndex,
         oru32_DataPoolIndex, oru32_DataPoolListIndex,
         opc_DataPoolListModelViewManager,
         orc_Indices);

      DoPush(pc_AddCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do add specific

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in]     orc_Indices                      Node data pool list indices
   \param[in]     orc_OscData                      OSC node data pool list data
   \param[in]     orc_UiData                       UI node data pool list data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListManager::DoAddSpecificElements(const uint32_t & oru32_NodeIndex,
                                                            const uint32_t & oru32_DataPoolIndex,
                                                            const uint32_t & oru32_DataPoolListIndex,
                                                            C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager, const std::vector<uint32_t> & orc_Indices, const std::vector<C_OscNodeDataPoolListElement> & orc_OscData,
                                                            const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiData)
{
   if (orc_Indices.size() > 0)
   {
      C_SdNdeUnoLeDataPoolListElementAddSpecificCommand * const pc_AddCommand =
         new C_SdNdeUnoLeDataPoolListElementAddSpecificCommand(
            oru32_NodeIndex,
            oru32_DataPoolIndex, oru32_DataPoolListIndex,
            opc_DataPoolListModelViewManager,
            orc_Indices, orc_OscData, orc_UiData);

      DoPush(pc_AddCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do data change

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in]     oru32_DataPoolListElementIndex   Node data pool list element index
   \param[in]     orc_NewData                      New data
   \param[in]     ore_DataChangeType               Data change type
   \param[in]     oru32_ArrayIndex                 Optional array index
   \param[in]     ors32_DataSetIndex               Optional data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListManager::DoDataChangeElements(const uint32_t & oru32_NodeIndex,
                                                           const uint32_t & oru32_DataPoolIndex,
                                                           const uint32_t & oru32_DataPoolListIndex,
                                                           C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager, const uint32_t & oru32_DataPoolListElementIndex, const QVariant & orc_NewData, const C_SdNdeDpUtil::E_ElementDataChangeType & ore_DataChangeType, const uint32_t & oru32_ArrayIndex,
                                                           const int32_t & ors32_DataSetIndex)
{
   C_SdNdeUnoLeDataPoolListElementDataChangeCommand * const pc_ChangeCommand =
      new C_SdNdeUnoLeDataPoolListElementDataChangeCommand(
         oru32_NodeIndex,
         oru32_DataPoolIndex, oru32_DataPoolListIndex,
         opc_DataPoolListModelViewManager,
         oru32_DataPoolListElementIndex, orc_NewData, ore_DataChangeType, oru32_ArrayIndex, ors32_DataSetIndex);

   DoPush(pc_ChangeCommand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set undo stack

   \param[in,out] opc_Value Undo stack
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListManager::SetUndoStack(C_UtiUndoStack * const opc_Value)
{
   this->mpc_UndoStack = opc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Push command on stack

   Object is handled by stack or deleted

   \param[in,out] opc_Command Command to push onto undo stack
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListManager::DoPush(QUndoCommand * const opc_Command)
{
   if (opc_Command != NULL)
   {
      if (mpc_UndoStack != NULL)
      {
         this->mpc_UndoStack->DoPush(opc_Command);
      }
      else
      {
         opc_Command->redo();
         delete (opc_Command);
      }
   }
}
