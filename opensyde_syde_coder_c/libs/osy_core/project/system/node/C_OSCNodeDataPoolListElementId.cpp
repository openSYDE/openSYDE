//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Utility class to store all indices to identify a data element stored in a node (implementation)

   Utility class to store all indices to identify a data element stored in a node

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.08.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <stddef.h>

#include "C_OSCNodeDataPoolListElementId.h"

#include "CSCLChecksums.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     22.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OSCNodeDataPoolListElementId::C_OSCNodeDataPoolListElementId(void) :
   C_OSCNodeDataPoolListId(),
   u32_ElementIndex(0U)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index
   \param[in] ou32_ElementIndex  Element index

   \created     30.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCNodeDataPoolListElementId::C_OSCNodeDataPoolListElementId(const stw_types::uint32 ou32_NodeIndex,
                                                               const stw_types::uint32 ou32_DataPoolIndex,
                                                               const stw_types::uint32 ou32_ListIndex,
                                                               const stw_types::uint32 ou32_ElementIndex) :
   C_OSCNodeDataPoolListId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex),
   u32_ElementIndex(ou32_ElementIndex)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief    Less operator.

   \param[in] orc_Cmp Compared instance

   \return
   true     Current smaller than orc_Cmp
   false    Else

   \created     23.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_OSCNodeDataPoolListElementId::operator <(const C_OSCNodeDataPoolListId & orc_Cmp) const
{
   bool q_Return;

   if (this->u32_NodeIndex > orc_Cmp.u32_NodeIndex)
   {
      q_Return = false;
   }
   else if (this->u32_NodeIndex == orc_Cmp.u32_NodeIndex)
   {
      if (this->u32_DataPoolIndex > orc_Cmp.u32_DataPoolIndex)
      {
         q_Return = false;
      }
      else if (this->u32_DataPoolIndex == orc_Cmp.u32_DataPoolIndex)
      {
         if (this->u32_ListIndex > orc_Cmp.u32_ListIndex)
         {
            q_Return = false;
         }
         else if (this->u32_ListIndex == orc_Cmp.u32_ListIndex)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            const C_OSCNodeDataPoolListElementId * const pc_NonBase =
               dynamic_cast<const C_OSCNodeDataPoolListElementId *>(&orc_Cmp);
            //Not current class, assume base comparison is correct
            if (pc_NonBase != NULL)
            {
               //If this class check members as well
               if (this->u32_ElementIndex >= pc_NonBase->u32_ElementIndex)
               {
                  q_Return = false;
               }
               else
               {
                  q_Return = true;
               }
            }
            else
            {
               q_Return = true;
            }
         }
         else
         {
            q_Return = true;
         }
      }
      else
      {
         q_Return = true;
      }
   }
   else
   {
      q_Return = true;
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Equal operator.

   \param[in] orc_Cmp Compared instance

   \return
   true     Current equals orc_Cmp
   false    Else

   \created     20.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_OSCNodeDataPoolListElementId::operator ==(const C_OSCNodeDataPoolListId & orc_Cmp) const
{
   bool q_Return = C_OSCNodeDataPoolListId::operator ==(orc_Cmp);

   if (q_Return == true)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_OSCNodeDataPoolListElementId * const pc_NonBase =
         dynamic_cast<const C_OSCNodeDataPoolListElementId *>(&orc_Cmp);
      //Not current class, assume base comparison is correct
      if (pc_NonBase != NULL)
      {
         //If this class check members as well
         if (this->u32_ElementIndex == pc_NonBase->u32_ElementIndex)
         {
            q_Return = true;
         }
         else
         {
            q_Return = false;
         }
      }
   }
   else
   {
      q_Return = false;
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with initial [in] value and result [out] value

   \created     24.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCNodeDataPoolListElementId::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_NodeIndex, sizeof(this->u32_NodeIndex), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_DataPoolIndex, sizeof(this->u32_DataPoolIndex), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_ListIndex, sizeof(this->u32_ListIndex), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_ElementIndex, sizeof(this->u32_ElementIndex), oru32_HashValue);
}
