//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Content including enum

   Content including enum

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLChecksums.h"
#include "C_OSCHalcDefContent.h"

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
C_OSCHalcDefContent::C_OSCHalcDefContent(void) :
   me_ComplexType(eCT_PLAIN)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set complex type

   \param[in] oe_Type Type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcDefContent::SetComplexType(const C_OSCHalcDefContent::E_ComplexType oe_Type)
{
   this->me_ComplexType = oe_Type;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get complex type

   \return
   Complex type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcDefContent::E_ComplexType C_OSCHalcDefContent::GetComplexType() const
{
   return this->me_ComplexType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new enum item entry

   \param[in] orc_DisplayName Enum display value
   \param[in] orc_Value       Enum internal value

   \return
   C_NO_ERR Added successfully
   C_RANGE  Display value already used
   C_CONFIG Content type invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefContent::AddEnumItem(const stw_scl::C_SCLString & orc_DisplayName,
                                        const C_OSCNodeDataPoolContent & orc_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (this->mc_EnumItems.find(orc_DisplayName) != this->mc_EnumItems.end())
   {
      s32_Retval = C_RANGE;
   }
   else
   {
      if ((orc_Value.GetType() == this->GetType()) && (orc_Value.GetArray() == this->GetArray()))
      {
         this->mc_EnumItems[orc_DisplayName] = orc_Value;
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set enum based on display value

   \param[in] orc_DisplayName Enum display value

   \return
   C_NO_ERR Value set
   C_RANGE  Display value does not exist
   C_CONFIG Enum content invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefContent::SetEnumValue(const stw_scl::C_SCLString & orc_DisplayName)
{
   sint32 s32_Retval = C_NO_ERR;
   const std::map<stw_scl::C_SCLString, C_OSCNodeDataPoolContent>::const_iterator c_It = this->mc_EnumItems.find(
      orc_DisplayName);

   if (c_It != this->mc_EnumItems.end())
   {
      const C_OSCNodeDataPoolContent & rc_NewContent = c_It->second;
      if ((this->GetArray() == rc_NewContent.GetArray()) && (this->GetType() == rc_NewContent.GetType()))
      {
         if (this->GetArray())
         {
            switch (this->GetType())
            {
            case eUINT8:
               this->SetValueAU8(rc_NewContent.GetValueAU8());
               break;
            case eUINT16:
               this->SetValueAU16(rc_NewContent.GetValueAU16());
               break;
            case eUINT32:
               this->SetValueAU32(rc_NewContent.GetValueAU32());
               break;
            case eUINT64:
               this->SetValueAU64(rc_NewContent.GetValueAU64());
               break;
            case eSINT8:
               this->SetValueAS8(rc_NewContent.GetValueAS8());
               break;
            case eSINT16:
               this->SetValueAS16(rc_NewContent.GetValueAS16());
               break;
            case eSINT32:
               this->SetValueAS32(rc_NewContent.GetValueAS32());
               break;
            case eSINT64:
               this->SetValueAS64(rc_NewContent.GetValueAS64());
               break;
            case eFLOAT32:
               this->SetValueAF32(rc_NewContent.GetValueAF32());
               break;
            case eFLOAT64:
               this->SetValueAF64(rc_NewContent.GetValueAF64());
               break;
            }
         }
         else
         {
            switch (this->GetType())
            {
            case eUINT8:
               this->SetValueU8(rc_NewContent.GetValueU8());
               break;
            case eUINT16:
               this->SetValueU16(rc_NewContent.GetValueU16());
               break;
            case eUINT32:
               this->SetValueU32(rc_NewContent.GetValueU32());
               break;
            case eUINT64:
               this->SetValueU64(rc_NewContent.GetValueU64());
               break;
            case eSINT8:
               this->SetValueS8(rc_NewContent.GetValueS8());
               break;
            case eSINT16:
               this->SetValueS16(rc_NewContent.GetValueS16());
               break;
            case eSINT32:
               this->SetValueS32(rc_NewContent.GetValueS32());
               break;
            case eSINT64:
               this->SetValueS64(rc_NewContent.GetValueS64());
               break;
            case eFLOAT32:
               this->SetValueF32(rc_NewContent.GetValueF32());
               break;
            case eFLOAT64:
               this->SetValueF64(rc_NewContent.GetValueF64());
               break;
            }
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all enum items

   \return
   All enum items
*/
//----------------------------------------------------------------------------------------------------------------------
const std::map<stw_scl::C_SCLString, C_OSCNodeDataPoolContent> & C_OSCHalcDefContent::GetEnumItems(void) const
{
   return this->mc_EnumItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add bitmask item

   \param[in] orc_Value New bitmask item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcDefContent::AddBitmaskItem(const C_OSCHalcDefContentBitmaskItem & orc_Value)
{
   this->mc_BitmaskItems.push_back(orc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all bitmask items

   \return
   All bitmask items
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_OSCHalcDefContentBitmaskItem> & C_OSCHalcDefContent::GetBitmaskItems(void) const
{
   return this->mc_BitmaskItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcDefContent::CalcHash(uint32 & oru32_HashValue) const
{
   C_OSCNodeDataPoolContent::CalcHash(oru32_HashValue);

   stw_scl::C_SCLChecksums::CalcCRC32(&this->me_ComplexType, sizeof(this->me_ComplexType), oru32_HashValue);

   for (std::map<stw_scl::C_SCLString, C_OSCNodeDataPoolContent>::const_iterator c_It = this->mc_EnumItems.begin();
        c_It != this->mc_EnumItems.end(); ++c_It)
   {
      stw_scl::C_SCLChecksums::CalcCRC32(c_It->first.c_str(), c_It->first.Length(), oru32_HashValue);
      c_It->second.CalcHash(oru32_HashValue);
   }

   for (uint32 u32_It = 0UL; u32_It < this->mc_BitmaskItems.size(); ++u32_It)
   {
      this->mc_BitmaskItems[u32_It].CalcHash(oru32_HashValue);
   }
}
