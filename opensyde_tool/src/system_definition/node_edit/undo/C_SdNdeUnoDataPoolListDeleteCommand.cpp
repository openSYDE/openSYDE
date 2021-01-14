//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list delete undo command (implementation)

   Data pool list delete undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdNdeUnoDataPoolListDeleteCommand.h"

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

   \param[in]     oru32_NodeIndex             Node index
   \param[in]     oru32_DataPoolIndex         Node data pool index
   \param[in,out] opc_DataPoolListsTreeWidget Data pool lists tree widget to perform actions on
   \param[in]     orc_Indices                 Node data pool list indices
   \param[in,out] opc_Parent                  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeUnoDataPoolListDeleteCommand::C_SdNdeUnoDataPoolListDeleteCommand(const uint32 & oru32_NodeIndex,
                                                                         const uint32 & oru32_DataPoolIndex,
                                                                         stw_opensyde_gui::C_SdNdeDpListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                                                         const std::vector<uint32> & orc_Indices,
                                                                         QUndoCommand * const opc_Parent) :
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand(oru32_NodeIndex, oru32_DataPoolIndex,
                                              opc_DataPoolListsTreeWidget, orc_Indices,
                                              "Delete List", opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListDeleteCommand::redo(void)
{
   this->m_Delete();
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeUnoDataPoolListDeleteCommand::undo(void)
{
   C_SdNdeUnoDataPoolListAddDeleteBaseCommand::undo();
   this->m_Add();
}
