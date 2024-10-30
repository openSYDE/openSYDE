//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Signal add undo command (implementation)

   Signal add undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_SdBueUnoSignalAddCommand.hpp"
#include "C_SdUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

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
   const std::vector<C_OscCanMessageIdentificationIndices> & orc_MessageId,
   const std::vector<uint32_t> & orc_SignalIndex, const std::vector<uint16_t> & orc_StartBit,
   const std::vector<C_OscCanSignal::E_MultiplexerType> & orc_MultiplexerType,
   const std::vector<uint16_t> & orc_MultiplexerValue, C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
   C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, const QString & orc_Text) :
   C_SdBueUnoSignalAddDeleteBaseCommand(orc_MessageId, orc_SignalIndex, orc_StartBit, orc_MultiplexerType,
                                        orc_MultiplexerValue, opc_MessageSyncManager,
                                        opc_MessageTreeWidget,
                                        orc_Text)
{
   //Adapt default signal
   for (uint32_t u32_ItStep = 0UL; u32_ItStep < this->mc_OscSignalCommon.size(); ++u32_ItStep)
   {
      if ((((this->mc_OscSignalCommon[u32_ItStep].GetArray() == false) &&
            (this->mc_OscSignalCommon[u32_ItStep].GetType() == C_OscNodeDataPoolContent::eUINT8)) &&
           (this->mc_UiSignalCommon[u32_ItStep].q_AutoMinMaxActive == true)) &&
          (this->mc_Signal[u32_ItStep].u16_ComBitLength == 1U))
      {
         this->mc_OscSignalCommon[u32_ItStep].c_MaxValue.SetValueU8(1);
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
   \param[in]  orc_OscSignalCommon  Signal data (osc common)
   \param[in]  orc_UiSignalCommon   Signal data (ui common)
   \param[in]  orc_UiSignal         Signal data (ui)
   \param[in]  orc_ProtocolType     Current active protocol to handle necessary adaptations
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalAddCommand::SetInitialData(const std::vector<C_OscCanSignal> & orc_Signal,
                                                const std::vector<C_OscNodeDataPoolListElement> & orc_OscSignalCommon,
                                                const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommon,
                                                const std::vector<C_PuiSdNodeCanSignal> & orc_UiSignal,
                                                const std::vector<C_OscCanProtocol::E_Type> & orc_ProtocolType)
{
   this->mc_Signal = orc_Signal;
   this->mc_OscSignalCommon = orc_OscSignalCommon;
   this->mc_UiSignalCommon = orc_UiSignalCommon;
   this->mc_UiSignal = orc_UiSignal;
   tgl_assert(this->mc_Signal.size() == this->mc_UniqueId.size());
   if (this->mc_Signal.size() == this->mc_UniqueId.size())
   {
      for (uint32_t u32_ItStep = 0UL; u32_ItStep < this->mc_UniqueId.size(); ++u32_ItStep)
      {
         this->mc_UiSignal[u32_ItStep].u8_ColorIndex = 0U;

         tgl_assert(this->mc_Signal.size() == this->mc_OscSignalCommon.size());
         if (this->mc_Signal.size() == this->mc_OscSignalCommon.size())
         {
            //Reinitialize index as current index is not related to data handling
            this->mc_Signal[u32_ItStep].u32_ComDataElementIndex = u32_ItStep;
            if ((this->mc_Signal[u32_ItStep].u32_ComDataElementIndex < this->mc_OscSignalCommon.size()) &&
                (this->mc_Signal[u32_ItStep].u32_ComDataElementIndex < this->mc_UiSignalCommon.size()))
            {
               // Adapt do safety protocol restrictions
               C_SdUtil::h_AdaptSignalToProtocolType(
                  this->mc_Signal[u32_ItStep],
                  this->mc_OscSignalCommon[this->mc_Signal[u32_ItStep].u32_ComDataElementIndex],
                  this->mc_UiSignalCommon[this->mc_Signal[u32_ItStep].u32_ComDataElementIndex],
                  orc_ProtocolType[u32_ItStep], NULL);
            }
         }
      }
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
