//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Diagnostic protocol driver for KEFEX protocol

   \class       stw::opensyde_core::C_OscDiagProtocolKfx
   \brief       openSYDE: Diagnostic protocol driver for KEFEX protocol

   Implements KEFEX protocol to be used with the openSYDE code.

   Concept for handling of incoming responses when waiting for a specific service response:
   - non event-driven responses not matching the expected service:
     - unexpected service will be dumped ...
     - unexpected length will be dumped
     - unexpected content will lead to error
   - event-driven responses will be reported via function pointers (see Cycle())

   Return values for KEFEX protocol functions:
   Unfortunately the pre-existing implementation of the KEFEX protocol does not allow to clearly identify error reasons.
   We can clearly differentiate between:
   * no error (C_NO_ERR)
   * error response (C_WARN)
   The other error codes cannot necessarily be mapped to correct error codes defined by the base class of this class.
   See the individual function headers for details.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDIAGPROTOCOLKFXH
#define C_OSCDIAGPROTOCOLKFXH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OscDiagProtocolBase.hpp"
#include "C_SclString.hpp"
#include "CKFXCommunicationKEFEX.hpp"
#include "CKFXCommConfiguration.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//----------------------------------------------------------------------------------------------------------------------
///interface class
class C_OscDiagProtocolKfx :
   public C_OscDiagProtocolBase
{
private:
   static const uint16_t mhu16_KFX_RAM_MAX_NUM_LISTS    = 64U;
   static const uint16_t mhu16_KFX_RAM_MAX_NUM_ELEMENTS = 1024U;

   //not implemented -> prevent copying
   C_OscDiagProtocolKfx(const C_OscDiagProtocolKfx & orc_Source);
   //not implemented -> prevent assignment
   C_OscDiagProtocolKfx & operator = (const C_OscDiagProtocolKfx & orc_Source); //lint !e1511 //we want to hide the base

   static uint16_t mh_PackDataPoolIndex(const uint16_t ou16_List, const uint16_t ou16_Variable);
   static void mh_UnpackDataPoolIndex(const uint16_t ou16_Index, uint16_t & oru16_List, uint16_t & oru16_Variable);

   bool mq_HasDispatcher;      ///< flag to remember whether dispatcher was installed
   bool mq_NvmValidFlagIsUsed; ///< true: use "SSL" layer services when writing to NVM

   static void mh_CyclicResponseReceived(void * const opv_Instance, const uint32_t ou32_Index, const int64_t os64_Value,
                                         const uint32_t ou32_TimeStamp, const bool oq_IsTimeStamped,
                                         const bool oq_Error);
   void m_CyclicResponseReceived(const uint32_t ou32_Index, const int64_t os64_Value, const uint32_t ou32_TimeStamp,
                                 const bool oq_IsTimeStamped, const bool oq_Error);

protected:
   uint16_t mu16_CommTimeoutMs;
   uint16_t mau16_EventRailIntervalsMs[3];
   stw::diag_lib::C_KFXCommunicationKEFEX * mpc_CommKefex;

public:
   C_OscDiagProtocolKfx();
   virtual ~C_OscDiagProtocolKfx(void);

   //class configuration:
   void SetDispatcher(stw::can::C_CanDispatcher * const opc_Dispatcher);
   void SetCommunicationParameters(const stw::diag_lib::C_KFXCommConfiguration & orc_Config);
   void SetNvmValidFlagUsed(const bool oq_IsUsed);

   //implemented functions from C_OscDiagProtocolBase:
   virtual uint8_t GetEndianness(void) const;

   virtual int32_t Cycle(void);
   virtual int32_t DataPoolReadNumeric(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                       const uint16_t ou16_ElementIndex, std::vector<uint8_t> & orc_ReadData,
                                       uint8_t * const opu8_NrCode);
   virtual int32_t DataPoolReadArray(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                     const uint16_t ou16_ElementIndex, std::vector<uint8_t> & orc_ReadData,
                                     uint8_t * const opu8_NrCode);
   virtual int32_t DataPoolWriteNumeric(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                        const uint16_t ou16_ElementIndex, const std::vector<uint8_t> & orc_DataToWrite,
                                        uint8_t * const opu8_NrCode);
   virtual int32_t DataPoolWriteArray(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                      const uint16_t ou16_ElementIndex, const std::vector<uint8_t> & orc_DataToWrite,
                                      uint8_t * const opu8_NrCode);
   virtual int32_t DataPoolSetEventDataRate(const uint8_t ou8_Rail, const uint16_t ou16_IntervalMs);
   virtual int32_t DataPoolReadCyclic(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                      const uint16_t ou16_ElementIndex, const uint8_t ou8_Rail,
                                      uint8_t * const opu8_NrCode);
   virtual int32_t DataPoolReadChangeDriven(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                            const uint16_t ou16_ElementIndex, const uint8_t ou8_Rail,
                                            const uint32_t ou32_Threshold, uint8_t * const opu8_NrCode);
   virtual int32_t DataPoolStopEventDriven(void);
   virtual int32_t NvmRead(const uint32_t ou32_MemoryAddress, std::vector<uint8_t> & orc_DataRecord,
                           uint8_t * const opu8_NrCode);
   virtual int32_t NvmWriteStartTransaction(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_NvmAccessCount);
   virtual int32_t NvmWrite(const uint32_t ou32_MemoryAddress, const std::vector<uint8_t> & orc_DataRecord,
                            uint8_t * const opu8_NrCode);
   virtual int32_t NvmWriteFinalizeTransaction(void);

   virtual int32_t DataPoolReadVersion(const uint8_t ou8_DataPoolIndex,
                                       uint8_t(&orau8_Version)[3],
                                       uint8_t * const opu8_NrCode);
   virtual int32_t DataPoolReadMetaData(const uint8_t ou8_DataPoolIndex,
                                        uint8_t(&orau8_Version)[3],
                                        stw::scl::C_SclString & orc_Name,
                                        uint8_t * const opu8_NrCode);
   virtual int32_t NvmNotifyOfChanges(const uint8_t ou8_DataPoolIndex, const uint8_t ou8_ListIndex,
                                      bool & orq_ApplicationAcknowledge, uint8_t * const opu8_NrCode);

   virtual int32_t DataPoolVerify(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_NumberOfDataPoolElements,
                                  const uint16_t ou16_DataPoolVersion, const uint32_t ou32_DataPoolChecksum,
                                  bool & orq_Match);

   int32_t Logoff(const bool oq_WaitForHandshake);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
