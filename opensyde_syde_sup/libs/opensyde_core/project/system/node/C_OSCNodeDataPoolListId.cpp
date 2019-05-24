//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Datapool list ID (implementation)

   Datapool list ID

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <stddef.h>

#include "C_OSCNodeDataPoolListId.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_core;

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
C_OSCNodeDataPoolListId::C_OSCNodeDataPoolListId(void) :
   C_OSCNodeDataPoolId(),
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
C_OSCNodeDataPoolListId::C_OSCNodeDataPoolListId(const stw_types::uint32 ou32_NodeIndex,
                                                 const stw_types::uint32 ou32_DataPoolIndex,
                                                 const stw_types::uint32 ou32_ListIndex) :
   C_OSCNodeDataPoolId(ou32_NodeIndex, ou32_DataPoolIndex),
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
bool C_OSCNodeDataPoolListId::operator <(const C_OSCNodeDataPoolId & orc_Cmp) const
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
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         const C_OSCNodeDataPoolListId * const pc_NonBase =
            dynamic_cast<const C_OSCNodeDataPoolListId *>(&orc_Cmp);
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
bool C_OSCNodeDataPoolListId::operator ==(const C_OSCNodeDataPoolId & orc_Cmp) const
{
   bool q_Return = C_OSCNodeDataPoolId::operator ==(orc_Cmp);

   if (q_Return == true)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_OSCNodeDataPoolListId * const pc_NonBase =
         dynamic_cast<const C_OSCNodeDataPoolListId *>(&orc_Cmp);
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
