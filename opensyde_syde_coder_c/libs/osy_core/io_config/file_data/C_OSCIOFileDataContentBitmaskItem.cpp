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
#include "C_OSCIOFileDataContentBitmaskItem.h"

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
C_OSCIOFileDataContentBitmaskItem::C_OSCIOFileDataContentBitmaskItem(void) :
   q_ApplyValueSetting(false),
   u64_Value(0ULL)
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
sint32 C_OSCIOFileDataContentBitmaskItem::SetValueByString(const stw_scl::C_SCLString & orc_Item)
{
   return C_OSCIOFileDataContentBitmaskItem::mh_ParseUintFromString(orc_Item, this->u64_Value);
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
sint32 C_OSCIOFileDataContentBitmaskItem::mh_ParseUintFromString(const stw_scl::C_SCLString & orc_Item,
                                                                 uint64 & oru64_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_Item.LowerCase() == stw_scl::C_SCLString("true").LowerCase())
   {
      oru64_Value = 1ULL;
   }
   else if (orc_Item.LowerCase() == stw_scl::C_SCLString("false").LowerCase())
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
