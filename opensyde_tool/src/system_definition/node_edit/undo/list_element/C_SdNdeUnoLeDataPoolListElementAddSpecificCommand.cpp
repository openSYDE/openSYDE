//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element add specific undo command (implementation)

   Data pool list element add specific undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdNdeUnoLeDataPoolListElementAddSpecificCommand.h"

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
   \param[in]     orc_Indices                      Node data pool list indices
   \param[in]     orc_OSCData                      OSC node data pool list data
   \param[in]     orc_UIData                       UI node data pool list data
   \param[in,out] opc_Parent                       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoLeDataPoolListElementAddSpecificCommand::C_SdNdeUnoLeDataPoolListElementAddSpecificCommand(
   const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex, const uint32 & oru32_DataPoolListIndex,
   C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager,
   const std::vector<uint32> & orc_Indices,
   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCData,
   const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UIData, QUndoCommand * const opc_Parent) :
   C_SdNdeUnoLeDataPoolListElementAddCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                             opc_DataPoolListModelViewManager, orc_Indices, opc_Parent)
{
   this->SetInitialData(orc_OSCData, orc_UIData);
}
