//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Store bus

   Data container class for all information describing a bus.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OSCSystemBus.h"

#include "CSCLChecksums.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_scl;
using namespace stw_opensyde_core;

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
C_OSCSystemBus::C_OSCSystemBus(void) :
   e_Type(eCAN),
   c_Name("NewBus"),
   c_Comment(""),
   u64_BitRate(125000ULL),
   u8_BusID(0),
   u16_RxTimeoutOffsetMs(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCSystemBus::~C_OSCSystemBus(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out] oru32_HashValue Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSystemBus::CalcHash(uint32 & oru32_HashValue) const
{
   C_SCLChecksums::CalcCRC32(&this->e_Type, sizeof(this->e_Type), oru32_HashValue);
   C_SCLChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   C_SCLChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   C_SCLChecksums::CalcCRC32(&this->u64_BitRate, sizeof(this->u64_BitRate), oru32_HashValue);
   C_SCLChecksums::CalcCRC32(&this->u8_BusID, sizeof(this->u8_BusID), oru32_HashValue);
   C_SCLChecksums::CalcCRC32(&this->u16_RxTimeoutOffsetMs, sizeof(this->u16_RxTimeoutOffsetMs), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Compare bus names for greater

   Buses are compared by name.
   Primary sorting criteria: Name length: Shortest first
   Secondary sorting criteria: First difference in alphabetic ordering

   \param[in] orc_Bus1 Bus 1
   \param[in] orc_Bus2 Bus 2

   \return
   true:  Bus 1 smaller than Bus 2
   false: Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCSystemBus::h_CompareNameGreater(const C_OSCSystemBus & orc_Bus1, const C_OSCSystemBus & orc_Bus2)
{
   bool q_Retval;

   if (orc_Bus1.c_Name.Length() == orc_Bus2.c_Name.Length())
   {
      q_Retval = (orc_Bus1.c_Name < orc_Bus2.c_Name);
   }
   else
   {
      q_Retval = orc_Bus1.c_Name.Length() < orc_Bus2.c_Name.Length();
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if bus ID invalid

   \return
   true  Error
   false No error
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCSystemBus::CheckErrorBusId(void) const
{
   return (this->u8_BusID > 15);
}
