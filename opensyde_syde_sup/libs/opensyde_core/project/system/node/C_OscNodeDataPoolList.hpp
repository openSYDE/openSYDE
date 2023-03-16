//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for a list in a data pool (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLLIST_HPP
#define C_OSCNODEDATAPOOLLIST_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <iostream>
#include <vector>
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscNodeDataPoolListElement.hpp"
#include "C_OscNodeDataPoolDataSet.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscNodeDataPoolList
{
public:
   C_OscNodeDataPoolList(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   void RecalculateAddress(void);
   void MoveElement(const uint32_t & oru32_Start, const uint32_t & oru32_Target);
   uint32_t GetNumBytesUsed(void) const;
   int32_t GetFreeBytes(void) const;
   void CheckErrorDataSet(const uint32_t & oru32_DataSetIndex, bool * const opq_NameConflict,
                          bool * const opq_NameInvalid) const;
   void CheckErrorElement(const uint32_t & oru32_ElementIndex, bool * const opq_NameConflict,
                          bool * const opq_NameInvalid, bool * const opq_MinOverMax,
                          bool * const opq_DataSetValueInvalid,
                          std::vector<uint32_t> * const opc_InvalidDataSetIndices) const;
   void CheckErrorDataSetValue(const uint32_t & oru32_ElementIndex, const uint32_t & oru32_DataSetIndex,
                               bool * const opq_ValueBelowMin, bool * const opq_ValueOverMax,
                               const uint32_t * const opu32_ArrayIndex) const;

   int32_t SetCrcFromBigEndianBlob(const std::vector<uint8_t> & orc_Data);
   int32_t SetCrcFromLittleEndianBlob(const std::vector<uint8_t> & orc_Data);
   void GetCrcAsBigEndianBlob(std::vector<uint8_t> & orc_Data) const;
   void GetCrcAsLittleEndianBlob(std::vector<uint8_t> & orc_Data) const;

   stw::scl::C_SclString c_Name;                         ///< User data list name
   stw::scl::C_SclString c_Comment;                      ///< User data list comment
   bool q_NvmCrcActive;                                  ///< Flag if NvM list checksum is used
   uint32_t u32_NvmCrc;                                  ///< NvM list checksum
   uint32_t u32_NvmStartAddress;                         ///< NvM start address of data list
   uint32_t u32_NvmSize;                                 ///< NvM size of data list
   std::vector<C_OscNodeDataPoolListElement> c_Elements; ///< List variables
   std::vector<C_OscNodeDataPoolDataSet> c_DataSets;     ///< Data sets
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
