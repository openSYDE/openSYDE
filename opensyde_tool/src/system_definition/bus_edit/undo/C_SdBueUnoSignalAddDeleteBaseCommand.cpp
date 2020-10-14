//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "constants.h"
#include "C_SdBueUnoSignalAddDeleteBaseCommand.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]      orc_MessageId             Message identification indices
   \param[in]      oru32_SignalIndex         Signal index
   \param[in]      ou16_StartBit             Start bit of signal
   \param[in]      oe_MultiplexerType        Multiplexer signal type
   \param[in]      ou16_MultiplexerValue     Multiplexer value (only relevant if
                                             oe_MultiplexerType is eMUX_MULTIPLEXED_SIGNAL)
   \param[in,out]  opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget     Message tree widget to perform actions on
   \param[in]      orc_Text                  Optional command text for informational display
   \param[in,out]  opc_Parent                Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoSignalAddDeleteBaseCommand::C_SdBueUnoSignalAddDeleteBaseCommand(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId, const uint32 & oru32_SignalIndex,
   const uint16 ou16_StartBit, const C_OSCCanSignal::E_MultiplexerType oe_MultiplexerType,
   const uint16 ou16_MultiplexerValue, C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
   C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, const QString & orc_Text,
   QUndoCommand * const opc_Parent) :
   C_SdBueUnoMessageBaseCommand(orc_MessageId, opc_MessageSyncManager, opc_MessageTreeWidget, orc_Text, opc_Parent),
   mu32_SignalIndex(oru32_SignalIndex)
{
   //Adapt default values
   this->mc_OSCSignalCommon.c_Name = "NewSignal";
   this->mc_OSCSignalCommon.c_DataSetValues.resize(1);
   this->mc_OSCSignalCommon.c_DataSetValues[0] = this->mc_OSCSignalCommon.c_MinValue;
   this->mc_Signal.u16_ComBitStart = ou16_StartBit;
   this->mc_Signal.u16_ComBitLength = mu16_BUS_DEFAULT_SIGNAL_LENGTH;
   this->mc_Signal.e_MultiplexerType = oe_MultiplexerType;
   if (oe_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL)
   {
      this->mc_Signal.u16_MultiplexValue = ou16_MultiplexerValue;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generic add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalAddDeleteBaseCommand::Add(void)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      tgl_assert(this->mpc_MessageSyncManager->InsertCanSignal(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                  this->mu64_UniqueId), this->mu32_SignalIndex,
                                                               this->mc_Signal, this->mc_OSCSignalCommon,
                                                               this->mc_UISignalCommon, this->mc_UISignal) == C_NO_ERR);
      if (this->mpc_MessageTreeWidget != NULL)
      {
         this->mpc_MessageTreeWidget->InternalAddSignal(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                           this->mu64_UniqueId), this->mu32_SignalIndex);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generic delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalAddDeleteBaseCommand::Delete(void)
{
   m_Store();
   m_Remove();
   if ((this->mpc_MessageTreeWidget != NULL) && (this->mpc_MessageSyncManager != NULL))
   {
      this->mpc_MessageTreeWidget->InternalDeleteSignal(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                           this->mu64_UniqueId), this->mu32_SignalIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store current values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalAddDeleteBaseCommand::m_Store(void)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      const stw_opensyde_core::C_OSCCanSignal * const pc_Signal = C_PuiSdHandler::h_GetInstance()->GetCanSignal(
         this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
            this->mu64_UniqueId), this->mu32_SignalIndex);
      const stw_opensyde_core::C_OSCNodeDataPoolListElement * const pc_OSCSignalCommon =
         C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                          this->mu64_UniqueId), this->mu32_SignalIndex);
      const C_PuiSdNodeDataPoolListElement * const pc_UISignalCommon =
         C_PuiSdHandler::h_GetInstance()->GetUiCanDataPoolListElement(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                         this->mu64_UniqueId), this->mu32_SignalIndex);
      const C_PuiSdNodeCanSignal * const pc_UISignal = C_PuiSdHandler::h_GetInstance()->GetUiCanSignal(
         this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
            this->mu64_UniqueId),
         this->mu32_SignalIndex);

      tgl_assert(pc_Signal != NULL);
      if (pc_Signal != NULL)
      {
         this->mc_Signal = *pc_Signal;
      }
      tgl_assert(pc_OSCSignalCommon != NULL);
      if (pc_OSCSignalCommon != NULL)
      {
         this->mc_OSCSignalCommon = *pc_OSCSignalCommon;
      }
      tgl_assert(pc_UISignalCommon != NULL);
      if (pc_UISignalCommon != NULL)
      {
         this->mc_UISignalCommon = *pc_UISignalCommon;
      }
      tgl_assert(pc_UISignal != NULL);
      if (pc_UISignal != NULL)
      {
         this->mc_UISignal = *pc_UISignal;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove current message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalAddDeleteBaseCommand::m_Remove(void) const
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      tgl_assert(this->mpc_MessageSyncManager->DeleteCanSignal(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                  this->mu64_UniqueId),
                                                               this->mu32_SignalIndex) == C_NO_ERR);
   }
}
