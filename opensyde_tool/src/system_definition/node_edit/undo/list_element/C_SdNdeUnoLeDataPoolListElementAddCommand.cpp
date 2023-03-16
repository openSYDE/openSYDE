//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element add undo command (implementation)

   Data pool list element add undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdNdeUnoLeDataPoolListElementAddCommand.hpp"
#include "C_PuiSdHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
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
   \param[in,out] opc_Parent                       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoLeDataPoolListElementAddCommand::C_SdNdeUnoLeDataPoolListElementAddCommand(const uint32_t & oru32_NodeIndex,
                                                                                     const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_DataPoolListIndex, C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                                                                     const std::vector<uint32_t> & orc_Indices,
                                                                                     QUndoCommand * const opc_Parent) :
   C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                       opc_DataPoolListModelViewManager,
                                                       orc_Indices,
                                                       "Add List element", opc_Parent)
{
   std::vector<C_OscNodeDataPoolListElement> c_OscContent;
   std::vector<C_PuiSdNodeDataPoolListElement> c_UiContent;
   const C_OscNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(oru32_NodeIndex,
                                                                                                 oru32_DataPoolIndex);
   c_OscContent.resize(orc_Indices.size());
   c_UiContent.resize(orc_Indices.size());
   if (pc_DataPool != NULL)
   {
      for (uint32_t u32_ItData = 0; u32_ItData < orc_Indices.size(); ++u32_ItData)
      {
         C_PuiSdHandler::h_InitDataElement(pc_DataPool->e_Type, pc_DataPool->q_IsSafety, c_OscContent[u32_ItData]);
      }
   }
   this->m_SetInitialData(c_OscContent, c_UiContent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddCommand::redo(void)
{
   this->m_Add();
   C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementAddCommand::undo(void)
{
   C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand::undo();
   this->m_Delete();
}
