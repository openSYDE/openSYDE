//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for a complete data pool (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.12.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOL_H
#define C_OSCNODEDATAPOOL_H

/* -- Includes ------------------------------------------------------------- */

#include <vector>
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCNodeApplication.h"
#include "C_OSCNodeDataPoolList.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCNodeDataPool
{
public:
   C_OSCNodeDataPool(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   void CalcDefinitionHash(stw_types::uint32 & oru32_HashValue) const;

   void MoveList(const stw_types::uint32 & oru32_Start, const stw_types::uint32 & oru32_Target);
   void RecalculateAddress(void);
   stw_types::uint32 GetNumBytesUsed(void) const;
   stw_types::sint32 GetFreeBytes(void) const;
   stw_types::uint32 GetListsSize(void) const;
   void CheckErrorList(const stw_types::uint32 & oru_ListIndex, bool * const opq_NameConflict,
                       bool * const opq_NameInvalid, bool * const opq_UsageInvalid, bool * const opq_OutOfDataPool,
                       bool * const opq_DataSetsInvalid, bool * const opq_ElementsInvalid,
                       std::vector<stw_types::uint32> * const opc_InvalidDataSetIndices,
                       std::vector<stw_types::uint32> * const opc_InvalidElementIndices) const;

   enum E_Type ///< type of data pool
   {
      eDIAG, ///< Data pool type: diagnostic (contains/handles variables)
      eNVM,  ///< Data pool type: non-volatile memory (contains/handles parameters)
      eCOM   ///< Data pool type: communication (contains/handles signals)
   };

   E_Type e_Type;                               ///< Data pool type
   stw_scl::C_SCLString c_Name;                 ///< User data pool name
   stw_types::uint8 au8_Version[3];             ///< User data pool version Major, minor, release
   stw_scl::C_SCLString c_Comment;              ///< User data pool comment
   stw_types::sint32 s32_RelatedDataBlockIndex; ///< Related application index
   ///< -1, No application selected
   ///< Else valid application assumed
   bool q_IsSafety;                            ///< Flag if data pool contains safety relevant content
   stw_types::uint32 u32_NvMStartAddress;      ///< NvM start address of data pool
   stw_types::uint32 u32_NvMSize;              ///< NvM size of data pool
   std::vector<C_OSCNodeDataPoolList> c_Lists; ///< Data pool lists

private:
   stw_types::uint32 m_GetElementHash(const stw_types::uint32 ou32_ListIndex,
                                      const stw_types::uint32 ou32_ElementIndex) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
