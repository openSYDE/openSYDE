//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Signal move undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEUNOSIGNALMOVECOMMAND_H
#define C_SDBUEUNOSIGNALMOVECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdBueUnoSignalAddDeleteBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoSignalMoveCommand :
   public C_SdBueUnoSignalAddDeleteBaseCommand
{
public:
   C_SdBueUnoSignalMoveCommand(
      const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_SourceMessageId,
      const std::vector<uint32_t> & orc_SourceSignalIndex,
      const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_TargetMessageId,
      const std::vector<uint32_t> & orc_TargetSignalIndex,
      C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
      stw::opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
      QUndoCommand * const opc_Parent = NULL);

   void redo(void) override;
   void undo(void) override;

private:
   void m_Move(const std::vector<uint64_t> & orc_SourceUniqueId, const std::vector<uint32_t> & orc_SourceSignalIndex,
               const std::vector<uint64_t> & orc_TargetUniqueId, const std::vector<uint32_t> & orc_TargetSignalIndex);

   std::vector<uint64_t> mc_SourceUniqueId;
   const std::vector<uint32_t> mc_SourceSignalIndex;
   std::vector<uint64_t> mc_TargetUniqueId;
   const std::vector<uint32_t> mc_TargetSignalIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
