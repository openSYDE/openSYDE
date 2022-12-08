//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Datapool list ID (implementation)

   Datapool list ID

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstddef>

#include "C_OscNodeDataPoolListId.hpp"

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
C_OscNodeDataPoolListId::C_OscNodeDataPoolListId(void) :
   C_OscNodeDataPoolId(),
   u32_ListIndex(0U)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolListId::C_OscNodeDataPoolListId(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                 const uint32_t ou32_ListIndex) :
   C_OscNodeDataPoolId(ou32_NodeIndex, ou32_DataPoolIndex),
   u32_ListIndex(ou32_ListIndex)
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
bool C_OscNodeDataPoolListId::operator <(const C_OscNodeDataPoolId & orc_Cmp) const
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
         const C_OscNodeDataPoolListId * const pc_NonBase =
            dynamic_cast<const C_OscNodeDataPoolListId *>(&orc_Cmp);
         //Not current class, assume base comparison is correct
         if (pc_NonBase != NULL)
         {
            //If this class check members as well
            if (this->u32_ListIndex >= pc_NonBase->u32_ListIndex)
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
bool C_OscNodeDataPoolListId::operator ==(const C_OscNodeDataPoolId & orc_Cmp) const
{
   bool q_Return = C_OscNodeDataPoolId::operator ==(orc_Cmp);

   if (q_Return == true)
   {
      const C_OscNodeDataPoolListId * const pc_NonBase =
         dynamic_cast<const C_OscNodeDataPoolListId *>(&orc_Cmp);
      //Not current class, assume base comparison is correct
      if (pc_NonBase != NULL)
      {
         //If this class check members as well
         if (this->u32_ListIndex == pc_NonBase->u32_ListIndex)
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
