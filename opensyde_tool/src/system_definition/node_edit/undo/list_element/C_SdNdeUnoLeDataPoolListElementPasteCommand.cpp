//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element paste undo command (implementation)

   Data pool list element paste undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_SdNdeUnoLeDataPoolListElementPasteCommand.h"
#include "C_SdClipBoardHelper.h"
#include "C_PuiSdHandler.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_OgeWiCustomMessage.h"
#include "C_SdNdeDpListTableView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_errors;
using namespace stw_opensyde_gui_elements;

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
   \param[in,out] opc_Parent                       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoLeDataPoolListElementPasteCommand::C_SdNdeUnoLeDataPoolListElementPasteCommand(const uint32 & oru32_NodeIndex,
                                                                                         const uint32 & oru32_DataPoolIndex, const uint32 & oru32_DataPoolListIndex, C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                                                                         QUndoCommand * const opc_Parent)
   :
   C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                       opc_DataPoolListModelViewManager,
                                                       std::vector<stw_types::uint32>(),
                                                       "Paste List element", opc_Parent)
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
bool C_SdNdeUnoLeDataPoolListElementPasteCommand::InitialSetup(const stw_types::uint32 & oru32_FirstIndex)
{
   std::vector<C_OSCNodeDataPoolListElement> c_OSCContent;
   std::vector<C_PuiSdNodeDataPoolListElement> c_UIContent;
   bool q_Retval =
      (C_SdClipBoardHelper::h_LoadToDataPoolListElementsFromClipBoard(c_OSCContent, c_UIContent) == C_NO_ERR);

   if (q_Retval == true)
   {
      q_Retval = (c_OSCContent.size() == c_UIContent.size());
      if (q_Retval == true)
      {
         const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);
         if (pc_List != NULL)
         {
            q_Retval =
               (static_cast<uint32>(pc_List->c_Elements.size() + c_OSCContent.size()) <=
                C_OSCNode::hu32_MAX_NUMBER_OF_ELEMENTS_PER_LIST);
            if (q_Retval == true)
            {
               std::vector<stw_types::uint32> c_Indices;

               c_Indices.reserve(c_OSCContent.size());
               q_Retval = (c_OSCContent.size() == c_UIContent.size());
               for (uint32 u32_NewIndices = 0; u32_NewIndices < c_OSCContent.size(); ++u32_NewIndices)
               {
                  c_Indices.push_back(oru32_FirstIndex + u32_NewIndices);
               }
               this->m_SetIndices(c_Indices);
               this->m_SetInitialData(c_OSCContent, c_UIContent);
            }
            else
            {
               QString c_Text;
               C_OgeWiCustomMessage c_MessageBox(this->mpc_DataPoolListModelViewManager->GetElementView(
                                                    this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                    this->mu32_DataPoolListIndex));
               c_MessageBox.SetType(C_OgeWiCustomMessage::E_Type::eERROR);
               const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
                  this->mu32_NodeIndex,
                  this->mu32_DataPoolIndex);
               if (pc_DataPool != NULL)
               {
                  c_Text = C_PuiSdHandlerNodeLogic::h_GetElementTypeName(pc_DataPool->e_Type);
               }
               else
               {
                  c_Text = C_GtGetText::h_GetText("Data element");
               }
               c_MessageBox.SetDescription(
                  static_cast<QString>(C_GtGetText::h_GetText("Only %1 %2s allowed per list.")).
                  arg(C_OSCNode::hu32_MAX_NUMBER_OF_ELEMENTS_PER_LIST).arg(c_Text));
               c_MessageBox.SetCustomMinHeight(180, 180);
               c_MessageBox.Execute();
            }
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementPasteCommand::redo(void)
{
   this->m_Add();
   C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementPasteCommand::undo(void)
{
   C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::undo();
   this->m_Delete();
}
