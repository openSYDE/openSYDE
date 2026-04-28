//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage container for all CAN message types (implementation)

   Storage container for all CAN message types

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_OscCanMessageContainer.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanMessageContainer::C_OscCanMessageContainer(void) :
   q_IsComProtocolUsedByInterface(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanMessageContainer::CalcHash(uint32_t & oru32_HashValue) const
{
   uint32_t u32_Tmp;

   stw::scl::C_SclChecksums::CalcCRC32(&this->q_IsComProtocolUsedByInterface,
                                       sizeof(this->q_IsComProtocolUsedByInterface), oru32_HashValue);

   for (uint32_t u32_Counter = 0U; u32_Counter < this->c_RxMessages.size(); ++u32_Counter)
   {
      this->c_RxMessages[u32_Counter].CalcHash(oru32_HashValue, false);
   }

   for (uint32_t u32_Counter = 0U; u32_Counter < this->c_TxMessages.size(); ++u32_Counter)
   {
      this->c_TxMessages[u32_Counter].CalcHash(oru32_HashValue, false);
   }

   //Force CRC change if single Rx message is moved to empty Tx (previously no change in CRC calculation)
   u32_Tmp = static_cast<uint32_t>(this->c_RxMessages.size());
   stw::scl::C_SclChecksums::CalcCRC32(&u32_Tmp, sizeof(u32_Tmp), oru32_HashValue);
   u32_Tmp = static_cast<uint32_t>(this->c_TxMessages.size());
   stw::scl::C_SclChecksums::CalcCRC32(&u32_Tmp, sizeof(u32_Tmp), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Recalculate all data element indices

   Based on assumption data elements are sorted in data pool by message and signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanMessageContainer::ReCalcDataElementIndices(void)
{
   std::vector<C_OscCanMessage> * pc_ComMessages;

   for (uint8_t u8_Toggle = 0; u8_Toggle < 2; ++u8_Toggle)
   {
      uint32_t u32_ListSignalIndex = 0;
      if ((u8_Toggle % 2) == 0)
      {
         pc_ComMessages = &this->c_TxMessages;
      }
      else
      {
         pc_ComMessages = &this->c_RxMessages;
      }
      //Check consistency
      for (uint32_t u32_ItMessage = 0; u32_ItMessage < pc_ComMessages->size(); ++u32_ItMessage)
      {
         C_OscCanMessage & rc_Message = (*pc_ComMessages)[u32_ItMessage];
         for (uint32_t u32_ItSignal = 0; u32_ItSignal < rc_Message.c_Signals.size(); ++u32_ItSignal)
         {
            C_OscCanSignal & rc_Signal = rc_Message.c_Signals[u32_ItSignal];
            rc_Signal.u32_ComDataElementIndex = u32_ListSignalIndex;
            ++u32_ListSignalIndex;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get start of message in data pool list element vector

   \param[in]  orq_IsTx             Flag if message is tx (else rx)
   \param[in]  oru32_MessageIndex   Message index

   \return
   Data pool list element index of message start
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscCanMessageContainer::GetMessageSignalDataStartIndex(const bool & orq_IsTx,
                                                                  const uint32_t & oru32_MessageIndex) const
{
   uint32_t u32_Retval = 0;

   const std::vector<C_OscCanMessage> & rc_ComMessages = this->GetMessagesConst(orq_IsTx);

   //Check consistency
   for (uint32_t u32_ItMessage = 0; (u32_ItMessage < rc_ComMessages.size()) && (u32_ItMessage < oru32_MessageIndex);
        ++u32_ItMessage)
   {
      const C_OscCanMessage & rc_Message = rc_ComMessages[u32_ItMessage];
      for (uint32_t u32_ItSignal = 0; u32_ItSignal < rc_Message.c_Signals.size(); ++u32_ItSignal)
      {
         ++u32_Retval;
      }
   }

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Request reference to either list

   \param[in]  orq_IsTx    Flag if tx message vector was requested (else rx message vector is output)

   \return
   Either tx or rx list (as requested)
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_OscCanMessage> & C_OscCanMessageContainer::GetMessagesConst(const bool & orq_IsTx) const
{
   return (orq_IsTx == true) ? this->c_TxMessages : this->c_RxMessages;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Request reference to either list

   \param[in]  orq_IsTx    Flag if tx message vector was requested (else rx message vector is output)

   \return
   Either tx or rx list (as requested)
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OscCanMessage> & C_OscCanMessageContainer::GetMessages(const bool & orq_IsTx)
{
   return (orq_IsTx == true) ? this->c_TxMessages : this->c_RxMessages;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check message error

   IMPORTANT: some error types (e.g. name & id conflict) can only be checked via system definition

   \param[in]   opc_List                              Node data pool list containing signal data
                                                      (Optional as it is only required by some checks)
   \param[in]   oru32_MessageIndex                    Message index
   \param[in]   orq_IsTx                              Flag if message is tx (else rx)
   \param[out]  opq_NameConflict                      Name conflict
   \param[out]  opq_NameInvalid                       Name not usable as variable
   \param[out]  opq_DelayTimeInvalid                  Delay time is invalid
   \param[out]  opq_IdConflict                        Id conflict
   \param[out]  opq_IdInvalid                         Id out of 11 bit / 29 bit range
   \param[out]  opq_SignalInvalid                     An error found for a signal
   \param[out]  opq_NoSignalsInvalid                  An error found for having no signals
   \param[out]  opq_TxMethodInvalid                   An error found for having a TX method which is not valid
   \param[in]   ou32_CanMessageValidSignalsDlcOffset  CAN message DLC offset for valid signal range check
   \param[in]   oq_CanMessageSignalGapsValid          Flag if gaps between signals are valid or handled as errors
   \param[in]   oq_ByteAlignmentRequired              Flag if byte alignment is required and if the signal does not
                                                      fit into byte alignment it will be handled as error
   \param[in]   oq_SignalsRequired                    Flag if signals are required for a message
   \param[in]   oq_CanOpenPdoSyncValid                Flag if the CANopen TX methods with PDO SYNC are valid or if no
                                                      check is necessary (other protocols)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanMessageContainer::CheckMessageLocalError(const C_OscNodeDataPoolList * const opc_List,
                                                      const uint32_t & oru32_MessageIndex, const bool & orq_IsTx,
                                                      bool * const opq_NameConflict, bool * const opq_NameInvalid,
                                                      bool * const opq_DelayTimeInvalid, bool * const opq_IdConflict,
                                                      bool * const opq_IdInvalid, bool * const opq_SignalInvalid,
                                                      bool * const opq_NoSignalsInvalid,
                                                      bool * const opq_TxMethodInvalid,
                                                      const uint32_t ou32_CanMessageValidSignalsDlcOffset,
                                                      const bool oq_CanMessageSignalGapsValid,
                                                      const bool oq_ByteAlignmentRequired,
                                                      const bool oq_SignalsRequired,
                                                      const bool oq_CanOpenPdoSyncValid) const
{
   const std::vector<C_OscCanMessage> & rc_Messages = this->GetMessagesConst(orq_IsTx);

   if (oru32_MessageIndex < rc_Messages.size())
   {
      const C_OscCanMessage & rc_Message = rc_Messages[oru32_MessageIndex];

      //Check variable name
      if (opq_NameInvalid != NULL)
      {
         if ((rc_Message.q_CanOpenManagerMessageActive == true) &&
             (C_OscUtils::h_CheckValidCeName(rc_Message.c_Name) == false))
         {
            *opq_NameInvalid = true;
         }
         else
         {
            *opq_NameInvalid = false;
         }
      }
      //Name conflict
      if (opq_NameConflict != NULL)
      {
         *opq_NameConflict = false;

         if (rc_Message.q_CanOpenManagerMessageActive == true)
         {
            //Tx
            for (uint32_t u32_ItMessage =
                    0; (u32_ItMessage < this->c_TxMessages.size()) && (*opq_NameConflict == false);
                 ++u32_ItMessage)
            {
               bool q_Skip = false;

               if (orq_IsTx == true)
               {
                  if (u32_ItMessage == oru32_MessageIndex)
                  {
                     q_Skip = true;
                  }
               }
               if (q_Skip == false)
               {
                  const C_OscCanMessage & rc_CurrentMessage = this->c_TxMessages[u32_ItMessage];
                  if (rc_Message.c_Name.LowerCase() == rc_CurrentMessage.c_Name.LowerCase())
                  {
                     *opq_NameConflict = true;
                  }
               }
            }

            //Rx
            for (uint32_t u32_ItMessage =
                    0; (u32_ItMessage < this->c_RxMessages.size()) && (*opq_NameConflict == false);
                 ++u32_ItMessage)
            {
               bool q_Skip = false;

               if (orq_IsTx == false)
               {
                  if (u32_ItMessage == oru32_MessageIndex)
                  {
                     q_Skip = true;
                  }
               }
               if (q_Skip == false)
               {
                  const C_OscCanMessage & rc_CurrentMessage = this->c_RxMessages[u32_ItMessage];
                  if (rc_Message.c_Name.LowerCase() == rc_CurrentMessage.c_Name.LowerCase())
                  {
                     *opq_NameConflict = true;
                  }
               }
            }
         }
      }

      //Id
      //Check valid id
      if (opq_IdInvalid != NULL)
      {
         *opq_IdInvalid = false;

         if (rc_Message.q_CanOpenManagerMessageActive == true)
         {
            if (rc_Message.q_IsExtended == true)
            {
               if (rc_Message.u32_CanId > 0x1FFFFFFFUL)
               {
                  *opq_IdInvalid = true;
               }
            }
            else
            {
               if (rc_Message.u32_CanId > 0x7FFUL)
               {
                  *opq_IdInvalid = true;
               }
            }
         }
      }
      //Name conflict
      if (opq_IdConflict != NULL)
      {
         *opq_IdConflict = false;

         if (rc_Message.q_CanOpenManagerMessageActive == true)
         {
            //Tx
            for (uint32_t u32_ItMessage = 0; (u32_ItMessage < this->c_TxMessages.size()) && (*opq_IdConflict == false);
                 ++u32_ItMessage)
            {
               bool q_Skip = false;

               if (orq_IsTx == true)
               {
                  if (u32_ItMessage == oru32_MessageIndex)
                  {
                     q_Skip = true;
                  }
               }
               if (q_Skip == false)
               {
                  const C_OscCanMessage & rc_CurrentMessage = this->c_TxMessages[u32_ItMessage];
                  if ((rc_Message.u32_CanId == rc_CurrentMessage.u32_CanId) &&
                      (rc_Message.q_IsExtended == rc_CurrentMessage.q_IsExtended))
                  {
                     *opq_IdConflict = true;
                  }
               }
            }

            //Rx
            for (uint32_t u32_ItMessage = 0; (u32_ItMessage < this->c_RxMessages.size()) && (*opq_IdConflict == false);
                 ++u32_ItMessage)
            {
               bool q_Skip = false;

               if (orq_IsTx == false)
               {
                  if (u32_ItMessage == oru32_MessageIndex)
                  {
                     q_Skip = true;
                  }
               }
               if (q_Skip == false)
               {
                  const C_OscCanMessage & rc_CurrentMessage = this->c_RxMessages[u32_ItMessage];
                  if ((rc_Message.u32_CanId == rc_CurrentMessage.u32_CanId) &&
                      (rc_Message.q_IsExtended == rc_CurrentMessage.q_IsExtended))
                  {
                     *opq_IdConflict = true;
                  }
               }
            }
         }
      }
      //Check
      if (opq_DelayTimeInvalid != NULL)
      {
         *opq_DelayTimeInvalid = false;

         if (rc_Message.q_CanOpenManagerMessageActive == true)
         {
            if ((rc_Message.e_TxMethod == C_OscCanMessage::eTX_METHOD_ON_CHANGE) ||
                (rc_Message.e_TxMethod == C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254) ||
                (rc_Message.e_TxMethod == C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_255))
            {
               if (static_cast<uint32_t>(rc_Message.u16_DelayTimeMs) > rc_Message.u32_CycleTimeMs)
               {
                  *opq_DelayTimeInvalid = true;
               }
            }
         }
      }

      //Signals
      if (opq_SignalInvalid != NULL)
      {
         *opq_SignalInvalid = false;

         if (rc_Message.q_CanOpenManagerMessageActive == true)
         {
            for (uint32_t u32_ItSignal = 0;
                 (u32_ItSignal < rc_Message.c_Signals.size()) && (*opq_SignalInvalid == false);
                 ++u32_ItSignal)
            {
               if (rc_Message.CheckErrorSignal(opc_List, u32_ItSignal, ou32_CanMessageValidSignalsDlcOffset,
                                               oq_CanMessageSignalGapsValid, oq_ByteAlignmentRequired))
               {
                  *opq_SignalInvalid = true;
               }
            }
         }
      }

      //Signal count
      if (opq_NoSignalsInvalid != NULL)
      {
         *opq_NoSignalsInvalid = false;

         // Only relevant if the message is active
         if ((oq_SignalsRequired == true) &&
             (rc_Message.q_CanOpenManagerMessageActive == true) &&
             (rc_Message.c_Signals.size() == 0))
         {
            *opq_NoSignalsInvalid = true;
         }
      }

      // TX method restrictions
      if (opq_TxMethodInvalid != NULL)
      {
         *opq_TxMethodInvalid = false;

         if ((oq_CanOpenPdoSyncValid == false) &&
             ((rc_Message.e_TxMethod == C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_0) ||
              (rc_Message.e_TxMethod == C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_1_TO_240)))
         {
            // TX methods are not valid
            *opq_TxMethodInvalid = true;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check messages error

   IMPORTANT: some error types (e.g. name & id conflict) can only be checked via system definition

   \param[in]      orc_ListTx                            Node data pool list containing tx signal data
   \param[in]      orc_ListRx                            Node data pool list containing rx signal data
   \param[in]      ou32_CanMessageValidSignalsDlcOffset  CAN message DLC offset for valid signal range check
   \param[in]      oq_CanMessageSignalGapsValid          Flag if gaps between signals are valid or handled as errors
   \param[in]      oq_ByteAlignmentRequired              Flag if byte alignment is required and if the signal does not
                                                         fit into byte alignment it will be handled as error
   \param[in]      oq_SignalsRequired                    Flag if signals are required for a message
   \param[in]      oq_CanOpenPdoSyncValid                Flag if the CANopen TX methods with PDO SYNC are valid or if no
                                                         check is necessary (other protocols)
   \param[in,out]  opc_InvalidTxMessages                 Optional vector of invalid Tx CAN message names
   \param[in,out]  opc_InvalidRxMessages                 Optional vector of invalid Rx CAN message names

   \return
   true  Error
   false No error detected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanMessageContainer::CheckLocalError(const C_OscNodeDataPoolList & orc_ListTx,
                                               const C_OscNodeDataPoolList & orc_ListRx,
                                               const uint32_t ou32_CanMessageValidSignalsDlcOffset,
                                               const bool oq_CanMessageSignalGapsValid,
                                               const bool oq_ByteAlignmentRequired, const bool oq_SignalsRequired,
                                               const bool oq_CanOpenPdoSyncValid,
                                               std::vector<uint32_t> * const opc_InvalidTxMessages,
                                               std::vector<uint32_t> * const opc_InvalidRxMessages) const
{
   bool q_Error = false;

   for (uint32_t u32_ItMessage = 0;
        (u32_ItMessage < this->c_TxMessages.size()) && ((q_Error == false) || (opc_InvalidTxMessages != NULL));
        ++u32_ItMessage)
   {
      bool q_NameConflict = false;
      bool q_NameInvalid = false;
      bool q_DelayInvalid = false;
      bool q_IdConflict = false;
      bool q_IdInvalid = false;
      bool q_SignalInvalid = false;
      bool q_NoSignalsInvalid = false;
      bool q_TxMethodInvalid = false;
      this->CheckMessageLocalError(&orc_ListTx, u32_ItMessage, true, &q_NameConflict, &q_NameInvalid, &q_DelayInvalid,
                                   &q_IdConflict, &q_IdInvalid, &q_SignalInvalid, &q_NoSignalsInvalid,
                                   &q_TxMethodInvalid,
                                   ou32_CanMessageValidSignalsDlcOffset,
                                   oq_CanMessageSignalGapsValid, oq_ByteAlignmentRequired, oq_SignalsRequired,
                                   oq_CanOpenPdoSyncValid);
      if ((q_NameConflict == true) || (q_NameInvalid == true) || (q_DelayInvalid == true) ||
          (q_IdConflict == true) || (q_IdInvalid == true) || (q_SignalInvalid == true)  ||
          (q_NoSignalsInvalid == true) || (q_TxMethodInvalid == true))
      {
         q_Error = true;
         if (opc_InvalidTxMessages != NULL)
         {
            opc_InvalidTxMessages->push_back(u32_ItMessage);
         }
      }
   }
   for (uint32_t u32_ItMessage = 0;
        (u32_ItMessage < this->c_RxMessages.size()) && ((q_Error == false) || (opc_InvalidRxMessages != NULL));
        ++u32_ItMessage)
   {
      bool q_NameConflict = false;
      bool q_NameInvalid = false;
      bool q_DelayInvalid = false;
      bool q_IdConflict = false;
      bool q_IdInvalid = false;
      bool q_SignalInvalid = false;
      bool q_NoSignalsInvalid = false;
      bool q_TxMethodInvalid = false;
      this->CheckMessageLocalError(&orc_ListRx, u32_ItMessage, false, &q_NameConflict, &q_NameInvalid, &q_DelayInvalid,
                                   &q_IdConflict, &q_IdInvalid, &q_SignalInvalid, &q_NoSignalsInvalid,
                                   &q_TxMethodInvalid,
                                   ou32_CanMessageValidSignalsDlcOffset,
                                   oq_CanMessageSignalGapsValid, oq_ByteAlignmentRequired, oq_SignalsRequired,
                                   oq_CanOpenPdoSyncValid);
      if ((q_NameConflict == true) || (q_NameInvalid == true) || (q_DelayInvalid == true) ||
          (q_IdConflict == true) || (q_IdInvalid == true) || (q_SignalInvalid == true) ||
          (q_NoSignalsInvalid == true) || (q_TxMethodInvalid == true))
      {
         q_Error = true;
         if (opc_InvalidRxMessages != NULL)
         {
            opc_InvalidRxMessages->push_back(u32_ItMessage);
         }
      }
   }
   return q_Error;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check whether container contains at least one Tx or Rx message

   \return
   true   at least one Tx or Rx message is defined
   false  else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanMessageContainer::ContainsAtLeastOneMessage(void) const
{
   bool q_Result = false;

   if ((this->c_TxMessages.size() > 0) || (this->c_RxMessages.size() > 0))
   {
      q_Result = true;
   }
   return q_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check whether container contains at least one Tx or Rx message with "q_Active" flag set

   \return
   true   at least one active Tx or Rx message is defined
   false  else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanMessageContainer::ContainsAtLeastOneActiveMessage(void) const
{
   uint32_t u32_ItMessage;
   bool q_Result = false;

   //check if there are any active TX messages:
   for (u32_ItMessage = 0U; u32_ItMessage < this->c_TxMessages.size(); ++u32_ItMessage)
   {
      if (this->c_TxMessages[u32_ItMessage].q_CanOpenManagerMessageActive == true)
      {
         q_Result = true;
         break;
      }
   }

   if (q_Result == false)
   {
      //check if there are any active RX PDO(s) with signals
      for (u32_ItMessage = 0; u32_ItMessage < this->c_RxMessages.size(); ++u32_ItMessage)
      {
         if (this->c_RxMessages[u32_ItMessage].q_CanOpenManagerMessageActive == true)
         {
            q_Result = true;
            break;
         }
      }
   }
   return q_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check min signal error

   \return
   Flags

   \retval   True    Min signals not present
   \retval   False   Number of signals valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanMessageContainer::CheckMinSignalError() const
{
   bool q_Retval = C_OscCanMessageContainer::mh_CheckMinSignalErrorPerVector(this->c_RxMessages);

   if (q_Retval == false)
   {
      q_Retval = C_OscCanMessageContainer::mh_CheckMinSignalErrorPerVector(this->c_TxMessages);
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle name max char limit

   \param[in]      ou32_NameMaxCharLimit  Name max char limit
   \param[in,out]  opc_ChangedItems       Changed items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanMessageContainer::HandleNameMaxCharLimit(const uint32_t ou32_NameMaxCharLimit,
                                                      std::list<C_OscSystemNameMaxCharLimitChangeReportItem> * const opc_ChangedItems)
{
   for (uint32_t u32_ItTxMsg = 0UL; u32_ItTxMsg < this->c_TxMessages.size(); ++u32_ItTxMsg)
   {
      C_OscCanMessage & rc_TxMsg = this->c_TxMessages[u32_ItTxMsg];
      C_OscSystemNameMaxCharLimitChangeReportItem::h_HandleNameMaxCharLimitItem(ou32_NameMaxCharLimit,
                                                                                "tx-message-name",
                                                                                rc_TxMsg.c_Name,
                                                                                opc_ChangedItems);
   }
   for (uint32_t u32_ItRxMsg = 0UL; u32_ItRxMsg < this->c_RxMessages.size(); ++u32_ItRxMsg)
   {
      C_OscCanMessage & rc_RxMsg = this->c_RxMessages[u32_ItRxMsg];
      C_OscSystemNameMaxCharLimitChangeReportItem::h_HandleNameMaxCharLimitItem(ou32_NameMaxCharLimit,
                                                                                "rx-message-name",
                                                                                rc_RxMsg.c_Name,
                                                                                opc_ChangedItems);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check min signal error per vector

   \param[in]  orc_Messages   Messages

   \return
   Flags

   \retval   True    Min signals not present
   \retval   False   Number of signals valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanMessageContainer::mh_CheckMinSignalErrorPerVector(const std::vector<C_OscCanMessage> & orc_Messages)
{
   bool q_Retval = false;
   bool q_SignalsPresent = false;

   for (uint32_t u32_ItMessage = 0; u32_ItMessage < orc_Messages.size(); ++u32_ItMessage)
   {
      if (orc_Messages[u32_ItMessage].c_Signals.size() > 0UL)
      {
         q_SignalsPresent = true;
         break;
      }
   }
   if ((q_SignalsPresent == false) && (orc_Messages.size() > 0UL))
   {
      q_Retval = true;
   }
   return q_Retval;
}
