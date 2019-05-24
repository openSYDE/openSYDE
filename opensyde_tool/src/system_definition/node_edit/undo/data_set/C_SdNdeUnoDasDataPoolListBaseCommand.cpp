//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set base command (implementation)

   Data pool list data set base command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdNdeUnoDasDataPoolListBaseCommand.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
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
   \param[in]     orc_Text                         Optional command text for informational display
   \param[in,out] opc_Parent                       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDasDataPoolListBaseCommand::C_SdNdeUnoDasDataPoolListBaseCommand(const uint32 & oru32_NodeIndex,
                                                                           const uint32 & oru32_DataPoolIndex,
                                                                           const uint32 & oru32_DataPoolListIndex,
                                                                           C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager, const QString & orc_Text,
                                                                           QUndoCommand * const opc_Parent) :
   QUndoCommand(orc_Text, opc_Parent),
   mu32_NodeIndex(oru32_NodeIndex),
   mu32_DataPoolIndex(oru32_DataPoolIndex),
   mu32_DataPoolListIndex(oru32_DataPoolListIndex),
   mpc_DataPoolListModelViewManager(opc_DataPoolListModelViewManager)
{
}
