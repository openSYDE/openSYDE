//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set base class for add and delete commands (implementation)

   Data pool list data set base class for add and delete commands

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdUtil.hpp"
#include "C_SdNdeDpListTableView.hpp"
#include "C_SdNdeDpListTableModel.hpp"
#include "C_SdNdeDpListDataSetView.hpp"
#include "C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand.hpp"

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

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in]     orc_Indices                      Node data pool list indices
   \param[in]     orc_Text                         Optional command text for informational display
   \param[in,out] opc_Parent                       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand::C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand(
   const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_DataPoolListIndex,
   C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager, const std::vector<uint32_t> & orc_Indices,
   const QString & orc_Text, QUndoCommand * const opc_Parent) :
   C_SdNdeUnoDasDataPoolListBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                        opc_DataPoolListModelViewManager,
                                        orc_Text, opc_Parent),
   mq_Initial(true),
   mc_Indices(orc_Indices)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add from internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand::m_Add(void)
{
   if (this->mpc_DataPoolListModelViewManager != NULL)
   {
      C_SdNdeDpListDataSetModel * const pc_DataSetModel = this->mpc_DataPoolListModelViewManager->GetDataSetModel(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex,
         this->mu32_DataPoolListIndex);
      if (pc_DataSetModel != NULL)
      {
         C_SdNdeDpListTableModel * const pc_ElementModel =
            this->mpc_DataPoolListModelViewManager->GetElementModel(
               this->mu32_NodeIndex, this->mu32_DataPoolIndex,
               this->mu32_DataPoolListIndex);
         C_SdNdeDpListTableView * const pc_ElementView =
            this->mpc_DataPoolListModelViewManager->GetElementView(
               this->mu32_NodeIndex, this->mu32_DataPoolIndex,
               this->mu32_DataPoolListIndex);
         C_SdNdeDpListDataSetView * const pc_View = this->mpc_DataPoolListModelViewManager->GetDataSetView(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex,
            this->mu32_DataPoolListIndex);
         m_SortAscending();
         //Clear selection
         if (pc_View != NULL)
         {
            pc_View->clearSelection();
         }
         for (uint32_t u32_Index = 0; u32_Index < this->mc_Indices.size(); ++u32_Index)
         {
            std::vector<stw::opensyde_core::C_OscNodeDataPoolDataSet> c_OscNames;
            std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> > c_OscDataSetValues;
            c_OscNames.push_back(this->mc_OscNames[u32_Index]);
            c_OscDataSetValues.push_back(this->mc_OscDataSetValues[u32_Index]);
            pc_DataSetModel->DoInsertColumns(c_OscNames, c_OscDataSetValues,
                                             this->mc_Indices[u32_Index]);
            //Select new row
            if (pc_View != NULL)
            {
               pc_View->selectColumn(this->mc_Indices[u32_Index]);
            }
            //Register ui changes
            if (this->mq_Initial == false)
            {
               this->mpc_DataPoolListModelViewManager->RegisterDataSetCountChange(this->mu32_NodeIndex,
                                                                                  this->mu32_DataPoolIndex,
                                                                                  this->mu32_DataPoolListIndex);
               if (pc_ElementModel != NULL)
               {
                  pc_ElementModel->DoInsertDataSet(this->mc_Indices[u32_Index]);
                  if (pc_ElementView != NULL)
                  {
                     pc_ElementView->UpdateSelectionForNewDataSetColumn(this->mc_Indices[u32_Index]);
                  }
               }
            }
         }
         //Register error change
         pc_DataSetModel->HandleErrorChange();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save and delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand::m_Delete(void)
{
   if (this->mpc_DataPoolListModelViewManager != NULL)
   {
      C_SdNdeDpListDataSetModel * const pc_DataSetModel = this->mpc_DataPoolListModelViewManager->GetDataSetModel(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex,
         this->mu32_DataPoolListIndex);
      if (pc_DataSetModel != NULL)
      {
         C_SdNdeDpListTableModel * const pc_ElementModel =
            this->mpc_DataPoolListModelViewManager->GetElementModel(
               this->mu32_NodeIndex, this->mu32_DataPoolIndex,
               this->mu32_DataPoolListIndex);
         C_SdNdeDpListDataSetView * const pc_View = this->mpc_DataPoolListModelViewManager->GetDataSetView(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex,
            this->mu32_DataPoolListIndex);
         this->mc_OscNames.resize(this->mc_Indices.size());
         this->mc_OscDataSetValues.resize(this->mc_Indices.size());
         m_SortDescending();
         //Clear selection
         if (pc_View != NULL)
         {
            pc_View->clearSelection();
         }
         for (uint32_t u32_Index = 0; u32_Index < this->mc_Indices.size(); ++u32_Index)
         {
            C_PuiSdHandler::h_GetInstance()->GetDataPoolListDataSet(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                    this->mu32_DataPoolListIndex,
                                                                    this->mc_Indices[u32_Index],
                                                                    this->mc_OscNames[u32_Index],
                                                                    this->mc_OscDataSetValues[u32_Index]);
            pc_DataSetModel->removeColumn(this->mc_Indices[u32_Index]);
            //Register ui changes
            if (this->mq_Initial == false)
            {
               this->mpc_DataPoolListModelViewManager->RegisterDataSetCountChange(this->mu32_NodeIndex,
                                                                                  this->mu32_DataPoolIndex,
                                                                                  this->mu32_DataPoolListIndex);
               if (pc_ElementModel != NULL)
               {
                  pc_ElementModel->DoDeleteDataSet(this->mc_Indices[u32_Index]);
               }
            }
         }
         //Register error change
         pc_DataSetModel->HandleErrorChange();
         //Select one over highest deleted index
         if (pc_View != NULL)
         {
            if (this->mc_Indices.size() > 0)
            {
               int32_t s32_NewSelection = this->mc_Indices[this->mc_Indices.size() - 1];
               if ((pc_DataSetModel->columnCount() > s32_NewSelection) && (s32_NewSelection >= 0))
               {
                  pc_View->selectColumn(s32_NewSelection);
               }
               else
               {
                  //Other direction
                  --s32_NewSelection;
                  if ((pc_DataSetModel->columnCount() > s32_NewSelection) && (s32_NewSelection >= 0))
                  {
                     pc_View->selectColumn(s32_NewSelection);
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set indices

   \param[in] orc_Value Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand::m_SetIndices(const std::vector<uint32_t> & orc_Value)
{
   mc_Indices = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set initial data

   \param[in] orc_OscNames         Initial Data set names
   \param[in] orc_OscDataSetValues Initial Data set values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand::m_SetInitialData(
   const std::vector<C_OscNodeDataPoolDataSet> & orc_OscNames,
   const std::vector<std::vector<C_OscNodeDataPoolContent> > & orc_OscDataSetValues)
{
   this->mc_OscNames = orc_OscNames;
   this->mc_OscDataSetValues = orc_OscDataSetValues;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort indices descending
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand::m_SortDescending(void)
{
   C_SdUtil::h_SortIndicesDescendingAndSync<C_OscNodeDataPoolDataSet, std::vector<C_OscNodeDataPoolContent> >(
      this->mc_Indices, this->mc_OscNames, this->mc_OscDataSetValues);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort indices ascending
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand::m_SortAscending(void)
{
   C_SdUtil::h_SortIndicesAscendingAndSync<C_OscNodeDataPoolDataSet, std::vector<C_OscNodeDataPoolContent> >(
      this->mc_Indices, this->mc_OscNames, this->mc_OscDataSetValues);
}
