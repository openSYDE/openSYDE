//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Content including enum

   Content including enum

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscHalcDefContent.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;

using namespace stw::errors;
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
C_OscHalcDefContent::C_OscHalcDefContent(void) :
   C_OscNodeDataPoolContent(),
   me_ComplexType(eCT_PLAIN)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set complex type

   \param[in]  oe_Type  Type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefContent::SetComplexType(const C_OscHalcDefContent::E_ComplexType oe_Type)
{
   this->me_ComplexType = oe_Type;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get complex type

   \return
   Complex type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscHalcDefContent::E_ComplexType C_OscHalcDefContent::GetComplexType() const
{
   return this->me_ComplexType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new enum item entry

   \param[in]  orc_DisplayName   Enum display value
   \param[in]  orc_Value         Enum internal value

   \return
   C_NO_ERR Added successfully
   C_RANGE  Display value already used
   C_CONFIG Content type invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscHalcDefContent::AddEnumItem(const stw::scl::C_SclString & orc_DisplayName,
                                         const C_OscNodeDataPoolContent & orc_Value)
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNodeDataPoolContent * const pc_Content = this->FindEnumItem(orc_DisplayName);

   if (pc_Content != NULL)
   {
      s32_Retval = C_RANGE;
   }
   else
   {
      if ((orc_Value.GetType() == this->GetType()) && (orc_Value.GetArray() == this->GetArray()))
      {
         this->mc_EnumItems.push_back(std::pair<stw::scl::C_SclString, C_OscNodeDataPoolContent>(orc_DisplayName,
                                                                                                 orc_Value));
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

   \param[in]  orc_DisplayName   Enum display value

   \return
   C_NO_ERR Value set
   C_RANGE  Display value does not exist
   C_CONFIG Enum content invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscHalcDefContent::SetEnumValue(const stw::scl::C_SclString & orc_DisplayName)
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNodeDataPoolContent * const pc_NewContent = this->FindEnumItem(orc_DisplayName);

   if (pc_NewContent != NULL)
   {
      if ((this->GetArray() == pc_NewContent->GetArray()) && (this->GetType() == pc_NewContent->GetType()))
      {
         if (this->GetArray())
         {
            switch (this->GetType())
            {
            case eUINT8:
               this->SetValueArrU8(pc_NewContent->GetValueArrU8());
               break;
            case eUINT16:
               this->SetValueArrU16(pc_NewContent->GetValueArrU16());
               break;
            case eUINT32:
               this->SetValueArrU32(pc_NewContent->GetValueArrU32());
               break;
            case eUINT64:
               this->SetValueArrU64(pc_NewContent->GetValueArrU64());
               break;
            case eSINT8:
               this->SetValueArrS8(pc_NewContent->GetValueArrS8());
               break;
            case eSINT16:
               this->SetValueArrS16(pc_NewContent->GetValueArrS16());
               break;
            case eSINT32:
               this->SetValueArrS32(pc_NewContent->GetValueArrS32());
               break;
            case eSINT64:
               this->SetValueArrS64(pc_NewContent->GetValueArrS64());
               break;
            case eFLOAT32:
               this->SetValueArrF32(pc_NewContent->GetValueArrF32());
               break;
            case eFLOAT64:
               this->SetValueArrF64(pc_NewContent->GetValueArrF64());
               break;
            default:
               break;
            }
         }
         else
         {
            switch (this->GetType())
            {
            case eUINT8:
               this->SetValueU8(pc_NewContent->GetValueU8());
               break;
            case eUINT16:
               this->SetValueU16(pc_NewContent->GetValueU16());
               break;
            case eUINT32:
               this->SetValueU32(pc_NewContent->GetValueU32());
               break;
            case eUINT64:
               this->SetValueU64(pc_NewContent->GetValueU64());
               break;
            case eSINT8:
               this->SetValueS8(pc_NewContent->GetValueS8());
               break;
            case eSINT16:
               this->SetValueS16(pc_NewContent->GetValueS16());
               break;
            case eSINT32:
               this->SetValueS32(pc_NewContent->GetValueS32());
               break;
            case eSINT64:
               this->SetValueS64(pc_NewContent->GetValueS64());
               break;
            case eFLOAT32:
               this->SetValueF32(pc_NewContent->GetValueF32());
               break;
            case eFLOAT64:
               this->SetValueF64(pc_NewContent->GetValueF64());
               break;
            default:
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
/*! \brief  Get enum value

   \param[in,out]  orc_DisplayName  Display name

   \return
   C_NO_ERR Value set
   C_RANGE  Display value does not exist for this value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscHalcDefContent::GetEnumValue(stw::scl::C_SclString & orc_DisplayName)
{
   int32_t s32_Retval = C_RANGE;

   for (std::vector<std::pair<stw::scl::C_SclString, C_OscNodeDataPoolContent> >::const_iterator c_It =
           this->mc_EnumItems.begin();
        c_It != this->mc_EnumItems.end(); ++c_It)
   {
      if (c_It->second == *this)
      {
         orc_DisplayName = c_It->first;
         s32_Retval = C_NO_ERR;
         break;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Find enum item

   \param[in]  orc_DisplayName   Display name

   \return
   Found item, if pointer not NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNodeDataPoolContent * C_OscHalcDefContent::FindEnumItem(const stw::scl::C_SclString & orc_DisplayName) const
{
   const C_OscNodeDataPoolContent * pc_Retval = NULL;

   for (std::vector<std::pair<stw::scl::C_SclString, C_OscNodeDataPoolContent> >::const_iterator c_It =
           this->mc_EnumItems.begin();
        c_It != this->mc_EnumItems.end(); ++c_It)
   {
      if (c_It->first == orc_DisplayName)
      {
         pc_Retval = &c_It->second;
         break;
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all enum items

   \return
   All enum items
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<std::pair<stw::scl::C_SclString,
                            C_OscNodeDataPoolContent> > & C_OscHalcDefContent::GetEnumItems(void) const
{
   return this->mc_EnumItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add bitmask item

   \param[in]  orc_Value   New bitmask item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefContent::AddBitmaskItem(const C_OscHalcDefContentBitmaskItem & orc_Value)
{
   this->mc_BitmaskItems.push_back(orc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all bitmask items

   \return
   All bitmask items
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_OscHalcDefContentBitmaskItem> & C_OscHalcDefContent::GetBitmaskItems(void) const
{
   return this->mc_BitmaskItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get bitmask status values

   \param[in,out]  opc_Displays  Displays
   \param[in,out]  opc_Values    Values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefContent::GetBitmaskStatusValues(std::vector<stw::scl::C_SclString> * const opc_Displays,
                                                 std::vector<bool> * const opc_Values) const
{
   if (opc_Displays != NULL)
   {
      opc_Displays->clear();
      opc_Displays->reserve(this->mc_BitmaskItems.size());
   }
   if (opc_Values != NULL)
   {
      opc_Values->clear();
      opc_Values->reserve(this->mc_BitmaskItems.size());
   }
   for (std::vector<C_OscHalcDefContentBitmaskItem>::const_iterator c_ItBitmask = this->mc_BitmaskItems.begin();
        c_ItBitmask != this->mc_BitmaskItems.end(); ++c_ItBitmask)
   {
      if (opc_Displays != NULL)
      {
         opc_Displays->push_back(c_ItBitmask->c_Display);
      }
      if (opc_Values != NULL)
      {
         opc_Values->push_back(c_ItBitmask->q_ApplyValueSetting);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get bitmask

   \param[in]   orc_DisplayName  Display name
   \param[out]  orq_Value        Value

   \return
   C_NO_ERR Value set
   C_RANGE  Display value does not exist
   C_CONFIG Type invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscHalcDefContent::GetBitmask(const stw::scl::C_SclString & orc_DisplayName, bool & orq_Value) const
{
   int32_t s32_Retval = C_NO_ERR;

   if ((this->GetArray() == false) && (this->GetComplexType() == C_OscHalcDefContent::eCT_BIT_MASK))
   {
      bool q_Found = false;

      for (std::vector<C_OscHalcDefContentBitmaskItem>::const_iterator c_ItBitmask = this->mc_BitmaskItems.begin();
           c_ItBitmask != this->mc_BitmaskItems.end(); ++c_ItBitmask)
      {
         if (orc_DisplayName == c_ItBitmask->c_Display)
         {
            q_Found = true;
            orq_Value = c_ItBitmask->q_ApplyValueSetting;
            //Stop after finding one
            break;
         }
      }
      if (q_Found == false)
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set bitmask

   \param[in]  orc_DisplayName   Display name
   \param[in]  oq_Value          Value

   \return
   C_NO_ERR Value set
   C_RANGE  Display value does not exist
   C_CONFIG Type invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscHalcDefContent::SetBitmask(const stw::scl::C_SclString & orc_DisplayName, const bool oq_Value)
{
   int32_t s32_Retval = C_NO_ERR;

   if ((this->GetArray() == false) && (this->GetComplexType() == C_OscHalcDefContent::eCT_BIT_MASK))
   {
      bool q_Found = false;

      //Step 1: update flag
      for (std::vector<C_OscHalcDefContentBitmaskItem>::iterator c_ItBitmask = this->mc_BitmaskItems.begin();
           c_ItBitmask != this->mc_BitmaskItems.end(); ++c_ItBitmask)
      {
         if (orc_DisplayName == c_ItBitmask->c_Display)
         {
            q_Found = true;
            //Update flag
            c_ItBitmask->q_ApplyValueSetting = oq_Value;
            //Stop after finding one
            break;
         }
      }
      if (q_Found == false)
      {
         s32_Retval = C_RANGE;
      }
      else
      {
         uint64_t u64_CurrentValue = 0ULL;
         //Current value
         switch (this->GetType()) //lint !e788 not all enum constants used; this is unsigned only
         {
         case C_OscNodeDataPoolContent::eUINT8:
            u64_CurrentValue = static_cast<uint64_t>(this->GetValueU8());
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            u64_CurrentValue = static_cast<uint64_t>(this->GetValueU16());
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            u64_CurrentValue = static_cast<uint64_t>(this->GetValueU32());
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            u64_CurrentValue = this->GetValueU64();
            break;
         default:
            s32_Retval = C_CONFIG;
            break;
         }
         if (s32_Retval == C_NO_ERR)
         {
            //Step 2: get initial value
            for (std::vector<C_OscHalcDefContentBitmaskItem>::iterator c_ItBitmask = this->mc_BitmaskItems.begin();
                 c_ItBitmask != this->mc_BitmaskItems.end(); ++c_ItBitmask)
            {
               //Mask any used bits
               u64_CurrentValue = u64_CurrentValue & (~(c_ItBitmask->u64_Value));
            }
            //Step 3: apply all active flags
            for (std::vector<C_OscHalcDefContentBitmaskItem>::iterator c_ItBitmask = this->mc_BitmaskItems.begin();
                 c_ItBitmask != this->mc_BitmaskItems.end(); ++c_ItBitmask)
            {
               if (c_ItBitmask->q_ApplyValueSetting)
               {
                  u64_CurrentValue = u64_CurrentValue | c_ItBitmask->u64_Value;
               }
            }
            //Step 4: update value
            switch (this->GetType()) //lint !e788 not all enum constants used; this is unsigned only
            {
            case C_OscNodeDataPoolContent::eUINT8:
               this->SetValueU8(static_cast<uint8_t>(u64_CurrentValue));
               break;
            case C_OscNodeDataPoolContent::eUINT16:
               this->SetValueU16(static_cast<uint16_t>(u64_CurrentValue));
               break;
            case C_OscNodeDataPoolContent::eUINT32:
               this->SetValueU32(static_cast<uint32_t>(u64_CurrentValue));
               break;
            case C_OscNodeDataPoolContent::eUINT64:
               this->SetValueU64(static_cast<uint64_t>(u64_CurrentValue));
               break;
            default:
               //Should not happen
               tgl_assert(false);
               break;
            }
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefContent::CalcHash(uint32_t & oru32_HashValue) const
{
   C_OscNodeDataPoolContent::CalcHash(oru32_HashValue);

   stw::scl::C_SclChecksums::CalcCRC32(&this->me_ComplexType, sizeof(this->me_ComplexType), oru32_HashValue);

   for (std::vector<std::pair<stw::scl::C_SclString, C_OscNodeDataPoolContent> >::const_iterator c_It =
           this->mc_EnumItems.begin();
        c_It != this->mc_EnumItems.end(); ++c_It)
   {
      stw::scl::C_SclChecksums::CalcCRC32(c_It->first.c_str(), c_It->first.Length(), oru32_HashValue);
      c_It->second.CalcHash(oru32_HashValue);
   }

   for (uint32_t u32_It = 0UL; u32_It < this->mc_BitmaskItems.size(); ++u32_It)
   {
      this->mc_BitmaskItems[u32_It].CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over one element

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
   \param[in]      ou32_Index       Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefContent::CalcHashElement(uint32_t & oru32_HashValue, const uint32_t ou32_Index) const
{
   C_OscNodeDataPoolContent::CalcHashElement(oru32_HashValue, ou32_Index);

   stw::scl::C_SclChecksums::CalcCRC32(&this->me_ComplexType, sizeof(this->me_ComplexType), oru32_HashValue);

   for (std::vector<std::pair<stw::scl::C_SclString, C_OscNodeDataPoolContent> >::const_iterator c_It =
           this->mc_EnumItems.begin();
        c_It != this->mc_EnumItems.end(); ++c_It)
   {
      stw::scl::C_SclChecksums::CalcCRC32(c_It->first.c_str(), c_It->first.Length(), oru32_HashValue);
      c_It->second.CalcHash(oru32_HashValue);
   }

   for (uint32_t u32_It = 0UL; u32_It < this->mc_BitmaskItems.size(); ++u32_It)
   {
      this->mc_BitmaskItems[u32_It].CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over structure only

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefContent::CalcHashStructure(uint32_t & oru32_HashValue) const
{
   C_OscNodeDataPoolContent::CalcHashStructure(oru32_HashValue);

   stw::scl::C_SclChecksums::CalcCRC32(&this->me_ComplexType, sizeof(this->me_ComplexType), oru32_HashValue);

   for (std::vector<std::pair<stw::scl::C_SclString, C_OscNodeDataPoolContent> >::const_iterator c_It =
           this->mc_EnumItems.begin();
        c_It != this->mc_EnumItems.end(); ++c_It)
   {
      stw::scl::C_SclChecksums::CalcCRC32(c_It->first.c_str(), c_It->first.Length(), oru32_HashValue);
      c_It->second.CalcHash(oru32_HashValue);
   }

   for (uint32_t u32_It = 0UL; u32_It < this->mc_BitmaskItems.size(); ++u32_It)
   {
      this->mc_BitmaskItems[u32_It].CalcHashStructure(oru32_HashValue);
   }
}
