//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       openSYDE: Diagnostic protocol driver for openSYDE protocol

   For details cf. documentation in .h file.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     02.03.2017  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCDiagProtocolOsy.h"
#include "TGLTime.h"
#include "TGLUtils.h"
#include "CSCLString.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_scl;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Handle notification about incoming response to event driven data pool reading

   Translates the openSYDE protocol specific notification to a generic notification.
   Invokes the notification function of C_OSCDiagProtocolBase to let the application know we have new data.

   \param[in]     ou8_DataPoolIndex    Datapool index
   \param[in]     ou16_ListIndex       List index
   \param[in]     ou16_ElementIndex    Element index
   \param[in]     orc_Value            Value of element stored in uint8 vector

   \created     14.07.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void C_OSCDiagProtocolOsy::m_OsyReadDataPoolDataEventReceived(const uint8 ou8_DataPoolIndex,
                                                              const uint16 ou16_ListIndex,
                                                              const uint16 ou16_ElementIndex,
                                                              const std::vector<uint8> & orc_Value)
{
   if (mpr_OnDataPoolReadEventReceived != NULL)
   {
      mpr_OnDataPoolReadEventReceived(mpv_OnAsyncInstance, ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex,
                                      orc_Value);
   }
   else
   {
      m_LogErrorWithHeader("Asynchronous communication",
                           "Async ReadDataPool response received, but no handler is installed. Ignoring.",
                           TGL_UTIL_FUNC_ID);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle notification about incoming negative response to event driven data pool reading

   Translates the openSYDE protocol specific notification to a generic notification.
   Invokes the notification function of C_OSCDiagProtocolBase to let the application know we have new data.

   \param[in]  ou8_DataPoolIndex    Datapool index
   \param[in]  ou16_ListIndex       List index
   \param[in]  ou16_ElementIndex    Element index
   \param[in]  ou8_NrCode           Negative response code

   \created     21.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCDiagProtocolOsy::m_OsyReadDataPoolDataEventErrorReceived(const uint8 ou8_DataPoolIndex,
                                                                   const uint16 ou16_ListIndex,
                                                                   const uint16 ou16_ElementIndex,
                                                                   const uint8 ou8_NrCode)
{
   if (mpr_OnDataPoolReadEventErrorReceived != NULL)
   {
      mpr_OnDataPoolReadEventErrorReceived(mpv_OnAsyncInstance, ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex,
                                           ou8_NrCode);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   constructor

   Set up class

   \created     21.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OSCDiagProtocolOsy::C_OSCDiagProtocolOsy(void) :
   C_OSCDiagProtocolBase(),
   C_OSCProtocolDriverOsy()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   constructor

   Tear down class

   \created     17.03.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_OSCDiagProtocolOsy::~C_OSCDiagProtocolOsy(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get endianness of data returned by and passed to protocol services

   Here: return "big"

   \return
   mhu8_ENDIANNESS_BIG
*/
//-----------------------------------------------------------------------------
uint8 C_OSCDiagProtocolOsy::GetEndianness(void) const
{
   return mhu8_ENDIANNESS_BIG;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Perform cyclic communication tasks

   Invoke protocol's "Cycle" function.

   \return
   C_NO_ERR   at least one service received
   C_CONFIG   transport protocol not installed

   \created     15.03.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::Cycle(void)
{
   return C_OSCProtocolDriverOsy::Cycle();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Read numeric data from server's data pool

   Send request and wait for response.
   In the openSYDE protocol the implementation is the same for array and numeric data.

   \param[in]  ou8_DataPoolIndex   index of data pool to read from
   \param[in]  ou16_ListIndex      list index to read from
   \param[in]  ou16_ElementIndex   element index to read
   \param[in,out] orc_ReadData     in: size of data to read
                                   out: read data
   \param[out]    opu8_NrCode      if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response with expected size received
   C_RANGE    data pool, list, element index out of range (checked by client-side function)
   C_NOACT    could not send request (TX buffer full)
   C_CONFIG   transport protocol not installed
   C_WARN     error response
   C_RD_WR    malformed protocol response
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::DataPoolReadNumeric(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                 const uint16 ou16_ElementIndex, std::vector<uint8> & orc_ReadData,
                                                 uint8 * const opu8_NrCode)
{
   //in this protocol: no need to differentiate between numeric and array ...
   return this->DataPoolReadArray(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, orc_ReadData, opu8_NrCode);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Read array data from server's data pool

   Send request and wait for response.
   In the openSYDE protocol the implementation is the same for array and numeric data.

   \param[in]     ou8_DataPoolIndex   index of data pool to read from
   \param[in]     ou16_ListIndex      list index to read from
   \param[in]     ou16_ElementIndex   element index to read
   \param[in,out] orc_ReadData        in: size of data to read
                                      out: read data
   \param[out]    opu8_NrCode         if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response with expected size received
   C_RANGE    data pool, list, element index out of range (checked by client-side function)
   C_NOACT    could not send request (TX buffer full)
   C_CONFIG   transport protocol not installed
   C_WARN     error response
   C_RD_WR    malformed protocol response (e.g. reported size does not match expected size)
   C_COM      communication driver reported error
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::DataPoolReadArray(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                               const uint16 ou16_ElementIndex, std::vector<uint8> & orc_ReadData,
                                               uint8 * const opu8_NrCode)
{
   sint32 s32_Return;

   uint32 u32_ExpectedSize = orc_ReadData.size();

   s32_Return = this->OsyReadDataPoolData(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, orc_ReadData,
                                          opu8_NrCode);
   if (orc_ReadData.size() != u32_ExpectedSize)
   {
      s32_Return = C_RD_WR;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Write numeric data to server's data pool

   Send request and wait for response.

   \param[in]  ou8_DataPoolIndex   index of data pool to read from
   \param[in]  ou16_ListIndex      list index to read from
   \param[in]  ou16_ElementIndex   element index to read
   \param[out] orc_DataToWrite     data to write
   \param[out] opu8_NrCode         if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_RANGE    data pool, list, element index out of range (checked by client-side function)
   C_NOACT    could not send request (TX buffer full)
   C_CONFIG   transport protocol not installed
   C_WARN     error response
   C_RD_WR    malformed protocol response
   C_COM      communication driver reported error
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::DataPoolWriteNumeric(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                  const uint16 ou16_ElementIndex,
                                                  const std::vector<uint8> & orc_DataToWrite, uint8 * const opu8_NrCode)
{
   return this->OsyWriteDataPoolData(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex,
                                     orc_DataToWrite, opu8_NrCode);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Write array data to server's data pool

   Send request and wait for response.
   In the openSYDE protocol this results in the same protocol service as writing numeric data.

   \param[in]  ou8_DataPoolIndex   index of data pool to write to
   \param[in]  ou16_ListIndex      list index to write to
   \param[in]  ou16_ElementIndex   element index to write
   \param[out] orc_DataToWrite     data to write
   \param[out] opu8_NrCode         if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_RANGE    data pool, list, element index out of range (checked by client-side function)
   C_NOACT    could not send request (TX buffer full)
   C_CONFIG   transport protocol not installed
   C_WARN     error response
   C_RD_WR    malformed protocol response
   C_COM      communication driver reported error
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::DataPoolWriteArray(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                const uint16 ou16_ElementIndex,
                                                const std::vector<uint8> & orc_DataToWrite, uint8 * const opu8_NrCode)
{
   return this->DataPoolWriteNumeric(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, orc_DataToWrite,
                                     opu8_NrCode);
}

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
   \param[in]  ou16_IntervalMs     interval to configure (in ms)

   \return
   C_NO_ERR   request sent, positive response received (or: rail value remembered)
   C_RANGE    parameter out of range (checked by client-side function)
   C_NOACT    could not send request (e.g. TX buffer full)
   C_CONFIG   pre-requisites not correct; e.g. driver not initialized
   C_WARN     error response
   C_RD_WR    malformed protocol response
   C_COM      communication driver reported error
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::DataPoolSetEventDataRate(const uint8 ou8_Rail, const uint16 ou16_IntervalMs)
{
   return this->OsyWriteDataPoolEventDataRate(ou8_Rail, ou16_IntervalMs);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Request cyclic driven transmission of data pool element.

   Request cyclic transmission of data pool element.
   Shall send the request and wait for the initial confirmation (or error) response.

   Data responses shall be handled asynchronously using the Cycle() function and
   be reported via calls to m_DataPoolReadEventReceived().

   \param[in]  ou8_DataPoolIndex   index of data pool to read from
   \param[in]  ou16_ListIndex      list index to read from
   \param[in]  ou16_ElementIndex   element index to read
   \param[in]  ou8_Rail            0..2   "rail" referring to time interval to use
   \param[out] opu8_NrCode         if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent
   C_RANGE    parameter out of range (checked by client-side function)
   C_NOACT    could not send request (e.g. TX buffer full)
   C_CONFIG   pre-requisites not correct; e.g. driver not initialized
   C_WARN     error response
   C_RD_WR    malformed protocol response
   C_COM      communication driver reported error
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::DataPoolReadCyclic(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                const uint16 ou16_ElementIndex, const uint8 ou8_Rail,
                                                uint8 * const opu8_NrCode)
{
   return this->OsyReadDataPoolDataCyclic(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, ou8_Rail, opu8_NrCode);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Request change driven transmission of data pool element.

   Request change driven transmission of data pool element.

   Shall send the request and wait for the initial confirmation (or error) response.

   Data responses shall be handled asynchronously using the Cycle() function and
   be reported via calls to m_DataPoolReadEventReceived().

   \param[in]  ou8_DataPoolIndex   index of data pool to read from
   \param[in]  ou16_ListIndex      list index to read from
   \param[in]  ou16_ElementIndex   element index to read
   \param[in]  ou8_Rail            0..2   "rail" referring to time interval to use for maximum timeout
   \param[in]  ou32_Threshold      threshold the value must cross before getting sent again
   \param[out] opu8_NrCode         if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent
   C_RANGE    parameter out of range (checked by client-side function)
   C_NOACT    could not send request (e.g. TX buffer full)
   C_CONFIG   pre-requisites not correct; e.g. driver not initialized
   C_WARN     error response
   C_RD_WR    malformed protocol response
   C_COM      communication driver reported error
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::DataPoolReadChangeDriven(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                      const uint16 ou16_ElementIndex, const uint8 ou8_Rail,
                                                      const uint32 ou32_Threshold, uint8 * const opu8_NrCode)
{
   return this->OsyReadDataPoolDataChangeDriven(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, ou8_Rail,
                                                ou32_Threshold, opu8_NrCode);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Request stop all change driven transmissions.

   Request stop all change driven transmissions.

   Shall send the request and wait for the initial confirmation (or error) response.

   \return
   C_NO_ERR   request sent
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send request (e.g. TX buffer full)
   C_CONFIG   pre-requisites not correct; e.g. driver not initialized
   C_WARN     error response
   C_RD_WR    malformed protocol response
   C_COM      communication driver reported error
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::DataPoolStopEventDriven(void)
{
   return this->OsyStopDataPoolEvents();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Request cyclic driven transmission of NVM content.

   \param[in]     ou32_MemoryAddress NVM memory address to read (first read byte)
   \param[in,out] orc_DataRecord     in: size defines number of data bytes to read
                                     out: read data bytes
   \param[out]    opu8_NrCode        if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in TX queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response
   C_COM      expected server response not received because of communication error

   \created     17.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::NvmRead(const uint32 ou32_MemoryAddress, std::vector<uint8> & orc_DataRecord,
                                     uint8 * const opu8_NrCode)
{
   sint32 s32_Retval;

   s32_Retval = this->OsyReadMemoryByAddress(ou32_MemoryAddress, orc_DataRecord, opu8_NrCode);

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start NVM write sequence

   For the openSYDE protocol there's nothing to do ...

   \param[in]  ou8_DataPoolIndex   Accessed data pool index
   \param[in]  ou16_NVMAccessCount Expected NVM access count

   \return
   C_NO_ERR   write sequence started

   \created     17.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::NvmWriteStartTransaction(const uint8 ou8_DataPoolIndex, const uint16 ou16_NVMAccessCount)
{
   (void)ou8_DataPoolIndex;
   (void)ou16_NVMAccessCount;
   return C_NO_ERR;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Write to NVM

   Usage:
   * First call NvmWriteStartTransaction() (Here you can register any non zero amount of NVM write operations)
   * Execute the specified amount NVM write operations
   * Afterwards call NvmWriteFinalizeTransaction() to complete this transaction.

   \param[in]  ou32_MemoryAddress NVM memory address to write to (first written byte)
   \param[in]  orc_DataRecord     Data bytes to write
   \param[out] opu8_NrCode        if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in TX queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response
   C_RD_WR    unexpected content in response
   C_RANGE    orc_DataRecord empty
   C_COM      communication driver reported error

   \created     17.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::NvmWrite(const uint32 ou32_MemoryAddress, const std::vector<uint8> & orc_DataRecord,
                                      uint8 * const opu8_NrCode)
{
   sint32 s32_Retval;

   s32_Retval = this->OsyWriteMemoryByAddress(ou32_MemoryAddress, orc_DataRecord, opu8_NrCode);

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   NVM write operation finalization

   For the openSYDE protocol there's nothing to do ...

   \return
   C_NO_ERR   no problem

   \created     17.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::NvmWriteFinalizeTransaction(void)
{
   return C_NO_ERR;
}

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
   C_NOACT    could not put request in TX queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response
   C_COM      communication driver reported error

   \created     17.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::DataPoolReadVersion(const uint8 ou8_DataPoolIndex, stw_types::uint8 (&orau8_Version)[3])
{
   sint32 s32_Retval;
   C_DataPoolMetaData c_MetaData;

   s32_Retval = this->OsyReadDataPoolMetaData(ou8_DataPoolIndex, c_MetaData);

   if (s32_Retval == C_NO_ERR)
   {
      orau8_Version[0] = c_MetaData.au8_Version[0];
      orau8_Version[1] = c_MetaData.au8_Version[1];
      orau8_Version[2] = c_MetaData.au8_Version[2];
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Verify data pool consistency

   Used to check whether the referenced data pool matches with the passed criteria.
   In the openSYDE protocol we verify:
   * a checksum over data data pool definition

   \param[in]  ou8_DataPoolIndex              Data pool index
   \param[in]  ou16_NumberOfDataPoolElements  number of elements in data pool (not used in this protocol)
   \param[in]  ou16_DataPoolVersion           version number of data pool (not used in this protocol)
   \param[in]  ou32_DataPoolChecksum          Data pool checksum
   \param[out] orq_Match                      Data pool checksum match response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in TX queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response
   C_RD_WR    unexpected content in response (here: wrong data pool index)
   C_COM      communication driver reported error

   \created     17.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::DataPoolVerify(const uint8 ou8_DataPoolIndex, const uint16 ou16_NumberOfDataPoolElements,
                                            const uint16 ou16_DataPoolVersion, const uint32 ou32_DataPoolChecksum,
                                            bool & orq_Match)
{
   sint32 s32_Retval;

   (void)ou16_NumberOfDataPoolElements; //not supported by this protocol (checksum should be good enough)
   (void)ou16_DataPoolVersion;          //not supported by this protocol (checksum should be good enough)

   s32_Retval = this->OsyVerifyDataPool(ou8_DataPoolIndex, ou32_DataPoolChecksum, orq_Match);

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Notify NVM data changes

   \param[in]  ou8_DataPoolIndex           Data pool index
   \param[in]  ou8_ListIndex               List index
   \param[out] orq_ApplicationAcknowledge  true: positive acknowledge from server
                                           false: negative acknowledge from server
   \param[out] opu8_NrCode                 if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not put request in TX queue ...
   C_CONFIG   no transport protocol installed
   C_WARN     error response
   C_RD_WR    unexpected content in response (here: wrong data pool or list index)
   C_COM      communication driver reported error

   \created     17.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolOsy::NvmNotifyOfChanges(const uint8 ou8_DataPoolIndex, const uint8 ou8_ListIndex,
                                                bool & orq_ApplicationAcknowledge, uint8 * const opu8_NrCode)
{
   sint32 s32_Retval;

   s32_Retval =
      this->OsyNotifyNvmDataChanges(ou8_DataPoolIndex, ou8_ListIndex, orq_ApplicationAcknowledge, opu8_NrCode);

   return s32_Retval;
}
//---------------------------------------------------------------------------
