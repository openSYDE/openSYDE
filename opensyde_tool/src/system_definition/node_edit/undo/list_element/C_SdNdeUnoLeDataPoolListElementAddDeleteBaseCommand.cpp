//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element base class for add and delete commands (implementation)

   Data pool list element base class for add and delete commands

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
#include "C_SdNdeDpListTableView.hpp"
#include "TglUtils.hpp"
#include "C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::tgl;
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
C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand(
   const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_DataPoolListIndex,
   C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager, const std::vector<uint32_t> & orc_Indices,
   const QString & orc_Text, QUndoCommand * const opc_Parent) :
   C_SdNdeUnoLeDataPoolListElementBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                              opc_DataPoolListModelViewManager,
                                              orc_Text, opc_Parent),
   mc_Indices(orc_Indices)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add from internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::m_Add(void)
{
   const uint16_t u16_TimerId = osc_write_log_performance_start();

   //Sort
   m_SortAscending();
   if (this->mpc_DataPoolListModelViewManager != NULL)
   {
      C_SdNdeDpListTableModel * const pc_Model = this->mpc_DataPoolListModelViewManager->GetElementModel(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);
      if (pc_Model != NULL)
      {
         std::vector<std::vector<uint32_t> > c_Continous;
         //Insert
         c_Continous = pc_Model->DoInsertRows(this->mc_OscContent, this->mc_UiContent, this->mc_Indices);
         m_ReSelect(c_Continous, true);
      }
   }

   osc_write_log_performance_stop(u16_TimerId, "Insert rows");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save and delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::m_Delete(void)
{
   const uint16_t u16_TimerId = osc_write_log_performance_start();

   this->mc_OscContent.resize(this->mc_Indices.size());
   this->mc_UiContent.resize(this->mc_Indices.size());
   m_SortAscending();
   if (this->mpc_DataPoolListModelViewManager != NULL)
   {
      C_SdNdeDpListTableView * const pc_View = this->mpc_DataPoolListModelViewManager->GetElementView(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);
      C_SdNdeDpListTableModel * const pc_Model = this->mpc_DataPoolListModelViewManager->GetElementModel(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);
      if (pc_Model != NULL)
      {
         //Clear selection
         if (pc_View != NULL)
         {
            pc_View->clearSelection();
         }
         //Store last
         for (uint32_t u32_Index = this->mc_Indices.size(); u32_Index > 0UL; --u32_Index)
         {
            tgl_assert(C_PuiSdHandler::h_GetInstance()->GetDataPoolListElement(
                          this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex,
                          this->mc_Indices[static_cast<std::vector<uint32_t>::size_type >(u32_Index - 1UL)],
                          this->mc_OscContent[static_cast<std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement>
                                                          ::
                                                          size_type >(u32_Index - 1UL)],
                          this->mc_UiContent[static_cast<std::vector<C_PuiSdNodeDataPoolListElement>::size_type >(
                                                u32_Index - 1UL)]) ==
                       C_NO_ERR);
         }
         //Remove action
         pc_Model->DoRemoveRows(this->mc_Indices);

         //Select one over highest deleted index
         if (pc_View != NULL)
         {
            if (this->mc_Indices.size() > 0)
            {
               int32_t s32_NewSelection = this->mc_Indices[this->mc_Indices.size() - 1];
               if ((pc_Model->rowCount() > s32_NewSelection) && (s32_NewSelection >= 0))
               {
                  pc_View->selectRow(s32_NewSelection);
               }
               else
               {
                  //Other direction
                  --s32_NewSelection;
                  if ((pc_Model->rowCount() > s32_NewSelection) && (s32_NewSelection >= 0))
                  {
                     pc_View->selectRow(s32_NewSelection);
                  }
               }
            }
         }
      }
   }

   osc_write_log_performance_stop(u16_TimerId, "Delete rows");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set indices

   \param[in] orc_Value Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::m_SetIndices(const std::vector<uint32_t> & orc_Value)
{
   this->mc_Indices = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set initial data

   \param[in] orc_OscContent Initial OSC content
   \param[in] orc_UiContent  Initial UI content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::m_SetInitialData(
   const std::vector<C_OscNodeDataPoolListElement> & orc_OscContent,
   const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiContent)
{
   const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);

   this->mc_OscContent = orc_OscContent;
   this->mc_UiContent = orc_UiContent;
   //Adapt data set size to requirement
   if (pc_List != NULL)
   {
      const uint32_t u32_DataSetSize = pc_List->c_DataSets.size();
      for (uint32_t u32_ItOscData = 0; u32_ItOscData < this->mc_OscContent.size(); ++u32_ItOscData)
      {
         C_OscNodeDataPoolListElement & rc_OscData = this->mc_OscContent[u32_ItOscData];
         const uint32_t u32_InitialDataSetSize = rc_OscData.c_DataSetValues.size();
         rc_OscData.c_DataSetValues.resize(u32_DataSetSize);
         //Initialize data set content
         for (uint32_t u32_ItNewDataSet = u32_InitialDataSetSize; u32_ItNewDataSet < u32_DataSetSize;
              ++u32_ItNewDataSet)
         {
            C_OscNodeDataPoolContent & rc_DataSetContent = rc_OscData.c_DataSetValues[u32_ItNewDataSet];
            rc_DataSetContent = rc_OscData.c_MinValue;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select the specified item ranges

   \param[in] orc_Items       Items to select (sorted ascending!)
   \param[in] oq_ScrollToLast Optional flag to scroll to the last element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::m_ReSelect(
   const std::vector<std::vector<uint32_t> > & orc_Items, const bool oq_ScrollToLast)
{
   C_SdNdeDpListTableView * const pc_View = this->mpc_DataPoolListModelViewManager->GetElementView(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);

   if (pc_View != NULL)
   {
      pc_View->clearSelection();
      //New selection
      for (uint32_t u32_ItSection = 0UL; u32_ItSection < orc_Items.size(); ++u32_ItSection)
      {
         const std::vector<uint32_t> & rc_Section = orc_Items[u32_ItSection];
         if (rc_Section.size() > 0UL)
         {
            pc_View->SelectRange(rc_Section[0UL], rc_Section[static_cast<std::vector<uint32_t>::
                                                                         size_type>(static_cast<uint32_t>(rc_Section
                                                                                                          .size())
                                                                                    -
                                                                                    1UL)]);
            //scroll to last
            if ((oq_ScrollToLast) && (u32_ItSection == (static_cast<uint32_t>(orc_Items.size()) - 1UL)))
            {
               C_SdNdeDpListTableModel * const pc_Model =
                  this->mpc_DataPoolListModelViewManager->GetElementModel(
                     this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);
               if (pc_Model != NULL)
               {
                  pc_View->scrollTo(pc_Model->index(rc_Section[static_cast<std::vector<uint32_t>::
                                                                           size_type>(static_cast<uint32_t>(rc_Section
                                                                                                            .size())
                                                                                      -
                                                                                      1UL)], 0));
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort indices descending
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::m_SortDescending(void)
{
   C_SdUtil::h_SortIndicesDescendingAndSync<C_OscNodeDataPoolListElement, C_PuiSdNodeDataPoolListElement>(
      this->mc_Indices, this->mc_OscContent, this->mc_UiContent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort indices ascending
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::m_SortAscending(void)
{
   C_SdUtil::h_SortIndicesAscendingAndSync<C_OscNodeDataPoolListElement, C_PuiSdNodeDataPoolListElement>(
      this->mc_Indices, this->mc_OscContent, this->mc_UiContent);
}
