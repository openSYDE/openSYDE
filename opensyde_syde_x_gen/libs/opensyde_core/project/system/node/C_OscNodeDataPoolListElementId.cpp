//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to store all indices to identify a data element stored in a node (implementation)

   Utility class to store all indices to identify a data element stored in a node

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstddef>

#include "C_OscNodeDataPoolListElementId.hpp"

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
C_OscNodeDataPoolListElementId::C_OscNodeDataPoolListElementId(void) :
   C_OscNodeDataPoolListId(),
   u32_ElementIndex(0U)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index
   \param[in] ou32_ElementIndex  Element index
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolListElementId::C_OscNodeDataPoolListElementId(const uint32_t ou32_NodeIndex,
                                                               const uint32_t ou32_DataPoolIndex,
                                                               const uint32_t ou32_ListIndex,
                                                               const uint32_t ou32_ElementIndex) :
   C_OscNodeDataPoolListId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex),
   u32_ElementIndex(ou32_ElementIndex)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Less operator.

   \param[in] orc_Cmp Compared instance

   \return
   true     Current smaller than orc_Cmp
   false    Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolListElementId::operator <(const C_OscNodeDataPoolId & orc_Cmp) const
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
         const C_OscNodeDataPoolListId * const pc_NonBaseList =
            dynamic_cast<const C_OscNodeDataPoolListId *>(&orc_Cmp);
         //Not current class, assume base comparison is correct
         if (pc_NonBaseList != NULL)
         {
            if (this->u32_ListIndex > pc_NonBaseList->u32_ListIndex)
            {
               q_Return = false;
            }
            else if (this->u32_ListIndex == pc_NonBaseList->u32_ListIndex)
            {
               const C_OscNodeDataPoolListElementId * const pc_NonBaseElement =
                  dynamic_cast<const C_OscNodeDataPoolListElementId *>(&orc_Cmp);
               //Not current class, assume base comparison is correct
               if (pc_NonBaseElement != NULL)
               {
                  //If this class check members as well
                  if (this->u32_ElementIndex >= pc_NonBaseElement->u32_ElementIndex)
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
                  q_Return = false;
               }
            }
            else
            {
               q_Return = true;
            }
         }
         else
         {
            q_Return = false;
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Equal operator.

   \param[in] orc_Cmp Compared instance

   \return
   true     Current equals orc_Cmp
   false    Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolListElementId::operator ==(const C_OscNodeDataPoolId & orc_Cmp) const
{
   bool q_Return = C_OscNodeDataPoolListId::operator ==(orc_Cmp);

   if (q_Return == true)
   {
      const C_OscNodeDataPoolListElementId * const pc_NonBase =
         dynamic_cast<const C_OscNodeDataPoolListElementId *>(&orc_Cmp);
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolListElementId::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_NodeIndex, sizeof(this->u32_NodeIndex), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_DataPoolIndex, sizeof(this->u32_DataPoolIndex), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_ListIndex, sizeof(this->u32_ListIndex), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_ElementIndex, sizeof(this->u32_ElementIndex), oru32_HashValue);
}
