//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group for message ID part which identifies a unique CAN message

   Group for message ID part which identifies a unique CAN message

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscCanMessageUniqueId.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanMessageUniqueId::C_OscCanMessageUniqueId() :
   u32_CanId(0x7FF),
   q_IsExtended(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Alternative constructor to immediately initialize

   \param[in]  ou32_CanId     Can id
   \param[in]  oq_IsExtended  Is extended
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanMessageUniqueId::C_OscCanMessageUniqueId(const uint32_t ou32_CanId, const bool oq_IsExtended) :
   u32_CanId(ou32_CanId),
   q_IsExtended(oq_IsExtended)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Less operator.

   \param[in]  orc_Cmp  Compared instance

   \return
   true     Current smaller than orc_Cmp
   false    Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanMessageUniqueId::operator <(const C_OscCanMessageUniqueId & orc_Cmp) const
{
   bool q_Return;

   if (this->u32_CanId > orc_Cmp.u32_CanId)
   {
      q_Return = false;
   }
   else if (this->u32_CanId == orc_Cmp.u32_CanId)
   {
      if ((this->q_IsExtended == orc_Cmp.q_IsExtended) || (this->q_IsExtended == true))
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

   \param[in]  orc_Cmp  Compared instance

   \return
   true     Current equals orc_Cmp
   false    Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanMessageUniqueId::operator ==(const C_OscCanMessageUniqueId & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->u32_CanId == orc_Cmp.u32_CanId) &&
       (this->q_IsExtended == orc_Cmp.q_IsExtended))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Not equal operator.

   \param[in]  orc_Cmp  Compared instance

   \return
   true     Current does not equal orc_Cmp
   false    Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanMessageUniqueId::operator !=(const C_OscCanMessageUniqueId & orc_Cmp) const
{
   const bool q_Return = !C_OscCanMessageUniqueId::operator ==(orc_Cmp);

   return q_Return;
}
