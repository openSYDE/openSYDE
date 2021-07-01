//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Diagnostic protocol driver for KEFEX protocol

   \class       stw_opensyde_core::C_OSCDiagProtocolKfx
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
#include "stwtypes.h"
#include "C_OSCDiagProtocolBase.h"
#include "CSCLString.h"
#include "CKFXCommunicationKEFEX.h"
#include "CKFXCommConfiguration.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//----------------------------------------------------------------------------------------------------------------------
///interface class
class C_OSCDiagProtocolKfx :
   public C_OSCDiagProtocolBase
{
private:
   static const stw_types::uint16 mhu16_KFX_RAM_MAX_NUM_LISTS    = 64U;
   static const stw_types::uint16 mhu16_KFX_RAM_MAX_NUM_ELEMENTS = 1024U;

   //not implemented -> prevent copying
   C_OSCDiagProtocolKfx(const C_OSCDiagProtocolKfx & orc_Source);
   //not implemented -> prevent assignment
   C_OSCDiagProtocolKfx & operator = (const C_OSCDiagProtocolKfx & orc_Source); //lint !e1511 //we want to hide the base

   static stw_types::uint16 mh_PackDataPoolIndex(const stw_types::uint16 ou16_List,
                                                 const stw_types::uint16 ou16_Variable);
   static void mh_UnpackDataPoolIndex(const stw_types::uint16 ou16_Index, stw_types::uint16 & oru16_List,
                                      stw_types::uint16 & oru16_Variable);

   bool mq_HasDispatcher;      ///< flag to remember whether dispatcher was installed
   bool mq_NvmValidFlagIsUsed; ///< true: use "SSL" layer services when writing to NVM

   static void mh_CyclicResponseReceived(void * const opv_Instance, const stw_types::uint32 ou32_Index,
                                         const stw_types::sint64 os64_Value, const stw_types::uint32 ou32_TimeStamp,
                                         const bool oq_IsTimeStamped, const bool oq_Error);
   void m_CyclicResponseReceived(const stw_types::uint32 ou32_Index, const stw_types::sint64 os64_Value,
                                 const stw_types::uint32 ou32_TimeStamp, const bool oq_IsTimeStamped,
                                 const bool oq_Error);

protected:
   stw_types::uint16 mu16_CommTimeoutMs;
   stw_types::uint16 mau16_EventRailIntervalsMs[3];
   stw_diag_lib::C_KFXCommunicationKEFEX * mpc_CommKefex;

public:
   C_OSCDiagProtocolKfx();
   virtual ~C_OSCDiagProtocolKfx(void);

   //class configuration:
   void SetDispatcher(stw_can::C_CAN_Dispatcher * const opc_Dispatcher);
   void SetCommunicationParameters(const stw_diag_lib::C_KFXCommConfiguration & orc_Config);
   void SetNvmValidFlagUsed(const bool oq_IsUsed);

   //implemented functions from C_OSCDiagProtocolBase:
   virtual stw_types::uint8 GetEndianness(void) const;

   virtual stw_types::sint32 Cycle(void);
   virtual stw_types::sint32 DataPoolReadNumeric(const stw_types::uint8 ou8_DataPoolIndex,
                                                 const stw_types::uint16 ou16_ListIndex,
                                                 const stw_types::uint16 ou16_ElementIndex,
                                                 std::vector<stw_types::uint8> & orc_ReadData,
                                                 stw_types::uint8 * const opu8_NrCode);
   virtual stw_types::sint32 DataPoolReadArray(const stw_types::uint8 ou8_DataPoolIndex,
                                               const stw_types::uint16 ou16_ListIndex,
                                               const stw_types::uint16 ou16_ElementIndex,
                                               std::vector<stw_types::uint8> & orc_ReadData,
                                               stw_types::uint8 * const opu8_NrCode);
   virtual stw_types::sint32 DataPoolWriteNumeric(const stw_types::uint8 ou8_DataPoolIndex,
                                                  const stw_types::uint16 ou16_ListIndex,
                                                  const stw_types::uint16 ou16_ElementIndex,
                                                  const std::vector<stw_types::uint8> & orc_DataToWrite,
                                                  stw_types::uint8 * const opu8_NrCode);
   virtual stw_types::sint32 DataPoolWriteArray(const stw_types::uint8 ou8_DataPoolIndex,
                                                const stw_types::uint16 ou16_ListIndex,
                                                const stw_types::uint16 ou16_ElementIndex,
                                                const std::vector<stw_types::uint8> & orc_DataToWrite,
                                                stw_types::uint8 * const opu8_NrCode);
   virtual stw_types::sint32 DataPoolSetEventDataRate(const stw_types::uint8 ou8_Rail,
                                                      const stw_types::uint16 ou16_IntervalMs);
   virtual stw_types::sint32 DataPoolReadCyclic(const stw_types::uint8 ou8_DataPoolIndex,
                                                const stw_types::uint16 ou16_ListIndex,
                                                const stw_types::uint16 ou16_ElementIndex,
                                                const stw_types::uint8 ou8_Rail, stw_types::uint8 * const opu8_NrCode);
   virtual stw_types::sint32 DataPoolReadChangeDriven(const stw_types::uint8 ou8_DataPoolIndex,
                                                      const stw_types::uint16 ou16_ListIndex,
                                                      const stw_types::uint16 ou16_ElementIndex,
                                                      const stw_types::uint8 ou8_Rail,
                                                      const stw_types::uint32 ou32_Threshold,
                                                      stw_types::uint8 * const opu8_NrCode);
   virtual stw_types::sint32 DataPoolStopEventDriven(void);
   virtual stw_types::sint32 NvmRead(const stw_types::uint32 ou32_MemoryAddress,
                                     std::vector<stw_types::uint8> & orc_DataRecord,
                                     stw_types::uint8 * const opu8_NrCode);
   virtual stw_types::sint32 NvmWriteStartTransaction(const stw_types::uint8 ou8_DataPoolIndex,
                                                      const stw_types::uint16 ou16_NVMAccessCount);
   virtual stw_types::sint32 NvmWrite(const stw_types::uint32 ou32_MemoryAddress,
                                      const std::vector<stw_types::uint8> & orc_DataRecord,
                                      stw_types::uint8 * const opu8_NrCode);
   virtual stw_types::sint32 NvmWriteFinalizeTransaction(void);

   virtual stw_types::sint32 DataPoolReadVersion(const stw_types::uint8 ou8_DataPoolIndex,
                                                 stw_types::uint8(&orau8_Version)[3],
                                                 stw_types::uint8 * const opu8_NrCode);
   virtual stw_types::sint32 DataPoolReadMetaData(const stw_types::uint8 ou8_DataPoolIndex,
                                                  stw_types::uint8(&orau8_Version)[3],
                                                  stw_scl::C_SCLString & orc_Name,
                                                  stw_types::uint8 * const opu8_NrCode);
   virtual stw_types::sint32 NvmNotifyOfChanges(const stw_types::uint8 ou8_DataPoolIndex,
                                                const stw_types::uint8 ou8_ListIndex, bool & orq_ApplicationAcknowledge,
                                                stw_types::uint8 * const opu8_NrCode);

   virtual stw_types::sint32 DataPoolVerify(const stw_types::uint8 ou8_DataPoolIndex,
                                            const stw_types::uint16 ou16_NumberOfDataPoolElements,
                                            const stw_types::uint16 ou16_DataPoolVersion,
                                            const stw_types::uint32 ou32_DataPoolChecksum, bool & orq_Match);

   stw_types::sint32 Logoff(const bool oq_WaitForHandshake);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
