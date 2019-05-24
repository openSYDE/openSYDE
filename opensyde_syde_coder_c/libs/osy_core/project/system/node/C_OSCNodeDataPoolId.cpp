//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Datapool ID (implementation)

   Datapool ID

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OSCNodeDataPoolId.h"

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
/*! \brief   Default constructor
*/
//-----------------------------------------------------------------------------
C_OSCNodeDataPoolId::C_OSCNodeDataPoolId(void) :
   u32_NodeIndex(0U),
   u32_DataPoolIndex(0U)
{
}

//-----------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
*/
//-----------------------------------------------------------------------------
C_OSCNodeDataPoolId::C_OSCNodeDataPoolId(const stw_types::uint32 ou32_NodeIndex,
                                         const stw_types::uint32 ou32_DataPoolIndex) :
   u32_NodeIndex(ou32_NodeIndex),
   u32_DataPoolIndex(ou32_DataPoolIndex)
{
}

//-----------------------------------------------------------------------------
/*! \brief    Less operator.

   \param[in] orc_Cmp Compared instance

   \return
   true     Current smaller than orc_Cmp
   false    Else
*/
//-----------------------------------------------------------------------------
bool C_OSCNodeDataPoolId::operator <(const C_OSCNodeDataPoolId & orc_Cmp) const
{
   bool q_Return;

   if (this->u32_NodeIndex > orc_Cmp.u32_NodeIndex)
   {
      q_Return = false;
   }
   else if (this->u32_NodeIndex == orc_Cmp.u32_NodeIndex)
   {
      if (this->u32_DataPoolIndex >= orc_Cmp.u32_DataPoolIndex)
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

   return q_Return;
}

//-----------------------------------------------------------------------------
/*! \brief    Equal operator.

   \param[in] orc_Cmp Compared instance

   \return
   true     Current equals orc_Cmp
   false    Else
*/
//-----------------------------------------------------------------------------
bool C_OSCNodeDataPoolId::operator ==(const C_OSCNodeDataPoolId & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->u32_NodeIndex == orc_Cmp.u32_NodeIndex) &&
       (this->u32_DataPoolIndex == orc_Cmp.u32_DataPoolIndex))
   {
      q_Return = true;
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*! \brief    Equal operator.

   \param[in] orc_Cmp Compared instance

   \return
   true     Current does not equal orc_Cmp
   false    Else
*/
//-----------------------------------------------------------------------------
bool C_OSCNodeDataPoolId::operator !=(const C_OSCNodeDataPoolId & orc_Cmp) const
{
   bool q_Return = !C_OSCNodeDataPoolId::operator ==(orc_Cmp);

   return q_Return;
}
