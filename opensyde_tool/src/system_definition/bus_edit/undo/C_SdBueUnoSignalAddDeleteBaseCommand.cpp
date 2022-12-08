//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "constants.hpp"
#include "C_SdBueUnoSignalAddDeleteBaseCommand.hpp"
#include "C_PuiSdHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::tgl;
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
   \param[in]      orc_StartBit              Start bit of signal
   \param[in]      orc_MultiplexerType       Multiplexer signal type
   \param[in]      orc_MultiplexerValue      Multiplexer value (only relevant if
                                             oe_MultiplexerType is eMUX_MULTIPLEXED_SIGNAL)
   \param[in,out]  opc_MessageSyncManager    Message sync manager to perform actions on
   \param[in,out]  opc_MessageTreeWidget     Message tree widget to perform actions on
   \param[in]      orc_Text                  Optional command text for informational display
   \param[in,out]  opc_Parent                Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueUnoSignalAddDeleteBaseCommand::C_SdBueUnoSignalAddDeleteBaseCommand(
   const std::vector<C_OscCanMessageIdentificationIndices> & orc_MessageId,
   const std::vector<uint32_t> & orc_SignalIndex, const std::vector<uint16_t> & orc_StartBit,
   const std::vector<C_OscCanSignal::E_MultiplexerType> & orc_MultiplexerType,
   const std::vector<uint16_t> & orc_MultiplexerValue, C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
   C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, const QString & orc_Text,
   QUndoCommand * const opc_Parent) :
   C_SdBueUnoMessageBaseCommand(orc_MessageId, opc_MessageSyncManager, opc_MessageTreeWidget, orc_Text, opc_Parent),
   mc_SignalIndex(orc_SignalIndex)
{
   this->mc_SignalIndex.resize(this->mc_UniqueId.size(), 0UL);
   this->mc_Signal.resize(this->mc_UniqueId.size(), C_OscCanSignal());
   this->mc_OscSignalCommon.resize(this->mc_UniqueId.size(), C_OscNodeDataPoolListElement());
   this->mc_UiSignalCommon.resize(this->mc_UniqueId.size(), C_PuiSdNodeDataPoolListElement());
   this->mc_UiSignal.resize(this->mc_UniqueId.size(), C_PuiSdNodeCanSignal());
   //Adapt default values
   for (uint32_t u32_ItStep = 0UL; u32_ItStep < this->mc_UniqueId.size(); ++u32_ItStep)
   {
      this->mc_OscSignalCommon[u32_ItStep].c_Name = "NewSignal";
      this->mc_OscSignalCommon[u32_ItStep].c_DataSetValues.resize(1);
      this->mc_OscSignalCommon[u32_ItStep].c_DataSetValues[0] = this->mc_OscSignalCommon[u32_ItStep].c_MinValue;
      if (u32_ItStep < orc_StartBit.size())
      {
         this->mc_Signal[u32_ItStep].u16_ComBitStart = orc_StartBit[u32_ItStep];
      }
      else
      {
         this->mc_Signal[u32_ItStep].u16_ComBitStart = 0U;
      }
      this->mc_Signal[u32_ItStep].u16_ComBitLength = mu16_BUS_DEFAULT_SIGNAL_LENGTH;
      if (u32_ItStep < orc_MultiplexerType.size())
      {
         this->mc_Signal[u32_ItStep].e_MultiplexerType = orc_MultiplexerType[u32_ItStep];
         if (u32_ItStep < orc_MultiplexerValue.size())
         {
            if (orc_MultiplexerType[u32_ItStep] == C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL)
            {
               this->mc_Signal[u32_ItStep].u16_MultiplexValue = orc_MultiplexerValue[u32_ItStep];
            }
            else
            {
               this->mc_Signal[u32_ItStep].u16_MultiplexValue = 0U;
            }
         }
         else
         {
            this->mc_Signal[u32_ItStep].u16_MultiplexValue = 0U;
         }
      }
      else
      {
         this->mc_Signal[u32_ItStep].u16_MultiplexValue = 0U;
         this->mc_Signal[u32_ItStep].e_MultiplexerType = C_OscCanSignal::eMUX_DEFAULT;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generic add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalAddDeleteBaseCommand::m_Add(void)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      for (uint32_t u32_ItStep = 0UL; u32_ItStep < this->mc_UniqueId.size(); ++u32_ItStep)
      {
         tgl_assert(this->mpc_MessageSyncManager->InsertCanSignal(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                     this->mc_UniqueId[u32_ItStep]),
                                                                  this->mc_SignalIndex[u32_ItStep],
                                                                  this->mc_Signal[u32_ItStep],
                                                                  this->mc_OscSignalCommon[u32_ItStep],
                                                                  this->mc_UiSignalCommon[u32_ItStep],
                                                                  this->mc_UiSignal[u32_ItStep]) == C_NO_ERR);
         if (this->mpc_MessageTreeWidget != NULL)
         {
            this->mpc_MessageTreeWidget->InternalAddSignal(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                              this->mc_UniqueId[u32_ItStep]),
                                                           this->mc_SignalIndex[u32_ItStep]);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generic delete
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalAddDeleteBaseCommand::m_Delete(void)
{
   m_Store();
   m_Remove();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check signals sorted

   \return
   Flags

   \retval   True    Sorted
   \retval   False   Not sorted
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueUnoSignalAddDeleteBaseCommand::m_CheckSignalsSortedAscending() const
{
   bool q_Sorted = this->m_CheckMessagesSortedAscending();

   if (q_Sorted)
   {
      tgl_assert(this->mc_UniqueId.size() == this->mc_SignalIndex.size());
      if (this->mc_UniqueId.size() == this->mc_SignalIndex.size())
      {
         tgl_assert(this->mpc_MessageSyncManager != NULL);
         if (this->mpc_MessageSyncManager != NULL)
         {
            C_OscCanMessageIdentificationIndices c_PrevId;
            uint32_t u32_PrevSignalIndex = 0UL;
            bool q_PrevIdValid = false;
            for (uint32_t u32_It = 0UL; u32_It < this->mc_UniqueId.size(); ++u32_It)
            {
               const C_OscCanMessageIdentificationIndices c_Id = this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                  this->mc_UniqueId[u32_It]);
               if (q_PrevIdValid)
               {
                  if (((((c_Id.u32_NodeIndex == c_PrevId.u32_NodeIndex) &&
                         (c_Id.e_ComProtocol == c_PrevId.e_ComProtocol)) &&
                        (c_Id.u32_DatapoolIndex == c_PrevId.u32_DatapoolIndex)) &&
                       (c_Id.u32_InterfaceIndex == c_PrevId.u32_InterfaceIndex)) &&
                      (c_Id.u32_InterfaceIndex == c_PrevId.u32_InterfaceIndex))
                  {
                     if (this->mc_SignalIndex[u32_It] >= u32_PrevSignalIndex)
                     {
                        //pass
                     }
                     else
                     {
                        q_Sorted = false;
                        break;
                     }
                  }
               }
               //prep next iteration
               c_PrevId = c_Id;
               u32_PrevSignalIndex = this->mc_SignalIndex[u32_It];
               q_PrevIdValid = true;
            }
         }
      }
   }
   return q_Sorted;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store current values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalAddDeleteBaseCommand::m_Store(void)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      //Reserve
      this->mc_SignalIndex.reserve(this->mc_UniqueId.size());
      this->mc_Signal.reserve(this->mc_UniqueId.size());
      this->mc_OscSignalCommon.reserve(this->mc_UniqueId.size());
      this->mc_UiSignalCommon.reserve(this->mc_UniqueId.size());
      this->mc_UiSignal.reserve(this->mc_UniqueId.size());
      for (uint32_t u32_ItStep = 0UL; u32_ItStep < this->mc_UniqueId.size(); ++u32_ItStep)
      {
         const stw::opensyde_core::C_OscCanSignal * const pc_Signal = C_PuiSdHandler::h_GetInstance()->GetCanSignal(
            this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
               this->mc_UniqueId[u32_ItStep]), this->mc_SignalIndex[u32_ItStep]);
         const stw::opensyde_core::C_OscNodeDataPoolListElement * const pc_OscSignalCommon =
            C_PuiSdHandler::h_GetInstance()->GetOscCanDataPoolListElement(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                             this->mc_UniqueId[u32_ItStep]),
                                                                          this->mc_SignalIndex[u32_ItStep]);
         const C_PuiSdNodeDataPoolListElement * const pc_UiSignalCommon =
            C_PuiSdHandler::h_GetInstance()->GetUiCanDataPoolListElement(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                            this->mc_UniqueId[u32_ItStep]),
                                                                         this->mc_SignalIndex[u32_ItStep]);
         const C_PuiSdNodeCanSignal * const pc_UiSignal = C_PuiSdHandler::h_GetInstance()->GetUiCanSignal(
            this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
               this->mc_UniqueId[u32_ItStep]),
            this->mc_SignalIndex[u32_ItStep]);

         tgl_assert(pc_Signal != NULL);
         if (pc_Signal != NULL)
         {
            if (u32_ItStep < this->mc_Signal.size())
            {
               this->mc_Signal[u32_ItStep] = *pc_Signal;
            }
            else
            {
               this->mc_Signal.push_back(*pc_Signal);
            }
         }
         tgl_assert(pc_OscSignalCommon != NULL);
         if (pc_OscSignalCommon != NULL)
         {
            if (u32_ItStep < this->mc_OscSignalCommon.size())
            {
               this->mc_OscSignalCommon[u32_ItStep] = *pc_OscSignalCommon;
            }
            else
            {
               this->mc_OscSignalCommon.push_back(*pc_OscSignalCommon);
            }
         }
         tgl_assert(pc_UiSignalCommon != NULL);
         if (pc_UiSignalCommon != NULL)
         {
            if (u32_ItStep < this->mc_UiSignalCommon.size())
            {
               this->mc_UiSignalCommon[u32_ItStep] = *pc_UiSignalCommon;
            }
            else
            {
               this->mc_UiSignalCommon.push_back(*pc_UiSignalCommon);
            }
         }
         tgl_assert(pc_UiSignal != NULL);
         if (pc_UiSignal != NULL)
         {
            if (u32_ItStep < this->mc_UiSignal.size())
            {
               this->mc_UiSignal[u32_ItStep] = *pc_UiSignal;
            }
            else
            {
               this->mc_UiSignal.push_back(*pc_UiSignal);
            }
         }
      }
      tgl_assert(this->m_CheckSignalsSortedAscending());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove current message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueUnoSignalAddDeleteBaseCommand::m_Remove(void)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      for (uint32_t u32_ItStep = this->mc_UniqueId.size(); u32_ItStep > 0UL; --u32_ItStep)
      {
         tgl_assert(this->mpc_MessageSyncManager->DeleteCanSignal(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                     this->mc_UniqueId[static_cast<std::vector<uint64_t>
                                                                                                   ::
                                                                                                   size_type>(u32_ItStep
                                                                                                              -
                                                                                                              1UL)]),
                                                                  this->mc_SignalIndex[static_cast<std::vector<uint32_t>
                                                                                                   ::
                                                                                                   size_type>(u32_ItStep
                                                                                                              -
                                                                                                              1UL)]) ==
                    C_NO_ERR);
         if (this->mpc_MessageTreeWidget != NULL)
         {
            this->mpc_MessageTreeWidget->InternalDeleteSignal(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                 this->mc_UniqueId[static_cast<std::vector<uint64_t>::
                                                                                               size_type>(u32_ItStep -
                                                                                                          1UL)]),
                                                              this->mc_SignalIndex[static_cast<std::vector<uint32_t>::
                                                                                               size_type>(u32_ItStep -
                                                                                                          1UL)]);
         }
      }
   }
}
