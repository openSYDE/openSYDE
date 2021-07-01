#ifndef CKFXCommH
#define CKFXCommH

#include "stwtypes.h"
#include "CKFXCommConfiguration.h"
#include "CSCLIniFile.h"
#include "DiagLib_config.h"
#include "CCANDispatcher.h"

//----------------------------------------------------------------------------------------------------------------------

namespace stw_diag_lib
{
//----------------------------------------------------------------------------------------------------------------------

//Name of KEFEX protocol:
//The string is used as unique IDs and must never be changed, so applications can rely on it !
//Notes:
//Ideally this should be placed as member variables of the actual implemented protocol drivers.
//This is done for other protocols.
//However some existing code (C_KFXSecureProject) requires the KEFEX protocol name for compatibility reasons.
//So if the name would be part of the specific protocol class an application would need to drag in
// that class even if the protocols is not used.
//So we keep the name global here.
//The array must be of POD type instead of a string class so we can rely on it being initialized when using it in
// a constructor.
extern const stw_types::charn KFX_COMM_PROTOCOL_NAME_KEFEX[6];

//----------------------------------------------------------------------------------------------------------------------

///what does the protocol driver support ?
class T_KFXProtocolFeatures
{
public:
   bool q_Logon;
   bool q_Service;
   bool q_IndexReadNumeric;
   bool q_IndexWriteNumeric;
   bool q_IndexReadAggregate;
   bool q_IndexWriteAggregate;
   bool q_EEPROMRead;
   bool q_EEPROMWrite;
   bool q_ROMRead;
   bool q_CyclicTransmissions;
   bool q_ResetIndication;
   bool q_CommProtocolRAW;
   bool q_CommProtocolHLP;
   bool q_DirectCheckSumHandling;
   //true  -> direct handling of checksums in EEPROM
   //false -> more abstract services: check_checksum + update_checksum
   bool q_ChecksumInIndexedMode;
   //true  -> checking/updating checksums is also expected/supported in indexed mode
   //         only should be used
   //         - in combination with q_DirectCheckSumHandling == false
   //         - in combination with q_CyclicTransmissions == false
   //false -> checking/updating checksums shall not be performed in indexed mode

   bool q_EEPROMWriteSSL; //true: EEPROM writing with application layer CRC over data supported

   bool aq_Reserved[16];

   T_KFXProtocolFeatures(void);
};

//----------------------------------------------------------------------------------------------------------------------

//use 32bit index in all functions for future enhancements (e.g. CANopen Index/Subindex) ...
typedef void (* PR_KFXCommCyclicTransmissionCallBack)(void * const opv_Instance, const stw_types::uint32 ou32_Index,
                                                      const stw_types::sint64 os64_Value,
                                                      const stw_types::uint32 os32_TimeStamp,
                                                      const bool oq_IsTimeStamped,
                                                      const bool oq_Error);
typedef void (* PR_KFXCommECUResetCallBack)(void * const opv_Instance);

//----------------------------------------------------------------------------------------------------------------------

///base class for "KEFEX-like" protocols
class STW_DIAGLIB_EXPORT C_KFXCommunicationBase
{
private:
   C_KFXCommunicationBase(const C_KFXCommunicationBase & orc_Source);               //not implemented -> prevent copying
   C_KFXCommunicationBase & operator = (const C_KFXCommunicationBase & orc_Source); //not implemented -> prevent
                                                                                    // assignment

protected:
   //reset callback handler:
   static void m_StaticResetHandler(void * const opv_InstancePointer);
   void m_ResetHandler(void);

   T_KFXProtocolFeatures mt_Features;
   C_KFXCommConfiguration mt_Configuration;

   stw_types::uint16 mu16_LastErrorCode;

   PR_KFXCommECUResetCallBack mpr_ECUResetCallBack;
   void * mpv_ECUResetInstance;

public:
   C_KFXCommunicationBase(void);
   virtual ~C_KFXCommunicationBase();

   stw_types::uint16 GetLastErrorResponse(void) const;

   virtual void InstallECUResetHandler(const PR_KFXCommECUResetCallBack opr_Callback, void * const opv_Instance);
   virtual void InstallCyclicTransmissionCallback(const PR_KFXCommCyclicTransmissionCallBack opr_Callback,
                                                  void * const opv_Instance);

   virtual void SetCommDispatcher(stw_can::C_CAN_Dispatcher * const opc_Dispatcher) = 0;

   //comm functions:
   virtual stw_types::sint32 Logon(const stw_types::uint64 ou64_ProjectChecksum,
                                   const stw_types::uint16 ou16_DataVersion, const stw_types::uint16 ou16_NumOfVars,
                                   const stw_types::uint8 ou8_ProjectIndex);
   virtual stw_types::sint32 Logoff(const bool oq_WaitForHandshake);

   virtual stw_types::sint32 ReadService(const stw_types::uint32 ou32_Index, stw_types::uint32 & oru32_Value);
   virtual stw_types::sint32 WriteService(const stw_types::uint32 ou32_Index, const stw_types::uint32 ou32_Value);

   virtual stw_types::sint32 ReadEEPROM(const stw_types::uint32 ou32_Address, const stw_types::uint32 ou32_NumBytes,
                                        stw_types::uint8 * const opu8_Data);
   virtual stw_types::sint32 WriteEEPROM(const stw_types::uint32 ou32_Address, const stw_types::uint32 ou32_NumBytes,
                                         const stw_types::uint8 * const opu8_Data);
   virtual stw_types::sint32 WriteEEPROMSSLStart(const stw_types::uint32 ou32_DesiredWriteSize);
   virtual stw_types::sint32 WriteEEPROMSSLEnd(void);

   virtual stw_types::sint32 ReadROM(const stw_types::uint32 ou32_Address, const stw_types::uint32 ou32_NumBytes,
                                     stw_types::uint8 * const opu8_Data);

   //integer + float variables:
   virtual stw_types::sint32 ReadNumericVariable(const stw_types::uint32 ou32_Index,
                                                 const stw_types::uint8 ou8_NumBytes, stw_types::sint64 & ors64_Value);
   virtual stw_types::sint32 WriteNumericVariable(const stw_types::uint32 ou32_Index,
                                                  const stw_types::uint8 ou8_NumBytes,
                                                  const stw_types::sint64 os64_Value);

   //aggregate types (arrays, structs, whatever ...)
   virtual stw_types::sint32 ReadAggregateVariable(const stw_types::uint32 ou32_Index,
                                                   const stw_types::uint32 ou32_NumBytes,
                                                   stw_types::uint8 * const opu8_Data);
   virtual stw_types::sint32 WriteAggregateVariable(const stw_types::uint32 ou32_Index,
                                                    const stw_types::uint32 ou32_NumBytes,
                                                    const stw_types::uint8 * const opu8_Data);

   virtual stw_types::sint32 SendChangeNotification(const stw_types::uint32 ou32_Index);

   //only for integer + float variables:
   virtual stw_types::sint32 RequestTimeTriggeredTransmission(const stw_types::uint32 ou32_Index,
                                                              const bool oq_TimeStamped,
                                                              const stw_types::uint32 ou32_Interval);
   virtual stw_types::sint32 RequestChangeTriggeredTransmission(const stw_types::uint32 ou32_Index,
                                                                const stw_types::uint32 ou32_MaxTimeout,
                                                                const stw_types::uint32 ou32_UpperHysteresis,
                                                                const stw_types::uint32 ou32_LowerHysteresis);

   virtual stw_types::sint32 TerminateCyclicTransmission(const stw_types::uint32 ou32_Index);
   virtual stw_types::sint32 TerminateAllCyclicTransmissions(const bool oq_WaitForHandshake);

   virtual stw_types::sint32 WaitForStartupIndication(const stw_types::uint32 ou32_MaxWaitTimeMS);

   virtual void Cycle(void) = 0;

   //abstract checksum handling:
   virtual stw_types::sint32 UpdateChecksum(const stw_types::uint32 ou32_Index);
   virtual stw_types::sint32 CheckChecksum(const stw_types::uint32 ou32_Index);

   virtual stw_types::sint32 GetConfig(C_KFXCommConfiguration * const opt_Config) const;
   virtual stw_types::sint32 SetConfig(const C_KFXCommConfiguration * const opt_Config);
   virtual stw_types::sint32 LoadConfigFromINI(stw_scl::C_SCLIniFile * const opc_File,
                                               const stw_scl::C_SCLString & orc_Section);
   virtual stw_types::sint32 SaveConfigToINI(stw_scl::C_SCLIniFile * const opc_File,
                                             const stw_scl::C_SCLString & orc_Section) const;

   virtual stw_scl::C_SCLString GetProtocolName(void);
   virtual void GetProtocolFeatures(T_KFXProtocolFeatures * const opt_Features);

   //0 = off; 1 -> RAW; 2 -> HLP
   virtual void SetCommProtocolMode(const stw_types::uint8 ou8_Mode) = 0;
   virtual stw_types::uint8 GetCommProtocolMode(void) = 0;

   //read protocol feature flags:
   bool HasFeatureLogon(void) const;
   bool HasFeatureService(void) const;
   bool HasFeatureIndexReadNumeric(void) const;
   bool HasFeatureIndexWriteNumeric(void) const;
   bool HasFeatureIndexReadAggregate(void) const;
   bool HasFeatureIndexWriteAggregate(void) const;
   bool HasFeatureEEPROMRead(void) const;
   bool HasFeatureEEPROMWrite(void) const;
   bool HasFeatureROMRead(void) const;
   bool HasFeatureCyclicTransmissions(void) const;
   bool HasFeatureResetIndication(void) const;
   bool HasFeatureCommProtocolRAW(void) const;
   bool HasFeatureCommProtocolHLP(void) const;
   //lint -sem(stw_diag_lib::C_KFXCommunicationBase::HasFeatureDirectCheckSumHandling,pure)
   bool HasFeatureDirectCheckSumHandling(void) const;
   bool HasFeatureChecksumInIndexedMode(void) const;
   bool HasFeatureEEPROMWriteSSL(void) const;
};

//---------------------------------------------------------------------------
///Wrapper for IV-style higher level layers (saves us from putting this in a component of its own)
class C_KFXCommunicationIV :
   public C_KFXCommunicationBase
{
public:
   C_KFXCommunicationIV(void);

   virtual stw_types::sint32 IV_ReadProtocolVersion(stw_types::uint16 & oru16_Version) = 0;
   virtual stw_types::sint32 IV_ReadDeviceID(stw_scl::C_SCLString & orc_DeviceID) = 0;
   virtual stw_types::sint32 IV_ReadODVersion(stw_types::uint16 & oru16_Version) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
}

#endif
