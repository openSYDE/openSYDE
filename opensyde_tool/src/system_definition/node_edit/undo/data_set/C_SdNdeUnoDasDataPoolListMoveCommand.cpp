//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set move undo command (implementation)

   Data pool list data set move undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdNdeUnoDasDataPoolListMoveCommand.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeUnoUtil.hpp"
#include "C_SdUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]      oru32_NodeIndex                    Node index
   \param[in]      oru32_DataPoolIndex                Node data pool index
   \param[in]      oru32_DataPoolListIndex            Node data pool list index
   \param[in,out]  opc_DataPoolListModelViewManager   Data pool list model view manager to perform actions on
   \param[in]      orc_SourceCol                      Source columns
   \param[in]      orc_TargetCol                      Target columns
   \param[in]      orq_AdaptIndices                   Flag, if target columns should be adapted to source column deletion
   \param[in,out]  opc_Parent                         Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDasDataPoolListMoveCommand::C_SdNdeUnoDasDataPoolListMoveCommand(const uint32_t & oru32_NodeIndex,
                                                                           const uint32_t & oru32_DataPoolIndex,
                                                                           const uint32_t & oru32_DataPoolListIndex,
                                                                           C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                                                           const std::vector<uint32_t> & orc_SourceCol,
                                                                           const std::vector<uint32_t> & orc_TargetCol, const bool & orq_AdaptIndices,
                                                                           QUndoCommand * const opc_Parent) :
   C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                 opc_DataPoolListModelViewManager, orc_SourceCol,
                                                 "Move List Dataset", opc_Parent),
   mc_SourceCol(orc_SourceCol),
   mc_TargetCol(orc_TargetCol)
{
   C_SdUtil::h_SortSourceDescending(this->mc_SourceCol, this->mc_TargetCol);
   if (orq_AdaptIndices == true)
   {
      C_SdNdeUnoUtil::h_AdaptTargetToDeletedSource(this->mc_SourceCol, this->mc_TargetCol);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListMoveCommand::redo(void)
{
   //DELETE
   this->m_SetIndices(mc_SourceCol);
   this->m_Delete();
   //ADD
   this->m_SetIndices(mc_TargetCol);
   this->m_Add();
   C_SdNdeUnoDasDataPoolListBaseCommand::redo();
   this->mq_Initial = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDasDataPoolListMoveCommand::undo(void)
{
   C_SdNdeUnoDasDataPoolListBaseCommand::undo();
   //DELETE
   this->m_SetIndices(mc_TargetCol);
   this->m_Delete();
   //ADD
   this->m_SetIndices(mc_SourceCol);
   this->m_Add();
}
