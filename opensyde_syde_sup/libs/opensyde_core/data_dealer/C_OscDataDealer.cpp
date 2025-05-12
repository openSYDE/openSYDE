//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Connects the openSYDE protocol driver to a node.

   Its main job is to place incoming data into the data pool.
   The application is responsible to not "::Cycle" and poll at the same time from different threads.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscDataDealer.hpp"
#include "TglUtils.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set up class

   Initializes class elements
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscDataDealer::C_OscDataDealer(void) :
   mpc_Node(NULL),
   mu32_NodeIndex(0U),
   mpc_DiagProtocol(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set up class

   Initializes class elements

   \param[in]     opc_Node          Pointer to node of data dealer
   \param[in]     ou32_NodeIndex    Index of node of data dealer
   \param[in]     opc_DiagProtocol  Pointer to used diagnostic protocol
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscDataDealer::C_OscDataDealer(C_OscNode * const opc_Node, const uint32_t ou32_NodeIndex,
                                 C_OscDiagProtocolBase * const opc_DiagProtocol) :
   mpc_Node(opc_Node),
   mu32_NodeIndex(ou32_NodeIndex),
   mpc_DiagProtocol(opc_DiagProtocol)
{
   this->m_Init();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up class
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscDataDealer::~C_OscDataDealer(void)
{
   mpc_Node = NULL;
   mpc_DiagProtocol = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializing the data dealer if the default constructor was used

   \param[in]     opc_Node                      Pointer to node of data dealer
   \param[in]     ou32_NodeIndex                Index of node of data dealer
   \param[in]     opc_DiagProtocol              Pointer to used diagnostic protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataDealer::Initialize(C_OscNode * const opc_Node, const uint32_t ou32_NodeIndex,
                                 C_OscDiagProtocolBase * const opc_DiagProtocol)
{
   this->mpc_Node = opc_Node;
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mpc_DiagProtocol = opc_DiagProtocol;
   this->m_Init();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the associated node index

   \return
   Node index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscDataDealer::GetNodeIndex(void) const
{
   return this->mu32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read data from server's data pool

   Read data pool data from server and place in local data pool.
   * reads data from server using the installed diagnostic protocol
   * places the read data in the associated data pool element (in correct endianness)

   \param[in]     ou8_DataPoolIndex    data pool index
   \param[in]     ou16_ListIndex       list index
   \param[in]     ou16_ElementIndex    element index
   \param[out]    opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR    data read and placed in data pool
   C_CONFIG    no node or diagnostic protocol are known (was this class properly Initialize()d ?)
               protocol driver reported configuration error (was the protocol driver properly initialized ?)
   C_RANGE     specified data pool, list, element does not exist in data pools of configured node
               protocol driver reported parameter out of range (does the protocol support the index range ?)
   C_TIMEOUT   expected response not received within timeout
   C_NOACT     could not send request (e.g. Tx buffer full)
   C_RD_WR     protocol driver reported protocol violation
   C_WARN      error response received
   C_OVERFLOW  size of data received from server does not match size of specified data pool element
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataDealer::DataPoolRead(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                      const uint16_t ou16_ElementIndex, uint8_t * const opu8_NrCode)
{
   int32_t s32_Return;

   if ((mpc_Node == NULL) || (mpc_DiagProtocol == NULL))
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      //get list element
      C_OscNodeDataPoolListElement * const pc_Element = this->mpc_Node->GetDataPoolListElement(ou8_DataPoolIndex,
                                                                                               ou16_ListIndex,
                                                                                               ou16_ElementIndex);
      //does the specified element exist ?
      if (pc_Element == NULL)
      {
         s32_Return = C_RANGE;
      }
      else
      {
         std::vector<uint8_t> c_Data;
         // Set the expected size. It will be checked in the read functions.
         c_Data.resize(pc_Element->GetSizeByte());
         //use communication function matching the element type
         if (pc_Element->GetArray() == false)
         {
            s32_Return = mpc_DiagProtocol->DataPoolReadNumeric(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex,
                                                               c_Data, opu8_NrCode);
         }
         else
         {
            s32_Return = mpc_DiagProtocol->DataPoolReadArray(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex,
                                                             c_Data, opu8_NrCode);
         }
         if (s32_Return == C_NO_ERR)
         {
            //we have data: is the size as expected ?
            if (c_Data.size() != pc_Element->GetSizeByte())
            {
               s32_Return = C_OVERFLOW;
            }
            else
            {
               //size OK
               //convert to native endianness depending on the type ...
               //no possible problem we did not check for already ...
               if (mpc_DiagProtocol->GetEndianness() == C_OscDiagProtocolBase::mhu8_ENDIANNESS_BIG)
               {
                  (void)pc_Element->c_Value.SetValueFromBigEndianBlob(c_Data);
               }
               else
               {
                  (void)pc_Element->c_Value.SetValueFromLittleEndianBlob(c_Data);
               }
            }
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write data to server's data pool

   Write data from local data pool data to server's data pool.
   * check whether data pool value lies within defined min/max ranges
   * get data from local data pool in correct endianness
   * write the data to the server's data pool using the installed diagnostic protocol

   \param[in]     ou8_DataPoolIndex    data pool index
   \param[in]     ou16_ListIndex       list index
   \param[in]     ou16_ElementIndex    element index
   \param[out]    opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR    data written to server
   C_CONFIG    no node or diagnostic protocol are known (was this class properly Initialize()d ?)
               protocol driver reported configuration error (was the protocol driver properly initialized ?)
   C_RANGE     specified data pool, list, element does not exist in data pools of configured node
               protocol driver reported parameter out of range (does the protocol support the index range ?)
   C_OVERFLOW  value in data pool does not lie within defined min/max range (checked on client side)
   C_TIMEOUT   expected response not received within timeout
   C_NOACT     could not send request (e.g. Tx buffer full)
   C_RD_WR     protocol driver reported protocol violation
   C_WARN      error response received
   C_COM       communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataDealer::DataPoolWrite(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                       const uint16_t ou16_ElementIndex, uint8_t * const opu8_NrCode)
{
   int32_t s32_Return;

   if ((mpc_Node == NULL) || (mpc_DiagProtocol == NULL))
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      //get list element
      const C_OscNodeDataPoolListElement * const pc_Element =
         this->mpc_Node->GetDataPoolListElement(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex);
      //does the specified element exist ?
      if (pc_Element == NULL)
      {
         s32_Return = C_RANGE;
      }
      else
      {
         std::vector<uint8_t> c_Data;

         //is the value within the defines min/max ranges ?
         s32_Return = pc_Element->CheckValueRange();
         if (s32_Return != C_NO_ERR)
         {
            s32_Return = C_OVERFLOW;
         }
         else
         {
            //convert native to protocol endianness depending on the type ...
            if (mpc_DiagProtocol->GetEndianness() == C_OscDiagProtocolBase::mhu8_ENDIANNESS_BIG)
            {
               pc_Element->c_Value.GetValueAsBigEndianBlob(c_Data);
            }
            else
            {
               pc_Element->c_Value.GetValueAsLittleEndianBlob(c_Data);
            }

            //use communication function matching the element type
            if (pc_Element->GetArray() == false)
            {
               s32_Return = mpc_DiagProtocol->DataPoolWriteNumeric(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex,
                                                                   c_Data, opu8_NrCode);
            }
            else
            {
               s32_Return = mpc_DiagProtocol->DataPoolWriteArray(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex,
                                                                 c_Data, opu8_NrCode);
            }
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read data from server's NVM

   Read NVM data from server and place in local data pool.
   * gets required offset address and size from local data pool description
   * reads data from server's NVM using the installed diagnostic protocol
   * places the read data in the associated data pool element (in correct endianness)

   The function does not verify whether the specified data pool is an NVM data pool.

   \param[in]     ou8_DataPoolIndex    data pool index
   \param[in]     ou16_ListIndex       list index
   \param[in]     ou16_ElementIndex    element index
   \param[out]    opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR    data read and placed in data pool
   C_CONFIG    no node or diagnostic protocol are known (was this class properly Initialize()d ?)
               protocol driver reported configuration error (was the protocol driver properly initialized ?)
   C_RANGE     specified data pool, list, element does not exist in data pools of configured node
               protocol driver reported parameter out of range (does the protocol support the address range ?)
   C_TIMEOUT   expected response not received within timeout
   C_NOACT     could not send request (e.g. Tx buffer full)
   C_RD_WR     protocol driver reported protocol violation
   C_WARN      error response received
   C_COM       expected server response not received because of communication error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataDealer::NvmRead(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                 const uint16_t ou16_ElementIndex, uint8_t * const opu8_NrCode)
{
   int32_t s32_Return;

   if ((mpc_Node == NULL) || (mpc_DiagProtocol == NULL))
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      //get list element
      C_OscNodeDataPoolListElement * const pc_Element = this->mpc_Node->GetDataPoolListElement(ou8_DataPoolIndex,
                                                                                               ou16_ListIndex,
                                                                                               ou16_ElementIndex);
      //does the specified element exist ?
      if (pc_Element == NULL)
      {
         s32_Return = C_RANGE;
      }
      else
      {
         std::vector<uint8_t> c_Data;

         c_Data.resize(pc_Element->GetSizeByte());
         //request data from server
         s32_Return = mpc_DiagProtocol->NvmRead(pc_Element->u32_NvmStartAddress, c_Data, opu8_NrCode);
         if (s32_Return == C_NO_ERR)
         {
            //we have data
            //convert to native endianness depending on the type ...
            //no possible problem we did not check for already ...
            if (mpc_DiagProtocol->GetEndianness() == C_OscDiagProtocolBase::mhu8_ENDIANNESS_BIG)
            {
               (void)pc_Element->c_NvmValue.SetValueFromBigEndianBlob(c_Data);
            }
            else
            {
               (void)pc_Element->c_NvmValue.SetValueFromLittleEndianBlob(c_Data);
            }
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write data to server's NVM

   Write data from local data pool data to server's NVM.
   * check whether data pool value lies within defined min/max ranges
   * get required offset address and size from local data pool description
   * get data from local data pool in correct endianness
   * write the data to the server's NVM using the installed diagnostic protocol

   The function does not verify whether the specified data pool is an NVM data pool.

   If the protocol requires a "start" -> "write" -> "finalize" sequence for writing to the NVM the
   application is responsible to handle it.
   This function can be called multiple times within such a transfer bracket.

   \param[in]     ou8_DataPoolIndex    data pool index
   \param[in]     ou16_ListIndex       list index
   \param[in]     ou16_ElementIndex    element index
   \param[out]    opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR    data written to server
   C_CONFIG    no node or diagnostic protocol are known (was this class properly Initialize()d ?)
               protocol driver reported configuration error (was the protocol driver properly initialized ?)
   C_RANGE     specified data pool, list, element does not exist in data pools of configured node
               protocol driver reported parameter out of range (does the protocol support the index range ?)
   C_OVERFLOW  value in data pool does not lie within defined min/max range (checked on client side)
   C_TIMEOUT   expected response not received within timeout
   C_NOACT     could not send request (e.g. Tx buffer full)
   C_RD_WR     protocol driver reported protocol violation
   C_WARN      error response received
   C_COM       expected server response not received because of communication error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataDealer::NvmWrite(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                  const uint16_t ou16_ElementIndex, uint8_t * const opu8_NrCode)
{
   int32_t s32_Return;

   if ((mpc_Node == NULL) || (mpc_DiagProtocol == NULL))
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      //get list element
      const C_OscNodeDataPoolListElement * const pc_Element =
         this->mpc_Node->GetDataPoolListElement(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex);
      //does the specified element exist ?
      if (pc_Element == NULL)
      {
         s32_Return = C_RANGE;
      }
      else
      {
         std::vector<uint8_t> c_Data;

         //is the value within the defined min/max ranges ?
         s32_Return = pc_Element->CheckNvmValueRange();
         if (s32_Return != C_NO_ERR)
         {
            s32_Return = C_OVERFLOW;
         }
         else
         {
            //convert native to protocol endianness depending on the type ...
            if (mpc_DiagProtocol->GetEndianness() == C_OscDiagProtocolBase::mhu8_ENDIANNESS_BIG)
            {
               pc_Element->c_NvmValue.GetValueAsBigEndianBlob(c_Data);
            }
            else
            {
               pc_Element->c_NvmValue.GetValueAsLittleEndianBlob(c_Data);
            }

            //write data to server:
            s32_Return = mpc_DiagProtocol->NvmWrite(pc_Element->u32_NvmStartAddress, c_Data, opu8_NrCode);
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handler for reception of async response

   Called by the DataDealer after it has placed the data in the data pool content structure associated with
    the installed node.

   Default implementation: log

   \param[in]  ou8_DataPoolIndex   data pool index that data was received for
   \param[in]  ou16_ListIndex      list index that data was received for
   \param[in]  ou16_ElementIndex   element index that data was received for
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataDealer::m_OnReadDataPoolEventReceived(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                                    const uint16_t ou16_ElementIndex)
{
   stw::scl::C_SclString c_Info;
   c_Info.PrintFormatted("C_OscDataDealer: ReadDataPool event received but no notification handler implemented. " \
                         "Element (dp: %d list: %d element: %d) !", ou8_DataPoolIndex, ou16_ListIndex,
                         ou16_ElementIndex);
   osc_write_log_warning("Asynchronous communication", c_Info);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handler for reception of async error response

   Called by the DataDealer after it has received an error response for a cyclic transmission.

   Default implementation: log

   \param[in]  ou8_DataPoolIndex   data pool index that data was received for
   \param[in]  ou16_ListIndex      list index that data was received for
   \param[in]  ou16_ElementIndex   element index that data was received for
   \param[in]  ou8_ErrorCode       received error code
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataDealer::m_OnReadDataPoolEventErrorReceived(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                                         const uint16_t ou16_ElementIndex, const uint8_t ou8_ErrorCode)
{
   stw::scl::C_SclString c_Info;
   c_Info.PrintFormatted("C_OscDataDealer: ReadDataPool error event received but no notification handler " \
                         "implemented. Element (dp: %d list: %d element: %d) Code: %d !", ou8_DataPoolIndex,
                         ou16_ListIndex, ou16_ElementIndex, ou8_ErrorCode);
   osc_write_log_warning("Asynchronous communication", c_Info);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handler for reception of async response

   Called by the DataDealer after it has placed the data in the data pool content structure associated with
    the installed node.

   Default implementation: log

   \param[in]  ou8_DataPoolIndex   data pool index that data was received for
   \param[in]  ou16_ListIndex      list index that data was received for
   \param[in]  ou16_ElementIndex   element index that data was received for
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataDealer::m_OnReadDataPoolNvmEventReceived(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                                       const uint16_t ou16_ElementIndex)
{
   stw::scl::C_SclString c_Info;
   c_Info.PrintFormatted("C_OscDataDealer: NvmRead event received but no notification handler implemented. " \
                         "Element (dp: %d list: %d element: %d) !", ou8_DataPoolIndex, ou16_ListIndex,
                         ou16_ElementIndex);
   osc_write_log_warning("Asynchronous communication", c_Info);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialization of the data dealer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataDealer::m_Init(void)
{
   if (this->mpc_DiagProtocol != NULL)
   {
      this->mpc_DiagProtocol->Initialize(&C_OscDataDealer::mh_ReadDataPoolDataEventReceived,
                                         &C_OscDataDealer::mh_ReadDataPoolDataEventErrorReceived, this);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handling an async received read datapool data event

   Target of C_OscDiagProtocolBase::PR_DataPoolReadEventReceived.

   Called by specific protocol implementation when a response to a
   previously requested event driven transmission was received.

   Here:
   Extract data and store in data pool.

   \param[in]     opv_Instance         Pointer to the instance of C_OscDataDealerOsy
   \param[in]     ou8_DataPoolIndex    Datapool index
   \param[in]     ou8_ListIndex        List index
   \param[in]     ou8_ElementIndex     Element index
   \param[in]     orc_Value            Value of element stored in uint8 vector
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataDealer::mh_ReadDataPoolDataEventReceived(void * const opv_Instance, const uint8_t ou8_DataPoolIndex,
                                                       const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex,
                                                       const std::vector<uint8_t> & orc_Value)
{
   //lint -e{9079}  This class is the only one which registers itself at the caller of this function. It must match.
   C_OscDataDealer * const pc_Dealer = reinterpret_cast<C_OscDataDealer *>(opv_Instance);

   tgl_assert(pc_Dealer != NULL);
   if (pc_Dealer != NULL)
   {
      pc_Dealer->m_ReadDataPoolDataEventReceived(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, orc_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handling an async received read datapool data event

   Target of C_OscDiagProtocolBase::PR_DataPoolReadEventReceived.

   Called by specific protocol implementation when a response to a
   previously requested event driven transmission was received.

   Here:
   Extract data and store in data pool. Inform application.

   \param[in]     ou8_DataPoolIndex    Datapool index
   \param[in]     ou8_ListIndex        List index
   \param[in]     ou8_ElementIndex     Element index
   \param[in]     orc_Value            Value of element stored in uint8 vector
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataDealer::m_ReadDataPoolDataEventReceived(const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                                                      const uint16_t ou16_ElementIndex,
                                                      const std::vector<uint8_t> & orc_Value)
{
   //check for weird misconfiguration:
   if ((this->mpc_Node == NULL) || (this->mpc_DiagProtocol == NULL))
   {
      osc_write_log_warning("Asynchronous communication",
                            "C_OscDataDealer: ReadDataPool event received but no node or diagnostic protocol known!");
   }
   else
   {
      C_OscNodeDataPoolListElement * const pc_Element = this->mpc_Node->GetDataPoolListElement(ou8_DataPoolIndex,
                                                                                               ou16_ListIndex,
                                                                                               ou16_ElementIndex);
      if (pc_Element == NULL)
      {
         stw::scl::C_SclString c_Error;
         c_Error.PrintFormatted("C_OscDataDealer: ReadDataPool event received but referenced Datapool element (" \
                                "dp: %d list: %d element: %d) not known!", ou8_DataPoolIndex, ou16_ListIndex,
                                ou16_ElementIndex);
         osc_write_log_warning("Asynchronous communication", c_Error);
      }
      else
      {
         //we have data: is the size as expected ?
         //Not all protocols are able to report the exact size of incoming data.
         //Early openSYDE CAN server protocol implementations always would send 4 bytes even if not all of them were
         // used.
         //So we do not check for an exact match but for "do we have enough ?"
         if (orc_Value.size() < pc_Element->GetSizeByte())
         {
            stw::scl::C_SclString c_Error;
            c_Error.PrintFormatted("C_OscDataDealer: ReadDataPool event received but size does not match Datapool " \
                                   "element (dp: %d list: %d element: %d)!", ou8_DataPoolIndex, ou16_ListIndex,
                                   ou16_ElementIndex);
            osc_write_log_warning("Asynchronous communication", c_Error);
         }
         else
         {
            //size OK; cut off potential cruft
            std::vector<uint8_t> c_Data = orc_Value;
            c_Data.resize(pc_Element->GetSizeByte());

            //convert to native endianness depending on the type ...
            //no possible problem we did not check for already ...
            if (mpc_DiagProtocol->GetEndianness() == C_OscDiagProtocolBase::mhu8_ENDIANNESS_BIG)
            {
               (void)pc_Element->c_Value.SetValueFromBigEndianBlob(c_Data);
            }
            else
            {
               (void)pc_Element->c_Value.SetValueFromLittleEndianBlob(c_Data);
            }
            //inform application
            m_OnReadDataPoolEventReceived(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handling an async received negative read datapool data event

   Target of C_OscDiagProtocolBase::PR_DataPoolReadEventErrorReceived.

   Called by specific protocol implementation when a negative message response to a
   event driven transmission was received.

   Here:
   Error handling

   \param[in]     opv_Instance   Pointer to the instance of C_OscDataDealerOsy
   \param[in]     ou8_DataPoolIndex    Datapool index
   \param[in]     ou8_ListIndex        List index
   \param[in]     ou8_ElementIndex     Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataDealer::mh_ReadDataPoolDataEventErrorReceived(void * const opv_Instance, const uint8_t ou8_DataPoolIndex,
                                                            const uint16_t ou16_ListIndex,
                                                            const uint16_t ou16_ElementIndex, const uint8_t ou8_NrCode)
{
   //lint -e{9079}  This class is the only one which registers itself at the caller of this function. It must match.
   C_OscDataDealer * const pc_Dealer = reinterpret_cast<C_OscDataDealer *>(opv_Instance);

   tgl_assert(pc_Dealer != NULL);
   if (pc_Dealer != NULL)
   {
      pc_Dealer->m_ReadDataPoolDataEventErrorReceived(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, ou8_NrCode);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handling an async received negative read datapool data event

   Target of C_OscDiagProtocolBase::PR_DataPoolReadEventErrorReceived.

   Called by specific protocol implementation when a negative message response to a
   event driven transmission was received.

   Here:
   Check whether this is something we know about and inform application.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataDealer::m_ReadDataPoolDataEventErrorReceived(const uint8_t ou8_DataPoolIndex,
                                                           const uint16_t ou16_ListIndex,
                                                           const uint16_t ou16_ElementIndex, const uint8_t ou8_NrCode)
{
   //check for weird misconfiguration:
   if ((this->mpc_Node == NULL) || (this->mpc_DiagProtocol == NULL))
   {
      osc_write_log_warning("Asynchronous communication",
                            "C_OscDataDealer: ReadDataPool error event received but no node or diagnostic protocol known!");
   }
   else
   {
      const C_OscNodeDataPoolListElement * const pc_Element = this->mpc_Node->GetDataPoolListElement(ou8_DataPoolIndex,
                                                                                                     ou16_ListIndex,
                                                                                                     ou16_ElementIndex);
      if (pc_Element == NULL)
      {
         stw::scl::C_SclString c_Error;
         c_Error.PrintFormatted("C_OscDataDealer: ReadDataPool error event received but referenced Datapool element " \
                                "(dp: %d list: %d element: %d) not known!", ou8_DataPoolIndex, ou16_ListIndex,
                                ou16_ElementIndex);
         osc_write_log_warning("Asynchronous communication", c_Error);
      }
      else
      {
         //report error:
         m_OnReadDataPoolEventErrorReceived(ou8_DataPoolIndex, ou16_ListIndex, ou16_ElementIndex, ou8_NrCode);
      }
   }
}
