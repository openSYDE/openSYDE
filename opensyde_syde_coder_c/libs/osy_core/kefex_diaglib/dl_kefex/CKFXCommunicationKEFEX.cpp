#include "precomp_headers.hpp" //pre-compiled headers

#include "DiagLib_config.hpp" //diaglib configuration

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "CKFXCommunicationKEFEX.hpp"
#include "C_SclIniFile.hpp"
#include "TglUtils.hpp"
#include "C_SclChecksums.hpp"

//---------------------------------------------------------------------------

using namespace stw::errors;
using namespace stw::diag_lib;
using namespace stw::scl;
using namespace stw::tgl;

//---------------------------------------------------------------------------

static const uint8_t KFX_LOCK_SRR  = 0U;
static const uint8_t KFX_LOCK_AIR  = 2U;
static const uint8_t KFX_LOCK_AAR  = 3U;
static const uint8_t KFX_LOCK_IWR  = 4U;
static const uint8_t KFX_LOCK_ReadEEP = 5U;
static const uint8_t KFX_LOCK_WriteEEP = 6U;
static const uint8_t KFX_LOCK_UpdateTask = 7U;
static const uint8_t KFX_LOCK_Segmented_Read = 8U;
static const uint8_t KFX_LOCK_Segmented_Write = 9U;

//---------------------------------------------------------------------------

C_KFXCommunicationKEFEX::C_KFXCommunicationKEFEX(const bool oq_SleepBetweenPolling) :
   C_KFXCommunicationBase()
{
   uint8_t u8_Index;

   for (u8_Index = 0U; u8_Index < KFX_NUM_LOCKS; u8_Index++)
   {
      m_apcLocks[u8_Index] = new C_TglCriticalSection();
      m_adwNumLocked[u8_Index] = 0U;
   }

   mu16_NumServicesToSend = 0U;
   mu16_CRCOverSentServices = 0U;

   mpc_Protocol = new C_KFXProtocol(&mt_Configuration, oq_SleepBetweenPolling);
   mpc_Protocol->InstallECUResetHandler(&C_KFXCommunicationKEFEX::m_StaticResetHandler, this);
   (void)mpc_Protocol->ConfigChanged();

   mt_Features.q_Logon = true;
   mt_Features.q_Service = true;
   mt_Features.q_IndexReadNumeric = true;
   mt_Features.q_IndexWriteNumeric = true;
   mt_Features.q_IndexReadAggregate = true;
   mt_Features.q_IndexWriteAggregate = true;
   mt_Features.q_EEPROMRead = true;
   mt_Features.q_EEPROMWrite = true;
   mt_Features.q_ROMRead = false;
   mt_Features.q_CyclicTransmissions = true;
   mt_Features.q_ResetIndication = true;
   mt_Features.q_CommProtocolRAW = true;
   mt_Features.q_CommProtocolHLP = true;
   mt_Features.q_DirectCheckSumHandling = true;
   mt_Features.q_ChecksumInIndexedMode = false;
   mt_Features.q_EEPROMWriteSSL = true;

   mt_Configuration.mc_ProtocolName = KFX_COMM_PROTOCOL_NAME_KEFEX;
}

//---------------------------------------------------------------------------

C_KFXCommunicationKEFEX::~C_KFXCommunicationKEFEX()
{
   uint8_t u8_Index;

   for (u8_Index = 0U; u8_Index < KFX_NUM_LOCKS; u8_Index++)
   {
      delete m_apcLocks[u8_Index];
   }
   delete mpc_Protocol;
}

//---------------------------------------------------------------------------

void C_KFXCommunicationKEFEX::Cycle(void)
{
   uint8_t u8_Index;

   //all incoming cyclic transmissions will be reported in callback in the background:
   //only do this if there is no other communication going on:
   for (u8_Index = 0U; u8_Index < KFX_NUM_LOCKS; u8_Index++)
   {
      if (m_adwNumLocked[u8_Index] > 0U)
      {
         return;
      }
   }
   mpc_Protocol->EvaluateAllResponses();
}

//---------------------------------------------------------------------------

//access to CriticalSections encapsulated
//reason: Releasing a section if it's not claimed
//  will cause a subsequent Acquire to hang
void C_KFXCommunicationKEFEX::m_Lock(const uint8_t ou8_Index)
{
   m_apcLocks[ou8_Index]->Acquire();
   m_adwNumLocked[ou8_Index]++;
}

//---------------------------------------------------------------------------

void C_KFXCommunicationKEFEX::m_Unlock(const uint8_t ou8_Index)
{
   if (m_adwNumLocked[ou8_Index] > 0U)
   {
      m_apcLocks[ou8_Index]->Release();
      m_adwNumLocked[ou8_Index]--;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief    perform logon

   Logon to the server ECU.
   This is required for subsequent access to data on the server ECU.
   Will actively wait for an error or OK response (or until timeout).

   \param[in]  ou64_ProjectChecksum    logon checksum
   \param[in]  ou16_DataVersion        version of data set
   \param[in]  ou16_NumOfVars          total number of variables in project
   \param[in]  ou8_ProjectIndex        index of project to log in to (0 for most server implementations)

   \return
   C_NO_ERR       -> logon finished                            \n
   C_CONFIG       -> project checksum invalid                  \n
   C_RD_WR        -> could not send request                    \n
   C_COM          -> no response                               \n
   C_WARN         -> error response
*/
//-----------------------------------------------------------------------------
int32_t C_KFXCommunicationKEFEX::Logon(const uint64_t ou64_ProjectChecksum, const uint16_t ou16_DataVersion,
                                       const uint16_t ou16_NumOfVars, const uint8_t ou8_ProjectIndex)
{
   int32_t s32_Return;
   C_KFXProcotolResponse c_Service;

   //we can only handle 16bit checksum in this protocol !
   if (ou64_ProjectChecksum > 0xFFFFU)
   {
      return C_CONFIG;
   }

   this->Cycle(); //clear all orphaned responses (will also trigger event handlers for incoming cyclic transmissions)

   s32_Return = mpc_Protocol->SendLogonRequest(static_cast<uint16_t>(ou64_ProjectChecksum), ou8_ProjectIndex,
                                               ou16_DataVersion, ou16_NumOfVars);
   if (s32_Return != C_NO_ERR)
   {
      return C_RD_WR;
   }
   //Get Response
   s32_Return = mpc_Protocol->EvaluateResponses(KFX_PROTOCOL_LOGON_HS, c_Service);
   if (s32_Return != C_NO_ERR)
   {
      return C_COM;
   }
   if (c_Service.q_Error == true)
   {
      mu16_LastErrorCode = c_Service.GetU16FromDataIntel(2U);
      return C_WARN;
   }
   return C_NO_ERR;
}

//-----------------------------------------------------------------------------
/*!
   \brief    perform logoff

   Logon from the server ECU.

   \param[in]  oq_WaitForHandshake     true  -> will acticely wait for error or OK handshake response
                                       false -> will not wait for handshake response

   \return
   C_NO_ERR       -> logoff finished                           \n
   C_RD_WR        -> could not send request                    \n
   C_COM          -> no response                               \n
   C_WARN         -> error response
*/
//-----------------------------------------------------------------------------
int32_t C_KFXCommunicationKEFEX::Logoff(const bool oq_WaitForHandshake)
{
   int32_t s32_Return;
   C_KFXProcotolResponse c_Service;

   this->Cycle(); //clear all potentially orphaned responses
                  //(will also trigger event handlers for incoming cyclic transmissions)

   s32_Return = mpc_Protocol->SendLogoffRequest();
   if (s32_Return != C_NO_ERR)
   {
      return C_RD_WR;
   }

   if (oq_WaitForHandshake == true)
   {
      //Get Response
      s32_Return = mpc_Protocol->EvaluateResponses(KFX_PROTOCOL_LOGOFF_HS, c_Service);
      if (s32_Return != C_NO_ERR)
      {
         return C_COM;
      }
      if (c_Service.q_Error == true)
      {
         mu16_LastErrorCode = 0U;
         return C_WARN;
      }
   }
   return C_NO_ERR;
}

//---------------------------------------------------------------------------
//note: which index contains which information is protocol dependent !!!
int32_t C_KFXCommunicationKEFEX::ReadService(const uint32_t ou32_Index, uint32_t & oru32_Value)
{
   int32_t s32_Return;
   C_KFXProcotolResponse c_Service;

   tgl_assert(ou32_Index <= 0xFFFFU); //this protocol implementation can only handle a 16bit index

   this->Cycle(); //clear all orphaned responses (will also trigger event handlers for incoming cyclic transmissions)

   s32_Return = mpc_Protocol->SendServiceReadRequest(static_cast<uint16_t>(ou32_Index));
   if (s32_Return != C_NO_ERR)
   {
      return C_RD_WR;
   }

   s32_Return = mpc_Protocol->EvaluateResponses(KFX_PROTOCOL_SERVICE_READ, c_Service);
   if (s32_Return != C_NO_ERR)
   {
      return C_NOACT;
   }
   if (ou32_Index != c_Service.u32_IndexOrAddress)
   {
      return C_NOACT;
   }
   if (c_Service.q_Error == true)
   {
      mu16_LastErrorCode = c_Service.GetU16FromDataIntel(0U);
      return C_WARN;
   }

   oru32_Value = static_cast<uint32_t>(c_Service.au8_Data[0]) +
                 ((static_cast<uint32_t>(c_Service.au8_Data[1])) << 8U) +
                 ((static_cast<uint32_t>(c_Service.au8_Data[2])) << 16U) +
                 ((static_cast<uint32_t>(c_Service.au8_Data[3])) << 24U);
   return C_NO_ERR;
}

//---------------------------------------------------------------------------
//note: which index contains which information is protocol dependent !!!
int32_t C_KFXCommunicationKEFEX::WriteService(const uint32_t ou32_Index, const uint32_t ou32_Value)
{
   int32_t s32_Return;
   C_KFXProcotolResponse c_Service;

   tgl_assert(ou32_Index <= 0xFFFFU); //this protocol implementation can only handle a 16bit index

   this->Cycle(); //clear all orphaned responses (will also trigger event handlers for incoming cyclic transmissions)

   s32_Return = mpc_Protocol->SendServiceWriteRequest(static_cast<uint16_t>(ou32_Index), ou32_Value);
   if (s32_Return != C_NO_ERR)
   {
      return C_RD_WR;
   }

   s32_Return = mpc_Protocol->EvaluateResponses(KFX_PROTOCOL_SERVICE_WRITE, c_Service);
   if (s32_Return != C_NO_ERR)
   {
      return C_NOACT;
   }
   if (ou32_Index != c_Service.u32_IndexOrAddress)
   {
      return C_NOACT;
   }
   if (c_Service.q_Error == true)
   {
      mu16_LastErrorCode = c_Service.GetU16FromDataIntel(0U);
      return C_WARN;
   }
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationKEFEX::ReadEEPROM(const uint32_t ou32_Address, const uint32_t ou32_NumBytes,
                                            uint8_t * const opu8_Data)
{
   int32_t s32_Return;
   uint32_t u32_Offset = 0U;
   uint32_t u32_BytesLeft = ou32_NumBytes;
   C_KFXProcotolResponse c_Service;

   this->Cycle(); //clear all orphaned responses (will also trigger event handlers for incoming cyclic transmissions)

   m_Lock(KFX_LOCK_ReadEEP);
   while (u32_BytesLeft > 0U)
   {
      if (u32_BytesLeft >= 2U)
      {
         //read two bytes at once
         s32_Return = mpc_Protocol->SendReadDataWordRequest(KFX_PROTOCOL_READ_EEPROM, ou32_Address + u32_Offset);
      }
      else
      {
         s32_Return = mpc_Protocol->SendReadDataByteRequest(KFX_PROTOCOL_READ_EEPROM, ou32_Address + u32_Offset);
      }
      if (s32_Return != C_NO_ERR)
      {
         m_Unlock(KFX_LOCK_ReadEEP);
         return C_RD_WR;
      }

      s32_Return = mpc_Protocol->EvaluateResponses(KFX_PROTOCOL_READ_EEPROM, c_Service);
      if (s32_Return != C_NO_ERR)
      {
         m_Unlock(KFX_LOCK_ReadEEP);
         return C_COM;
      }
      if (c_Service.q_Error == true)
      {
         mu16_LastErrorCode = c_Service.GetU16FromDataIntel(0U);
         m_Unlock(KFX_LOCK_ReadEEP);
         return C_WARN;
      }
      if ((ou32_Address + u32_Offset) == c_Service.u32_IndexOrAddress)
      {
         if (u32_BytesLeft >= 2U)
         {
            opu8_Data[u32_Offset]      = c_Service.au8_Data[0];
            opu8_Data[u32_Offset + 1U] = c_Service.au8_Data[1];
            u32_Offset += 2U;
            u32_BytesLeft = static_cast<uint32_t>(u32_BytesLeft - 2U);
         }
         else
         {
            opu8_Data[u32_Offset] = c_Service.au8_Data[0];
            u32_Offset++;
            u32_BytesLeft--;
         }
      }
      else
      {
         mu16_LastErrorCode = 0U;
         m_Unlock(KFX_LOCK_ReadEEP);
         return C_WARN;
      }
   }
   m_Unlock(KFX_LOCK_ReadEEP);
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationKEFEX::WriteEEPROM(const uint32_t ou32_Address, const uint32_t ou32_NumBytes,
                                             const uint8_t * const opu8_Data)
{
   int32_t s32_Return;
   uint32_t u32_Offset = 0U;
   uint32_t u32_BytesLeft = ou32_NumBytes;
   C_KFXProcotolResponse c_Service;
   uint8_t au8_Data[4];

   this->Cycle(); //clear all orphaned responses (will also trigger event handlers for incoming cyclic transmissions)

   m_Lock(KFX_LOCK_WriteEEP);
   while (u32_BytesLeft > 0U)
   {
      au8_Data[0] = static_cast<uint8_t>(ou32_Address + u32_Offset);
      au8_Data[1] = static_cast<uint8_t>((ou32_Address + u32_Offset) >> 8U);
      au8_Data[2] = static_cast<uint8_t>((ou32_Address + u32_Offset) >> 16U);
      au8_Data[3] = static_cast<uint8_t>((ou32_Address + u32_Offset) >> 24U);
      C_SclChecksums::CalcCRC16(&au8_Data[0], 4U, mu16_CRCOverSentServices);
      if (u32_BytesLeft >= 2U)
      {
         //write two bytes at once
         C_SclChecksums::CalcCRC16(&opu8_Data[u32_Offset], 2U, mu16_CRCOverSentServices);
         s32_Return = mpc_Protocol->SendWriteDataWordRequest(KFX_PROTOCOL_WRITE_EEPROM_HS, ou32_Address + u32_Offset,
                                                             &opu8_Data[u32_Offset]);
      }
      else
      {
         C_SclChecksums::CalcCRC16(&opu8_Data[u32_Offset], 1U, mu16_CRCOverSentServices);
         s32_Return = mpc_Protocol->SendWriteDataByteRequest(KFX_PROTOCOL_WRITE_EEPROM_HS, ou32_Address + u32_Offset,
                                                             opu8_Data[u32_Offset]);
      }
      if (s32_Return != C_NO_ERR)
      {
         m_Unlock(KFX_LOCK_WriteEEP);
         return C_RD_WR;
      }

      s32_Return = mpc_Protocol->EvaluateResponses(KFX_PROTOCOL_WRITE_EEPROM_HS, c_Service);
      if (s32_Return != C_NO_ERR)
      {
         m_Unlock(KFX_LOCK_WriteEEP);
         return C_COM;
      }
      if (c_Service.q_Error == true)
      {
         mu16_LastErrorCode = c_Service.GetU16FromDataIntel(0U);
         m_Unlock(KFX_LOCK_WriteEEP);
         return C_WARN;
      }
      if ((ou32_Address + u32_Offset) != c_Service.u32_IndexOrAddress)
      {
         mu16_LastErrorCode = 0U;
         m_Unlock(KFX_LOCK_WriteEEP);
         return C_WARN;
      }
      if (u32_BytesLeft >= 2U)
      {
         if ((c_Service.au8_Data[0] != opu8_Data[u32_Offset]) ||
             (c_Service.au8_Data[1] != opu8_Data[u32_Offset + 1U]))
         {
            mu16_LastErrorCode = 0U;
            m_Unlock(KFX_LOCK_WriteEEP);
            return C_WARN;
         }
         u32_Offset += 2U;
         u32_BytesLeft = static_cast<uint32_t>(u32_BytesLeft - 2U);
      }
      else
      {
         if (c_Service.au8_Data[0] != opu8_Data[u32_Offset])
         {
            mu16_LastErrorCode = 0U;
            m_Unlock(KFX_LOCK_WriteEEP);
            return C_WARN;
         }
         u32_Offset++;
         u32_BytesLeft--;
      }
   }

   m_Unlock(KFX_LOCK_WriteEEP);
   return C_NO_ERR;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Execute WRITE EEPROM SSL START service

   Send request to start EEPROM write procedure with CRC over the sent data.
   Then wait for response and report result.

   \param[in]  ou32_DesiredWriteSize   number of following bytes to write

   \return
   C_NO_ERR  request sent
   C_RD_WR   could not send request
   C_COM     no response
   C_WARN    error response or inconsistent response
   C_RANGE   ou32_DesiredWriteSize out of range
*/
//-----------------------------------------------------------------------------
int32_t C_KFXCommunicationKEFEX::WriteEEPROMSSLStart(const uint32_t ou32_DesiredWriteSize)
{
   int32_t s32_Return;
   uint32_t u32_NumServices;
   C_KFXProcotolResponse c_Service;

   //be defensive: set values that don't add up ...
   mu16_NumServicesToSend = 0U;
   mu16_CRCOverSentServices = 0U;

   u32_NumServices = (ou32_DesiredWriteSize + 1U) / 2U; //2 bytes fit in one service
   if (u32_NumServices > 0xFFFFU)
   {
      return C_RANGE; //more than we can handle
   }

   this->Cycle(); //clear all orphaned responses (will also trigger event handlers for incoming cyclic transmissions)

   s32_Return = mpc_Protocol->SendWriteEepromSslStartRequest(static_cast<uint16_t>(u32_NumServices));
   if (s32_Return != C_NO_ERR)
   {
      return C_RD_WR;
   }

   s32_Return = mpc_Protocol->EvaluateResponses(KFX_PROTOCOL_WRITE_EEPROM_SSL, c_Service);
   if (s32_Return != C_NO_ERR)
   {
      return C_COM;
   }

   if (c_Service.q_Error == true)
   {
      mu16_LastErrorCode = c_Service.GetU16FromDataIntel(1U);
      return C_WARN;
   }

   if ((c_Service.au8_Data[0] != 1U) || //"START" service ?
       (c_Service.u32_IndexOrAddress != u32_NumServices))
   {
      mu16_LastErrorCode = c_Service.GetU16FromDataIntel(1U);
      return C_WARN;
   }

   mu16_NumServicesToSend = static_cast<uint16_t>(u32_NumServices);
   mu16_CRCOverSentServices = 0x1D0FU; //set CCITT25 start value

   return C_NO_ERR;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Execute WRITE EEPROM SSL END service

   Send request for confirmation of finished EEPROM write procedure with CRC over the sent data.
   Then wait for response and report result.

   The following data is used:
   - the data size set with WriteEEPROMSSLStart()
   - the CRC calculated in WriteEEPROM() since the last call of WriteEEPROMSSLStart()

   \return
   C_NO_ERR    request sent
   C_RD_WR     could not send request
   C_COM       no response
   C_WARN      error response or inconsistent response
*/
//-----------------------------------------------------------------------------
int32_t C_KFXCommunicationKEFEX::WriteEEPROMSSLEnd(void)
{
   int32_t s32_Return;
   C_KFXProcotolResponse c_Service;

   this->Cycle(); //clear all orphaned responses (will also trigger event handlers for incoming cyclic transmissions)

   s32_Return = mpc_Protocol->SendWriteEepromSslEndRequest(mu16_NumServicesToSend, mu16_CRCOverSentServices);
   if (s32_Return != C_NO_ERR)
   {
      return C_RD_WR;
   }

   s32_Return = mpc_Protocol->EvaluateResponses(KFX_PROTOCOL_WRITE_EEPROM_SSL, c_Service);
   if (s32_Return != C_NO_ERR)
   {
      return C_COM;
   }

   if (c_Service.q_Error == true)
   {
      mu16_LastErrorCode = c_Service.GetU16FromDataIntel(1U);
      return C_WARN;
   }

   if ((c_Service.au8_Data[0] != 2U) || //"END" service ?
       (c_Service.u32_IndexOrAddress != static_cast<uint32_t>(mu16_NumServicesToSend)))
   {
      mu16_LastErrorCode = c_Service.GetU16FromDataIntel(1U);
      return C_WARN;
   }

   return C_NO_ERR;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationKEFEX::ReadNumericVariable(const uint32_t ou32_Index, const uint8_t ou8_NumBytes,
                                                     int64_t & ors64_Value)
{
   int32_t s32_Return;
   C_KFXProcotolResponse c_Service;

   tgl_assert(ou32_Index <= 0xFFFFU); //this protocol implementation can only handle a 16bit index

   this->Cycle(); //clear all orphaned responses (will also trigger event handlers for incoming cyclic transmissions)

   //in KEFEX protocol we can only transfer variables <= 4 bytes expedited !
   if (ou8_NumBytes > 4U)
   {
      return this->ReadAggregateVariable(ou32_Index, ou8_NumBytes, reinterpret_cast<uint8_t *>(&ors64_Value));
   }

   m_Lock(KFX_LOCK_SRR);
   c_Service.q_NewValue = false;
   s32_Return = mpc_Protocol->SendSRRRequest(static_cast<uint16_t>(ou32_Index));
   if (s32_Return != C_NO_ERR)
   {
      m_Unlock(KFX_LOCK_SRR);
      return C_RD_WR;
   }

   s32_Return = mpc_Protocol->EvaluateResponses(KFX_PROTOCOL_SRR, c_Service);
   m_Unlock(KFX_LOCK_SRR);
   if (s32_Return != C_NO_ERR)
   {
      return C_COM;
   }
   if (ou32_Index != c_Service.u32_IndexOrAddress)
   {
      return C_COM;
   }

   ors64_Value = static_cast<int64_t>(c_Service.GetU32FromDataIntel(0U));

   if (c_Service.q_Error == true)
   {
      mu16_LastErrorCode = static_cast<uint16_t>(ors64_Value);
      return C_WARN;
   }
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationKEFEX::WriteNumericVariable(const uint32_t ou32_Index, const uint8_t ou8_NumBytes,
                                                      const int64_t os64_Value)
{
   int32_t s32_Return;
   C_KFXProcotolResponse c_Service;

   tgl_assert(ou32_Index <= 0xFFFFU); //this protocol implementation can only handle a 16bit index

   this->Cycle(); //clear all orphaned responses (will also trigger event handlers for incoming cyclic transmissions)

   //in KEFEX protocol we can only transfer variables <= 4 bytes expedited !
   if (ou8_NumBytes > 4U)
   {
      return this->WriteAggregateVariable(ou32_Index, ou8_NumBytes, reinterpret_cast<const uint8_t *>(&os64_Value));
   }

   m_Lock(KFX_LOCK_IWR);
   s32_Return = mpc_Protocol->SendImmediateWriteRequest(static_cast<uint16_t>(ou32_Index),
                                                        static_cast<uint32_t>(os64_Value));
   if (s32_Return != C_NO_ERR)
   {
      m_Unlock(KFX_LOCK_IWR);
      return C_RD_WR;
   }

   s32_Return = mpc_Protocol->EvaluateResponses(KFX_PROTOCOL_IMMEDIATE_WRITE_HS, c_Service);
   m_Unlock(KFX_LOCK_IWR);
   if (s32_Return != C_NO_ERR)
   {
      return C_COM;
   }
   if (c_Service.q_Error == true)
   {
      mu16_LastErrorCode = c_Service.GetU16FromDataIntel(0U);
      return C_WARN;
   }

   if (ou32_Index != c_Service.u32_IndexOrAddress)
   {
      mu16_LastErrorCode = 0U;
      return C_WARN;
   }
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationKEFEX::ReadAggregateVariable(const uint32_t ou32_Index, const uint32_t ou32_NumBytes,
                                                       uint8_t * const opu8_Data)
{
   int32_t s32_Return;

   tgl_assert(ou32_Index <= 0xFFFFU); //this protocol implementation can only handle a 16bit index

   //only one at a time (m_Lock and m_Unlock only prevent multiple
   // threads at the same time, but we must make sure there is also only
   // one segmented transfer for one server at a time !)
   if (m_adwNumLocked[KFX_LOCK_Segmented_Read] > 0U)
   {
      return C_BUSY;
   }

   this->Cycle(); //clear all orphaned responses (will also trigger event handlers for incoming cyclic transmissions)

   m_Lock(KFX_LOCK_Segmented_Read);
   s32_Return = mpc_Protocol->SegmentedSRRTransfer(static_cast<uint16_t>(ou32_Index), ou32_NumBytes, opu8_Data,
                                                   &mu16_LastErrorCode);
   m_Unlock(KFX_LOCK_Segmented_Read);
   return s32_Return;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationKEFEX::WriteAggregateVariable(const uint32_t ou32_Index, const uint32_t ou32_NumBytes,
                                                        const uint8_t * const opu8_Data)
{
   int32_t s32_Return;

   tgl_assert(ou32_Index <= 0xFFFFU); //this protocol implementation can only handle a 16bit index

   //only one at a time (m_Lock and m_Unlock only prevent multiple
   // threads at the same time, but we must make sure there is also only
   // one segmented transfer for one server at a time !)
   if (m_adwNumLocked[KFX_LOCK_Segmented_Write] > 0U)
   {
      return C_BUSY;
   }

   this->Cycle(); //clear all orphaned responses (will also trigger event handlers for incoming cyclic transmissions)

   m_Lock(KFX_LOCK_Segmented_Write);
   s32_Return = mpc_Protocol->SegmentedIWRTransfer(static_cast<uint16_t>(ou32_Index), ou32_NumBytes, opu8_Data);
   m_Unlock(KFX_LOCK_Segmented_Write);
   return s32_Return;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationKEFEX::SendChangeNotification(const uint32_t ou32_Index)
{
   int32_t s32_Return;
   C_KFXProcotolResponse c_Service;

   tgl_assert(ou32_Index <= 0xFFFFU); //this protocol implementation can only handle a 16bit index

   this->Cycle(); //clear all orphaned responses (will also trigger event handlers for incoming cyclic transmissions)

   m_Lock(KFX_LOCK_UpdateTask);
   s32_Return = mpc_Protocol->SendUpdateTaskRequest(static_cast<uint16_t>(ou32_Index), true);
   if (s32_Return != C_NO_ERR)
   {
      m_Unlock(KFX_LOCK_UpdateTask);
      return C_RD_WR;
   }
   s32_Return = mpc_Protocol->EvaluateResponses(KFX_PROTOCOL_UPDATE_TASK_HS, c_Service);
   m_Unlock(KFX_LOCK_UpdateTask);
   if (s32_Return != C_NO_ERR)
   {
      return C_COM;
   }
   if (c_Service.q_Error == true)
   {
      mu16_LastErrorCode = c_Service.GetU16FromDataIntel(0U);
      return C_WARN;
   }
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationKEFEX::RequestTimeTriggeredTransmission(const uint32_t ou32_Index, const bool oq_TimeStamped,
                                                                  const uint32_t ou32_Interval)
{
   int32_t s32_Return;

   tgl_assert(ou32_Index <= 0xFFFFU); //this protocol implementation can only handle a 16bit index
   s32_Return = mpc_Protocol->SendTCRRRequest(static_cast<uint16_t>(ou32_Index), oq_TimeStamped,
                                              static_cast<uint16_t>(ou32_Interval));
   return s32_Return;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationKEFEX::RequestChangeTriggeredTransmission(const uint32_t ou32_Index,
                                                                    const uint32_t ou32_MaxTimeout,
                                                                    const uint32_t ou32_UpperHysteresis,
                                                                    const uint32_t ou32_LowerHysteresis)
{
   int32_t s32_Return;

   tgl_assert(ou32_Index <= 0xFFFFU); //this protocol implementation can only handle a 16bit index
   s32_Return = mpc_Protocol->SendECRRRequestAbsolute(static_cast<uint16_t>(ou32_Index),
                                                      static_cast<uint16_t>(ou32_MaxTimeout),
                                                      ou32_UpperHysteresis, ou32_LowerHysteresis);
   if (s32_Return == C_WARN) //just a warning that the requested hysteresis values can not be exactly matched
   {
      s32_Return = C_NO_ERR;
   }
   return s32_Return;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationKEFEX::TerminateCyclicTransmission(const uint32_t ou32_Index)
{
   int32_t s32_Return;
   C_KFXProcotolResponse c_Service;

   tgl_assert(ou32_Index <= 0xFFFFU); //this protocol implementation can only handle a 16bit index

   this->Cycle(); //clear all orphaned responses (will also trigger event handlers for incoming cyclic transmissions)

   m_Lock(KFX_LOCK_AIR);
   s32_Return = mpc_Protocol->SendAbortIndividualResponseRequest(static_cast<uint16_t>(ou32_Index), true);
   if (s32_Return != C_NO_ERR)
   {
      m_Unlock(KFX_LOCK_AIR);
      return C_RD_WR;
   }

   s32_Return = mpc_Protocol->EvaluateResponses(KFX_PROTOCOL_ABORT_INDIVIDUAL_RESPONSE_HS, c_Service);
   m_Unlock(KFX_LOCK_AIR);
   if (s32_Return != C_NO_ERR)
   {
      return C_COM;
   }
   if ((c_Service.q_Error == true) || (c_Service.u32_IndexOrAddress != ou32_Index))
   {
      mu16_LastErrorCode = 0U;
      return C_WARN;
   }
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationKEFEX::TerminateAllCyclicTransmissions(const bool oq_WaitForHandshake)
{
   int32_t s32_Return;
   C_KFXProcotolResponse c_Service;

   this->Cycle(); //clear all orphaned responses (will also trigger event handlers for incoming cyclic transmissions)

   if (oq_WaitForHandshake == true)
   {
      m_Lock(KFX_LOCK_AAR);
   }
   s32_Return = mpc_Protocol->SendAbortAllResponsesRequest(oq_WaitForHandshake);
   if (s32_Return != C_NO_ERR)
   {
      if (oq_WaitForHandshake == true)
      {
         m_Unlock(KFX_LOCK_AAR);
      }
      return C_RD_WR;
   }

   if (oq_WaitForHandshake == true)
   {
      s32_Return = mpc_Protocol->EvaluateResponses(KFX_PROTOCOL_ABORT_ALL_RESPONSES_HS, c_Service);
      m_Unlock(KFX_LOCK_AAR);
      if (s32_Return != C_NO_ERR)
      {
         return C_COM;
      }
      if (c_Service.q_Error == true)
      {
         mu16_LastErrorCode = 0U;
         return C_WARN;
      }
   }
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationKEFEX::LoadConfigFromINI(C_SclIniFile * const opc_File, const C_SclString & orc_Section)
{
   int32_t s32_Return;

   s32_Return = C_KFXCommunicationBase::LoadConfigFromINI(opc_File, orc_Section);
   if (s32_Return != C_NO_ERR)
   {
      return s32_Return;
   }
   (void)mpc_Protocol->ConfigChanged();
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationKEFEX::SetConfig(const C_KFXCommConfiguration * const opt_Config)
{
   int32_t s32_Return;

   s32_Return = C_KFXCommunicationBase::SetConfig(opt_Config);
   if (s32_Return != C_NO_ERR)
   {
      return s32_Return;
   }
   (void)mpc_Protocol->ConfigChanged();
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

void C_KFXCommunicationKEFEX::SetCommDispatcher(stw::can::C_CanDispatcher * const opc_Dispatcher)
{
   mpc_Protocol->SetCommDispatcher(opc_Dispatcher);
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationKEFEX::WaitForStartupIndication(const uint32_t ou32_MaxWaitTimeMS)
{
   int32_t s32_Return;
   uint16_t u16_OldTimeOut;
   C_KFXProcotolResponse c_Service;
   bool q_OK = false;

   if (ou32_MaxWaitTimeMS > 0xFFFFU)
   {
      return C_RANGE;
   }

   //Get "Response"
   u16_OldTimeOut = mt_Configuration.GetTimeout();
   mt_Configuration.SetTimeout(static_cast<uint16_t>(ou32_MaxWaitTimeMS));
   (void)mpc_Protocol->ConfigChanged();

   s32_Return = mpc_Protocol->EvaluateResponses(KFX_PROTOCOL_STARTUP_INDICATION, c_Service);
   if (s32_Return == C_NO_ERR)
   {
      //We got the StartupIndication !
      //restore original timout:
      q_OK = true;
   }

   mt_Configuration.SetTimeout(u16_OldTimeOut);
   (void)mpc_Protocol->ConfigChanged();
   return (q_OK == true) ? C_NO_ERR : C_WARN;
}

//---------------------------------------------------------------------------

void C_KFXCommunicationKEFEX::SetCommProtocolMode(const uint8_t ou8_Mode)
{
   mpc_Protocol->u8_CreateCommProtocol = ou8_Mode;
}

//---------------------------------------------------------------------------

uint8_t C_KFXCommunicationKEFEX::GetCommProtocolMode(void)
{
   return mpc_Protocol->u8_CreateCommProtocol;
}

//---------------------------------------------------------------------------

void C_KFXCommunicationKEFEX::ResetCommProtocol(void)
{
   mpc_Protocol->ResetCommProtocol();
}

//---------------------------------------------------------------------------

#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
void C_KFXCommunicationKEFEX::SetKEFEXVariableInfo(const C_KFXVariableLists * const opt_KFXLists,
                                                   const uint16_t ou16_ListOffset) const
{
   mpc_Protocol->SetKEFEXVariableInfo(opt_KFXLists, ou16_ListOffset);
}
#endif

//---------------------------------------------------------------------------

void C_KFXCommunicationKEFEX::InstallCyclicTransmissionCallback(const PR_KFXCommCyclicTransmissionCallBack opr_Callback,
                                                                void * const opv_Instance)
{
   C_KFXCommunicationBase::InstallCyclicTransmissionCallback(opr_Callback, opv_Instance);
   mpc_Protocol->mpr_OnNewCyclicTransmissionReceived = opr_Callback;
   mpc_Protocol->mpv_CyclicCallbackInstance          = opv_Instance;
}

//---------------------------------------------------------------------------
