#ifndef CKFXCommunicationKEFEXH
#define CKFXCommunicationKEFEXH

#include "stwtypes.h"
#include "CKFXComm.h"
#include "CKFXProtocol.h"
#include "CSCLIniFile.h"
#include "DiagLib_config.h"

//----------------------------------------------------------------------------------------------------------------------

namespace stw_diag_lib
{
//----------------------------------------------------------------------------------------------------------------------

const stw_types::uint16 KFX_NUM_LOCKS = 10U;

///KEFEX protocol communication driver
class STW_DIAGLIB_EXPORT C_KFXCommunicationKEFEX :
   public C_KFXCommunicationBase
{
private:
   stw_tgl::C_TGLCriticalSection * m_apcLocks[KFX_NUM_LOCKS];
   stw_types::uint32 m_adwNumLocked[KFX_NUM_LOCKS];

   void m_Lock(const stw_types::uint8 ou8_Index);
   void m_Unlock(const stw_types::uint8 ou8_Index);

   //in this implementation: refer to CKFXProtocol class ...
   C_KFXProtocol * mpc_Protocol;
   void m_KFXInitBasics(void);

   //WriteEEPROMSSL state:
   stw_types::uint16 mu16_NumServicesToSend;
   stw_types::uint16 mu16_CRCOverSentServices;

public:
   C_KFXCommunicationKEFEX(const C_KFXCommunicationKEFEX & orc_Source);               //not implemented -> prevent
                                                                                      // copying
   C_KFXCommunicationKEFEX & operator = (const C_KFXCommunicationKEFEX & orc_Source); //not implemented -> prevent
                                                                                      // assignment

   C_KFXCommunicationKEFEX(const bool oq_SleepBetweenPolling = false);
   virtual ~C_KFXCommunicationKEFEX();

   virtual void SetCommDispatcher(stw_can::C_CAN_Dispatcher * const opc_Dispatcher);

   virtual void InstallCyclicTransmissionCallback(const PR_KFXCommCyclicTransmissionCallBack opr_Callback,
                                                  void * const opv_Instance);

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

   virtual stw_types::sint32 ReadNumericVariable(const stw_types::uint32 ou32_Index,
                                                 const stw_types::uint8 ou8_NumBytes, stw_types::sint64 & ors64_Value);
   virtual stw_types::sint32 WriteNumericVariable(const stw_types::uint32 ou32_Index,
                                                  const stw_types::uint8 ou8_NumBytes,
                                                  const stw_types::sint64 os64_Value);

   virtual stw_types::sint32 ReadAggregateVariable(const stw_types::uint32 ou32_Index,
                                                   const stw_types::uint32 ou32_NumBytes,
                                                   stw_types::uint8 * const opu8_Data);
   virtual stw_types::sint32 WriteAggregateVariable(const stw_types::uint32 ou32_Index,
                                                    const stw_types::uint32 ou32_NumBytes,
                                                    const stw_types::uint8 * const opu8_Data);

   virtual stw_types::sint32 SendChangeNotification(const stw_types::uint32 ou32_Index);

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

   virtual void Cycle(void);

   virtual stw_types::sint32 LoadConfigFromINI(stw_scl::C_SCLIniFile * const opc_File,
                                               const stw_scl::C_SCLString & orc_Section);
   virtual stw_types::sint32 SetConfig(const C_KFXCommConfiguration * const opt_Config);

   //here: 0 = off; 1 -> KFX+L2; 2 -> KFX
   virtual void SetCommProtocolMode(const stw_types::uint8 ou8_Mode);
   virtual stw_types::uint8 GetCommProtocolMode(void);

   void ResetCommProtocol(void);
#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
   void SetKEFEXVariableInfo(const C_KFXVariableLists * opt_KFXLists, const stw_types::uint16 ou16_ListOffset) const;
#endif
};

//----------------------------------------------------------------------------------------------------------------------
}

#endif
