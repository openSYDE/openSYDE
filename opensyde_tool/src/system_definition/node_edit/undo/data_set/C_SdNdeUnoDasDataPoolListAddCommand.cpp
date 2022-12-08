//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set add undo command (implementation)

   Data pool list data set add undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdNdeUnoDasDataPoolListAddCommand.hpp"
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
C_SdNdeUnoDasDataPoolListAddCommand::C_SdNdeUnoDasDataPoolListAddCommand(const uint32_t & oru32_NodeIndex,
                                                                         const uint32_t & oru32_DataPoolIndex,
                                                                         const uint32_t & oru32_DataPoolListIndex,
                                                                         C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                                                         const std::vector<uint32_t> & orc_Indices,
                                                                         QUndoCommand * const opc_Parent) :
   C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                 opc_DataPoolListModelViewManager,
                                                 orc_Indices,
                                                 "Add List Dataset", opc_Parent)
{
   const C_OscNodeDataPoolList * const pc_List =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(oru32_NodeIndex, oru32_DataPoolIndex,
                                                          oru32_DataPoolListIndex);

   std::vector<C_OscNodeDataPoolDataSet> c_OscNames;
   std::vector<std::vector<C_OscNodeDataPoolContent> > c_OscDataSetValues;
   c_OscNames.resize(orc_Indices.size());
   c_OscDataSetValues.resize(orc_Indices.size());
   //Init data set values
   if (pc_List != NULL)
   {
      for (uint32_t u32_ItDataSet = 0; u32_ItDataSet < c_OscDataSetValues.size(); ++u32_ItDataSet)
      {
         c_OscDataSetValues[u32_ItDataSet].resize(pc_List->c_Elements.size());
         for (uint32_t u32_ItElement = 0; u32_ItElement < pc_List->c_Elements.size(); ++u32_ItElement)
         {
            const C_OscNodeDataPoolListElement & rc_DataElement = pc_List->c_Elements[u32_ItElement];
            c_OscDataSetValues[u32_ItDataSet][u32_ItElement] = rc_DataElement.c_MinValue;
         }
      }
   }
   this->m_SetInitialData(c_OscNames, c_OscDataSetValues);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListAddCommand::redo(void)
{
   this->m_Add();
   C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListAddCommand::undo(void)
{
   C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand::undo();
   this->m_Delete();
   this->mq_Initial = false;
}
