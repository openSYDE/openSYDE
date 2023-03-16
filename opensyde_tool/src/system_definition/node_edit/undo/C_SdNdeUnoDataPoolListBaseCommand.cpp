//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list base command (implementation)

   Data pool list base command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdNdeUnoDataPoolListBaseCommand.hpp"

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

   \param[in]     oru32_NodeIndex             Node index
   \param[in]     oru32_DataPoolIndex         Node data pool index
   \param[in,out] opc_DataPoolListsTreeWidget Data pool lists tree widget to perform actions on
   \param[in]     orc_Text                    Optional command text for informational display
   \param[in,out] opc_Parent                  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDataPoolListBaseCommand::C_SdNdeUnoDataPoolListBaseCommand(const uint32_t & oru32_NodeIndex,
                                                                     const uint32_t & oru32_DataPoolIndex,
                                                                     stw::opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget, const QString & orc_Text,
                                                                     QUndoCommand * const opc_Parent) :
   QUndoCommand(orc_Text, opc_Parent),
   mu32_NodeIndex(oru32_NodeIndex),
   mu32_DataPoolIndex(oru32_DataPoolIndex),
   mpc_DataPoolListsTreeWidget(opc_DataPoolListsTreeWidget)
{
}
