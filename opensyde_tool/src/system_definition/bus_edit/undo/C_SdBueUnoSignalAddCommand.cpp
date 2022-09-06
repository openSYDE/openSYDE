//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Signal add undo command (implementation)

   Signal add undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_SdBueUnoSignalAddCommand.h"
#include "C_SdUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

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
   \param[in]      orc_StartBit              Start bit of signal
   \param[in]      orc_MultiplexerType       Multiplexer signal type
   \param[in]      orc_MultiplexerValue      Multiplexer value (only relevant if
                                             oe_MultiplexerType is eMUX_MULTIPLEXED_SIGNAL)
   \param[in,out]  opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget     Message tree widget to perform actions on
   \param[in]      orc_Text                  Optional command text for informational display
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoSignalAddCommand::C_SdBueUnoSignalAddCommand(
   const std::vector<C_OSCCanMessageIdentificationIndices> & orc_MessageId, const std::vector<uint32> & orc_SignalIndex,
   const std::vector<uint16> & orc_StartBit, const std::vector<C_OSCCanSignal::E_MultiplexerType> & orc_MultiplexerType,
   const std::vector<uint16> & orc_MultiplexerValue, C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
   C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, const QString & orc_Text) :
   C_SdBueUnoSignalAddDeleteBaseCommand(orc_MessageId, orc_SignalIndex, orc_StartBit, orc_MultiplexerType,
                                        orc_MultiplexerValue, opc_MessageSyncManager,
                                        opc_MessageTreeWidget,
                                        orc_Text)
{
   //Adapt default signal
   for (uint32 u32_ItStep = 0UL; u32_ItStep < this->mc_OSCSignalCommon.size(); ++u32_ItStep)
   {
      if ((((this->mc_OSCSignalCommon[u32_ItStep].GetArray() == false) &&
            (this->mc_OSCSignalCommon[u32_ItStep].GetType() == C_OSCNodeDataPoolContent::eUINT8)) &&
           (this->mc_UISignalCommon[u32_ItStep].q_AutoMinMaxActive == true)) &&
          (this->mc_Signal[u32_ItStep].u16_ComBitLength == 1U))
      {
         this->mc_OSCSignalCommon[u32_ItStep].c_MaxValue.SetValueU8(1);
      }
      else
      {
         //Unexpected default values
         tgl_assert(false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set initial data

   \param[in]  orc_Signal           Signal data (osc)
   \param[in]  orc_OSCSignalCommon  Signal data (osc common)
   \param[in]  orc_UISignalCommon   Signal data (ui common)
   \param[in]  orc_UISignal         Signal data (ui)
   \param[in]  orc_ProtocolType     Current active protocol to handle necessary adaptations
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalAddCommand::SetInitialData(const std::vector<C_OSCCanSignal> & orc_Signal,
                                                const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalCommon,
                                                const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommon,
                                                const std::vector<C_PuiSdNodeCanSignal> & orc_UISignal,
                                                const std::vector<C_OSCCanProtocol::E_Type> & orc_ProtocolType)
{
   this->mc_Signal = orc_Signal;
   this->mc_OSCSignalCommon = orc_OSCSignalCommon;
   this->mc_UISignalCommon = orc_UISignalCommon;
   this->mc_UISignal = orc_UISignal;
   for (uint32 u32_ItStep = 0UL; u32_ItStep < this->mc_UniqueId.size(); ++u32_ItStep)
   {
      this->mc_UISignal[u32_ItStep].u8_ColorIndex = 0U;

      // Adapt do safety protocol restrictions
      C_SdUtil::h_AdaptSignalToProtocolType(this->mc_Signal[u32_ItStep], orc_ProtocolType[u32_ItStep], NULL);
   }
   tgl_assert(this->m_CheckSignalsSortedAscending());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Redo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalAddCommand::redo(void)
{
   this->m_Add();
   C_SdBueUnoSignalAddDeleteBaseCommand::redo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Undo add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalAddCommand::undo(void)
{
   C_SdBueUnoSignalAddDeleteBaseCommand::undo();
   this->m_Delete();
}
