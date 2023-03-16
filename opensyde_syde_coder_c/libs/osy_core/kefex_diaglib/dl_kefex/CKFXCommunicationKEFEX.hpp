#ifndef CKFXCommunicationKEFEXH
#define CKFXCommunicationKEFEXH

#include "stwtypes.hpp"
#include "CKFXComm.hpp"
#include "CKFXProtocol.hpp"
#include "C_SclIniFile.hpp"
#include "DiagLib_config.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace stw
{
namespace diag_lib
{
//----------------------------------------------------------------------------------------------------------------------

const uint16_t KFX_NUM_LOCKS = 10U;

///KEFEX protocol communication driver
class STW_DIAGLIB_EXPORT C_KFXCommunicationKEFEX :
   public C_KFXCommunicationBase
{
private:
   stw::tgl::C_TglCriticalSection * m_apcLocks[KFX_NUM_LOCKS];
   uint32_t m_adwNumLocked[KFX_NUM_LOCKS];

   void m_Lock(const uint8_t ou8_Index);
   void m_Unlock(const uint8_t ou8_Index);

   //in this implementation: refer to CKFXProtocol class ...
   C_KFXProtocol * mpc_Protocol;
   void m_KFXInitBasics(void);

   //WriteEEPROMSSL state:
   uint16_t mu16_NumServicesToSend;
   uint16_t mu16_CRCOverSentServices;

public:
   C_KFXCommunicationKEFEX(const C_KFXCommunicationKEFEX & orc_Source);               //not implemented -> prevent
                                                                                      // copying
   C_KFXCommunicationKEFEX & operator = (const C_KFXCommunicationKEFEX & orc_Source); //not implemented -> prevent
                                                                                      // assignment

   C_KFXCommunicationKEFEX(const bool oq_SleepBetweenPolling = false);
   virtual ~C_KFXCommunicationKEFEX();

   virtual void SetCommDispatcher(stw::can::C_CanDispatcher * const opc_Dispatcher);

   virtual void InstallCyclicTransmissionCallback(const PR_KFXCommCyclicTransmissionCallBack opr_Callback,
                                                  void * const opv_Instance);

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

   virtual int32_t ReadNumericVariable(const uint32_t ou32_Index, const uint8_t ou8_NumBytes, int64_t & ors64_Value);
   virtual int32_t WriteNumericVariable(const uint32_t ou32_Index, const uint8_t ou8_NumBytes,
                                        const int64_t os64_Value);

   virtual int32_t ReadAggregateVariable(const uint32_t ou32_Index, const uint32_t ou32_NumBytes,
                                         uint8_t * const opu8_Data);
   virtual int32_t WriteAggregateVariable(const uint32_t ou32_Index, const uint32_t ou32_NumBytes,
                                          const uint8_t * const opu8_Data);

   virtual int32_t SendChangeNotification(const uint32_t ou32_Index);

   virtual int32_t RequestTimeTriggeredTransmission(const uint32_t ou32_Index, const bool oq_TimeStamped,
                                                    const uint32_t ou32_Interval);
   virtual int32_t RequestChangeTriggeredTransmission(const uint32_t ou32_Index, const uint32_t ou32_MaxTimeout,
                                                      const uint32_t ou32_UpperHysteresis,
                                                      const uint32_t ou32_LowerHysteresis);

   virtual int32_t TerminateCyclicTransmission(const uint32_t ou32_Index);
   virtual int32_t TerminateAllCyclicTransmissions(const bool oq_WaitForHandshake);

   virtual int32_t WaitForStartupIndication(const uint32_t ou32_MaxWaitTimeMS);

   virtual void Cycle(void);

   virtual int32_t LoadConfigFromINI(stw::scl::C_SclIniFile * const opc_File,
                                     const stw::scl::C_SclString & orc_Section);
   virtual int32_t SetConfig(const C_KFXCommConfiguration * const opt_Config);

   //here: 0 = off; 1 -> KFX+L2; 2 -> KFX
   virtual void SetCommProtocolMode(const uint8_t ou8_Mode);
   virtual uint8_t GetCommProtocolMode(void);

   void ResetCommProtocol(void);
#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
   void SetKEFEXVariableInfo(const C_KFXVariableLists * opt_KFXLists, const uint16_t ou16_ListOffset) const;
#endif
};

//----------------------------------------------------------------------------------------------------------------------
}
}

#endif
