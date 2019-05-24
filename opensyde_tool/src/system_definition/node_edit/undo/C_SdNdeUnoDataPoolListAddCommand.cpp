//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list add undo command (implementation)

   Data pool list add undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeUnoDataPoolListAddCommand.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
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
   \param[in,out] opc_Parent                  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDataPoolListAddCommand::C_SdNdeUnoDataPoolListAddCommand(const uint32 & oru32_NodeIndex,
                                                                   const uint32 & oru32_DataPoolIndex,
                                                                   stw_opensyde_gui::C_SdNdeDataPoolListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                                                   const std::vector<uint32> & orc_Indices,
                                                                   QUndoCommand * const opc_Parent) :
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex,
                                              opc_DataPoolListsTreeWidget, orc_Indices,
                                              "Add List", opc_Parent)
{
   std::vector<C_OSCNodeDataPoolList> c_OSCContent;
   std::vector<C_PuiSdNodeDataPoolList> c_UIContent;
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(oru32_NodeIndex,
                                                                                                 oru32_DataPoolIndex);
   c_OSCContent.resize(orc_Indices.size());
   c_UIContent.resize(orc_Indices.size());
   //Init
   if (pc_DataPool != NULL)
   {
      for (uint32 u32_ItList = 0; u32_ItList < c_OSCContent.size(); ++u32_ItList)
      {
         C_OSCNodeDataPoolList & rc_List = c_OSCContent[u32_ItList];
         for (uint32 u32_ItElement = 0; u32_ItElement < rc_List.c_Elements.size(); ++u32_ItElement)
         {
            C_PuiSdHandler::h_InitDataElement(pc_DataPool->e_Type, pc_DataPool->q_IsSafety,
                                              rc_List.c_Elements[u32_ItElement]);
         }
         if ((pc_DataPool->e_Type == C_OSCNodeDataPool::eNVM) && (pc_DataPool->q_IsSafety == true))
         {
            rc_List.q_NvMCRCActive = true;
         }
      }
   }
   this->SetInitialData(c_OSCContent, c_UIContent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddCommand::redo(void)
{
   this->Add();
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddCommand::undo(void)
{
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand::undo();
   this->Delete();
}
