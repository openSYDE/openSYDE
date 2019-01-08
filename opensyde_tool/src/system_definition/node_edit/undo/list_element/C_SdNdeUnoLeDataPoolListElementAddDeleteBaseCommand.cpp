//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data pool list element base class for add and delete commands (implementation)

   Data pool list element base class for add and delete commands

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_PuiSdHandler.h"
#include "C_SdUtil.h"
#include "C_SdNdeDataPoolListTableView.h"
#include "TGLUtils.h"

#include "C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand.h"
/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;

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
   \param[in]     orc_Text                         Optional command text for informational display
   \param[in,out] opc_Parent                       Optional pointer to parent

   \created     25.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand(
   const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex, const uint32 & oru32_DataPoolListIndex,
   C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager,
   const std::vector<uint32> & orc_Indices, const QString & orc_Text, QUndoCommand * const opc_Parent) :
   C_SdNdeUnoLeDataPoolListElementBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                              opc_DataPoolListModelViewManager,
                                              orc_Text, opc_Parent),
   mc_Indices(orc_Indices)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add from internal data

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::Add(void)
{
   m_SortAscending();
   if (this->mpc_DataPoolListModelViewManager != NULL)
   {
      C_SdNdeDataPoolListTableView * const pc_View = this->mpc_DataPoolListModelViewManager->GetElementView(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);
      C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_DataPoolListModelViewManager->GetElementModel(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);
      if (pc_Model != NULL)
      {
         const bool q_Continous = this->m_CheckContinuous();
         //Clear selection
         if (pc_View != NULL)
         {
            pc_View->clearSelection();
         }
         if (q_Continous == false)
         {
            for (uint32 u32_Index = 0; u32_Index < this->mc_Indices.size(); ++u32_Index)
            {
               std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> c_OSCContent;
               std::vector<C_PuiSdNodeDataPoolListElement> c_UIContent;
               c_OSCContent.push_back(this->mc_OSCContent[u32_Index]);
               c_UIContent.push_back(this->mc_UIContent[u32_Index]);
               pc_Model->DoInsertRows(c_OSCContent, c_UIContent, this->mc_Indices[u32_Index]);
               //Select new row
               if (pc_View != NULL)
               {
                  pc_View->selectRow(this->mc_Indices[u32_Index]);
               }
            }
         }
         else
         {
            pc_Model->DoInsertRows(this->mc_OSCContent, this->mc_UIContent, this->mc_Indices[0]);
            //Select new rows
            if (pc_View != NULL)
            {
               for (uint32 u32_Index = 0; u32_Index < this->mc_Indices.size(); ++u32_Index)
               {
                  pc_View->selectRow(this->mc_Indices[u32_Index]);
               }
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save and delete

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::Delete(void)
{
   this->mc_OSCContent.resize(this->mc_Indices.size());
   this->mc_UIContent.resize(this->mc_Indices.size());
   m_SortAscending();
   if (this->mpc_DataPoolListModelViewManager != NULL)
   {
      C_SdNdeDataPoolListTableView * const pc_View = this->mpc_DataPoolListModelViewManager->GetElementView(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);
      C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_DataPoolListModelViewManager->GetElementModel(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);
      if (pc_Model != NULL)
      {
         const bool q_Continous = this->m_CheckContinuous();
         //Clear selection
         if (pc_View != NULL)
         {
            pc_View->clearSelection();
         }
         for (uint32 u32_Index = this->mc_Indices.size(); u32_Index > 0UL; --u32_Index)
         {
            tgl_assert(C_PuiSdHandler::h_GetInstance()->GetDataPoolListElement(this->mu32_NodeIndex,
                                                                               this->mu32_DataPoolIndex,
                                                                               this->mu32_DataPoolListIndex,
                                                                               this->mc_Indices[static_cast<std::vector
                                                                                                            <
                                                                                                               stw_types
                                                                                                               ::uint32>
                                                                                                            ::
                                                                                                            size_type >
                                                                                                (u32_Index - 1UL)],
                                                                               this->mc_OSCContent[static_cast<std::
                                                                                                               vector<
                                                                                                                  stw_opensyde_core
                                                                                                                  ::
                                                                                                                  C_OSCNodeDataPoolListElement>
                                                                                                               ::
                                                                                                               size_type >
                                                                                                   (u32_Index - 1UL)],
                                                                               this->mc_UIContent[static_cast<std::
                                                                                                              vector<
                                                                                                                 C_PuiSdNodeDataPoolListElement>
                                                                                                              ::
                                                                                                              size_type >
                                                                                                  (u32_Index - 1UL)]) ==
                       C_NO_ERR);
            if (q_Continous == false)
            {
               pc_Model->removeRow(this->mc_Indices[static_cast<std::vector< stw_types::uint32>::size_type >
                                                    (u32_Index - 1UL)]);
            }
         }
         if (q_Continous == true)
         {
            pc_Model->removeRows(this->mc_Indices[0], this->mc_Indices.size());
         }
         //Select one over highest deleted index
         if (pc_View != NULL)
         {
            if (this->mc_Indices.size() > 0)
            {
               sint32 s32_NewSelection = this->mc_Indices[this->mc_Indices.size() - 1];
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
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set indices

   \param[in] orc_Value Value

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::SetIndices(const std::vector<stw_types::uint32> & orc_Value)
{
   this->mc_Indices = orc_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set initial data

   \param[in] orc_OSCContent Initial OSC content
   \param[in] orc_UIContent  Initial UI content

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::SetInitialData(
   const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCContent,
   const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UIContent)
{
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);

   this->mc_OSCContent = orc_OSCContent;
   this->mc_UIContent = orc_UIContent;
   //Adapt data set size to requirement
   if (pc_List != NULL)
   {
      const uint32 u32_DataSetSize = pc_List->c_DataSets.size();
      for (uint32 u32_ItOSCData = 0; u32_ItOSCData < this->mc_OSCContent.size(); ++u32_ItOSCData)
      {
         C_OSCNodeDataPoolListElement & rc_OSCData = this->mc_OSCContent[u32_ItOSCData];
         rc_OSCData.c_DataSetValues.resize(u32_DataSetSize);
         //Assign type to new data sets
         rc_OSCData.SetType(rc_OSCData.GetType());
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sort indices descending

   \created     10.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::m_SortDescending(void)
{
   C_SdUtil::h_SortIndicesDescendingAndSync<C_OSCNodeDataPoolListElement, C_PuiSdNodeDataPoolListElement>(
      this->mc_Indices, this->mc_OSCContent, this->mc_UIContent);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sort indices ascending

   \created     13.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::m_SortAscending(void)
{
   C_SdUtil::h_SortIndicesAscendingAndSync<C_OSCNodeDataPoolListElement, C_PuiSdNodeDataPoolListElement>(
      this->mc_Indices, this->mc_OSCContent, this->mc_UIContent);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if indices continuos

   \return
   true  Indices all continuous
   false Indices do contain holes

   \created     18.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::m_CheckContinuous(void)
{
   bool q_Retval = true;

   //Sorting necessary
   m_SortAscending();
   if (this->mc_Indices.size() > 1)
   {
      uint32 u32_Start = this->mc_Indices[0];
      for (uint32 u32_ItIndex = 1; (u32_ItIndex < this->mc_Indices.size()) && (q_Retval == true); ++u32_ItIndex)
      {
         //Check if expected value
         if (this->mc_Indices[u32_ItIndex] != (u32_Start + u32_ItIndex))
         {
            q_Retval = false;
         }
      }
   }

   return q_Retval;
}