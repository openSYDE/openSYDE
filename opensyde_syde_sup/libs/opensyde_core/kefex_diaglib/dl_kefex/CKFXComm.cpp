#include "precomp_headers.hpp" //pre-compiled headers

#include "DiagLib_config.hpp" //diaglib configuration

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "CKFXComm.hpp"
#include "C_SclString.hpp"

//---------------------------------------------------------------------------

using namespace stw::errors;
using namespace stw::diag_lib;
using namespace stw::scl;

//---------------------------------------------------------------------------

namespace stw
{
namespace diag_lib
{
const char_t KFX_COMM_PROTOCOL_NAME_KEFEX[6] = "KEFEX";
}
}

//---------------------------------------------------------------------------

T_KFXProtocolFeatures::T_KFXProtocolFeatures(void)
{
   q_Logon = false;
   q_Service = false;
   q_IndexReadNumeric = false;
   q_IndexWriteNumeric = false;
   q_IndexReadAggregate = false;
   q_IndexWriteAggregate = false;
   q_EEPROMRead = false;
   q_EEPROMWrite = false;
   q_ROMRead = false;
   q_CyclicTransmissions = false;
   q_ResetIndication = false;
   q_CommProtocolRAW = false;
   q_CommProtocolHLP = false;
   q_DirectCheckSumHandling = false;
   q_ChecksumInIndexedMode = false;
   q_EEPROMWriteSSL = false;

   aq_Reserved[0] = false;
   aq_Reserved[1] = false;
   aq_Reserved[2] = false;
   aq_Reserved[3] = false;
   aq_Reserved[4] = false;
   aq_Reserved[5] = false;
   aq_Reserved[6] = false;
   aq_Reserved[7] = false;
   aq_Reserved[8] = false;
   aq_Reserved[9] = false;
   aq_Reserved[10] = false;
   aq_Reserved[11] = false;
   aq_Reserved[12] = false;
   aq_Reserved[13] = false;
   aq_Reserved[14] = false;
   aq_Reserved[15] = false;
}

//---------------------------------------------------------------------------

C_KFXCommunicationBase::C_KFXCommunicationBase(void)
{
   mpr_ECUResetCallBack = NULL;
   mpv_ECUResetInstance = NULL;
   mu16_LastErrorCode = 0U;
   mt_Configuration.SetAllDefaults();
}

//---------------------------------------------------------------------------

C_KFXCommunicationBase::~C_KFXCommunicationBase()
{
   mpv_ECUResetInstance = NULL;
}

//---------------------------------------------------------------------------
//static callback handler. Parameter: instance pointer to this class.
void C_KFXCommunicationBase::m_StaticResetHandler(void * const opv_InstancePointer)
{
   //call "real" non-static callback handler
   (reinterpret_cast<C_KFXCommunicationBase *>(opv_InstancePointer))->m_ResetHandler();
}

//---------------------------------------------------------------------------

void C_KFXCommunicationBase::m_ResetHandler(void)
{
   if (this->mpr_ECUResetCallBack != NULL)
   {
      this->mpr_ECUResetCallBack(mpv_ECUResetInstance);
   }
}

//---------------------------------------------------------------------------

uint16_t C_KFXCommunicationBase::GetLastErrorResponse(void)  const
{
   return mu16_LastErrorCode;
}

//---------------------------------------------------------------------------

void C_KFXCommunicationBase::InstallECUResetHandler(const PR_KFXCommECUResetCallBack opr_Callback,
                                                    void * const opv_Instance)
{
   mpr_ECUResetCallBack = opr_Callback;
   mpv_ECUResetInstance = opv_Instance;
}

//---------------------------------------------------------------------------

void C_KFXCommunicationBase::InstallCyclicTransmissionCallback(const PR_KFXCommCyclicTransmissionCallBack opr_Callback,
                                                               void * const opv_Instance)
{
   (void)opr_Callback;
   (void)opv_Instance;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::GetConfig(C_KFXCommConfiguration * const opt_Config) const
{
   if (opt_Config == NULL)
   {
      return C_CONFIG;
   }
   *opt_Config = mt_Configuration;
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::SetConfig(const C_KFXCommConfiguration * const opt_Config)
{
   if (opt_Config == NULL)
   {
      return C_CONFIG;
   }
   mt_Configuration = *opt_Config;
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

void C_KFXCommunicationBase::GetProtocolFeatures(T_KFXProtocolFeatures * const opt_Features)
{
   *opt_Features = mt_Features;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get name of protocol as string

   \return
   Text representation of protocol name
*/
//-----------------------------------------------------------------------------
C_SclString C_KFXCommunicationBase::GetProtocolName(void)
{
   return mt_Configuration.mc_ProtocolName;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::LoadConfigFromINI(C_SclIniFile * const opc_File, const C_SclString & orc_Section)
{
   return mt_Configuration.LoadConfigFromINI(*opc_File, orc_Section);
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::SaveConfigToINI(C_SclIniFile * const opc_File, const C_SclString & orc_Section) const
{
   return mt_Configuration.SaveConfigToINI(*opc_File, orc_Section);
}

//---------------------------------------------------------------------------

//Implement all comm functions to return C_CONFIG, so we don't need to overload them
//if they are not implemented.
//Drawback: we don't get a compiler error if we accidentally ferget to implement one, as we would
// if keeping them pure virtual in the base class.
int32_t C_KFXCommunicationBase::Logon(const uint64_t ou64_ProjectChecksum, const uint16_t ou16_DataVersion,
                                      const uint16_t ou16_NumOfVars, const uint8_t ou8_ProjectIndex)
{
   (void)ou64_ProjectChecksum;
   (void)ou16_DataVersion;
   (void)ou16_NumOfVars;
   (void)ou8_ProjectIndex;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::Logoff(const bool oq_WaitForHandshake)
{
   (void)oq_WaitForHandshake;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::ReadService(const uint32_t ou32_Index, uint32_t & oru32_Value)
{
   (void)ou32_Index;
   (void)oru32_Value;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::WriteService(const uint32_t ou32_Index, const uint32_t ou32_Value)
{
   (void)ou32_Index;
   (void)ou32_Value;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::ReadEEPROM(const uint32_t ou32_Address, const uint32_t ou32_NumBytes,
                                           uint8_t * const opu8_Data)
{
   (void)ou32_Address;
   (void)ou32_NumBytes;
   (void)opu8_Data;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::WriteEEPROM(const uint32_t ou32_Address, const uint32_t ou32_NumBytes,
                                            const uint8_t * const opu8_Data)
{
   (void)ou32_Address;
   (void)ou32_NumBytes;
   (void)opu8_Data;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::WriteEEPROMSSLStart(const uint32_t ou32_DesiredWriteSize)
{
   (void)ou32_DesiredWriteSize;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::WriteEEPROMSSLEnd(void)
{
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::ReadROM(const uint32_t ou32_Address, const uint32_t ou32_NumBytes,
                                        uint8_t * const opu8_Data)
{
   (void)ou32_Address;
   (void)ou32_NumBytes;
   (void)opu8_Data;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::ReadNumericVariable(const uint32_t ou32_Index, const uint8_t ou8_NumBytes,
                                                    int64_t & ors64_Value)
{
   (void)ou32_Index;
   (void)ou8_NumBytes;
   (void)ors64_Value;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::WriteNumericVariable(const uint32_t ou32_Index, const uint8_t ou8_NumBytes,
                                                     const int64_t os64_Value)
{
   (void)ou32_Index;
   (void)ou8_NumBytes;
   (void)os64_Value;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::ReadAggregateVariable(const uint32_t ou32_Index, const uint32_t ou32_NumBytes,
                                                      uint8_t * const opu8_Data)
{
   (void)ou32_Index;
   (void)ou32_NumBytes;
   (void)opu8_Data;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::WriteAggregateVariable(const uint32_t ou32_Index, const uint32_t ou32_NumBytes,
                                                       const uint8_t * const opu8_Data)
{
   (void)ou32_Index;
   (void)ou32_NumBytes;
   (void)opu8_Data;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::SendChangeNotification(const uint32_t ou32_Index)
{
   (void)ou32_Index;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::RequestTimeTriggeredTransmission(const uint32_t ou32_Index, const bool oq_TimeStamped,
                                                                 const uint32_t ou32_Interval)
{
   (void)ou32_Index;
   (void)oq_TimeStamped;
   (void)ou32_Interval;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::RequestChangeTriggeredTransmission(const uint32_t ou32_Index,
                                                                   const uint32_t ou32_MaxTimeout,
                                                                   const uint32_t ou32_UpperHysteresis,
                                                                   const uint32_t ou32_LowerHysteresis)
{
   (void)ou32_Index;
   (void)ou32_MaxTimeout;
   (void)ou32_UpperHysteresis;
   (void)ou32_LowerHysteresis;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::TerminateCyclicTransmission(const uint32_t ou32_Index)
{
   (void)ou32_Index;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::TerminateAllCyclicTransmissions(const bool oq_WaitForHandshake)
{
   (void)oq_WaitForHandshake;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::WaitForStartupIndication(const uint32_t ou32_MaxWaitTimeMS)
{
   (void)ou32_MaxWaitTimeMS;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::UpdateChecksum(const uint32_t ou32_Index)
{
   (void)ou32_Index;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommunicationBase::CheckChecksum(const uint32_t ou32_Index)
{
   (void)ou32_Index;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureLogon(void) const
{
   return mt_Features.q_Logon;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureService(void) const
{
   return mt_Features.q_Service;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureIndexReadNumeric(void) const
{
   return mt_Features.q_IndexReadNumeric;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureIndexWriteNumeric(void) const
{
   return mt_Features.q_IndexWriteNumeric;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureIndexReadAggregate(void) const
{
   return mt_Features.q_IndexReadAggregate;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureIndexWriteAggregate(void) const
{
   return mt_Features.q_IndexWriteAggregate;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureEEPROMRead(void) const
{
   return mt_Features.q_EEPROMRead;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureEEPROMWrite(void) const
{
   return mt_Features.q_EEPROMWrite;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureROMRead(void) const
{
   return mt_Features.q_ROMRead;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureCyclicTransmissions(void) const
{
   return mt_Features.q_CyclicTransmissions;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureResetIndication(void) const
{
   return mt_Features.q_ResetIndication;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureCommProtocolRAW(void) const
{
   return mt_Features.q_CommProtocolRAW;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureCommProtocolHLP(void) const
{
   return mt_Features.q_CommProtocolHLP;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureDirectCheckSumHandling(void) const
{
   return mt_Features.q_DirectCheckSumHandling;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureChecksumInIndexedMode(void) const
{
   return mt_Features.q_ChecksumInIndexedMode;
}

//---------------------------------------------------------------------------

bool C_KFXCommunicationBase::HasFeatureEEPROMWriteSSL(void) const
{
   return mt_Features.q_EEPROMWriteSSL;
}

//---------------------------------------------------------------------------

C_KFXCommunicationIV::C_KFXCommunicationIV(void) :
   C_KFXCommunicationBase()
{
}

//---------------------------------------------------------------------------
