//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set add specific undo command (implementation)

   Data pool list data set add specific undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdNdeUnoDasDataPoolListAddSpecificCommand.h"

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
   \param[in]     orc_OSCNames                     Initial Data set names
   \param[in]     orc_OSCDataSetValues             Initial Data set values
   \param[in,out] opc_Parent                       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDasDataPoolListAddSpecificCommand::C_SdNdeUnoDasDataPoolListAddSpecificCommand(const uint32 & oru32_NodeIndex,
                                                                                         const uint32 & oru32_DataPoolIndex, const uint32 & oru32_DataPoolListIndex, C_SdNdeDpListModelViewManager * const opc_DataPoolListModelViewManager,
                                                                                         const std::vector<uint32> & orc_Indices, const std::vector<
                                                                                            stw_opensyde_core::
                                                                                            C_OSCNodeDataPoolDataSet> & orc_OSCNames,
                                                                                         const std::vector<std::vector<
                                                                                                              stw_opensyde_core
                                                                                                              ::
                                                                                                              C_OSCNodeDataPoolContent> > & orc_OSCDataSetValues,
                                                                                         QUndoCommand * const opc_Parent)
   :
   C_SdNdeUnoDasDataPoolListAddCommand(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex,
                                       opc_DataPoolListModelViewManager, orc_Indices, opc_Parent)
{
   this->SetInitialData(orc_OSCNames, orc_OSCDataSetValues);
}
