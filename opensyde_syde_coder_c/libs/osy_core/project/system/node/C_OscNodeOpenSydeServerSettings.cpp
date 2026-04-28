//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for openSYDE server specific settings(implementation)

   Data class for openSYDE server specific settings

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscNodeOpenSydeServerSettings.hpp"

#include "C_SclChecksums.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeOpenSydeServerSettings::C_OscNodeOpenSydeServerSettings(void)
{
   this->Initialize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize class content

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeOpenSydeServerSettings::Initialize(void)
{
   u8_MaxClients = 1U;
   u8_MaxParallelTransmissions = 64U;
   s16_DpdDataBlockIndex = -1;
   u16_MaxMessageBufferTx = 585U;
   u16_MaxRoutingMessageBufferRx = 585U;
   e_MaxServiceSizeMode = eMSMT_AUTO;
   u16_MaxServiceSizeByte = 4096U;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeOpenSydeServerSettings::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_MaxClients, sizeof(this->u8_MaxClients), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_MaxParallelTransmissions, sizeof(this->u8_MaxParallelTransmissions),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->s16_DpdDataBlockIndex, sizeof(this->s16_DpdDataBlockIndex),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u16_MaxMessageBufferTx, sizeof(this->u16_MaxMessageBufferTx),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u16_MaxRoutingMessageBufferRx,
                                       sizeof(this->u16_MaxRoutingMessageBufferRx),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_MaxServiceSizeMode,
                                       sizeof(this->e_MaxServiceSizeMode),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u16_MaxServiceSizeByte,
                                       sizeof(this->u16_MaxServiceSizeByte),
                                       oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get transport buffer size in byte

   \param[in]  orc_DataPools                             Data pools
   \param[in]  ou16_ProtocolDriverOsyMaximumServiceSize  Just pass
                                                         C_OscProtocolDriverOsyTpBase::hu16_OSY_MAXIMUM_SERVICE_SIZE

   \return
   Transport buffer size in byte
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscNodeOpenSydeServerSettings::GetTransportBufferSizeInByte(
   const std::vector<C_OscNodeDataPool> & orc_DataPools, const uint16_t ou16_ProtocolDriverOsyMaximumServiceSize)
const
{
   uint32_t u32_Retval;

   if (this->e_MaxServiceSizeMode == eMSMT_MANUAL)
   {
      u32_Retval = this->u16_MaxServiceSizeByte;
   }
   else
   {
      u32_Retval = h_GetTransportBufferSizeInByteWithAutoCalculation(orc_DataPools,
                                                                     ou16_ProtocolDriverOsyMaximumServiceSize);
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get transport buffer size in byte with auto calculation

   \param[in]  orc_DataPools                             Data pools
   \param[in]  ou16_ProtocolDriverOsyMaximumServiceSize  Just pass
                                                         C_OscProtocolDriverOsyTpBase::hu16_OSY_MAXIMUM_SERVICE_SIZE

   \return
   Transport buffer size in byte with auto calculation
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscNodeOpenSydeServerSettings::h_GetTransportBufferSizeInByteWithAutoCalculation(
   const std::vector<C_OscNodeDataPool> & orc_DataPools, const uint16_t ou16_ProtocolDriverOsyMaximumServiceSize)
{
   //get size of greatest element so we know how to set up the buffers
   //add protocol overhead for DPD and NVM access services (greatest overhead: write_memory_by_address)
   uint32_t u32_BufferSize = mh_GetSizeOfLargestDataPoolElementByte(orc_DataPools) + 11U;

   //add an additional 2 bytes; compensates for an buffer size issue with older server implementations; #62305
   u32_BufferSize += 2U;
   //consider minimum for non-DP services
   if (u32_BufferSize < hu8_MIN_SIZE_DPD_BUF_INSTANCE)
   {
      u32_BufferSize = hu8_MIN_SIZE_DPD_BUF_INSTANCE;
   }
   //limit to maximum service size; larger access must be segmented by protocol driver
   if (u32_BufferSize > ou16_ProtocolDriverOsyMaximumServiceSize)
   {
      u32_BufferSize = ou16_ProtocolDriverOsyMaximumServiceSize;
   }
   return u32_BufferSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: get size of greatest Datapool element or list

   For NVM Datapools:
   *  Go through vector of Datapools and check which is the greatest list
   For other Datapools:
   *  Go through vector of Datapools and check which is the greatest element

   This is used for defining the buffer size for the protocol driver. So the function considers local and remote
    Datapools.

   \param[in]  orc_DataPools  Datapools to scan

   \return
   size of the greatest Datapool element/list in bytes
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscNodeOpenSydeServerSettings::mh_GetSizeOfLargestDataPoolElementByte(
   const std::vector<C_OscNodeDataPool> & orc_DataPools)
{
   uint32_t u32_GreatestSize = 0U;

   for (uint32_t u32_DataPool = 0U; u32_DataPool < orc_DataPools.size(); u32_DataPool++)
   {
      for (uint32_t u32_List = 0U; u32_List < orc_DataPools[u32_DataPool].c_Lists.size(); u32_List++)
      {
         if ((orc_DataPools[u32_DataPool].e_Type == C_OscNodeDataPool::eNVM) ||
             (orc_DataPools[u32_DataPool].e_Type == C_OscNodeDataPool::eHALC_NVM))
         {
            const uint32_t u32_NumBytesUsed = orc_DataPools[u32_DataPool].c_Lists[u32_List].GetNumBytesUsed();
            if (u32_NumBytesUsed > u32_GreatestSize)
            {
               u32_GreatestSize = u32_NumBytesUsed;
            }
         }
         else
         {
            for (uint32_t u32_Element = 0U;
                 u32_Element < orc_DataPools[u32_DataPool].c_Lists[u32_List].c_Elements.size();
                 u32_Element++)
            {
               const uint32_t u32_Size =
                  orc_DataPools[u32_DataPool].c_Lists[u32_List].c_Elements[u32_Element].GetSizeByte();
               if (u32_Size > u32_GreatestSize)
               {
                  u32_GreatestSize = u32_Size;
               }
            }
         }
      }
   }

   return u32_GreatestSize;
}
