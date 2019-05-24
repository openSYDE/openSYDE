//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message and signal undo base command (implementation)

   Message and signal undo base command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdBueUnoMessageBaseCommand.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]     orc_MessageId          Message identification indices
   \param[in,out] opc_MessageSyncManager Message sync manager to perform actions on
   \param[in,out] opc_MessageTreeWidget  Message tree widget to perform actions on
   \param[in]     orc_Text               Optional command text for informational display
   \param[in,out] opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoMessageBaseCommand::C_SdBueUnoMessageBaseCommand(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                           C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager, C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, const QString & orc_Text,
                                                           QUndoCommand * const opc_Parent) :
   QUndoCommand(orc_Text, opc_Parent),
   mu64_UniqueId(0ULL),
   mpc_MessageSyncManager(opc_MessageSyncManager),
   mpc_MessageTreeWidget(opc_MessageTreeWidget)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      this->mu64_UniqueId = this->mpc_MessageSyncManager->GetUniqueId(orc_MessageId);
   }
}
