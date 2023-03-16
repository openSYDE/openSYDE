//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Signal delete undo command (implementation)

   Signal delete undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SdBueUnoSignalDeleteCommand.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]      orc_MessageId             Message identification indices
   \param[in]      orc_SignalIndex           Signal index
   \param[in,out]  opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget     Message tree widget to perform actions on
   \param[in,out]  opc_Parent                Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoSignalDeleteCommand::C_SdBueUnoSignalDeleteCommand(
   const std::vector<C_OscCanMessageIdentificationIndices> & orc_MessageId,
   const std::vector<uint32_t> & orc_SignalIndex, C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
   C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, QUndoCommand * const opc_Parent) :
   C_SdBueUnoSignalAddDeleteBaseCommand(orc_MessageId, orc_SignalIndex, std::vector<uint16_t>(),
                                        std::vector<stw::opensyde_core::C_OscCanSignal::E_MultiplexerType>(),
                                        std::vector<uint16_t>(),
                                        opc_MessageSyncManager,
                                        opc_MessageTreeWidget,
                                        "Delete Signal",
                                        opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalDeleteCommand::redo(void)
{
   this->m_Delete();
   C_SdBueUnoSignalAddDeleteBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalDeleteCommand::undo(void)
{
   C_SdBueUnoSignalAddDeleteBaseCommand::undo();
   this->m_Add();
}
