//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool command stack (implementation)

   Data pool command stack

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdNdeUnoDataPoolManager.hpp"
#include "C_SdNdeUnoDataPoolListMoveCommand.hpp"
#include "C_SdNdeDpListsTreeWidget.hpp"
#include "C_SdNdeUnoDataPoolListDeleteCommand.hpp"
#include "C_SdNdeUnoDataPoolListPasteCommand.hpp"
#include "C_SdNdeUnoDataPoolListAddCommand.hpp"
#include "C_SdNdeUnoDataPoolListDataChangeCommand.hpp"

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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDataPoolManager::C_SdNdeUnoDataPoolManager(QObject * const opc_Parent) :
   C_UtiUndoStack(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDataPoolManager::~C_SdNdeUnoDataPoolManager(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register move command

   \param[in]     oru32_NodeIndex             Node index
   \param[in]     oru32_DataPoolIndex         Node data pool index
   \param[in,out] opc_DataPoolListsTreeWidget Data pool lists tree widget to perform actions on
   \param[in]     orc_StartIndices          Source row index
   \param[in]     orc_TargetIndices         Target row index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolManager::DoMoveList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                           C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                           const std::vector<uint32_t> & orc_StartIndices,
                                           const std::vector<uint32_t> & orc_TargetIndices)
{
   if (((orc_StartIndices.size() > 0) && (orc_TargetIndices.size() > 0)) &&
       (orc_StartIndices.size() == orc_TargetIndices.size()))
   {
      C_SdNdeUnoDataPoolListMoveCommand * const pc_MoveCommand = new C_SdNdeUnoDataPoolListMoveCommand(oru32_NodeIndex,
                                                                                                       oru32_DataPoolIndex,
                                                                                                       opc_DataPoolListsTreeWidget,
                                                                                                       orc_StartIndices,
                                                                                                       orc_TargetIndices);

      this->DoPush(pc_MoveCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do delete

   \param[in]     oru32_NodeIndex             Node index
   \param[in]     oru32_DataPoolIndex         Node data pool index
   \param[in,out] opc_DataPoolListsTreeWidget Data pool lists tree widget to perform actions on
   \param[in]     orc_Indices                 Node data pool list indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolManager::DoDeleteList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                             stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                             const std::vector<uint32_t> & orc_Indices)
{
   if (orc_Indices.size() > 0)
   {
      C_SdNdeUnoDataPoolListDeleteCommand * const pc_DeleteCommand = new C_SdNdeUnoDataPoolListDeleteCommand(
         oru32_NodeIndex,
         oru32_DataPoolIndex,
         opc_DataPoolListsTreeWidget,
         orc_Indices);
      //Check if last element(s) -> add replacement, so we never encounter zero list items
      if (opc_DataPoolListsTreeWidget != NULL)
      {
         if (static_cast<uint32_t>(opc_DataPoolListsTreeWidget->topLevelItemCount()) <= orc_Indices.size())
         {
            std::vector<uint32_t> c_Tmp;
            c_Tmp.push_back(0);
            new C_SdNdeUnoDataPoolListAddCommand(oru32_NodeIndex, oru32_DataPoolIndex, opc_DataPoolListsTreeWidget,
                                                 c_Tmp, pc_DeleteCommand);
         }
      }
      this->DoPush(pc_DeleteCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do paste

   \param[in]     oru32_NodeIndex             Node index
   \param[in]     oru32_DataPoolIndex         Node data pool index
   \param[in,out] opc_DataPoolListsTreeWidget Data pool lists tree widget to perform actions on
   \param[in]     oru32_InsertListIndex        Node data pool list index to insert new elements at
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolManager::DoPaste(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                        C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                        const uint32_t & oru32_InsertListIndex)
{
   C_SdNdeUnoDataPoolListPasteCommand * const pc_PasteCommand = new C_SdNdeUnoDataPoolListPasteCommand(oru32_NodeIndex,
                                                                                                       oru32_DataPoolIndex,
                                                                                                       opc_DataPoolListsTreeWidget);

   if (pc_PasteCommand->InitialSetup(oru32_InsertListIndex) == true)
   {
      //Accept
      this->DoPush(pc_PasteCommand);
   }
   else
   {
      //Discard
      delete (pc_PasteCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do add

   \param[in]     oru32_NodeIndex             Node index
   \param[in]     oru32_DataPoolIndex         Node data pool index
   \param[in,out] opc_DataPoolListsTreeWidget Data pool lists tree widget to perform actions on
   \param[in]     orc_Indices                 Node data pool list indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolManager::DoAddList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                          stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                          const std::vector<uint32_t> & orc_Indices)
{
   if (orc_Indices.size() > 0)
   {
      C_SdNdeUnoDataPoolListAddCommand * const pc_AddCommand = new C_SdNdeUnoDataPoolListAddCommand(oru32_NodeIndex,
                                                                                                    oru32_DataPoolIndex,
                                                                                                    opc_DataPoolListsTreeWidget,
                                                                                                    orc_Indices);

      this->DoPush(pc_AddCommand);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do data change

   \param[in]     oru32_NodeIndex                Node index
   \param[in]     oru32_DataPoolIndex            Node data pool index
   \param[in,out] opc_DataPoolListsTreeWidget    Data pool lists tree widget to perform actions on
   \param[in]     oru32_DataPoolListIndex        Node data pool list index
   \param[in]     orc_NewData                    New data
   \param[in]     ore_DataChangeType             Data change type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolManager::DoChangeListData(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                                 stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget, const uint32_t & oru32_DataPoolListIndex, const QVariant & orc_NewData,
                                                 const C_SdNdeDpUtil::E_ListDataChangeType & ore_DataChangeType)
{
   C_SdNdeUnoDataPoolListDataChangeCommand * const pc_DataChangeCommand =
      new C_SdNdeUnoDataPoolListDataChangeCommand(
         oru32_NodeIndex,
         oru32_DataPoolIndex,
         opc_DataPoolListsTreeWidget,
         oru32_DataPoolListIndex, orc_NewData,
         ore_DataChangeType);

   this->DoPush(pc_DataChangeCommand);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolManager::m_CleanupAction(void)
{
   Q_EMIT this->SigChanged();
   C_UtiUndoStack::m_CleanupAction();
}
