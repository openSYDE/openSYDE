//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Diagnostic protocol driver for KEFEX protocol

   For details cf. documentation in .h file.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCDiagProtocolKfx.h"
#include "CSCLString.h"
#include "TGLUtils.h"

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

uint16 C_OSCDiagProtocolKfx::mh_PackDataPoolIndex(const uint16 ou16_List, const uint16 ou16_Variable)
{
   return ((static_cast<uint16>(ou16_List << 10U)) | (ou16_Variable & 0x03FFU));
}

//-----------------------------------------------------------------------------

void C_OSCDiagProtocolKfx::mh_UnpackDataPoolIndex(const uint16 ou16_Index, uint16 & oru16_List, uint16 & oru16_Variable)
{
   oru16_List    = static_cast<uint16>((ou16_Index & 0xFC00U) >> 10);
   oru16_Variable = static_cast<uint16>(ou16_Index & 0x03FFU);
}

//-----------------------------------------------------------------------------
/*! \brief   Handling of ECRR/TCRR reception

   Called by KEFEX protocol implementation when a response to a
   previously requested ECRR or TCRR request was received.

   \param[in]     opv_Instance         Pointer to the instance of C_OSCDiagProtocolKfx that we installed in constructor
   \param[in]     ou32_Index           packed variable index
   \param[in]     os64_Value           reported variable value (if oq_Error is true this contains the error code)
   \param[in]     ou32_TimeStamp       timestamp of reported value
   \param[in]     oq_IsTimeStamped     true: response contained timestamp
   \param[in]     oq_Error             true: error response; false: data response
*/
//-----------------------------------------------------------------------------
void C_OSCDiagProtocolKfx::mh_CyclicResponseReceived(void * const opv_Instance, const uint32 ou32_Index,
                                                     const sint64 os64_Value, const uint32 ou32_TimeStamp,
                                                     const bool oq_IsTimeStamped, const bool oq_Error)
{
   //lint -e{925}  We know that we registered; this cannot fail
   C_OSCDiagProtocolKfx * const pc_MeMyselfAndI = reinterpret_cast<C_OSCDiagProtocolKfx * const>(opv_Instance);

   tgl_assert(pc_MeMyselfAndI != NULL);
   if (pc_MeMyselfAndI != NULL)
   {
      pc_MeMyselfAndI->m_CyclicResponseReceived(ou32_Index, os64_Value, ou32_TimeStamp, oq_IsTimeStamped, oq_Error);
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Handling an ECRR/TCRR reception

   Called by KEFEX protocol implementation when a response to a
   previously requested ECRR or TCRR request was received.

   We report this on to the application if it is interested.

   \param[in]     ou32_Index           packed variable index
   \param[in]     os64_Value           reported variable value (if oq_Error is true this contains the error code)
   \param[in]     ou32_TimeStamp       timestamp of reported value (ignored by us ...)
   \param[in]     oq_IsTimeStamped     true: response contained timestamp (ignored by us ...)
   \param[in]     oq_Error             true: error response; false: data response
*/
//-----------------------------------------------------------------------------
void C_OSCDiagProtocolKfx::m_CyclicResponseReceived(const uint32 ou32_Index, const sint64 os64_Value,
                                                    const uint32 ou32_TimeStamp, const bool oq_IsTimeStamped,
                                                    const bool oq_Error)
{
   uint16 u16_List;
   uint16 u16_Variable;

   (void)oq_IsTimeStamped;
   (void)ou32_TimeStamp;

   tgl_assert(ou32_Index <= 0xFFFF); //this should be true for any KEFEX transmission

   mh_UnpackDataPoolIndex(static_cast<uint16>(ou32_Index), u16_List, u16_Variable);

   if (oq_Error == true)
   {
      if (mpr_OnDataPoolReadEventErrorReceived != NULL)
      {
         mpr_OnDataPoolReadEventErrorReceived(mpv_OnAsyncInstance, 0U, u16_List, u16_Variable,
                                              static_cast<uint8>(os64_Value));
      }
      else
      {
         osc_write_log_warning("Asynchronous communication",
                               "C_OSCDiagProtocolKfx: Cyclic error response received but no handler installed !");
      }
   }
   else
   {
      if (mpr_OnDataPoolReadEventReceived != NULL)
      {
         //convert value; we don't know the size of the element; we know the maximum is 32bits so always return those
         const uint32 u32_Value = static_cast<uint32>(os64_Value);
         std::vector<uint8> c_Value;
         c_Value.resize(4);
         c_Value[0] = static_cast<uint8>(u32_Value);
         c_Value[1] = static_cast<uint8>(u32_Value >> 8U);
         c_Value[2] = static_cast<uint8>(u32_Value >> 16U);
         c_Value[3] = static_cast<uint8>(u32_Value >> 24U);
         mpr_OnDataPoolReadEventReceived(mpv_OnAsyncInstance, 0U, u16_List, u16_Variable, c_Value);
      }
      else
      {
         osc_write_log_warning("Asynchronous communication",
                               "C_OSCDiagProtocolKfx: Cyclic data response received but no handler installed !");
      }
   }
}

//-----------------------------------------------------------------------------
/*! \brief   constructor

   Set up class
*/
//-----------------------------------------------------------------------------
C_OSCDiagProtocolKfx::C_OSCDiagProtocolKfx(void) :
   C_OSCDiagProtocolBase(),
   mq_HasDispatcher(false),
   mq_NvmValidFlagIsUsed(false),
   mu16_CommTimeoutMs(1000)
{
   mau16_EventRailIntervalsMs[0] = 100U;
   mau16_EventRailIntervalsMs[1] = 500U;
   mau16_EventRailIntervalsMs[2] = 1000U;

   mpc_CommKefex = new stw_diag_lib::C_KFXCommunicationKEFEX(true);
   //register us as recipient of cyclic transmission callbacks:
   mpc_CommKefex->InstallCyclicTransmissionCallback(&mh_CyclicResponseReceived, this);
}

//-----------------------------------------------------------------------------
/*! \brief   destructor

   Tear down class
*/
//-----------------------------------------------------------------------------
C_OSCDiagProtocolKfx::~C_OSCDiagProtocolKfx(void)
{
   delete mpc_CommKefex;
   mpc_CommKefex = NULL;
}

//-----------------------------------------------------------------------------
/*! \brief   Make communication dispatcher to use known

   Pass dispatcher down to KEFEX protocol driver class and set RX filters.

   \param[in]    opc_Dispatcher  CAN dispatcher to use for communication
*/
//-----------------------------------------------------------------------------
void C_OSCDiagProtocolKfx::SetDispatcher(stw_can::C_CAN_Dispatcher * const opc_Dispatcher)
{
   stw_diag_lib::C_KFXCommConfiguration c_Config;
   this->mpc_CommKefex->SetCommDispatcher(opc_Dispatcher);
   //the protocol driver does not automatically apply the RX filters when setting a new dispatcher
   //-> trigger manually by "re-setting" parameters
   if (opc_Dispatcher == NULL)
   {
      (void)this->mpc_CommKefex->GetConfig(&c_Config); //can only fail if pointer is invalid; ignore result
      (void)this->mpc_CommKefex->SetConfig(&c_Config); //assume the read data is valid ...
      mq_HasDispatcher = false;
   }
   else
   {
      mq_HasDispatcher = true;
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Configure communication parameters

   Pass parameters down to KEFEX protocol driver class.
   Will update the RX filters of the installed CAN dispatcher.

   \param[in]    orc_Config   communication parameters
*/
//-----------------------------------------------------------------------------
void C_OSCDiagProtocolKfx::SetCommunicationParameters(const stw_diag_lib::C_KFXCommConfiguration & orc_Config)
{
   //returned errors are only checking for valid pointer
   (void)this->mpc_CommKefex->SetConfig(&orc_Config);

   //there are a few services we need to poll ourselves; so remember the timeout here as well
   mu16_CommTimeoutMs = orc_Config.GetTimeout();
}

//-----------------------------------------------------------------------------
/*! \brief   Configure whether the NvmWrite Start and Finalize services are required

   Configure whether the NvmWriteStart and NvmWriteFinalize services need to be executed for the target.
   If set to "true" the service functions will try to execute the services.
   If set to "false" the service functions will simply return C_NO_ERR.

   This setting is required as those two services have been added to the protocol after its initial creation and
    are only supported on newer target driver's.

   \param[in]    oq_IsUsed   on/off (see function description)
*/
//-----------------------------------------------------------------------------
void C_OSCDiagProtocolKfx::SetNvmValidFlagUsed(const bool oq_IsUsed)
{
   mq_NvmValidFlagIsUsed = oq_IsUsed;
}

//-----------------------------------------------------------------------------
/*! \brief   Get endianness of data returned by and passed to protocol services

   Here: return "little"

   \return
   mhu8_ENDIANNESS_LITTLE
*/
//-----------------------------------------------------------------------------
uint8 C_OSCDiagProtocolKfx::GetEndianness(void) const
{
   return mhu8_ENDIANNESS_LITTLE;
}

//-----------------------------------------------------------------------------
/*! \brief   Perform cyclic communication tasks

   Invoke protocol's "Cycle" function.
   * read and handle all incoming service responses
   * invoke async response function pointers for detected OK or error responses

   \return
   C_NO_ERR   finished cycle
   C_CONFIG   CAN dispatcher not installed
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::Cycle(void)
{
   sint32 s32_Return = C_CONFIG;

   if (mq_HasDispatcher == true)
   {
      this->mpc_CommKefex->Cycle(); //responses will be reported through installed function pointers
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Read numeric data from server's data pool

   Send request and wait for response.
   This function is used for plain numeric data pool elements (i.e. non-array elements).

   \param[in]  ou8_DataPoolIndex   index of data pool to read from
   \param[in]  ou16_ListIndex      list index to read from
   \param[in]  ou16_ElementIndex   element index to read
   \param[in,out] orc_ReadData     in: size must match expected number of bytes
                                   out: read data (in protocol-specific endianness)
   \param[out]    opu8_NrCode      if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_RANGE    parameter out of range (checked by client-side function):
              - data pool index != 0
              - list index > 63
              - element index > 1023
              - expected size not 1,2,4,8 (valid sizes of numeric elements)
   C_TIMEOUT  expected response not received within timeout (for elements <= 4 bytes)
   C_NOACT    could not send request (TX error or no CAN dispatcher installed)
              expected response not received within timeout (for elements with 8 bytes size)
   C_CONFIG   segmented transfer required but BSmax set to zero; communication already in use by another thread
              CAN dispatcher not installed
   C_WARN     error response
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::DataPoolReadNumeric(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                 const uint16 ou16_ElementIndex, std::vector<uint8> & orc_ReadData,
                                                 uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;

   if (mq_HasDispatcher == false)
   {
      s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
   }
   else
   {
      if ((ou8_DataPoolIndex == 0U) && (ou16_ListIndex < mhu16_KFX_RAM_MAX_NUM_LISTS) &&
          (ou16_ElementIndex < mhu16_KFX_RAM_MAX_NUM_ELEMENTS) &&
          ((orc_ReadData.size() == 1U) || (orc_ReadData.size() == 2U) || (orc_ReadData.size() == 4U) ||
           (orc_ReadData.size() == 8U)))
      {
         const uint16 u16_Index = mh_PackDataPoolIndex(ou16_ListIndex, ou16_ElementIndex);
         sint64 s64_Value;

         s32_Return = this->mpc_CommKefex->ReadNumericVariable(u16_Index,
                                                               static_cast<uint8>(orc_ReadData.size()), s64_Value);
         //map KEFEX to openSYDE error codes as good as possible:
         switch (s32_Return)
         {
         case C_NO_ERR:
            //copy s64_Value to orc_ReadData:
            switch (orc_ReadData.size())
            {
            case 1:
               orc_ReadData[0] = static_cast<uint8>(s64_Value);
               break;
            case 2:
               orc_ReadData[0] = static_cast<uint8>(s64_Value);
               orc_ReadData[1] = static_cast<uint8>(static_cast<uint64>(s64_Value) >> 8);
               break;
            case 4:
               orc_ReadData[0] = static_cast<uint8>(s64_Value);
               orc_ReadData[1] = static_cast<uint8>(static_cast<uint64>(s64_Value) >> 8);
               orc_ReadData[2] = static_cast<uint8>(static_cast<uint64>(s64_Value) >> 16);
               orc_ReadData[3] = static_cast<uint8>(static_cast<uint64>(s64_Value) >> 24);
               break;
            case 8:
               orc_ReadData[0] = static_cast<uint8>(s64_Value);
               orc_ReadData[1] = static_cast<uint8>(static_cast<uint64>(s64_Value) >> 8);
               orc_ReadData[2] = static_cast<uint8>(static_cast<uint64>(s64_Value) >> 16);
               orc_ReadData[3] = static_cast<uint8>(static_cast<uint64>(s64_Value) >> 24);
               orc_ReadData[4] = static_cast<uint8>(static_cast<uint64>(s64_Value) >> 32);
               orc_ReadData[5] = static_cast<uint8>(static_cast<uint64>(s64_Value) >> 40);
               orc_ReadData[6] = static_cast<uint8>(static_cast<uint64>(s64_Value) >> 48);
               orc_ReadData[7] = static_cast<uint8>(static_cast<uint64>(s64_Value) >> 56);
               break;
            default:
               break; //checked at beginning of function
            }
            break;
         case C_RD_WR: //could not send request; no dispatcher installed; wrong size in response;
                       // timeout of segmented transfer (no way to differentiate)
            s32_Return = C_NOACT;
            break;
         case C_COM: //no response of expedited transfer; transport protocol violation;
                     // wrong index in response (no way to differentiate)
            s32_Return = C_TIMEOUT;
            break;
         case C_WARN: //error response
            //TODO: Convert to OSY response codes
            if (opu8_NrCode != NULL)
            {
               *opu8_NrCode = 0;
            }
            break;
         case C_BUSY: //locked (multithreaded use ...)
            s32_Return = C_CONFIG;
            break;
         case C_CONFIG: //BSmax is 0 if segemented transfer is required
            break;
         default:
            s32_Return = C_CONFIG; //no other documented error code
            break;
         }
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Read array data from server's data pool

   Send request and wait for response.
   This function is used for plain numeric data pool elements (i.e. non-array elements).

   This function is used for array elements.

   \param[in]  ou8_DataPoolIndex   index of data pool to read from
   \param[in]  ou16_ListIndex      list index to read from
   \param[in]  ou16_ElementIndex   element index to read
   \param[in,out] orc_ReadData     in: size must match expected number of bytes
                                   out: read data (in protocol-specific endianness)
   \param[out]    opu8_NrCode      if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_RANGE    parameter out of range (checked by client-side function):
              - data pool index != 0
              - list index > 63
              - element index > 1023
              - expected size 0
              - expected size > 0xFFFFFF (limit by protocol)
   C_NOACT    could not send request (TX error)
              expected response not received within timeout
   C_CONFIG   BSmax set to zero; communication already in use by another thread
              CAN dispatcher not installed
   C_WARN     error response
   C_RD_WR    malformed protocol response
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::DataPoolReadArray(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                               const uint16 ou16_ElementIndex, std::vector<uint8> & orc_ReadData,
                                               uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;

   if (mq_HasDispatcher == false)
   {
      s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
   }
   else
   {
      if ((ou8_DataPoolIndex == 0U) && (ou16_ListIndex < mhu16_KFX_RAM_MAX_NUM_LISTS) &&
          (ou16_ElementIndex < mhu16_KFX_RAM_MAX_NUM_ELEMENTS) &&
          (orc_ReadData.size() > 0) && (orc_ReadData.size() <= 0xFFFFFFU))
      {
         const uint16 u16_Index = mh_PackDataPoolIndex(ou16_ListIndex, ou16_ElementIndex);

         //lint -e{864} //false positive due to const/non-const misinterpretation
         s32_Return = this->mpc_CommKefex->ReadAggregateVariable(u16_Index, orc_ReadData.size(), &orc_ReadData[0]);
         //map KEFEX to openSYDE error codes as good as possible:
         switch (s32_Return)
         {
         case C_NO_ERR:
            break;
         case C_RD_WR: //could not send request; no dispatcher installed; wrong size in response;
                       // timeout of segmented transfer (no way to differentiate)
            s32_Return = C_NOACT;
            break;
         case C_COM: //transport protocol violation; wrong index in response (no way to differentiate)
            s32_Return = C_RD_WR;
            break;
         case C_WARN: //error response
            //TODO: Convert to OSY response codes
            if (opu8_NrCode != NULL)
            {
               *opu8_NrCode = 0;
            }
            break;
         case C_BUSY: //locked (multithreaded use ...)
            s32_Return = C_CONFIG;
            break;
         case C_CONFIG: //BSmax is 0 if segmented transfer is required
            break;
         default:
            s32_Return = C_CONFIG; //no other documented error code
            break;
         }
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Write numeric data to server's data pool

   Send request and wait for response.

   \param[in]  ou8_DataPoolIndex   index of data pool to write to
   \param[in]  ou16_ListIndex      list index to write to
   \param[in]  ou16_ElementIndex   element index to write
   \param[out] orc_DataToWrite     data to write (in protocol-specific endianness)
   \param[out] opu8_NrCode         if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_RANGE    parameter out of range (checked by client-side function):
              - data pool index != 0
              - list index > 63
              - element index > 1023
              - expected size not 1,2,4,8 (valid sizes of numeric elements)
   C_TIMEOUT  expected response not received within timeout (for elements <= 4 bytes)
   C_NOACT    could not send request (TX error)
              expected response not received within timeout (for elements with 8 bytes size)
   C_CONFIG   pre-requisites not correct; e.g. driver not initialized
              CAN dispatcher not installed
   C_WARN     error response
   C_RD_WR    malformed protocol response
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::DataPoolWriteNumeric(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                  const uint16 ou16_ElementIndex,
                                                  const std::vector<uint8> & orc_DataToWrite, uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;

   if (mq_HasDispatcher == false)
   {
      s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
   }
   else
   {
      if ((ou8_DataPoolIndex == 0U) && (ou16_ListIndex < mhu16_KFX_RAM_MAX_NUM_LISTS) &&
          (ou16_ElementIndex < mhu16_KFX_RAM_MAX_NUM_ELEMENTS) &&
          ((orc_DataToWrite.size() == 1U) || (orc_DataToWrite.size() == 2U) || (orc_DataToWrite.size() == 4U) ||
           (orc_DataToWrite.size() == 8U)))
      {
         const uint16 u16_Index = mh_PackDataPoolIndex(ou16_ListIndex, ou16_ElementIndex);
         uint64 u64_Value;

         //sigh... the KEFEX protocol takes a sint64 parameter; convert back to numeric value :-)
         switch (orc_DataToWrite.size())
         {
         case 1:
            u64_Value = orc_DataToWrite[0];
            break;
         case 2:
            u64_Value = static_cast<uint64>(orc_DataToWrite[0] +
                                            ((static_cast<uint64>(orc_DataToWrite[1])) << 8));
            break;
         case 4:
            u64_Value = static_cast<uint64>(orc_DataToWrite[0] +
                                            ((static_cast<uint64>(orc_DataToWrite[1])) << 8) +
                                            ((static_cast<uint64>(orc_DataToWrite[2])) << 16) +
                                            ((static_cast<uint64>(orc_DataToWrite[3])) << 24));
            break;
         case 8:
            u64_Value = static_cast<uint64>(orc_DataToWrite[0] +
                                            ((static_cast<uint64>(orc_DataToWrite[1])) << 8) +
                                            ((static_cast<uint64>(orc_DataToWrite[2])) << 16) +
                                            ((static_cast<uint64>(orc_DataToWrite[3])) << 24) +
                                            ((static_cast<uint64>(orc_DataToWrite[4])) << 32) +
                                            ((static_cast<uint64>(orc_DataToWrite[5])) << 40) +
                                            ((static_cast<uint64>(orc_DataToWrite[6])) << 48) +
                                            ((static_cast<uint64>(orc_DataToWrite[7])) << 56));
            break;
         default:
            u64_Value = 0U;
            break; //checked at beginning of function
         }

         s32_Return = this->mpc_CommKefex->WriteNumericVariable(u16_Index, static_cast<uint8>(orc_DataToWrite.size()),
                                                                static_cast<sint64>(u64_Value));

         //map KEFEX to openSYDE error codes as good as possible:
         switch (s32_Return)
         {
         case C_NO_ERR:
            break;
         case C_RD_WR: //could not send request; no dispatcher installed; wrong size in response;
                       // timeout of segmented transfer (no way to differentiate)
            s32_Return = C_NOACT;
            break;
         case C_COM: //no response of expedited transfer; transport protocol violation;
                     // wrong index in response (no way to differentiate)
            s32_Return = C_TIMEOUT;
            break;
         case C_WARN: //error response
            //TODO: Convert to OSY response codes
            if (opu8_NrCode != NULL)
            {
               *opu8_NrCode = 0;
            }
            break;
         case C_BUSY: //locked (multithreaded use ...)
            s32_Return = C_CONFIG;
            break;
         case C_CONFIG: //BSmax is 0 if segmented transfer is required
            break;
         default:
            s32_Return = C_CONFIG; //no other documented error code
            break;
         }
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Write array data to server's data pool

   Send request and wait for response.

   \param[in]  ou8_DataPoolIndex   index of data pool to read from
   \param[in]  ou16_ListIndex      list index to read from
   \param[in]  ou16_ElementIndex   element index to read
   \param[out] orc_DataToWrite     data to write
   \param[out] opu8_NrCode         if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_RANGE    parameter out of range (checked by client-side function):
              - data pool index != 0
              - list index > 63
              - element index > 1023
              - size to write is zero or > 0xFFFFFF
   C_NOACT    could not send request (TX error)
              expected response not received within timeout
   C_CONFIG   pre-requisites not correct; e.g. driver not initialized
              CAN dispatcher not installed
   C_WARN     error response
   C_RD_WR    malformed protocol response
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::DataPoolWriteArray(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                const uint16 ou16_ElementIndex,
                                                const std::vector<uint8> & orc_DataToWrite, uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;

   if (mq_HasDispatcher == false)
   {
      s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
   }
   else
   {
      if ((ou8_DataPoolIndex == 0U) && (ou16_ListIndex < mhu16_KFX_RAM_MAX_NUM_LISTS) &&
          (ou16_ElementIndex < mhu16_KFX_RAM_MAX_NUM_ELEMENTS) &&
          ((orc_DataToWrite.size() > 0U) && (orc_DataToWrite.size() <= 0xFFFFFFU)))
      {
         const uint16 u16_Index = mh_PackDataPoolIndex(ou16_ListIndex, ou16_ElementIndex);

         s32_Return =
            this->mpc_CommKefex->WriteAggregateVariable(u16_Index, orc_DataToWrite.size(), &orc_DataToWrite[0]);
         //map KEFEX to openSYDE error codes as good as possible:
         switch (s32_Return)
         {
         case C_NO_ERR:
            break;
         case C_RD_WR: //could not send request; no dispatcher installed; wrong size in response;
                       // timeout (no way to differentiate)
            s32_Return = C_NOACT;
            break;
         case C_COM: //transport protocol violation; wrong index in response (no way to differentiate)
            s32_Return = C_RD_WR;
            break;
         case C_WARN: //error response
            //TODO: Convert to OSY response codes
            if (opu8_NrCode != NULL)
            {
               *opu8_NrCode = 0;
            }
            break;
         case C_BUSY: //locked (multithreaded use ...)
            s32_Return = C_CONFIG;
            break;
         case C_CONFIG: //BSmax is 0 if segmented transfer is required
            break;
         default:
            s32_Return = C_CONFIG; //no other documented error code
            break;
         }
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Set rate of event driven transmissions

   Configure update rate of event driven transmissions in ms.
   Three "rails" can be configured for event driven transmissions.
   Each transmission can be assigned to one of these rails.

   In this implementation we remember the configured interval values to use in subsequent service calls.

   \param[in]  ou8_Rail            0..2 "rail" to configure
   \param[in]  ou16_IntervalMs     interval to configure (in ms) (0.. 0xFFFF)

   \return
   C_NO_ERR   rail value configured
   C_RANGE    invalid rail index or interval value
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::DataPoolSetEventDataRate(const uint8 ou8_Rail, const uint16 ou16_IntervalMs)
{
   sint32 s32_Return = C_NO_ERR;

   if ((ou8_Rail > 2U) || (ou16_IntervalMs == 0U))
   {
      s32_Return = C_RANGE;
   }
   else
   {
      mau16_EventRailIntervalsMs[ou8_Rail] = ou16_IntervalMs;
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Request cyclic driven transmission of data pool element.

   Request cyclic transmission of data pool element.

   Base spec says: Shall send the request and wait for the initial confirmation (or error) response.
   However we do not get an initial confirmation in the KEFEX protocol.
   Therefore all data and error responses are handled asynchronously using the Cycle() function and
   be reported via calls to installed function pointer.
   So we only send the request here.

   \param[in]  ou8_DataPoolIndex   index of data pool to read from
   \param[in]  ou16_ListIndex      list index to read from
   \param[in]  ou16_ElementIndex   element index to read
   \param[in]  ou8_Rail            0..2   "rail" referring to time interval to use
   \param[out] opu8_NrCode         if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent
   C_RANGE    ou8_DataPoolIndex is zero; list index > 63; element index > 1023; rail > 2
   C_NOACT    could not send request (e.g. TX buffer full)
   C_CONFIG   CAN dispatcher not installed
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::DataPoolReadCyclic(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                const uint16 ou16_ElementIndex, const uint8 ou8_Rail,
                                                uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;

   if (mq_HasDispatcher == false)
   {
      s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
   }
   else
   {
      if ((ou8_DataPoolIndex == 0U) && (ou16_ListIndex < 64) && (ou16_ElementIndex < 1024) && (ou8_Rail < 3))
      {
         const uint16 u16_Index = mh_PackDataPoolIndex(ou16_ListIndex, ou16_ElementIndex);
         s32_Return =
            this->mpc_CommKefex->RequestTimeTriggeredTransmission(u16_Index, false,
                                                                  mau16_EventRailIntervalsMs[ou8_Rail]);
         if (s32_Return != C_NO_ERR)
         {
            //could not send request
            s32_Return = C_NOACT;
            //TODO: Convert to OSY response codes
            if (opu8_NrCode != NULL)
            {
               *opu8_NrCode = 0;
            }
         }
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Request change driven transmission of data pool element.

   Request change driven transmission of data pool element.

   Base spec says: Shall send the request and wait for the initial confirmation (or error) response.
   However we do not get an initial confirmation in the KEFEX protocol.
   Therefore all data and error responses are handled asynchronously using the Cycle() function and
   be reported via calls to installed function pointer.
   So we only send the request here.

   \param[in]  ou8_DataPoolIndex   index of data pool to read from
   \param[in]  ou16_ListIndex      list index to read from
   \param[in]  ou16_ElementIndex   element index to read
   \param[in]  ou8_Rail            0..2   "rail" referring to time interval to use for maximum timeout
   \param[in]  ou32_Threshold      threshold the value must cross before getting sent again
   \param[out] opu8_NrCode         if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent
   C_RANGE    ou8_DataPoolIndex is zero; list index > 63; element index > 1023; rail > 2
   C_NOACT    could not send request (e.g. TX buffer full)
   C_CONFIG   CAN dispatcher not installed
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::DataPoolReadChangeDriven(const uint8 ou8_DataPoolIndex, const uint16 ou16_ListIndex,
                                                      const uint16 ou16_ElementIndex, const uint8 ou8_Rail,
                                                      const uint32 ou32_Threshold, uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;

   if (mq_HasDispatcher == false)
   {
      s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
   }
   else
   {
      if ((ou8_DataPoolIndex == 0U) && (ou16_ListIndex < 64) && (ou16_ElementIndex < 1024) && (ou8_Rail < 3))
      {
         const uint16 u16_Index = mh_PackDataPoolIndex(ou16_ListIndex, ou16_ElementIndex);
         s32_Return =
            this->mpc_CommKefex->RequestChangeTriggeredTransmission(u16_Index, mau16_EventRailIntervalsMs[ou8_Rail],
                                                                    ou32_Threshold, ou32_Threshold);
         if (s32_Return != C_NO_ERR)
         {
            //could not send request
            s32_Return = C_NOACT;
            //TODO: Convert to OSY response codes
            if (opu8_NrCode != NULL)
            {
               *opu8_NrCode = 0;
            }
         }
      }
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Request stop all change driven transmissions.

   Request stop of all change driven transmissions.
   Shall send the request and wait for the response.

   \return
   C_NO_ERR   request sent
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send request (e.g. TX buffer full)
   C_CONFIG   CAN dispatcher not installed
   C_WARN     error response (none specified in protocol; but who knows ...)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::DataPoolStopEventDriven(void)
{
   sint32 s32_Return;

   if (mq_HasDispatcher == false)
   {
      s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
   }
   else
   {
      s32_Return = this->mpc_CommKefex->TerminateAllCyclicTransmissions(true); //wait for handshake ...
      //map KEFEX to openSYDE error codes as good as possible:
      switch (s32_Return)
      {
      case C_NO_ERR:
         break;
      case C_WARN: //error response
         break;
      case C_RD_WR: //could not send request
         s32_Return = C_NOACT;
         break;
      case C_COM:
         s32_Return = C_TIMEOUT;
         break;
      default: //what now ?
         s32_Return = C_NOACT;
         break;
      }
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Request cyclic driven transmission of NVM content.

   \param[in]     ou32_MemoryAddress   NVM memory address to read (first read byte)
   \param[in,out] orc_DataRecord       in: size defines number of bytes to read
                                       out: Read data bytes
   \param[out]    opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_RANGE    address is > 0xFFFFFF (limited by protocol)
              size is 0 (makes no sense)
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send protocol request
   C_CONFIG   CAN dispatcher not installed
   C_WARN     error response
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::NvmRead(const uint32 ou32_MemoryAddress, std::vector<uint8> & orc_DataRecord,
                                     uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;

   if (mq_HasDispatcher == false)
   {
      s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
   }
   else
   {
      if ((ou32_MemoryAddress <= 0xFFFFFFU) && (orc_DataRecord.size() > 0))
      {
         //lint -e{864} //false positive due to const/non-const misinterpretation
         s32_Return = this->mpc_CommKefex->ReadEEPROM(ou32_MemoryAddress, orc_DataRecord.size(), &orc_DataRecord[0]);
         //map KEFEX to openSYDE error codes as good as possible:
         switch (s32_Return)
         {
         case C_NO_ERR:
            break;
         case C_WARN: //error response; or: wrong address in response
            //TODO: Convert to OSY response codes
            if (opu8_NrCode != NULL)
            {
               *opu8_NrCode = 0;
            }
            break;
         case C_RD_WR: //could not send request
            s32_Return = C_NOACT;
            break;
         case C_COM:
            s32_Return = C_TIMEOUT;
            break;
         default: //what now ?
            s32_Return = C_NOACT;
            break;
         }
      }
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Announce writing data to NVM

   In this protocol the corresponding services is not available/required on all targets.
   So the function checks a configurable flag and returns "OK" if the service is marked as not available.

   \param[in]  ou8_DataPoolIndex    Accessed data pool index
   \param[in]  ou16_NVMAccessCount  Expected number of subsequent executions of service to write to NVM

   \return
   C_NO_ERR   request sent, positive response received; or: no action required
   C_RANGE    access count is 0; data pool index > 0
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send protocol request
   C_CONFIG   CAN dispatcher not installed
   C_WARN     error response
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::NvmWriteStartTransaction(const uint8 ou8_DataPoolIndex, const uint16 ou16_NVMAccessCount)
{
   sint32 s32_Return = C_NO_ERR;

   if (mq_NvmValidFlagIsUsed != false)
   {
      s32_Return = C_RANGE;

      if (mq_HasDispatcher == false)
      {
         s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
      }
      else
      {
         if ((ou8_DataPoolIndex == 0U) && (ou16_NVMAccessCount > 0U))
         {
            s32_Return = this->mpc_CommKefex->WriteEEPROMSSLStart(static_cast<uint32>(ou16_NVMAccessCount) * 2U);
            //map KEFEX to openSYDE error codes as good as possible:
            switch (s32_Return)
            {
            case C_NO_ERR:
               break;
            case C_WARN: //error response
               break;
            case C_RD_WR: //could not send request
               s32_Return = C_NOACT;
               break;
            case C_COM:
               s32_Return = C_TIMEOUT;
               break;
            default: //what now ?
               s32_Return = C_NOACT;
               break;
            }
         }
      }
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Cyclic driven request for NVM write operation

   Usage:
   * First call NvmWriteStartTransaction. (Here you can register any non zero amount of NVM write operations)
   * Execute the specified amount NVM write operations
   * Afterwards call NvmWriteFinalizeTransaction to complete this transaction.

   \param[in]  ou32_MemoryAddress NVM memory address to write to (first written byte)
   \param[in]  orc_DataRecord     Data bytes to write
   \param[out] opu8_NrCode        if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, positive response received
   C_RANGE    address is > 0xFFFFFF (limited by protocol)
              size is 0 (makes no sense)
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send protocol request
   C_CONFIG   CAN dispatcher not installed
   C_WARN     error response
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::NvmWrite(const uint32 ou32_MemoryAddress, const std::vector<uint8> & orc_DataRecord,
                                      uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;

   if (mq_HasDispatcher == false)
   {
      s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
   }
   else
   {
      if ((ou32_MemoryAddress <= 0xFFFFFFU) && (orc_DataRecord.size() > 0))
      {
         s32_Return = this->mpc_CommKefex->WriteEEPROM(ou32_MemoryAddress, orc_DataRecord.size(), &orc_DataRecord[0]);
         //map KEFEX to openSYDE error codes as good as possible:
         switch (s32_Return)
         {
         case C_NO_ERR:
            break;
         case C_WARN: //error response; or: wrong address in response
            //TODO: Convert to OSY response codes
            if (opu8_NrCode != NULL)
            {
               *opu8_NrCode = 0;
            }
            break;
         case C_RD_WR: //could not send request
            s32_Return = C_NOACT;
            break;
         case C_COM:
            s32_Return = C_TIMEOUT;
            break;
         default: //what now ?
            s32_Return = C_NOACT;
            break;
         }
      }
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Finalize writing to NVM

   \return
   C_NO_ERR   request sent, positive response received; or: no action required
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send protocol request
   C_CONFIG   CAN dispatcher not installed
   C_WARN     error response
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::NvmWriteFinalizeTransaction(void)
{
   sint32 s32_Return = C_NO_ERR;

   if (mq_NvmValidFlagIsUsed != false)
   {
      if (mq_HasDispatcher == false)
      {
         s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
      }
      else
      {
         s32_Return = this->mpc_CommKefex->WriteEEPROMSSLEnd();
         //map KEFEX to openSYDE error codes as good as possible:
         switch (s32_Return)
         {
         case C_NO_ERR:
            break;
         case C_WARN: //error response
            break;
         case C_RD_WR: //could not send request
            s32_Return = C_NOACT;
            break;
         case C_COM:
            s32_Return = C_TIMEOUT;
            break;
         default: //what now ?
            s32_Return = C_NOACT;
            break;
         }
      }
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Read data pool version

   Version format: One byte for Major, Minor, Release

   Example: v1.23r4 in 3 Bytes   -> (0x01, 0x17, 0x04)

   \param[in]  ou8_DataPoolIndex   Data pool index
   \param[out] orau8_Version       Read version; format: see function description

   \return
   C_NO_ERR   request sent, positive response received; or: no action required
   C_RANGE    data pool index is zero
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send protocol request
   C_CONFIG   CAN dispatcher not installed
   C_WARN     error response
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::DataPoolReadVersion(const uint8 ou8_DataPoolIndex, stw_types::uint8 (&orau8_Version)[3])
{
   sint32 s32_Return = C_RANGE;
   static const uint16 hu16_KFX_SERVICE_PROJECT_VERSION = 16U;

   if (mq_HasDispatcher == false)
   {
      s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
   }
   else
   {
      if (ou8_DataPoolIndex == 0U)
      {
         uint32 u32_Result;
         s32_Return = this->mpc_CommKefex->ReadService(hu16_KFX_SERVICE_PROJECT_VERSION, u32_Result);
         switch (s32_Return)
         {
         case C_NO_ERR:
            //KEFEX format: 0xMmmr  -> convert
            orau8_Version[0] = static_cast<uint8>((u32_Result & 0xF000U) >> 12);
            orau8_Version[1] = static_cast<uint8>((u32_Result & 0x0FF0U) >> 4);
            orau8_Version[2] = static_cast<uint8>(u32_Result & 0x000FU);
            break;
         case C_WARN: //error response
            break;
         case C_RD_WR: //could not send request
            s32_Return = C_NOACT;
            break;
         case C_NOACT:
            s32_Return = C_TIMEOUT;
            break;
         default: //what now ?
            s32_Return = C_NOACT;
            break;
         }
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Notify NVM data changes

   \param[in]  ou8_DataPoolIndex            Data pool index
   \param[in]  ou8_ListIndex                List index
   \param[out] orq_ApplicationAcknowledge   true: positive acknowledge from server
                                            false: negative acknowledge from server
   \param[out] opu8_NrCode                  if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   request sent, response received
   C_RANGE    data pool index is zero; list index is > 63
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send protocol request
   C_CONFIG   CAN dispatcher not installed
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::NvmNotifyOfChanges(const uint8 ou8_DataPoolIndex, const uint8 ou8_ListIndex,
                                                bool & orq_ApplicationAcknowledge, uint8 * const opu8_NrCode)
{
   sint32 s32_Return = C_RANGE;

   if (mq_HasDispatcher == false)
   {
      s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
   }
   else
   {
      if ((ou8_DataPoolIndex == 0U) && (ou8_ListIndex < mhu16_KFX_RAM_MAX_NUM_LISTS))
      {
         s32_Return = this->mpc_CommKefex->SendChangeNotification(ou8_ListIndex);
         switch (s32_Return)
         {
         case C_NO_ERR:
            orq_ApplicationAcknowledge = true;
            break;
         case C_WARN: //error response
            //we cannot distinguish between e.g. "invalid list index" and "nope"
            orq_ApplicationAcknowledge = false;
            //TODO: Convert to OSY response codes
            if (opu8_NrCode != NULL)
            {
               *opu8_NrCode = 0;
            }
            s32_Return = C_NO_ERR;
            break;
         case C_RD_WR: //could not send request
            s32_Return = C_NOACT;
            break;
         case C_COM:
            s32_Return = C_TIMEOUT;
            break;
         default: //what now ?
            s32_Return = C_NOACT;
            break;
         }
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Verify data pool consistency

   Here: try to logon; server will verify ...

   \param[in]  ou8_DataPoolIndex               Data pool index
   \param[in]  ou16_NumberOfDataPoolElements   Number of elements in data pool
   \param[in]  ou16_DataPoolVersion            Version number of data pool
   \param[in]  ou32_DataPoolChecksum           Data pool checksum
   \param[out] orq_Match                       Data pool checksum match response ?

   \return
   C_NO_ERR   request sent, positive response received
   C_RANGE    data pool index is zero
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send protocol request
   C_CONFIG   CAN dispatcher not installed
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::DataPoolVerify(const uint8 ou8_DataPoolIndex, const uint16 ou16_NumberOfDataPoolElements,
                                            const uint16 ou16_DataPoolVersion, const uint32 ou32_DataPoolChecksum,
                                            bool & orq_Match)
{
   sint32 s32_Return = C_RANGE;

   if (mq_HasDispatcher == false)
   {
      s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
   }
   else
   {
      if ((ou8_DataPoolIndex == 0U) && (ou32_DataPoolChecksum <= 0xFFFFU))
      {
         s32_Return = this->mpc_CommKefex->Logon(static_cast<uint64>(ou32_DataPoolChecksum), ou16_DataPoolVersion,
                                                 ou16_NumberOfDataPoolElements, 0U);
         switch (s32_Return)
         {
         case C_NO_ERR:
            orq_Match = true;
            break;
         case C_WARN: //error response
            //should be a "does not match" situation
            orq_Match = false;
            s32_Return = C_NO_ERR;
            break;
         case C_RD_WR: //could not send request
            s32_Return = C_NOACT;
            break;
         case C_COM:
            s32_Return = C_TIMEOUT;
            break;
         default: //what now ?
            s32_Return = C_NOACT;
            break;
         }
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Perform a logoff

   Logoff from the server ECU.

   \param[in]  oq_WaitForHandshake     true  -> will actively wait for error or OK handshake response
                                       false -> will not wait for handshake response

   \return
   C_NO_ERR   request sent, positive response received
   C_TIMEOUT  expected response not received within timeout
   C_NOACT    could not send protocol request
   C_WARN     error response
   C_CONFIG   CAN dispatcher not installed
*/
//-----------------------------------------------------------------------------
sint32 C_OSCDiagProtocolKfx::Logoff(const bool oq_WaitForHandshake)
{
   sint32 s32_Return;

   if (mq_HasDispatcher == false)
   {
      s32_Return = C_CONFIG; //not all protocol driver functions check for installed dispatcher ...
   }
   else
   {
      s32_Return = this->mpc_CommKefex->Logoff(oq_WaitForHandshake);
      switch (s32_Return)
      {
      case C_NO_ERR: // no error, all ok
      case C_WARN:   // error response
         // No change
         break;
      case C_RD_WR: //could not send request
         s32_Return = C_NOACT;
         break;
      case C_COM:
         s32_Return = C_TIMEOUT;
         break;
      default: //what now ?
         s32_Return = C_NOACT;
         break;
      }
   }

   return s32_Return;
}
