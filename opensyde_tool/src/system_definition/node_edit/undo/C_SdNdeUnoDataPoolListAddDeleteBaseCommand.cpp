//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list base class for add and delete commands (implementation)

   Data pool list base class for add and delete commands

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QElapsedTimer>

#include "stwtypes.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_PuiSdHandler.h"
#include "C_SdUtil.h"
#include "TGLUtils.h"

#include "C_SdNdeUnoDataPoolListAddDeleteBaseCommand.h"
/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

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
C_SdNdeUnoDataPoolListAddDeleteBaseCommand::C_SdNdeUnoDataPoolListAddDeleteBaseCommand(const uint32 & oru32_NodeIndex,
                                                                                       const uint32 & oru32_DataPoolIndex, stw_opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                                                                       const std::vector<uint32> & orc_Indices, const QString & orc_Text,
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
void C_SdNdeUnoDataPoolListAddDeleteBaseCommand::Add(void)
{
   QElapsedTimer c_Timer;

   if (mq_TIMING_OUTPUT)
   {
      c_Timer.start();
   }

   m_SortAscending();
   for (uint32 u32_Index = 0; u32_Index < this->mc_Indices.size(); ++u32_Index)
   {
      this->mpc_DataPoolListsTreeWidget->InsertRowWithoutData(this->mc_Indices[u32_Index]);
      tgl_assert(C_PuiSdHandler::h_GetInstance()->InsertDataPoolList(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                     this->mc_Indices[u32_Index],
                                                                     this->mc_OSCContent[u32_Index],
                                                                     this->mc_UIContent[u32_Index]) == C_NO_ERR);
   }
   //Before selection: update UI
   m_UpdateModels();
   this->mpc_DataPoolListsTreeWidget->UpdateUI();
   //Register error change
   this->mpc_DataPoolListsTreeWidget->HandleErrorChange();
   //Selection
   this->mpc_DataPoolListsTreeWidget->clearSelection();
   for (uint32 u32_Index = 0; u32_Index < this->mc_Indices.size(); ++u32_Index)
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
   if (mq_TIMING_OUTPUT)
   {
      std::cout << "List add " << c_Timer.elapsed() << " ms" << &std::endl;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save and delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddDeleteBaseCommand::Delete(void)
{
   QElapsedTimer c_Timer;

   if (mq_TIMING_OUTPUT)
   {
      c_Timer.start();
   }

   this->mc_OSCContent.resize(this->mc_Indices.size());
   this->mc_UIContent.resize(this->mc_Indices.size());
   m_SortDescending();
   //Clear selection
   for (uint32 u32_Index = 0; u32_Index < this->mc_Indices.size(); ++u32_Index)
   {
      tgl_assert(C_PuiSdHandler::h_GetInstance()->GetDataPoolList(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                  this->mc_Indices[u32_Index],
                                                                  this->mc_OSCContent[u32_Index],
                                                                  this->mc_UIContent[u32_Index]) == C_NO_ERR);
      tgl_assert(C_PuiSdHandler::h_GetInstance()->RemoveDataPoolList(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                     this->mc_Indices[u32_Index]) == C_NO_ERR);
      this->mpc_DataPoolListsTreeWidget->takeTopLevelItem(this->mc_Indices[u32_Index]);
   }
   //Before selection: update UI
   m_UpdateModels();
   this->mpc_DataPoolListsTreeWidget->UpdateUI();
   //Register error change
   this->mpc_DataPoolListsTreeWidget->HandleErrorChange();
   //Selection
   this->mpc_DataPoolListsTreeWidget->clearSelection();
   //Select one over highest deleted index
   if (this->mc_Indices.size() > 0)
   {
      sint32 s32_NewSelection = this->mc_Indices[this->mc_Indices.size() - 1];
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
   if (mq_TIMING_OUTPUT)
   {
      std::cout << "List delete " << c_Timer.elapsed() << " ms" << &std::endl;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set indices

   \param[in] orc_Value Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddDeleteBaseCommand::SetIndices(const std::vector<stw_types::uint32> & orc_Value)
{
   mc_Indices = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set initial data

   \param[in] orc_OSCContent Initial OSC content
   \param[in] orc_UIContent  Initial UI content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddDeleteBaseCommand::SetInitialData(
   const std::vector<C_OSCNodeDataPoolList> & orc_OSCContent,
   const std::vector<C_PuiSdNodeDataPoolList> & orc_UIContent)
{
   this->mc_OSCContent = orc_OSCContent;
   this->mc_UIContent = orc_UIContent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort indices descending
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddDeleteBaseCommand::m_SortDescending(void)
{
   C_SdUtil::h_SortIndicesDescendingAndSync<C_OSCNodeDataPoolList, C_PuiSdNodeDataPoolList>(this->mc_Indices,
                                                                                            this->mc_OSCContent,
                                                                                            this->mc_UIContent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort indices ascending
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddDeleteBaseCommand::m_SortAscending(void)
{
   C_SdUtil::h_SortIndicesAscendingAndSync<C_OSCNodeDataPoolList, C_PuiSdNodeDataPoolList>(this->mc_Indices,
                                                                                           this->mc_OSCContent,
                                                                                           this->mc_UIContent);
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
