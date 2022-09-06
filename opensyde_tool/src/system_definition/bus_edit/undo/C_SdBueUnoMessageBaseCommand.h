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
#include "C_SdBueMessageSelectorTreeWidget.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueUnoMessageBaseCommand :
   public QUndoCommand
{
public:
   C_SdBueUnoMessageBaseCommand(
      const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MessageId,
      C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
      stw_opensyde_gui::C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget,
      const QString & orc_Text = "", QUndoCommand * const opc_Parent = NULL);

protected:
   std::vector<stw_types::uint64> mc_UniqueId;
   C_PuiSdNodeCanMessageSyncManager * const mpc_MessageSyncManager;
   stw_opensyde_gui::C_SdBueMessageSelectorTreeWidget * const mpc_MessageTreeWidget;

   bool m_CheckMessagesSortedAscending(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
