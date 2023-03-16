//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message and signal undo base command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEUNOMESSAGEBASECOMMAND_H
#define C_SDBUEUNOMESSAGEBASECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <QString>
#include <QUndoCommand>
#include "C_SdBueMessageSelectorTreeWidget.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoMessageBaseCommand :
   public QUndoCommand
{
public:
   C_SdBueUnoMessageBaseCommand(
      const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MessageId,
      C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
      stw::opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
      const QString & orc_Text = "", QUndoCommand * const opc_Parent = NULL);

protected:
   std::vector<uint64_t> mc_UniqueId;
   C_PuiSdNodeCanMessageSyncManager * const mpc_MessageSyncManager;
   stw::opensyde_gui::C_SdBueMessageSelectorTreeWidget * const mpc_MessageTreeWidget;

   bool m_CheckMessagesSortedAscending(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
