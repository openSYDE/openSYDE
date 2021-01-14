//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list move undo command (implementation)

   Data pool list move undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_Uti.h"
#include "TGLUtils.h"
#include "C_SdNdeUnoDataPoolListMoveCommand.h"
#include "C_PuiSdHandler.h"
#include "C_SdUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]     oru32_NodeIndex             Node index
   \param[in]     oru32_DataPoolIndex         Node data pool index
   \param[in,out] opc_DataPoolListsTreeWidget Data pool lists tree widget to perform actions on
   \param[in]     oru32_SourceRows            Source rows
   \param[in]     oru32_TargetRows            Target rows
   \param[in,out] opc_Parent                  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDataPoolListMoveCommand::C_SdNdeUnoDataPoolListMoveCommand(const uint32 & oru32_NodeIndex,
                                                                     const uint32 & oru32_DataPoolIndex,
                                                                     stw_opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListMoveCommand::redo(void)
{
   this->m_DoMoveRows(this->mc_SourceRows, this->mc_TargetRows);
   //Update
   this->mpc_DataPoolListsTreeWidget->UpdateUI();
   C_SdNdeUnoDataPoolListBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListMoveCommand::undo(void)
{
   C_SdNdeUnoDataPoolListBaseCommand::undo();
   this->m_DoMoveRows(this->mc_TargetRows, this->mc_SourceRows);
   //Update
   this->mpc_DataPoolListsTreeWidget->UpdateUI();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle move selected items action

   Warning 1: only works if target indices have same contiguous property as selected indices
   i.e. if you make the selected indices contiguous by resorting the target indices should also become contiguous
   Warning 2: does no boundary check

   \param[in] orc_SelectedIndices Selected row indices (Expected: unique)
   \param[in] orc_TargetIndices   Target row indices (Expected: unique)
                                  Example value:
                                   "move up" -> orc_SelectedIndices - 1
                                   "move down" -> orc_TargetIndices + 1
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListMoveCommand::m_DoMoveRows(const std::vector<uint32> & orc_SelectedIndices,
                                                     const std::vector<uint32> & orc_TargetIndices)
{
   if (orc_SelectedIndices.size() == orc_TargetIndices.size())
   {
      std::vector<uint32> c_SelectedIndicesCopy = orc_SelectedIndices;
      std::vector<uint32> c_TargetIndicesCopy = orc_TargetIndices;
      std::vector<std::vector<uint32> > c_ContiguousSections;
      uint32 u32_TargetAccessIndex = 0UL;

      //Step 1 sort (so the next step can assume the contiguous selection has the same order!
      C_Uti::h_SortIndicesAscendingAndSync(c_SelectedIndicesCopy, c_TargetIndicesCopy);

      //Step 2: get contiguous selection
      c_ContiguousSections = C_Uti::h_GetContiguousSectionsAscending(orc_SelectedIndices);

      //Step 3: move
      for (uint32 u32_ItSection = 0UL; u32_ItSection < c_ContiguousSections.size(); ++u32_ItSection)
      {
         const std::vector<uint32> & rc_Section = c_ContiguousSections[u32_ItSection];
         if (rc_Section.size() > 0UL)
         {
            uint32 u32_TargetIndex = c_TargetIndicesCopy[u32_TargetAccessIndex];
            uint32 u32_TargetIndexParam = c_TargetIndicesCopy[u32_TargetAccessIndex];
            //+1UL seems to fit the documentation of beginMoveRows of what we want to achieve here
            if (u32_TargetIndex > rc_Section[0UL])
            {
               //Qt interface seems to insert the items before removing anything so the "new position" has to have an
               // bigger offset
               u32_TargetIndex += rc_Section.size();
               //We insert after removing so this has to be considered
               u32_TargetIndexParam = u32_TargetIndex - 1UL;
            }
            this->m_MoveItems(rc_Section, u32_TargetIndexParam);
            u32_TargetAccessIndex += rc_Section.size();
         }
      }
   }
   this->mpc_DataPoolListsTreeWidget->clearSelection();
   for (uint32 u32_Index = 0; u32_Index < orc_TargetIndices.size(); ++u32_Index)
   {
      QTreeWidgetItem * const pc_Item = this->mpc_DataPoolListsTreeWidget->topLevelItem(orc_TargetIndices[u32_Index]);
      tgl_assert(pc_Item != NULL);
      if (pc_Item != NULL)
      {
         pc_Item->setSelected(true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move items to target index

   \param[in] orc_ContiguousIndices Contiguous section of ascending indices
   \param[in] ou32_TargetIndex      Target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListMoveCommand::m_MoveItems(const std::vector<uint32> & orc_ContiguousIndices,
                                                    const uint32 ou32_TargetIndex) const
{
   if (orc_ContiguousIndices.size() > 0UL)
   {
      std::vector<uint32> c_ContiguousIndicesCopy = orc_ContiguousIndices;
      bool q_Forward;
      if (c_ContiguousIndicesCopy[0UL] < ou32_TargetIndex)
      {
         q_Forward = true;
      }
      else
      {
         q_Forward = false;
      }
      for (uint32 u32_ItItem = 0UL; u32_ItItem < c_ContiguousIndicesCopy.size(); ++u32_ItItem)
      {
         if (q_Forward == true)
         {
            //There should always be a "new item" at the same position
            this->m_MoveItem(orc_ContiguousIndices[0UL], ou32_TargetIndex);
         }
         else
         {
            //The item position should not change but the target will
            this->m_MoveItem(orc_ContiguousIndices[u32_ItItem], ou32_TargetIndex + u32_ItItem);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move specific item

   For implementation: First delete ou32_SourceIndex then insert ou32_TargetIndex
   Warning: not expected to fail

   \param[in] ou32_SourceIndex Source index
   \param[in] ou32_TargetIndex Target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListMoveCommand::m_MoveItem(const uint32 ou32_SourceIndex, const uint32 ou32_TargetIndex) const
{
   C_PuiSdHandler::h_GetInstance()->MoveDataPoolList(this->mu32_NodeIndex, this->mu32_DataPoolIndex, ou32_SourceIndex,
                                                     ou32_TargetIndex);
}
