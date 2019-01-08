//22.07.10   AST    Moved C_KFXCommunicationKEFEX to a .cpp file of its own
//19.07.10   AST    Added explicit calls to C_KFX_Protocol::Cycle (cf. KEFEX Ticket #146)
//11.05.09   AST    All index parameters are uint32 in preparation for upcoming protocols (e.g. CANopen)

#include "precomp_headers.h"  //pre-compiled headers
#ifdef __BORLANDC__   //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "DiagLib_config.h"  //diaglib configuration

#include "stwtypes.h"
#include "stwerrors.h"
#include "CKFXComm.h"
#include "CSCLString.h"

//---------------------------------------------------------------------------

using namespace stw_types;
using namespace stw_errors;
using namespace stw_diag_lib;
using namespace stw_scl;

//---------------------------------------------------------------------------

//place string here instead of header (else Borland precompiled headers will not work !)
namespace stw_diag_lib
{
const charn KFX_COMM_PROTOCOL_NAME_KEFEX[6] = "KEFEX";
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

uint16 C_KFXCommunicationBase::GetLastErrorResponse(void)  const
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

sint32 C_KFXCommunicationBase::GetConfig(C_KFXCommConfiguration * const opt_Config) const
{
   if (opt_Config == NULL)
   {
      return C_CONFIG;
   }
   *opt_Config = mt_Configuration;
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::SetConfig(const C_KFXCommConfiguration * const opt_Config)
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

   \created     xx.xx.200x  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_KFXCommunicationBase::GetProtocolName(void)
{
   return mt_Configuration.mc_ProtocolName;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::LoadConfigFromINI(C_SCLIniFile * const opc_File, const C_SCLString & orc_Section)
{
   return mt_Configuration.LoadConfigFromINI(*opc_File, orc_Section);
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::SaveConfigToINI(C_SCLIniFile * const opc_File, const C_SCLString & orc_Section) const
{
   return mt_Configuration.SaveConfigToINI(*opc_File, orc_Section);
}

//---------------------------------------------------------------------------

//Implement all comm functions to return C_CONFIG, so we don't need to overload them
//if they are not implemented.
//Drawback: we don't get a compiler error if we accidentally ferget to implement one, as we would
// if keeping them pure virtual in the base class.
sint32 C_KFXCommunicationBase::Logon(const uint64 ou64_ProjectChecksum, const uint16 ou16_DataVersion,
                                     const uint16 ou16_NumOfVars, const uint8 ou8_ProjectIndex)
{
   (void)ou64_ProjectChecksum;
   (void)ou16_DataVersion;
   (void)ou16_NumOfVars;
   (void)ou8_ProjectIndex;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::Logoff(const bool oq_WaitForHandshake)
{
   (void)oq_WaitForHandshake;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::ReadService(const uint32 ou32_Index, uint32 & oru32_Value)
{
   (void)ou32_Index;
   (void)oru32_Value;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::WriteService(const uint32 ou32_Index, const uint32 ou32_Value)
{
   (void)ou32_Index;
   (void)ou32_Value;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::ReadEEPROM(const uint32 ou32_Address, const uint32 ou32_NumBytes,
                                          uint8 * const opu8_Data)
{
   (void)ou32_Address;
   (void)ou32_NumBytes;
   (void)opu8_Data;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::WriteEEPROM(const uint32 ou32_Address, const uint32 ou32_NumBytes,
                                           const uint8 * const opu8_Data)
{
   (void)ou32_Address;
   (void)ou32_NumBytes;
   (void)opu8_Data;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::WriteEEPROMSSLStart(const uint32 ou32_DesiredWriteSize)
{
   (void)ou32_DesiredWriteSize;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::WriteEEPROMSSLEnd(void)
{
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::ReadROM(const uint32 ou32_Address, const uint32 ou32_NumBytes,
                                       uint8 * const opu8_Data)
{
   (void)ou32_Address;
   (void)ou32_NumBytes;
   (void)opu8_Data;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::ReadNumericVariable(const uint32 ou32_Index, const uint8 ou8_NumBytes,
                                                   sint64 & ors64_Value)
{
   (void)ou32_Index;
   (void)ou8_NumBytes;
   (void)ors64_Value;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::WriteNumericVariable(const uint32 ou32_Index, const uint8 ou8_NumBytes,
                                                    const sint64 os64_Value)
{
   (void)ou32_Index;
   (void)ou8_NumBytes;
   (void)os64_Value;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::ReadAggregateVariable(const uint32 ou32_Index, const uint32 ou32_NumBytes,
                                                     uint8 * const opu8_Data)
{
   (void)ou32_Index;
   (void)ou32_NumBytes;
   (void)opu8_Data;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::WriteAggregateVariable(const uint32 ou32_Index, const uint32 ou32_NumBytes,
                                                      const uint8 * const opu8_Data)
{
   (void)ou32_Index;
   (void)ou32_NumBytes;
   (void)opu8_Data;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::SendChangeNotification(const uint32 ou32_Index)
{
   (void)ou32_Index;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::RequestTimeTriggeredTransmission(const uint32 ou32_Index,
                                                                const bool oq_TimeStamped,
                                                                const uint32 ou32_Interval)
{
   (void)ou32_Index;
   (void)oq_TimeStamped;
   (void)ou32_Interval;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::RequestChangeTriggeredTransmission(const uint32 ou32_Index,
                                                                  const uint32 ou32_MaxTimeout,
                                                                  const uint32 ou32_UpperHysteresis,
                                                                  const uint32 ou32_LowerHysteresis)
{
   (void)ou32_Index;
   (void)ou32_MaxTimeout;
   (void)ou32_UpperHysteresis;
   (void)ou32_LowerHysteresis;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::TerminateCyclicTransmission(const uint32 ou32_Index)
{
   (void)ou32_Index;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::TerminateAllCyclicTransmissions(const bool oq_WaitForHandshake)
{
   (void)oq_WaitForHandshake;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::WaitForStartupIndication(const uint32 ou32_MaxWaitTimeMS)
{
   (void)ou32_MaxWaitTimeMS;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::UpdateChecksum(const uint32 ou32_Index)
{
   (void)ou32_Index;
   return C_CONFIG;
}

//---------------------------------------------------------------------------

sint32 C_KFXCommunicationBase::CheckChecksum(const uint32 ou32_Index)
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
