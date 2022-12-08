//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element paste undo command (implementation)

   Data pool list element paste undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SdNdeUnoLeDataPoolListElementPasteCommand.hpp"
#include "C_SdClipBoardHelper.hpp"
#include "C_PuiSdHandler.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_SdNdeDpListTableView.hpp"

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

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in,out] opc_Parent                       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoLeDataPoolListElementPasteCommand::C_SdNdeUnoLeDataPoolListElementPasteCommand(
   const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_DataPoolListIndex,
   C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager, QUndoCommand * const opc_Parent) :
   C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                       opc_DataPoolListModelViewManager,
                                                       std::vector<uint32_t>(),
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
bool C_SdNdeUnoLeDataPoolListElementPasteCommand::InitialSetup(const uint32_t & oru32_FirstIndex)
{
   std::vector<C_OscNodeDataPoolListElement> c_OscContent;
   std::vector<C_PuiSdNodeDataPoolListElement> c_UiContent;
   bool q_Retval =
      (C_SdClipBoardHelper::h_LoadToDataPoolListElementsFromClipBoard(c_OscContent, c_UiContent) == C_NO_ERR);

   if (q_Retval == true)
   {
      q_Retval = (c_OscContent.size() == c_UiContent.size());
      if (q_Retval == true)
      {
         const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);
         if (pc_List != NULL)
         {
            q_Retval =
               (static_cast<uint32_t>(pc_List->c_Elements.size() + c_OscContent.size()) <=
                C_OscNode::hu32_MAX_NUMBER_OF_ELEMENTS_PER_LIST);
            if (q_Retval == true)
            {
               std::vector<uint32_t> c_Indices;

               c_Indices.reserve(c_OscContent.size());
               q_Retval = (c_OscContent.size() == c_UiContent.size());
               for (uint32_t u32_NewIndices = 0; u32_NewIndices < c_OscContent.size(); ++u32_NewIndices)
               {
                  c_Indices.push_back(oru32_FirstIndex + u32_NewIndices);
               }
               this->m_SetIndices(c_Indices);
               this->m_SetInitialData(c_OscContent, c_UiContent);
            }
            else
            {
               QString c_Text;
               C_OgeWiCustomMessage c_MessageBox(this->mpc_DataPoolListModelViewManager->GetElementView(
                                                    this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                    this->mu32_DataPoolListIndex));
               c_MessageBox.SetType(C_OgeWiCustomMessage::E_Type::eERROR);
               const C_OscNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(
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
                  arg(C_OscNode::hu32_MAX_NUMBER_OF_ELEMENTS_PER_LIST).arg(c_Text));
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
