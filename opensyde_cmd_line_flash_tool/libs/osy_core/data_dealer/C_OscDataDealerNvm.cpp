//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Encapsulates non-trivial sequences for NVM access.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstring>
#include "stwtypes.hpp"

#include "stwerrors.hpp"
#include "C_OscDataDealerNvm.hpp"
#include "C_SclChecksums.hpp"

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
C_OscDataDealerNvm::C_OscDataDealerNvm(void) :
   C_OscDataDealer()
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
C_OscDataDealerNvm::C_OscDataDealerNvm(C_OscNode * const opc_Node, const uint32_t ou32_NodeIndex,
                                       C_OscDiagProtocolBase * const opc_DiagProtocol) :
   C_OscDataDealer(opc_Node, ou32_NodeIndex, opc_DiagProtocol)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up class
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscDataDealerNvm::~C_OscDataDealerNvm(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the CRC for the list

   The 16 bit CRC_CCITT is used with start value 0x1D0F.

   \param[in]     orc_List       List to calculate CRC over

   \return
   Calculated CRC
*/
//----------------------------------------------------------------------------------------------------------------------
uint16_t C_OscDataDealerNvm::NvmCalcCrc(const C_OscNodeDataPoolList & orc_List) const
{
   uint16_t u16_Crc = 0x1D0FU;

   if (this->mpc_DiagProtocol != NULL)
   {
      uint32_t u32_Counter;
      const uint8_t u8_Endianness = this->mpc_DiagProtocol->GetEndianness();

      for (u32_Counter = 0U; u32_Counter < orc_List.c_Elements.size(); ++u32_Counter)
      {
         const C_OscNodeDataPoolListElement * const pc_Element = &orc_List.c_Elements[u32_Counter];
         std::vector<uint8_t> c_Data;

         //convert to native endianness depending on the type ...
         //no possible problem we did not check for already ...
         if (u8_Endianness == C_OscDiagProtocolBase::mhu8_ENDIANNESS_BIG)
         {
            pc_Element->c_NvmValue.GetValueAsBigEndianBlob(c_Data);
         }
         else
         {
            pc_Element->c_NvmValue.GetValueAsLittleEndianBlob(c_Data);
         }
         stw::scl::C_SclChecksums::CalcCRC16(&c_Data[0], static_cast<uint32_t>(c_Data.size()), u16_Crc);
      }
   }

   return u16_Crc;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reads a specific NVM list of ECU

   The CRC of the list will be checked.

   \param[in]     ou32_DataPoolIndex   Node datapool index
   \param[in]     ou32_ListIndex       Node datapool list index
   \param[out]    opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR    Reading of list successful
   C_CONFIG    no node or diagnostic protocol are known (was this class properly Initialize()d ?)
               protocol driver reported configuration error (was the protocol driver properly initialized ?)
   C_RANGE     Input parameter invalid
   C_OVERFLOW  List has no elements. Nothing to read
   C_RD_WR     Datapool element size configuration does not match with count of read bytes
   C_CHECKSUM  Checksum of read datapool list is invalid
   C_TIMEOUT   Expected response not received within timeout
   C_NOACT     Could not send request (e.g. Tx buffer full)
   C_WARN      Error response or malformed protocol response
   C_COM       Pre-requisites not correct; e.g. driver not initialized or
               parameter out of range (checked by client side)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataDealerNvm::NvmReadList(const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex,
                                        uint8_t * const opu8_NrCode)
{
   int32_t s32_Return;

   if ((mpc_Node == NULL) || (mpc_DiagProtocol == NULL))
   {
      s32_Return = C_CONFIG;
   }
   else if ((this->mpc_Node->c_DataPools.size() > ou32_DataPoolIndex) &&
            (this->mpc_Node->c_DataPools[ou32_DataPoolIndex].c_Lists.size() > ou32_ListIndex))
   {
      std::vector<uint8_t> c_Values;
      C_OscNodeDataPoolList & rc_List = this->mpc_Node->c_DataPools[ou32_DataPoolIndex].c_Lists[ou32_ListIndex];

      s32_Return = this->m_NvmReadListRaw(rc_List, c_Values, opu8_NrCode);

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = this->m_SaveDumpToList(c_Values, rc_List);
      }
   }
   else
   {
      s32_Return = C_RANGE;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Notify server application about NVM data changes

   \param[in]     ou8_DataPoolIndex             Node datapool index
   \param[in]     ou8_ListIndex                 Node datapool list index
   \param[out]    orq_ApplicationAcknowledge    True: positive acknowledge from server
                                                False: negative acknowledge from server
   \param[out]    opu8_NrCode                   if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   Request sent, positive response received
   C_CONFIG   no node or diagnostic protocol are known (was this class properly Initialize()d ?)
              protocol driver reported configuration error (was the protocol driver properly initialized ?)
   C_TIMEOUT  Expected server response not received within timeout
   C_NOACT    Could not send request (e.g. Tx buffer full)
   C_WARN     Server sent error response
   C_RD_WR    unexpected content in server response (here: wrong data pool index)
   C_COM      communication driver reported error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataDealerNvm::NvmNotifyOfChanges(const uint8_t ou8_DataPoolIndex, const uint8_t ou8_ListIndex,
                                               bool & orq_ApplicationAcknowledge, uint8_t * const opu8_NrCode)
{
   int32_t s32_Return;

   if ((mpc_Node == NULL) || (mpc_DiagProtocol == NULL))
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      s32_Return = this->mpc_DiagProtocol->NvmNotifyOfChanges(ou8_DataPoolIndex, ou8_ListIndex,
                                                              orq_ApplicationAcknowledge, opu8_NrCode);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reads the dump of a specific NVM list of ECU

   \param[in]      orc_List       List with read information
   \param[out]     orc_Values     List dump
   \param[out]    opu8_NrCode          if != NULL: negative response code in case of an error response

   \return
   C_NO_ERR   Request sent, positive response received
   C_OVERFLOW List has no elements. Nothing to read
   C_TIMEOUT  Expected response not received within timeout
   C_NOACT    Could not send request (e.g. Tx buffer full)
   C_WARN     Error response
   C_CONFIG   Pre-requisites not correct; e.g. driver not initialized or
              parameter out of range (checked by client side)
   C_COM      expected server response not received because of communication error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataDealerNvm::m_NvmReadListRaw(const C_OscNodeDataPoolList & orc_List, std::vector<uint8_t> & orc_Values,
                                             uint8_t * const opu8_NrCode)
{
   int32_t s32_Return;

   // If CRC is active, at least 2 byte are necessary for the CRC
   if ((orc_List.u32_NvmSize > 2U) ||
       ((orc_List.u32_NvmSize > 0U) && (orc_List.q_NvmCrcActive == false)))
   {
      // Size is input parameter for NvmRead
      const uint32_t u32_NumBytesToRead = orc_List.GetNumBytesUsed();
      orc_Values.resize(u32_NumBytesToRead);

      // Read the entire list
      // TODO: KEFEX Protocol dependent count of calls necessary and problems with fragmented lists im NVM memory
      // TODO: KEFEX Protocol dependent position of CRC!
      s32_Return = this->mpc_DiagProtocol->NvmRead(orc_List.u32_NvmStartAddress, orc_Values, opu8_NrCode);

      // Adapt return value
      s32_Return = C_OscDataDealerNvm::mh_AdaptProtocolReturnValue(s32_Return);
   }
   else
   {
      s32_Return = C_OVERFLOW;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Interprets the value and if necessary the CRC of the container orc_Values into the datapool list

   If CRC is active, the CRC will be checked and updated.

   \param[in]     orc_Values     List dump
   \param[in,out] orc_List       List for filling up

   \return
   C_NO_ERR    Filling of list successful
   C_RD_WR     Datapool element size configuration does not match with count of read bytes
   C_CHECKSUM  Checksum of read datapool list is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataDealerNvm::m_SaveDumpToList(std::vector<uint8_t> & orc_Values, C_OscNodeDataPoolList & orc_List) const
{
   int32_t s32_Return = this->m_SaveDumpValuesToListValues(orc_Values, orc_List);

   if ((orc_List.q_NvmCrcActive == true) && (s32_Return == C_NO_ERR))
   {
      if (orc_Values.size() >= 2)
      {
         // Update CRC
         std::vector<uint8_t> c_CrcData;
         uint16_t u16_CalcCrc;

         c_CrcData.resize(2);
         // TODO: KEFEX position of CRC can be different
         (void)std::memcpy(&c_CrcData[0], &orc_Values[0], 2);

         if (this->mpc_DiagProtocol->GetEndianness() == C_OscDiagProtocolBase::mhu8_ENDIANNESS_BIG)
         {
            orc_List.SetCrcFromBigEndianBlob(c_CrcData);
         }
         else
         {
            orc_List.SetCrcFromLittleEndianBlob(c_CrcData);
         }

         // Check CRC
         u16_CalcCrc = this->NvmCalcCrc(orc_List);
         if (u16_CalcCrc != orc_List.u32_NvmCrc)
         {
            s32_Return = C_CHECKSUM;
         }
      }
      else
      {
         s32_Return = C_CHECKSUM;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Interprets the value of the container orc_Values into the NVM datapool list

   The flag q_IsValid of each successfully read element will be set to true.
   The CRC will not be checked, calculated and updated.

   \param[in]     orc_Values     List dump
   \param[in,out] orc_List       List for filling up

   \return
   C_NO_ERR    Filling of list successful
   C_RD_WR     Datapool element size configuration does not match with count of read bytes
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataDealerNvm::m_SaveDumpValuesToListValues(std::vector<uint8_t> & orc_Values,
                                                         C_OscNodeDataPoolList & orc_List) const
{
   int32_t s32_Return = C_NO_ERR;
   uint32_t u32_Counter;

   for (u32_Counter = 0U; u32_Counter < orc_List.c_Elements.size(); ++u32_Counter)
   {
      C_OscNodeDataPoolListElement * const pc_Element = &orc_List.c_Elements[u32_Counter];
      // Index of data container
      const uint32_t u32_SizeElement = pc_Element->GetSizeByte();
      const uint32_t u32_Index = pc_Element->u32_NvmStartAddress - orc_List.u32_NvmStartAddress;

      if (orc_Values.size() >= (static_cast<size_t>(u32_Index) + u32_SizeElement))
      {
         std::vector<uint8_t> c_ElementData;

         // Get the relevant data for this element
         c_ElementData.resize(u32_SizeElement);
         (void)std::memcpy(&c_ElementData[0], &orc_Values[u32_Index], u32_SizeElement);

         //we have data
         //convert to native endianness depending on the type ...
         //no possible problem we did not check for already ...
         if (this->mpc_DiagProtocol->GetEndianness() == C_OscDiagProtocolBase::mhu8_ENDIANNESS_BIG)
         {
            (void)pc_Element->c_NvmValue.SetValueFromBigEndianBlob(c_ElementData);
         }
         else
         {
            (void)pc_Element->c_NvmValue.SetValueFromLittleEndianBlob(c_ElementData);
         }
         pc_Element->q_NvmValueIsValid = true;
      }
      else
      {
         s32_Return = C_RD_WR;
         break;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the return value of a diag protocol base function

   \param[in]     os32_ProtReturnValue   Return value of diag protocol base function

   \return
   C_NO_ERR   Request sent, positive response received
   C_TIMEOUT  Expected response not received within timeout
   C_NOACT    Could not send request (e.g. Tx buffer full)
   C_WARN     Error response or malformed protocol response
   C_COM      Expected server response not received because of communication error
   C_CONFIG   Pre-requisites not correct; e.g. driver not initialized or
              parameter out of range (checked by client side)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDataDealerNvm::mh_AdaptProtocolReturnValue(const int32_t os32_ProtReturnValue)
{
   int32_t s32_Return;

   switch (os32_ProtReturnValue)
   {
   case C_NO_ERR:  // No adaptation necessary
   case C_TIMEOUT: // No adaptation necessary
   case C_WARN:    // No adaptation necessary
   case C_NOACT:   // No adaptation necessary
   case C_COM:     // No adaptation necessary
      // Nothing to adapt
      s32_Return = os32_ProtReturnValue;
      break;
   case C_RD_WR:
      s32_Return = C_WARN;
      break;
   default:
      // All other errors
      s32_Return = C_CONFIG;
      break;
   }

   return s32_Return;
}
