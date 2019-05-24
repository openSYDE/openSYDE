//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element move undo command (implementation)

   Data pool list element move undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QElapsedTimer>

#include "C_Uti.h"
#include "stwtypes.h"
#include "constants.h"
#include "C_SdNdeUnoLeDataPoolListElementMoveCommand.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeUnoUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

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
   \param[in]     orc_SourceRow                    Source rows
   \param[in]     orc_TargetRow                    Target rows
   \param[in]     orq_AdaptIndices                 Flag, if target rows should be adapted to source row deletion
   \param[in,out] opc_Parent                       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoLeDataPoolListElementMoveCommand::C_SdNdeUnoLeDataPoolListElementMoveCommand(const uint32 & oru32_NodeIndex,
                                                                                       const uint32 & oru32_DataPoolIndex, const uint32 & oru32_DataPoolListIndex, C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager,
                                                                                       const std::vector<uint32> & orc_SourceRow,
                                                                                       const std::vector<uint32> & orc_TargetRow, const bool & orq_AdaptIndices,
                                                                                       QUndoCommand * const opc_Parent)
   :
   C_SdNdeUnoLeDataPoolListElementAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                                       opc_DataPoolListModelViewManager,  orc_SourceRow,
                                                       "Move List element", opc_Parent),
   mc_SourceRow(orc_SourceRow),
   mc_TargetRow(orc_TargetRow)
{
   if (orq_AdaptIndices == true)
   {
      C_SdNdeUnoUtil::h_AdaptTargetToDeletedSource(this->mc_SourceRow, this->mc_TargetRow);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementMoveCommand::redo(void)
{
   if (this->mpc_DataPoolListModelViewManager != NULL)
   {
      C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_DataPoolListModelViewManager->GetElementModel(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);
      if (pc_Model != NULL)
      {
         std::vector<std::vector<uint32> > c_Items;
         QElapsedTimer c_Timer;

         if (mq_TIMING_OUTPUT)
         {
            c_Timer.start();
         }
         pc_Model->DoMoveRows(mc_SourceRow, mc_TargetRow);
         //Sort ascending, only done for the "source" row which is not necessary here
         c_Items = C_Uti::h_GetContiguousSectionsAscending(C_Uti::h_UniquifyAndSortAscending(mc_TargetRow));
         m_ReSelect(c_Items, false);
         if (mq_TIMING_OUTPUT)
         {
            std::cout << "Move elements " << c_Timer.restart() << " ms" << &std::endl;
         }
      }
   }
   C_SdNdeUnoLeDataPoolListElementBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoLeDataPoolListElementMoveCommand::undo(void)
{
   C_SdNdeUnoLeDataPoolListElementBaseCommand::undo();
   if (this->mpc_DataPoolListModelViewManager != NULL)
   {
      C_SdNdeDataPoolListTableModel * const pc_Model = this->mpc_DataPoolListModelViewManager->GetElementModel(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_DataPoolListIndex);
      if (pc_Model != NULL)
      {
         std::vector<std::vector<uint32> > c_Items;
         QElapsedTimer c_Timer;
         if (mq_TIMING_OUTPUT)
         {
            c_Timer.start();
         }
         pc_Model->DoMoveRows(mc_TargetRow, mc_SourceRow);
         //Sort ascending, only done for the "source" row which is not necessary here
         c_Items = C_Uti::h_GetContiguousSectionsAscending(C_Uti::h_UniquifyAndSortAscending(mc_SourceRow));
         m_ReSelect(c_Items, false);
         if (mq_TIMING_OUTPUT)
         {
            std::cout << "Move elements " << c_Timer.restart() << " ms" << &std::endl;
         }
      }
   }
}
