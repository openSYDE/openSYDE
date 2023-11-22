//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for a complete data pool (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOL_HPP
#define C_OSCNODEDATAPOOL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscNodeApplication.hpp"
#include "C_OscNodeDataPoolList.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscNodeDataPool
{
public:
   C_OscNodeDataPool(void);

   enum E_CrcType ///< Type of CRC for data pool
   {
      eCT_COMPAT_V1,                 ///< CRC type: compatibility with V1 calculation
      eCT_NON_NVM,                   ///< CRC type: for non NVM data pools
      eCT_NON_NVM_DEFAULT_COMPAT_V1, ///< CRC type: for non NVM data pools with V1 calculation and default values
      eCT_NVM                        ///< CRC type: for NVM data pools
   };

   void CalcHash(uint32_t & oru32_HashValue) const;
   void CalcGeneratedDefinitionHash(uint32_t & oru32_HashValue) const;
   void CalcDefinitionHash(uint32_t & oru32_HashValue, const E_CrcType oe_CrcType) const;

   void MoveList(const uint32_t & oru32_Start, const uint32_t & oru32_Target);
   void RecalculateAddress(void);
   uint32_t GetNumBytesUsed(void) const;
   int32_t GetFreeBytes(void) const;
   uint32_t GetListsSize(void) const;
   void CheckErrorList(const uint32_t & oru32_ListIndex, bool * const opq_NameConflict, bool * const opq_NameInvalid,
                       bool * const opq_UsageInvalid, bool * const opq_OutOfDataPool, bool * const opq_DataSetsInvalid,
                       bool * const opq_ElementsInvalid, std::vector<uint32_t> * const opc_InvalidDataSetIndices,
                       std::vector<uint32_t> * const opc_InvalidElementIndices) const;

   enum E_Type ///< Type of data pool
   {
      eDIAG,    ///< Data pool type: diagnostic (contains/handles variables)
      eNVM,     ///< Data pool type: non-volatile memory (contains/handles parameters)
      eCOM,     ///< Data pool type: communication (contains/handles signals)
      eHALC,    ///< Data pool type: HALC (contains/handles HALC related variables)
      eHALC_NVM ///< Data pool type: non-volatile memory for HALC (contains/handles HALC related parameters)
   };

   E_Type e_Type;                     ///< Data pool type
   stw::scl::C_SclString c_Name;      ///< User data pool name
   uint8_t au8_Version[3];            ///< User data pool version Major, minor, release
   uint16_t u16_DefinitionCrcVersion; ///< Version of data pool definition CRC
   stw::scl::C_SclString c_Comment;   ///< User data pool comment
   int32_t s32_RelatedDataBlockIndex; ///< Related application index
   ///< -1, No application selected
   ///< Else valid application assumed
   bool q_IsSafety;                             ///< Flag if data pool contains safety relevant content
   bool q_ScopeIsPrivate;                       ///< Flag if data pools scope of content is private (vs. public)
   uint32_t u32_NvmStartAddress;                ///< NvM start address of data pool
   uint32_t u32_NvmSize;                        ///< NvM size of data pool
   static const uint32_t hu32_DEFAULT_NVM_SIZE; ///< Default NvM size of data pool
   std::vector<C_OscNodeDataPoolList> c_Lists;  ///< Data pool lists

private:
   uint32_t m_GetElementHash(const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
