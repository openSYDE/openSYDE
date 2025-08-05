//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Datapool ID (implementation)

   Datapool ID

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscNodeDataPoolId.hpp"

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
C_OscNodeDataPoolId::C_OscNodeDataPoolId(void) :
   u32_NodeIndex(0U),
   u32_DataPoolIndex(0U)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolId::~C_OscNodeDataPoolId()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolId::C_OscNodeDataPoolId(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex) :
   u32_NodeIndex(ou32_NodeIndex),
   u32_DataPoolIndex(ou32_DataPoolIndex)
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
bool C_OscNodeDataPoolId::operator <(const C_OscNodeDataPoolId & orc_Cmp) const
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Equal operator.

   \param[in] orc_Cmp Compared instance

   \return
   true     Current equals orc_Cmp
   false    Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolId::operator ==(const C_OscNodeDataPoolId & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->u32_NodeIndex == orc_Cmp.u32_NodeIndex) &&
       (this->u32_DataPoolIndex == orc_Cmp.u32_DataPoolIndex))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Equal operator.

   \param[in] orc_Cmp Compared instance

   \return
   true     Current does not equal orc_Cmp
   false    Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolId::operator !=(const C_OscNodeDataPoolId & orc_Cmp) const
{
   const bool q_Return = !C_OscNodeDataPoolId::operator ==(orc_Cmp);

   return q_Return;
}
