//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list base class for add and delete commands (implementation)

   Data pool list base class for add and delete commands

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdUtil.hpp"
#include "TglUtils.hpp"

#include "C_SdNdeUnoDataPoolListAddDeleteBaseCommand.hpp"
/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::tgl;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

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
   \param[in]     orc_Indices                 Node data pool list indices
   \param[in]     orc_Text                    Optional command text for informational display
   \param[in,out] opc_Parent                  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDataPoolListAddDeleteBaseCommand::C_SdNdeUnoDataPoolListAddDeleteBaseCommand(const uint32_t & oru32_NodeIndex,
                                                                                       const uint32_t & oru32_DataPoolIndex, stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                                                                       const std::vector<uint32_t> & orc_Indices, const QString & orc_Text,
                                                                                       QUndoCommand * const opc_Parent)
   :
   C_SdNdeUnoDataPoolListBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, opc_DataPoolListsTreeWidget,
                                     orc_Text, opc_Parent),
   mc_Indices(orc_Indices)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add from internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddDeleteBaseCommand::m_Add(void)
{
   const uint16_t u16_TimerId = osc_write_log_performance_start();

   m_SortAscending();
   for (uint32_t u32_Index = 0; u32_Index < this->mc_Indices.size(); ++u32_Index)
   {
      this->mpc_DataPoolListsTreeWidget->InsertRowWithoutData(this->mc_Indices[u32_Index]);
      tgl_assert(C_PuiSdHandler::h_GetInstance()->InsertDataPoolList(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                     this->mc_Indices[u32_Index],
                                                                     this->mc_OscContent[u32_Index],
                                                                     this->mc_UiContent[u32_Index]) == C_NO_ERR);
   }
   //Before selection: update UI
   m_UpdateModels();
   this->mpc_DataPoolListsTreeWidget->UpdateUi();
   //Register error change
   this->mpc_DataPoolListsTreeWidget->HandleErrorChange();
   //Selection
   this->mpc_DataPoolListsTreeWidget->clearSelection();
   for (uint32_t u32_Index = 0; u32_Index < this->mc_Indices.size(); ++u32_Index)
   {
      QTreeWidgetItem * const pc_Item = this->mpc_DataPoolListsTreeWidget->topLevelItem(this->mc_Indices[u32_Index]);
      tgl_assert(pc_Item != NULL);
      if (pc_Item != NULL)
      {
         pc_Item->setSelected(true);
         //Check last item (It's sorted)
         if ((u32_Index + 1UL) == this->mc_Indices.size())
         {
            this->mpc_DataPoolListsTreeWidget->setCurrentItem(pc_Item, 0, QItemSelectionModel::SelectCurrent);
         }
      }
   }

   osc_write_log_performance_stop(u16_TimerId, "Add list");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save and delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddDeleteBaseCommand::m_Delete(void)
{
   const uint16_t u16_TimerId = osc_write_log_performance_start();

   this->mc_OscContent.resize(this->mc_Indices.size());
   this->mc_UiContent.resize(this->mc_Indices.size());
   m_SortDescending();
   //Clear selection
   for (uint32_t u32_Index = 0; u32_Index < this->mc_Indices.size(); ++u32_Index)
   {
      tgl_assert(C_PuiSdHandler::h_GetInstance()->GetDataPoolList(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                  this->mc_Indices[u32_Index],
                                                                  this->mc_OscContent[u32_Index],
                                                                  this->mc_UiContent[u32_Index]) == C_NO_ERR);
      tgl_assert(C_PuiSdHandler::h_GetInstance()->RemoveDataPoolList(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                     this->mc_Indices[u32_Index]) == C_NO_ERR);
      this->mpc_DataPoolListsTreeWidget->takeTopLevelItem(this->mc_Indices[u32_Index]);
   }
   //Before selection: update UI
   m_UpdateModels();
   this->mpc_DataPoolListsTreeWidget->UpdateUi();
   //Register error change
   this->mpc_DataPoolListsTreeWidget->HandleErrorChange();
   //Selection
   this->mpc_DataPoolListsTreeWidget->clearSelection();
   //Select one over highest deleted index
   if (this->mc_Indices.size() > 0)
   {
      int32_t s32_NewSelection = this->mc_Indices[this->mc_Indices.size() - 1];
      if ((this->mpc_DataPoolListsTreeWidget->topLevelItemCount() > s32_NewSelection) && (s32_NewSelection >= 0))
      {
         QTreeWidgetItem * const pc_Item = this->mpc_DataPoolListsTreeWidget->topLevelItem(s32_NewSelection);
         if (pc_Item != NULL)
         {
            pc_Item->setSelected(true);
            this->mpc_DataPoolListsTreeWidget->setCurrentItem(pc_Item);
         }
      }
      else
      {
         //Other direction
         --s32_NewSelection;
         if ((this->mpc_DataPoolListsTreeWidget->topLevelItemCount() > s32_NewSelection) && (s32_NewSelection >= 0))
         {
            QTreeWidgetItem * const pc_Item = this->mpc_DataPoolListsTreeWidget->topLevelItem(s32_NewSelection);
            if (pc_Item != NULL)
            {
               pc_Item->setSelected(true);
               this->mpc_DataPoolListsTreeWidget->setCurrentItem(pc_Item);
            }
         }
      }
   }

   osc_write_log_performance_stop(u16_TimerId, "Delete list");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set indices

   \param[in] orc_Value Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddDeleteBaseCommand::m_SetIndices(const std::vector<uint32_t> & orc_Value)
{
   mc_Indices = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set initial data

   \param[in] orc_OscContent Initial OSC content
   \param[in] orc_UiContent  Initial UI content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddDeleteBaseCommand::m_SetInitialData(
   const std::vector<C_OscNodeDataPoolList> & orc_OscContent,
   const std::vector<C_PuiSdNodeDataPoolList> & orc_UiContent)
{
   this->mc_OscContent = orc_OscContent;
   this->mc_UiContent = orc_UiContent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort indices descending
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddDeleteBaseCommand::m_SortDescending(void)
{
   C_SdUtil::h_SortIndicesDescendingAndSync<C_OscNodeDataPoolList, C_PuiSdNodeDataPoolList>(this->mc_Indices,
                                                                                            this->mc_OscContent,
                                                                                            this->mc_UiContent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort indices ascending
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddDeleteBaseCommand::m_SortAscending(void)
{
   C_SdUtil::h_SortIndicesAscendingAndSync<C_OscNodeDataPoolList, C_PuiSdNodeDataPoolList>(this->mc_Indices,
                                                                                           this->mc_OscContent,
                                                                                           this->mc_UiContent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register model change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddDeleteBaseCommand::m_UpdateModels(void)
{
   if (this->mpc_DataPoolListsTreeWidget != NULL)
   {
      this->mpc_DataPoolListsTreeWidget->UpdateModels();
   }
}
