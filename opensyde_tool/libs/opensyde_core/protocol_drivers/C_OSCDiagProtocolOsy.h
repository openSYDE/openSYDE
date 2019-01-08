//-----------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Diagnostic protocol driver for openSYDE protocol

   \class       stw_opensyde_core::C_OSCDiagProtocolOsy
   \brief       openSYDE: Diagnostic protocol driver for openSYDE protocol

   Implements openSYDE protocol.

   Concept for handling of incoming responses when waiting for a specific service response:
   - non event-driven responses not matching the expected service:
     - unexpected service will be dumped ...
     - unexpected length will be dumped
     - unexpected content will lead to error
   - event-driven responses will be reported via function pointers (see Cycle())

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     02.03.2017  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCDIAGPROTOCOLOSYH
#define C_OSCDIAGPROTOCOLOSYH

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "C_OSCDiagProtocolBase.h"
#include "C_OSCProtocolDriverOsy.h"
#include "CSCLString.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

//-----------------------------------------------------------------------------
///interface class
class C_OSCDiagProtocolOsy :
   public C_OSCDiagProtocolBase,
   public C_OSCProtocolDriverOsy
{
private:
   C_OSCDiagProtocolOsy(const C_OSCDiagProtocolOsy & orc_Souce);               //not implemented -> prevent copying
   C_OSCDiagProtocolOsy & operator = (const C_OSCDiagProtocolOsy & orc_Souce); //not implemented -> prevent assignment

protected:
   //implemented function from C_OSCProtocolDriverOsy:
   virtual void m_OsyReadDataPoolDataEventReceived(const stw_types::uint8 ou8_DataPoolIndex,
                                                   const stw_types::uint16 ou16_ListIndex,
                                                   const stw_types::uint16 ou16_ElementIndex,
                                                   const std::vector<stw_types::uint8> & orc_Value);
   virtual void m_OsyReadDataPoolDataEventErrorReceived(const stw_types::uint8 ou8_DataPoolIndex,
                                                        const stw_types::uint16 ou16_ListIndex,
                                                        const stw_types::uint16 ou16_ElementIndex,
                                                        const stw_types::uint8 ou8_NrCode);

public:
   C_OSCDiagProtocolOsy(void);
   virtual ~C_OSCDiagProtocolOsy(void);

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
                                                 stw_types::uint8(&orau8_Version)[3]);
   virtual stw_types::sint32 DataPoolVerify(const stw_types::uint8 ou8_DataPoolIndex,
                                            const stw_types::uint16 ou16_NumberOfDataPoolElements,
                                            const stw_types::uint16 ou16_DataPoolVersion,
                                            const stw_types::uint32 ou32_DataPoolChecksum, bool & orq_Match);
   virtual stw_types::sint32 NvmNotifyOfChanges(const stw_types::uint8 ou8_DataPoolIndex,
                                                const stw_types::uint8 ou8_ListIndex, bool & orq_ApplicationAcknowledge,
                                                stw_types::uint8 * const opu8_NrCode);
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
