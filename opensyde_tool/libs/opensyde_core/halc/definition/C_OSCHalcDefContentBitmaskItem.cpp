//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bitmask item

   Bitmask item

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <sstream>

#include "stwerrors.h"
#include "CSCLChecksums.h"
#include "C_OSCHalcDefContentBitmaskItem.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
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
C_OSCHalcDefContentBitmaskItem::C_OSCHalcDefContentBitmaskItem(void) :
   q_ApplyValueSetting(false),
   u64_Value(0ULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Denstructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcDefContentBitmaskItem::~C_OSCHalcDefContentBitmaskItem()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set mask value by string

   \param[in] orc_Item Mask string

   \return
   C_NO_ERR All fine
   C_RANGE  String invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefContentBitmaskItem::SetValueByString(const stw_scl::C_SCLString & orc_Item)
{
   return C_OSCHalcDefContentBitmaskItem::mh_ParseUintFromString(orc_Item, this->u64_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcDefContentBitmaskItem::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Display.c_str(), this->c_Display.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_ApplyValueSetting, sizeof(this->q_ApplyValueSetting), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u64_Value, sizeof(this->u64_Value), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse uint value from string

   \param[in] orc_Item    Generic value string
   \param[in] oru64_Value Value to set

   \return
   C_NO_ERR All fine
   C_RANGE  String invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefContentBitmaskItem::mh_ParseUintFromString(const stw_scl::C_SCLString & orc_Item,
                                                              uint64 & oru64_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_Item.LowerCase() == "true")
   {
      oru64_Value = 1ULL;
   }
   else if (orc_Item.LowerCase() == "false")
   {
      oru64_Value = 0ULL;
   }
   else
   {
      if (((orc_Item.Length() > 2UL) && (orc_Item[1] == '0')) && (orc_Item[2] == 'x'))
      {
         const stw_scl::C_SCLString c_Hex = orc_Item.SubString(3UL, orc_Item.Length() - 2UL);
         std::stringstream c_Stream(c_Hex.c_str());
         (c_Stream >> &std::hex) >> oru64_Value;
         if (c_Stream.fail())
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         std::stringstream c_Stream(orc_Item.c_str());
         c_Stream >> oru64_Value;
         if (c_Stream.fail())
         {
            s32_Retval = C_RANGE;
         }
      }
   }
   return s32_Retval;
}
