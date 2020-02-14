//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list paste undo command (implementation)

   Data pool list paste undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_SdNdeUnoDataPoolListPasteCommand.h"
#include "C_SdClipBoardHelper.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "constants.h"
#include "C_OgeWiCustomMessage.h"

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

   \param[in]     oru32_NodeIndex             Node index
   \param[in]     oru32_DataPoolIndex         Node data pool index
   \param[in,out] opc_DataPoolListsTreeWidget Data pool lists tree widget to perform actions on
   \param[in,out] opc_Parent                  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDataPoolListPasteCommand::C_SdNdeUnoDataPoolListPasteCommand(const uint32 & oru32_NodeIndex,
                                                                       const uint32 & oru32_DataPoolIndex,
                                                                       stw_opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                                                       QUndoCommand * const opc_Parent) :
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex,
                                              opc_DataPoolListsTreeWidget, std::vector<stw_types::uint32>(),
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
bool C_SdNdeUnoDataPoolListPasteCommand::InitialSetup(const uint32 & oru32_FirstIndex)
{
   std::vector<C_OSCNodeDataPoolList> c_OSCContent;
   std::vector<C_PuiSdNodeDataPoolList> c_UIContent;
   C_OSCNodeDataPool::E_Type e_Type;
   bool q_Retval =
      (C_SdClipBoardHelper::h_LoadToDataPoolLists(c_OSCContent, c_UIContent, e_Type) == C_NO_ERR);

   if (q_Retval == true)
   {
      //Check type
      const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex);
      if (pc_DataPool != NULL)
      {
         if (pc_DataPool->e_Type == e_Type)
         {
            q_Retval = (c_OSCContent.size() == c_UIContent.size());
            if (q_Retval == true)
            {
               q_Retval =
                  (static_cast<uint32>(pc_DataPool->c_Lists.size() + c_OSCContent.size()) <=
                   mu32_NODE_DATA_POOL_LIST_MAX);
               if (q_Retval == true)
               {
                  std::vector<stw_types::uint32> c_Indices;
                  for (uint32 u32_NewIndices = 0; u32_NewIndices < c_OSCContent.size(); ++u32_NewIndices)
                  {
                     c_Indices.push_back(oru32_FirstIndex + u32_NewIndices);
                  }
                  this->SetIndices(c_Indices);
                  this->SetInitialData(c_OSCContent, c_UIContent);
               }
               else
               {
                  C_OgeWiCustomMessage c_MessageBox(this->mpc_DataPoolListsTreeWidget);
                  c_MessageBox.SetDescription(QString(C_GtGetText::h_GetText("Only %1 lists allowed per Datapool.")).
                                              arg(mu32_NODE_DATA_POOL_LIST_MAX));
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
   this->Add();
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListPasteCommand::undo(void)
{
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand::undo();
   this->Delete();
}
