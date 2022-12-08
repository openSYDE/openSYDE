#ifndef CKFXCommH
#define CKFXCommH

#include "stwtypes.hpp"
#include "CKFXCommConfiguration.hpp"
#include "C_SclIniFile.hpp"
#include "DiagLib_config.hpp"
#include "C_CanDispatcher.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace stw
{
namespace diag_lib
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
extern const char_t KFX_COMM_PROTOCOL_NAME_KEFEX[6];

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
typedef void (* PR_KFXCommCyclicTransmissionCallBack)(void * const opv_Instance, const uint32_t ou32_Index,
                                                      const int64_t os64_Value, const uint32_t os32_TimeStamp,
                                                      const bool oq_IsTimeStamped, const bool oq_Error);
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

   uint16_t mu16_LastErrorCode;

   PR_KFXCommECUResetCallBack mpr_ECUResetCallBack;
   void * mpv_ECUResetInstance;

public:
   C_KFXCommunicationBase(void);
   virtual ~C_KFXCommunicationBase();

   uint16_t GetLastErrorResponse(void) const;

   virtual void InstallECUResetHandler(const PR_KFXCommECUResetCallBack opr_Callback, void * const opv_Instance);
   virtual void InstallCyclicTransmissionCallback(const PR_KFXCommCyclicTransmissionCallBack opr_Callback,
                                                  void * const opv_Instance);

   virtual void SetCommDispatcher(stw::can::C_CanDispatcher * const opc_Dispatcher) = 0;

   //comm functions:
   virtual int32_t Logon(const uint64_t ou64_ProjectChecksum, const uint16_t ou16_DataVersion,
                         const uint16_t ou16_NumOfVars, const uint8_t ou8_ProjectIndex);
   virtual int32_t Logoff(const bool oq_WaitForHandshake);

   virtual int32_t ReadService(const uint32_t ou32_Index, uint32_t & oru32_Value);
   virtual int32_t WriteService(const uint32_t ou32_Index, const uint32_t ou32_Value);

   virtual int32_t ReadEEPROM(const uint32_t ou32_Address, const uint32_t ou32_NumBytes, uint8_t * const opu8_Data);
   virtual int32_t WriteEEPROM(const uint32_t ou32_Address, const uint32_t ou32_NumBytes,
                               const uint8_t * const opu8_Data);
   virtual int32_t WriteEEPROMSSLStart(const uint32_t ou32_DesiredWriteSize);
   virtual int32_t WriteEEPROMSSLEnd(void);

   virtual int32_t ReadROM(const uint32_t ou32_Address, const uint32_t ou32_NumBytes, uint8_t * const opu8_Data);

   //integer + float variables:
   virtual int32_t ReadNumericVariable(const uint32_t ou32_Index, const uint8_t ou8_NumBytes, int64_t & ors64_Value);
   virtual int32_t WriteNumericVariable(const uint32_t ou32_Index, const uint8_t ou8_NumBytes,
                                        const int64_t os64_Value);

   //aggregate types (arrays, structs, whatever ...)
   virtual int32_t ReadAggregateVariable(const uint32_t ou32_Index, const uint32_t ou32_NumBytes,
                                         uint8_t * const opu8_Data);
   virtual int32_t WriteAggregateVariable(const uint32_t ou32_Index, const uint32_t ou32_NumBytes,
                                          const uint8_t * const opu8_Data);

   virtual int32_t SendChangeNotification(const uint32_t ou32_Index);

   //only for integer + float variables:
   virtual int32_t RequestTimeTriggeredTransmission(const uint32_t ou32_Index, const bool oq_TimeStamped,
                                                    const uint32_t ou32_Interval);
   virtual int32_t RequestChangeTriggeredTransmission(const uint32_t ou32_Index, const uint32_t ou32_MaxTimeout,
                                                      const uint32_t ou32_UpperHysteresis,
                                                      const uint32_t ou32_LowerHysteresis);

   virtual int32_t TerminateCyclicTransmission(const uint32_t ou32_Index);
   virtual int32_t TerminateAllCyclicTransmissions(const bool oq_WaitForHandshake);

   virtual int32_t WaitForStartupIndication(const uint32_t ou32_MaxWaitTimeMS);

   virtual void Cycle(void) = 0;

   //abstract checksum handling:
   virtual int32_t UpdateChecksum(const uint32_t ou32_Index);
   virtual int32_t CheckChecksum(const uint32_t ou32_Index);

   virtual int32_t GetConfig(C_KFXCommConfiguration * const opt_Config) const;
   virtual int32_t SetConfig(const C_KFXCommConfiguration * const opt_Config);
   virtual int32_t LoadConfigFromINI(stw::scl::C_SclIniFile * const opc_File,
                                     const stw::scl::C_SclString & orc_Section);
   virtual int32_t SaveConfigToINI(stw::scl::C_SclIniFile * const opc_File,
                                   const stw::scl::C_SclString & orc_Section) const;

   virtual stw::scl::C_SclString GetProtocolName(void);
   virtual void GetProtocolFeatures(T_KFXProtocolFeatures * const opt_Features);

   //0 = off; 1 -> RAW; 2 -> HLP
   virtual void SetCommProtocolMode(const uint8_t ou8_Mode) = 0;
   virtual uint8_t GetCommProtocolMode(void) = 0;

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
   //lint -sem(stw::diag_lib::C_KFXCommunicationBase::HasFeatureDirectCheckSumHandling,pure)
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

   virtual int32_t IV_ReadProtocolVersion(uint16_t & oru16_Version) = 0;
   virtual int32_t IV_ReadDeviceID(stw::scl::C_SclString & orc_DeviceId) = 0;
   virtual int32_t IV_ReadODVersion(uint16_t & oru16_Version) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
}
}

#endif
