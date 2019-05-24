//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element add undo command (implementation)

   Data pool list element add undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdNdeUnoLeDataPoolListElementAddCommand.h"
#include "C_PuiSdHandler.h"

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

   \param[in]     oru32_NodeIndex                  Node index
   \param[in]     oru32_DataPoolIndex              Node data pool index
   \param[in]     oru32_DataPoolListIndex          Node data pool list index
   \param[in,out] opc_DataPoolListModelViewManager Data pool lists model view manager to get objects to perform actions on
   \param[in]     orc_Indices                      Node data pool list indices
   \param[in,out] opc_Parent                       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoLeDataPoolListElementAddCommand::C_SdNdeUnoLeDataPoolListElementAddCommand(const uint32 & oru32_NodeIndex,
                                                                                     const uint32 & oru32_DataPoolIndex,
                                                                                     const uint32 & oru32_DataPoolListIndex, C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager,
                                                                                     const std::vector<uint32> & orc_Indices,
                                                                                     QUndoCommand * const opc_Parent) :
   C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                       opc_DataPoolListModelViewManager,
                                                       orc_Indices,
                                                       "Add List element", opc_Parent)
{
   std::vector<C_OSCNodeDataPoolListElement> c_OSCContent;
   std::vector<C_PuiSdNodeDataPoolListElement> c_UIContent;
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(oru32_NodeIndex,
                                                                                                 oru32_DataPoolIndex);
   c_OSCContent.resize(orc_Indices.size());
   c_UIContent.resize(orc_Indices.size());
   if (pc_DataPool != NULL)
   {
      for (uint32 u32_ItData = 0; u32_ItData < orc_Indices.size(); ++u32_ItData)
      {
         C_PuiSdHandler::h_InitDataElement(pc_DataPool->e_Type, pc_DataPool->q_IsSafety, c_OSCContent[u32_ItData]);
      }
   }
   this->SetInitialData(c_OSCContent, c_UIContent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddCommand::redo(void)
{
   this->Add();
   C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddCommand::undo(void)
{
   C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::undo();
   this->Delete();
}
