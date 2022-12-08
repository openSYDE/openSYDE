//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Diagnostic protocol driver for openSYDE protocol

   \class       stw::opensyde_core::C_OscDiagProtocolOsy
   \brief       openSYDE: Diagnostic protocol driver for openSYDE protocol

   Implements openSYDE protocol.

   Concept for handling of incoming responses when waiting for a specific service response:
   - non event-driven responses not matching the expected service:
     - unexpected service will be dumped ...
     - unexpected length will be dumped
     - unexpected content will lead to error
   - event-driven responses will be reported via function pointers (see Cycle())

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDIAGPROTOCOLOSYH
#define C_OSCDIAGPROTOCOLOSYH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OscDiagProtocolBase.hpp"
#include "C_OscProtocolDriverOsy.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//----------------------------------------------------------------------------------------------------------------------
///interface class
class C_OscDiagProtocolOsy :
   public C_OscDiagProtocolBase,
   public C_OscProtocolDriverOsy
{
private:
   //not implemented -> prevent copying
   C_OscDiagProtocolOsy(const C_OscDiagProtocolOsy & orc_Source);
   //not implemented -> prevent assignment
   C_OscDiagProtocolOsy & operator = (const C_OscDiagProtocolOsy & orc_Source); //lint !e1511 //we want to hide the base
                                                                                // function

protected:
   //implemented function from C_OscProtocolDriverOsy:
   virtual void m_OsyReadDataPoolDataEventReceived(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                                   const uint16_t ou16_ElementIndex,
                                                   const std::vector<uint8_t> & orc_Value);
   virtual void m_OsyReadDataPoolDataEventErrorReceived(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                                        const uint16_t ou16_ElementIndex, const uint8_t ou8_NrCode);

public:
   C_OscDiagProtocolOsy(void);
   virtual ~C_OscDiagProtocolOsy(void);

   //implemented functions from C_OscDiagProtocolBase:
   virtual uint8_t GetEndianness(void) const;

   //this is intended to implement the pure virtual function from C_OscDiagProtocolBase
   virtual int32_t Cycle(void); //lint !e1511

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
   virtual int32_t DataPoolVerify(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_NumberOfDataPoolElements,
                                  const uint16_t ou16_DataPoolVersion, const uint32_t ou32_DataPoolChecksum,
                                  bool & orq_Match);
   virtual int32_t NvmNotifyOfChanges(const uint8_t ou8_DataPoolIndex, const uint8_t ou8_ListIndex,
                                      bool & orq_ApplicationAcknowledge, uint8_t * const opu8_NrCode);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
