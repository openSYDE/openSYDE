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
   const std::vector<C_OSCCanMessageIdentificationIndices> & orc_MessageId, const std::vector<uint32> & orc_SignalIndex,
   const std::vector<uint16> & orc_StartBit, const std::vector<C_OSCCanSignal::E_MultiplexerType> & orc_MultiplexerType,
   const std::vector<uint16> & orc_MultiplexerValue, C_PuiSdNodeCanMessageSyncManager * const opc_MessageSyncManager,
   C_SdBueMessageSelectorTreeWidget * const opc_MessageTreeWidget, const QString & orc_Text,
   QUndoCommand * const opc_Parent) :
   C_SdBueUnoMessageBaseCommand(orc_MessageId, opc_MessageSyncManager, opc_MessageTreeWidget, orc_Text, opc_Parent),
   mc_SignalIndex(orc_SignalIndex)
{
   this->mc_SignalIndex.resize(this->mc_UniqueId.size(), 0UL);
   this->mc_Signal.resize(this->mc_UniqueId.size(), C_OSCCanSignal());
   this->mc_OSCSignalCommon.resize(this->mc_UniqueId.size(), C_OSCNodeDataPoolListElement());
   this->mc_UISignalCommon.resize(this->mc_UniqueId.size(), C_PuiSdNodeDataPoolListElement());
   this->mc_UISignal.resize(this->mc_UniqueId.size(), C_PuiSdNodeCanSignal());
   //Adapt default values
   for (uint32 u32_ItStep = 0UL; u32_ItStep < this->mc_UniqueId.size(); ++u32_ItStep)
   {
      this->mc_OSCSignalCommon[u32_ItStep].c_Name = "NewSignal";
      this->mc_OSCSignalCommon[u32_ItStep].c_DataSetValues.resize(1);
      this->mc_OSCSignalCommon[u32_ItStep].c_DataSetValues[0] = this->mc_OSCSignalCommon[u32_ItStep].c_MinValue;
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
            if (orc_MultiplexerType[u32_ItStep] == C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL)
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
         this->mc_Signal[u32_ItStep].e_MultiplexerType = C_OSCCanSignal::eMUX_DEFAULT;
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
      for (uint32 u32_ItStep = 0UL; u32_ItStep < this->mc_UniqueId.size(); ++u32_ItStep)
      {
         tgl_assert(this->mpc_MessageSyncManager->InsertCanSignal(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                     this->mc_UniqueId[u32_ItStep]),
                                                                  this->mc_SignalIndex[u32_ItStep],
                                                                  this->mc_Signal[u32_ItStep],
                                                                  this->mc_OSCSignalCommon[u32_ItStep],
                                                                  this->mc_UISignalCommon[u32_ItStep],
                                                                  this->mc_UISignal[u32_ItStep]) == C_NO_ERR);
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
            C_OSCCanMessageIdentificationIndices c_PrevId;
            uint32 u32_PrevSignalIndex = 0UL;
            bool q_PrevIdValid = false;
            for (uint32 u32_It = 0UL; u32_It < this->mc_UniqueId.size(); ++u32_It)
            {
               const C_OSCCanMessageIdentificationIndices c_Id = this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
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
      this->mc_OSCSignalCommon.reserve(this->mc_UniqueId.size());
      this->mc_UISignalCommon.reserve(this->mc_UniqueId.size());
      this->mc_UISignal.reserve(this->mc_UniqueId.size());
      for (uint32 u32_ItStep = 0UL; u32_ItStep < this->mc_UniqueId.size(); ++u32_ItStep)
      {
         const stw_opensyde_core::C_OSCCanSignal * const pc_Signal = C_PuiSdHandler::h_GetInstance()->GetCanSignal(
            this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
               this->mc_UniqueId[u32_ItStep]), this->mc_SignalIndex[u32_ItStep]);
         const stw_opensyde_core::C_OSCNodeDataPoolListElement * const pc_OSCSignalCommon =
            C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                             this->mc_UniqueId[u32_ItStep]),
                                                                          this->mc_SignalIndex[u32_ItStep]);
         const C_PuiSdNodeDataPoolListElement * const pc_UISignalCommon =
            C_PuiSdHandler::h_GetInstance()->GetUiCanDataPoolListElement(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                            this->mc_UniqueId[u32_ItStep]),
                                                                         this->mc_SignalIndex[u32_ItStep]);
         const C_PuiSdNodeCanSignal * const pc_UISignal = C_PuiSdHandler::h_GetInstance()->GetUiCanSignal(
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
         tgl_assert(pc_OSCSignalCommon != NULL);
         if (pc_OSCSignalCommon != NULL)
         {
            if (u32_ItStep < this->mc_OSCSignalCommon.size())
            {
               this->mc_OSCSignalCommon[u32_ItStep] = *pc_OSCSignalCommon;
            }
            else
            {
               this->mc_OSCSignalCommon.push_back(*pc_OSCSignalCommon);
            }
         }
         tgl_assert(pc_UISignalCommon != NULL);
         if (pc_UISignalCommon != NULL)
         {
            if (u32_ItStep < this->mc_UISignalCommon.size())
            {
               this->mc_UISignalCommon[u32_ItStep] = *pc_UISignalCommon;
            }
            else
            {
               this->mc_UISignalCommon.push_back(*pc_UISignalCommon);
            }
         }
         tgl_assert(pc_UISignal != NULL);
         if (pc_UISignal != NULL)
         {
            if (u32_ItStep < this->mc_UISignal.size())
            {
               this->mc_UISignal[u32_ItStep] = *pc_UISignal;
            }
            else
            {
               this->mc_UISignal.push_back(*pc_UISignal);
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
      for (uint32 u32_ItStep = this->mc_UniqueId.size(); u32_ItStep > 0UL; --u32_ItStep)
      {
         tgl_assert(this->mpc_MessageSyncManager->DeleteCanSignal(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                     this->mc_UniqueId[static_cast<std::vector<uint64>::
                                                                                                   size_type>(u32_ItStep
                                                                                                              -
                                                                                                              1UL)]),
                                                                  this->mc_SignalIndex[static_cast<std::vector<uint32>::
                                                                                                   size_type>(u32_ItStep
                                                                                                              -
                                                                                                              1UL)]) ==
                    C_NO_ERR);
         if (this->mpc_MessageTreeWidget != NULL)
         {
            this->mpc_MessageTreeWidget->InternalDeleteSignal(this->mpc_MessageSyncManager->GetMessageIdForUniqueId(
                                                                 this->mc_UniqueId[static_cast<std::vector<uint64>::
                                                                                               size_type>(u32_ItStep -
                                                                                                          1UL)]),
                                                              this->mc_SignalIndex[static_cast<std::vector<uint32>::
                                                                                               size_type>(u32_ItStep -
                                                                                                          1UL)]);
         }
      }
   }
}
