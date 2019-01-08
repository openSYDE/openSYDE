//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for a list in a data pool (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.12.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLLIST_H
#define C_OSCNODEDATAPOOLLIST_H

/* -- Includes ------------------------------------------------------------- */
#include <iostream>
#include <vector>
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCNodeDataPoolListElement.h"
#include "C_OSCNodeDataPoolDataSet.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCNodeDataPoolList
{
public:
   C_OSCNodeDataPoolList(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   void RecalculateAddress(void);
   void MoveElement(const stw_types::uint32 & oru32_Start, const stw_types::uint32 & oru32_Target);
   stw_types::uint32 GetNumBytesUsed(void) const;
   stw_types::sint32 GetFreeBytes(void) const;
   void CheckErrorDataSet(const stw_types::uint32 & oru32_DataSetIndex, bool * const opq_NameConflict,
                          bool * const opq_NameInvalid) const;
   void CheckErrorElement(const stw_types::uint32 & oru32_ElementIndex, bool * const opq_NameConflict,
                          bool * const opq_NameInvalid, bool * const opq_MinOverMax,
                          bool * const opq_DataSetValueInvalid,
                          std::vector<stw_types::uint32> * const opc_InvalidDataSetIndices) const;
   void CheckErrorDataSetValue(const stw_types::uint32 & oru32_ElementIndex,
                               const stw_types::uint32 & oru32_DataSetIndex, bool * const opq_ValueBelowMin,
                               bool * const opq_ValueOverMax, const stw_types::uint32 * const opu32_ArrayIndex) const;

   stw_types::sint32 SetCRCFromBigEndianBlob(const std::vector<stw_types::uint8> & orc_Data);
   stw_types::sint32 SetCRCFromLittleEndianBlob(const std::vector<stw_types::uint8> & orc_Data);
   void GetCRCAsBigEndianBlob(std::vector<stw_types::uint8> & orc_Data) const;
   void GetCRCAsLittleEndianBlob(std::vector<stw_types::uint8> & orc_Data) const;

   stw_scl::C_SCLString c_Name;                          ///< User data list name
   stw_scl::C_SCLString c_Comment;                       ///< User data list comment
   bool q_NvMCRCActive;                                  ///< Flag if NvM list checksum is used
   stw_types::uint32 u32_NvMCRC;                         ///< NvM list checksum
   stw_types::uint32 u32_NvMStartAddress;                ///< NvM start address of data list
   stw_types::uint32 u32_NvMSize;                        ///< NvM size of data list
   std::vector<C_OSCNodeDataPoolListElement> c_Elements; ///< List variables
   std::vector<C_OSCNodeDataPoolDataSet> c_DataSets;     ///< Data sets
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
