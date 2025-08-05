//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to group a unique node ID and CAN interace

   Utility class to group a unique node ID and CAN interace

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscCanInterfaceId.hpp"

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
C_OscCanInterfaceId::C_OscCanInterfaceId() :
   u32_NodeIndex(0),
   u8_InterfaceNumber(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou8_InterfaceNumber  Interface number
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanInterfaceId::C_OscCanInterfaceId(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber) :
   u32_NodeIndex(ou32_NodeIndex),
   u8_InterfaceNumber(ou8_InterfaceNumber)
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
bool C_OscCanInterfaceId::operator <(const C_OscCanInterfaceId & orc_Cmp) const
{
   bool q_Return;

   if (this->u32_NodeIndex > orc_Cmp.u32_NodeIndex)
   {
      q_Return = false;
   }
   else if (this->u32_NodeIndex == orc_Cmp.u32_NodeIndex)
   {
      if (this->u8_InterfaceNumber < orc_Cmp.u8_InterfaceNumber)
      {
         q_Return = true;
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
bool C_OscCanInterfaceId::operator ==(const C_OscCanInterfaceId & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->u32_NodeIndex == orc_Cmp.u32_NodeIndex) &&
       (this->u8_InterfaceNumber == orc_Cmp.u8_InterfaceNumber))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Not equal operator.

   \param[in]  orc_Cmp  Compared instance

   \return
   true     Current equals orc_Cmp
   false    Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanInterfaceId::operator !=(const C_OscCanInterfaceId & orc_Cmp) const
{
   bool q_Return = false;

   if ((this->u32_NodeIndex != orc_Cmp.u32_NodeIndex) ||
       (this->u8_InterfaceNumber != orc_Cmp.u8_InterfaceNumber))
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with unit [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanInterfaceId::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_NodeIndex, sizeof(this->u32_NodeIndex), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_InterfaceNumber, sizeof(this->u8_InterfaceNumber), oru32_HashValue);
}
