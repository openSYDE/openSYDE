//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list add undo command (implementation)

   Data pool list add undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeUnoDataPoolListAddCommand.hpp"

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

   \param[in]     oru32_NodeIndex             Node index
   \param[in]     oru32_DataPoolIndex         Node data pool index
   \param[in,out] opc_DataPoolListsTreeWidget Data pool lists tree widget to perform actions on
   \param[in]     orc_Indices                 Node data pool list indices
   \param[in,out] opc_Parent                  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDataPoolListAddCommand::C_SdNdeUnoDataPoolListAddCommand(const uint32_t & oru32_NodeIndex,
                                                                   const uint32_t & oru32_DataPoolIndex,
                                                                   stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                                                   const std::vector<uint32_t> & orc_Indices,
                                                                   QUndoCommand * const opc_Parent) :
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex,
                                              opc_DataPoolListsTreeWidget, orc_Indices,
                                              "Add List", opc_Parent)
{
   std::vector<C_OscNodeDataPoolList> c_OscContent;
   std::vector<C_PuiSdNodeDataPoolList> c_UiContent;
   const C_OscNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(oru32_NodeIndex,
                                                                                                 oru32_DataPoolIndex);
   c_OscContent.resize(orc_Indices.size());
   c_UiContent.resize(orc_Indices.size());
   //Init
   if (pc_DataPool != NULL)
   {
      for (uint32_t u32_ItList = 0; u32_ItList < c_OscContent.size(); ++u32_ItList)
      {
         C_OscNodeDataPoolList & rc_List = c_OscContent[u32_ItList];
         for (uint32_t u32_ItElement = 0; u32_ItElement < rc_List.c_Elements.size(); ++u32_ItElement)
         {
            C_PuiSdHandler::h_InitDataElement(pc_DataPool->e_Type, pc_DataPool->q_IsSafety,
                                              rc_List.c_Elements[u32_ItElement]);
         }
         if ((pc_DataPool->e_Type == C_OscNodeDataPool::eNVM) && (pc_DataPool->q_IsSafety == true))
         {
            rc_List.q_NvmCrcActive = true;
         }
      }
   }
   this->m_SetInitialData(c_OscContent, c_UiContent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddCommand::redo(void)
{
   this->m_Add();
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListAddCommand::undo(void)
{
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand::undo();
   this->m_Delete();
}
