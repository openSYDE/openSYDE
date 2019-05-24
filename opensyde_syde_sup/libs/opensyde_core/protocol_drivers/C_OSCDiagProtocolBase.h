//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Diagnostic protocol driver abstract base class definition

   \class       stw_opensyde_core::C_OSCDiagProtocolBase
   \brief       openSYDE: Diagnostic protocol driver abstract base class definition

   Abstract base class for diagnostic protocol (i.e.: reading/writing of server diagnostic data and parameters)
   - defines interface to specific protocol drivers
   - provides functionality common to all protocol drivers

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDIAGPROTOCOLBASEH
#define C_OSCDIAGPROTOCOLBASEH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//----------------------------------------------------------------------------------------------------------------------
///interface class
class C_OSCDiagProtocolBase
{
private:
   C_OSCDiagProtocolBase(const C_OSCDiagProtocolBase & orc_Source);               //not implemented -> prevent copying
   C_OSCDiagProtocolBase & operator = (const C_OSCDiagProtocolBase & orc_Source); //not implemented -> prevent
                                                                                  // assignment

protected:
   //callbacks for reporting async responses
   //The endianness of reported value is specific to the protocol
   typedef void (* PR_DataPoolReadEventReceived)(void * const opv_Instance, const stw_types::uint8 ou8_DataPoolIndex,
                                                 const stw_types::uint16 ou16_ListIndex,
                                                 const stw_types::uint16 ou16_ElementIndex,
                                                 const std::vector<stw_types::uint8> & orc_Value);
   typedef void (* PR_DataPoolReadEventErrorReceived)(void * const opv_Instance,
                                                      const stw_types::uint8 ou8_DataPoolIndex,
                                                      const stw_types::uint16 ou16_ListIndex,
                                                      const stw_types::uint16 ou16_ElementIndex,
                                                      const stw_types::uint8 ou8_NrCode);

   PR_DataPoolReadEventReceived mpr_OnDataPoolReadEventReceived;
   PR_DataPoolReadEventErrorReceived mpr_OnDataPoolReadEventErrorReceived;
   void * mpv_OnAsyncInstance;

public:
   //endianness types (see GetEndianness())
   static const stw_types::uint8 mhu8_ENDIANNESS_LITTLE = 0U; //aka "intel"
   static const stw_types::uint8 mhu8_ENDIANNESS_BIG = 1U;    //aka "motorola"

   C_OSCDiagProtocolBase(void);
   virtual ~C_OSCDiagProtocolBase(void);

   void Initialize(PR_DataPoolReadEventReceived const opr_DataPoolReadEventReceived,
                   PR_DataPoolReadEventErrorReceived const opr_DataPoolReadEventErrorReceived,
                   void * const opv_Instance);

   //-----------------------------------------------------------------------------
   /*!
      \brief   Get endianness of data returned by and passed to protocol services

      The endianness of data returned by and passed to protocol services depends on
      the protocol used. As we do not know the content of any specific data we cannot swap
      to a defined endianness in the implementation.
      But we can return information about the endianness to the caller.
      The endianness affects binary data passed to and returned by:
      * DataPoolReadNumeric
      * DataPoolReadArray
      * DataPoolWriteNumeric
      * DataPoolWriteArray
      * NvmRead
      * NvmWrite
      * PR_DataPoolReadEventReceived

      Workflow for the application:
      Check GetEndianness() and swap byte order to the client's native byte order before doing anything with the data.

      \return
      endianness (see mhu8_ENDIANNESS_* constants)
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::uint8 GetEndianness(void) const = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Perform cyclic communication tasks

      Invoke protocol's "Cycle" function.
      That function shall:
      * read and handle all incoming service responses
      * invoke async response function pointers for detected OK or error responses

      The function shall check whether another "polled" transfer is already ongoing
       (typically by using a critical section).
      If so, then it shall return without further actions (as "cycling" is in progress anyway)

      \return
      C_NO_ERR   finished cycle
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 Cycle(void) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Read numeric data from server's data pool

      Send request and wait for response.
      This function is used for plain numeric data pool elements (i.e. non-array elements).
      The endianness of read data is specific to the protocol.

      \param[in]  ou8_DataPoolIndex   index of data pool to read from
      \param[in]  ou16_ListIndex      list index to read from
      \param[in]  ou16_ElementIndex   element index to read
      \param[in,out] orc_ReadData     in: size must match expected number of bytes
                                      out: read data
      \param[out]    opu8_NrCode      if != NULL: negative response code in case of an error response

      \return
      C_NO_ERR   request sent, positive response with expected size received
      C_RANGE    parameter out of range (checked by client-side function)
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
      C_RD_WR    malformed protocol response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 DataPoolReadNumeric(const stw_types::uint8 ou8_DataPoolIndex,
                                                 const stw_types::uint16 ou16_ListIndex,
                                                 const stw_types::uint16 ou16_ElementIndex,
                                                 std::vector<stw_types::uint8> & orc_ReadData,
                                                 stw_types::uint8 * const opu8_NrCode) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Read array data from server's data pool

      Send request and wait for response.
      This function is used for array elements.
      The endianness of read data is specific to the protocol.

      \param[in]     ou8_DataPoolIndex   index of data pool to read from
      \param[in]     ou16_ListIndex      list index to read from
      \param[in]     ou16_ElementIndex   element index to read
      \param[in,out] orc_ReadData        in: size must match expected number of bytes
                                         out: read data
      \param[out]    opu8_NrCode      if != NULL: negative response code in case of an error response

      \return
      C_NO_ERR   request sent, positive response with expected size received
      C_RANGE    parameter out of range (checked by client-side function)
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
      C_RD_WR    malformed protocol response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 DataPoolReadArray(const stw_types::uint8 ou8_DataPoolIndex,
                                               const stw_types::uint16 ou16_ListIndex,
                                               const stw_types::uint16 ou16_ElementIndex,
                                               std::vector<stw_types::uint8> & orc_ReadData,
                                               stw_types::uint8 * const opu8_NrCode) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Write numeric data to server's data pool

      Send request and wait for response.
      This function is used for plain numeric data pool elements (i.e. non-array elements).
      The endianness of data to write is specific to the protocol.

      \param[in]  ou8_DataPoolIndex   index of data pool to write to
      \param[in]  ou16_ListIndex      list index to write to
      \param[in]  ou16_ElementIndex   element index to write
      \param[out] orc_DataToWrite     data to write
      \param[out] opu8_NrCode         if != NULL: negative response code in case of an error response

      \return
      C_NO_ERR   request sent, positive response received
      C_RANGE    parameter out of range (checked by client-side function)
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
      C_RD_WR    malformed protocol response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 DataPoolWriteNumeric(const stw_types::uint8 ou8_DataPoolIndex,
                                                  const stw_types::uint16 ou16_ListIndex,
                                                  const stw_types::uint16 ou16_ElementIndex,
                                                  const std::vector<stw_types::uint8> & orc_DataToWrite,
                                                  stw_types::uint8 * const opu8_NrCode) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Write array data to server's data pool

      Send request and wait for response.
      This function is used for array elements.
      The endianness is specific to the protocol.
      The endianness of data to write is specific to the protocol.

      \param[in]  ou8_DataPoolIndex   index of data pool to write to
      \param[in]  ou16_ListIndex      list index to write to
      \param[in]  ou16_ElementIndex   element index to write
      \param[out] orc_DataToWrite     data to write
      \param[out] opu8_NrCode         if != NULL: negative response code in case of an error response

      \return
      C_NO_ERR   request sent, positive response received
      C_RANGE    parameter out of range (checked by client-side function)
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
      C_RD_WR    malformed protocol response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 DataPoolWriteArray(const stw_types::uint8 ou8_DataPoolIndex,
                                                const stw_types::uint16 ou16_ListIndex,
                                                const stw_types::uint16 ou16_ElementIndex,
                                                const std::vector<stw_types::uint8> & orc_DataToWrite,
                                                stw_types::uint8 * const opu8_NrCode) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Set rate of event driven transmissions

      Configure update rate of event driven transmissions in ms.
      Three "rails" can be configured for event driven transmissions.
      Each transmission can be assigned to one of these rails.

      Depending on the protocol capabilities the implementation shall either:
      * configure the rail time of the server driver
      * remember the configured rail time for starting subsequent event driven transmissions

      \param[in]  ou8_Rail            0..2 "rail" to configure
                                      0: slow
                                      1: medium
                                      2: fast
      \param[in]  ou16_IntervalMs     interval to configure (in ms)

      \return
      C_NO_ERR   request sent, positive response received (or: rail value remembered)
      C_RANGE    parameter out of range (checked by client-side function)
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
      C_RD_WR    malformed protocol response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 DataPoolSetEventDataRate(const stw_types::uint8 ou8_Rail,
                                                      const stw_types::uint16 ou16_IntervalMs) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Request cyclic driven transmission of data pool element.

      Request cyclic transmission of data pool element.
      Shall send the request and wait for the initial confirmation (or error) response.

      Data responses shall be handled asynchronously using the Cycle() function and
      be reported via calls to installed function pointer.

      \param[in]  ou8_DataPoolIndex   index of data pool to read from
      \param[in]  ou16_ListIndex      list index to read from
      \param[in]  ou16_ElementIndex   element index to read
      \param[in]  ou8_Rail            0..2   "rail" referring to time interval to use
                                      0: slow
                                      1: medium
                                      2: fast
      \param[out] opu8_NrCode         if != NULL: negative response code in case of an error response

      \return
      C_NO_ERR   request sent
      C_RANGE    parameter out of range (checked by client-side function)
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
      C_RD_WR    malformed protocol response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 DataPoolReadCyclic(const stw_types::uint8 ou8_DataPoolIndex,
                                                const stw_types::uint16 ou16_ListIndex,
                                                const stw_types::uint16 ou16_ElementIndex,
                                                const stw_types::uint8 ou8_Rail,
                                                stw_types::uint8 * const opu8_NrCode) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Request change driven transmission of data pool element.

      Request change driven transmission of data pool element.

      Shall send the request and wait for the initial confirmation (or error) response.

      Data responses shall be handled asynchronously using the Cycle() function and
      be reported via calls to installed function pointer.

      \param[in]  ou8_DataPoolIndex   index of data pool to read from
      \param[in]  ou16_ListIndex      list index to read from
      \param[in]  ou16_ElementIndex   element index to read
      \param[in]  ou8_Rail            0..2   "rail" referring to time interval to use for maximum timeout
                                      0: slow
                                      1: medium
                                      2: fast
      \param[in]  ou32_Threshold      threshold the value must cross before getting sent again
      \param[out] opu8_NrCode         if != NULL: negative response code in case of an error response

      \return
      C_NO_ERR   request sent
      C_RANGE    parameter out of range (checked by client-side function)
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
      C_RD_WR    malformed protocol response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 DataPoolReadChangeDriven(const stw_types::uint8 ou8_DataPoolIndex,
                                                      const stw_types::uint16 ou16_ListIndex,
                                                      const stw_types::uint16 ou16_ElementIndex,
                                                      const stw_types::uint8 ou8_Rail,
                                                      const stw_types::uint32 ou32_Threshold,
                                                      stw_types::uint8 * const opu8_NrCode) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Request stop all change driven transmissions.

      Request stop all event driven transmissions.

      Shall send the request and wait for the confirmation (or error) response.

      \return
      C_NO_ERR   request sent
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
      C_RD_WR    malformed protocol response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 DataPoolStopEventDriven(void) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Read data from NVM

      The endianness of read data is specific to the protocol.

      \param[in]     ou32_MemoryAddress  NVM memory address to read (first read byte)
      \param[in,out] orc_DataRecord      in: size set to number of bytes to read
                                         out: Read data bytes
      \param[out]    opu8_NrCode         if != NULL: negative response code in case of an error response

      \return
      C_NO_ERR   request sent, positive response received
      C_RANGE    parameter out of range (checked by client side)
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
      C_RD_WR    malformed protocol response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 NvmRead(const stw_types::uint32 ou32_MemoryAddress,
                                     std::vector<stw_types::uint8> & orc_DataRecord,
                                     stw_types::uint8 * const opu8_NrCode) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Announce writing data to NVM

      Sequence for writing:
      * NvmWriteStartTransaction()
      * NvmWrite()
      * NvmWriteFinalizeTransaction()

      \param[in]  ou8_DataPoolIndex   Accessed data pool index
      \param[in]  ou16_NVMAccessCount Expected NVM access count

      \return
      C_NO_ERR   request sent, positive response received
      C_RANGE    parameter out of range (checked by client side); access count zero
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
      C_RD_WR    malformed protocol response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 NvmWriteStartTransaction(const stw_types::uint8 ou8_DataPoolIndex,
                                                      const stw_types::uint16 ou16_NVMAccessCount) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Write data to NVM

      The endianness of data to write is specific to the protocol.

      Usage:
      * First call NvmWriteStartTransaction(). (Here you can register any non zero amount of NVM write operations)
      * Execute the specified amount NVM write operations
      * Afterwards call NvmWriteFinalizeTransaction() to complete this transaction.

      \param[in]  ou32_MemoryAddress NVM memory address to write to (first writen byte)
      \param[in]  orc_DataRecord     Data bytes to write
      \param[out] opu8_NrCode        if != NULL: negative response code in case of an error response

      \return
      C_NO_ERR   request sent, positive response received
      C_RANGE    parameter out of range (checked by client side); e.g. data record size zero
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
      C_RD_WR    malformed protocol response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 NvmWrite(const stw_types::uint32 ou32_MemoryAddress,
                                      const std::vector<stw_types::uint8> & orc_DataRecord,
                                      stw_types::uint8 * const opu8_NrCode) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Finalize writing to NVM

      \return
      C_NO_ERR   request sent, positive response received
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
      C_RD_WR    malformed protocol response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 NvmWriteFinalizeTransaction(void) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Read data pool version

      Version format: One byte for Major, Minor, Release

      Example: v1.23r4 in 3 Bytes   -> (0x01, 0x17, 0x04)

      \param[in]  ou8_DataPoolIndex   Data pool index
      \param[out] orau8_Version       Read version; format: see function description

      \return
      C_NO_ERR   request sent, positive response received
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 DataPoolReadVersion(const stw_types::uint8 ou8_DataPoolIndex,
                                                 stw_types::uint8(&orau8_Version)[3]) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Verify data pool consistency

      Used to check whether the referenced data pool matches with the passed criteria.
      Depending on the protocol this can involve verification of:
      * a checksum over data data pool definition
      * the total number of data pool elements
      * the version number of the data pool

      \param[in]  ou8_DataPoolIndex               Data pool index
      \param[in]  ou16_NumberOfDataPoolElements   Number of elements in data pool
      \param[in]  ou16_DataPoolVersion            Version number of data pool
      \param[in]  ou32_DataPoolChecksum           Data pool checksum
      \param[out] orq_Match                       Data pool checksum match response

      \return
      C_NO_ERR   request sent, positive response received
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
      C_RD_WR    malformed protocol response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 DataPoolVerify(const stw_types::uint8 ou8_DataPoolIndex,
                                            const stw_types::uint16 ou16_NumberOfDataPoolElements,
                                            const stw_types::uint16 ou16_DataPoolVersion,
                                            const stw_types::uint32 ou32_DataPoolChecksum, bool & orq_Match) = 0;

   //-----------------------------------------------------------------------------
   /*!
      \brief   Notify server application about NVM data changes

      Informs the server application of changes in the server's NVM.
      The server responds with an information showing whether it acknowledges the information
       positively or negatively.

      \param[in]  ou8_DataPoolIndex           Data pool index
      \param[in]  ou8_ListIndex               List index
      \param[out] orq_ApplicationAcknowledge  true: positive acknowledge from server
                                              false: negative acknowledge from server
      \param[out] opu8_NrCode                 if != NULL: negative response code in case of an error response

      \return
      C_NO_ERR   request sent, positive response received
      C_TIMEOUT  expected response not received within timeout
      C_NOACT    could not send request (e.g. TX buffer full)
      C_CONFIG   pre-requisites not correct; e.g. driver not initialized
      C_WARN     error response
      C_RD_WR    malformed protocol response
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::sint32 NvmNotifyOfChanges(const stw_types::uint8 ou8_DataPoolIndex,
                                                const stw_types::uint8 ou8_ListIndex, bool & orq_ApplicationAcknowledge,
                                                stw_types::uint8 * const opu8_NrCode) = 0;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
