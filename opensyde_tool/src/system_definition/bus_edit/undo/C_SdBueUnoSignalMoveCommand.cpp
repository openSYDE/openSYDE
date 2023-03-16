//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Signal move undo command (implementation)

   Signal move undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdBueUnoSignalMoveCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]      orc_SourceMessageId       Source message identification indices
   \param[in]      orc_SourceSignalIndex     Source signal indices
   \param[in]      orc_TargetMessageId       Target message identification indices
   \param[in]      orc_TargetSignalIndex     Target signal indices
   \param[in,out]  opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget     Message tree widget to perform actions on
   \param[in,out]  opc_Parent                Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoSignalMoveCommand::C_SdBueUnoSignalMoveCommand(
   const std::vector<C_OscCanMessageIdentificationIndices> & orc_SourceMessageId,
   const std::vector<uint32_t> & orc_SourceSignalIndex,
   const std::vector<C_OscCanMessageIdentificationIndices> & orc_TargetMessageId,
   const std::vector<uint32_t> & orc_TargetSignalIndex, C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
   C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, QUndoCommand * const opc_Parent) :
   C_SdBueUnoSignalAddDeleteBaseCommand(orc_SourceMessageId, orc_SourceSignalIndex, std::vector<uint16_t>(),
                                        std::vector<C_OscCanSignal::E_MultiplexerType>(),
                                        std::vector<uint16_t>(), opc_MessageSyncManager, opc_MessageTreeWidget, "Move Signal(s)",
                                        opc_Parent),
   mc_SourceSignalIndex(orc_SourceSignalIndex),
   mc_TargetSignalIndex(orc_TargetSignalIndex)
{
   if (opc_MessageSyncManager != NULL)
   {
      this->mc_SourceUniqueId.reserve(orc_SourceMessageId.size());
      this->mc_TargetUniqueId.reserve(orc_TargetMessageId.size());
      for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < orc_SourceMessageId.size(); ++u32_ItMessage)
      {
         this->mc_SourceUniqueId.push_back(opc_MessageSyncManager->GetUniqueId(orc_SourceMessageId[u32_ItMessage]));
      }
      for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < orc_TargetMessageId.size(); ++u32_ItMessage)
      {
         this->mc_TargetUniqueId.push_back(opc_MessageSyncManager->GetUniqueId(orc_TargetMessageId[u32_ItMessage]));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo move
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalMoveCommand::redo(void)
{
   //Correct order assumed
   this->m_Move(mc_SourceUniqueId, mc_SourceSignalIndex, mc_TargetUniqueId, mc_TargetSignalIndex);
   C_SdBueUnoSignalAddDeleteBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo move
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalMoveCommand::undo(void)
{
   std::vector<uint64_t> c_SourceUniqueId = mc_SourceUniqueId;
   std::vector<uint32_t> c_SourceSignalIndex = mc_SourceSignalIndex;
   std::vector<uint64_t> c_TargetUniqueId = mc_TargetUniqueId;
   std::vector<uint32_t> c_TargetSignalIndex = mc_TargetSignalIndex;

   C_SdBueUnoSignalAddDeleteBaseCommand::undo();
   //Reverse order necessary
   std::reverse(c_TargetUniqueId.begin(), c_TargetUniqueId.end());
   std::reverse(c_SourceUniqueId.begin(), c_SourceUniqueId.end());
   std::reverse(c_TargetSignalIndex.begin(), c_TargetSignalIndex.end());
   std::reverse(c_SourceSignalIndex.begin(), c_SourceSignalIndex.end());
   this->m_Move(c_TargetUniqueId, c_TargetSignalIndex, c_SourceUniqueId, c_SourceSignalIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move signal data

   \param[in]  orc_SourceUniqueId      Source message identification indices (referenced by unique ID)
   \param[in]  orc_SourceSignalIndex   Source signal index
   \param[in]  orc_TargetUniqueId      Target message identification indices (referenced by unique ID)
   \param[in]  orc_TargetSignalIndex   Target signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalMoveCommand::m_Move(const std::vector<uint64_t> & orc_SourceUniqueId,
                                         const std::vector<uint32_t> & orc_SourceSignalIndex,
                                         const std::vector<uint64_t> & orc_TargetUniqueId,
                                         const std::vector<uint32_t> & orc_TargetSignalIndex)
{
   //Step 1: delete source (Internal save of data)
   this->mc_UniqueId = orc_SourceUniqueId;
   this->mc_SignalIndex = orc_SourceSignalIndex;
   this->m_Delete();
   //Reset color in all cases
   for (uint32_t u32_ItSig = 0UL; u32_ItSig < this->mc_UiSignal.size(); ++u32_ItSig)
   {
      this->mc_UiSignal[u32_ItSig].u8_ColorIndex = 0;
   }
   //Step 2: add target (Use internally stored data)
   this->mc_UniqueId = orc_TargetUniqueId;
   this->mc_SignalIndex = orc_TargetSignalIndex;
   this->m_Add();
}
