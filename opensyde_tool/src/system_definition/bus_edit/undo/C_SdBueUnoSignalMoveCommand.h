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
#include "C_SdBueUnoSignalAddDeleteBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoSignalMoveCommand :
   public C_SdBueUnoSignalAddDeleteBaseCommand
{
public:
   C_SdBueUnoSignalMoveCommand(
      const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_SourceMessageId,
      const std::vector<stw_types::uint32> & orc_SourceSignalIndex,
      const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_TargetMessageId,
      const std::vector<stw_types::uint32> & orc_TargetSignalIndex,
      C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
      stw_opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
      QUndoCommand * const opc_Parent = NULL);

   virtual void redo(void);
   virtual void undo(void);

private:
   void m_Move(const std::vector<stw_types::uint64> & orc_SourceUniqueId,
               const std::vector<stw_types::uint32> & orc_SourceSignalIndex,
               const std::vector<stw_types::uint64> & orc_TargetUniqueId,
               const std::vector<stw_types::uint32> & orc_TargetSignalIndex);

   std::vector<stw_types::uint64> mc_SourceUniqueId;
   const std::vector<stw_types::uint32> mc_SourceSignalIndex;
   std::vector<stw_types::uint64> mc_TargetUniqueId;
   const std::vector<stw_types::uint32> mc_TargetSignalIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
