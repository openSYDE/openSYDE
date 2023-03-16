//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list paste undo command (implementation)

   Data pool list paste undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SdNdeUnoDataPoolListPasteCommand.hpp"
#include "C_SdClipBoardHelper.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "constants.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::errors;
using namespace stw::opensyde_gui_elements;

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
   \param[in,out] opc_Parent                  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDataPoolListPasteCommand::C_SdNdeUnoDataPoolListPasteCommand(const uint32_t & oru32_NodeIndex,
                                                                       const uint32_t & oru32_DataPoolIndex,
                                                                       stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                                                       QUndoCommand * const opc_Parent) :
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex,
                                              opc_DataPoolListsTreeWidget, std::vector<uint32_t>(),
                                              "Paste List", opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initial setup of paste command (Check clipboard for valid data)

   \param[in] oru32_FirstIndex First new index

   \return
   true:  Worked
   false: Discarded
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeUnoDataPoolListPasteCommand::InitialSetup(const uint32_t & oru32_FirstIndex)
{
   std::vector<C_OscNodeDataPoolList> c_OscContent;
   std::vector<C_PuiSdNodeDataPoolList> c_UiContent;
   C_OscNodeDataPool::E_Type e_Type;
   bool q_Retval =
      (C_SdClipBoardHelper::h_LoadToDataPoolLists(c_OscContent, c_UiContent, e_Type) == C_NO_ERR);

   if (q_Retval == true)
   {
      //Check type
      const C_OscNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex);
      if (pc_DataPool != NULL)
      {
         if (pc_DataPool->e_Type == e_Type)
         {
            q_Retval = (c_OscContent.size() == c_UiContent.size());
            if (q_Retval == true)
            {
               q_Retval =
                  (static_cast<uint32_t>(pc_DataPool->c_Lists.size() + c_OscContent.size()) <=
                   C_OscNode::hu32_MAX_NUMBER_OF_LISTS_PER_DATA_POOL);
               if (q_Retval == true)
               {
                  std::vector<uint32_t> c_Indices;
                  for (uint32_t u32_NewIndices = 0; u32_NewIndices < c_OscContent.size(); ++u32_NewIndices)
                  {
                     c_Indices.push_back(oru32_FirstIndex + u32_NewIndices);
                  }
                  this->m_SetIndices(c_Indices);
                  this->m_SetInitialData(c_OscContent, c_UiContent);
               }
               else
               {
                  C_OgeWiCustomMessage c_MessageBox(this->mpc_DataPoolListsTreeWidget);
                  c_MessageBox.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                                      "Only %1 lists allowed per Datapool.")).
                                              arg(C_OscNode::hu32_MAX_NUMBER_OF_LISTS_PER_DATA_POOL));
                  c_MessageBox.SetCustomMinHeight(180, 180);
                  c_MessageBox.Execute();
               }
            }
         }
         else
         {
            C_OgeWiCustomMessage c_MessageBox(this->mpc_DataPoolListsTreeWidget);
            c_MessageBox.SetDescription(C_GtGetText::h_GetText("Cannot paste list. List type is not compatible."));
            c_MessageBox.SetCustomMinHeight(180, 180);
            c_MessageBox.Execute();
            q_Retval = false;
         }
      }
      else
      {
         q_Retval = false;
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListPasteCommand::redo(void)
{
   this->m_Add();
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListPasteCommand::undo(void)
{
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand::undo();
   this->m_Delete();
}
