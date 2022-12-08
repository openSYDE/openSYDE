//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition data base

   HALC definition data base

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscHalcDefBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;

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
C_OscHalcDefBase::C_OscHalcDefBase(void) :
   u32_ContentVersion(0UL),
   e_SafetyMode(eTWO_LEVELS_WITH_DROPPING),
   u8_NumConfigCopies(1U),
   q_NvmBasedConfig(false),
   u32_NvmReservedListSizeParameters(100UL),
   u32_NvmReservedListSizeInputValues(100UL),
   u32_NvmReservedListSizeOutputValues(100UL),
   u32_NvmReservedListSizeStatusValues(100UL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscHalcDefBase::~C_OscHalcDefBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if all IDs are unique

   \param[in,out]  orc_DuplicateIds    Duplicate ids

   \retval true  All unique
   \retval false Duplicate Ids found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscHalcDefBase::CheckIdsUnique(std::vector<stw::scl::C_SclString> & orc_DuplicateIds) const
{
   bool q_Retval = true;

   std::vector<stw::scl::C_SclString> c_AllIds;

   //Find all valid IDs
   for (uint32_t u32_ItDomain = 0UL; u32_ItDomain < this->GetDomainSize(); ++u32_ItDomain)
   {
      const C_OscHalcDefDomain * const pc_Domain = this->GetDomainDefDataConst(u32_ItDomain);
      tgl_assert(pc_Domain != NULL);
      if (pc_Domain != NULL)
      {
         c_AllIds.push_back(pc_Domain->c_Id);
         for (uint32_t u32_ItChannel = 0UL; u32_ItChannel < pc_Domain->c_ChannelUseCases.size(); ++u32_ItChannel)
         {
            const C_OscHalcDefChannelUseCase & rc_Channel = pc_Domain->c_ChannelUseCases[u32_ItChannel];
            c_AllIds.push_back(rc_Channel.c_Id);
         }
         C_OscHalcDefBase::mh_AggregateIds(pc_Domain->c_ChannelValues.c_Parameters, c_AllIds);
         C_OscHalcDefBase::mh_AggregateIds(pc_Domain->c_DomainValues.c_Parameters, c_AllIds);
      }
   }

   //Check all IDs
   for (uint32_t u32_ItCompare1 = 0UL; u32_ItCompare1 < c_AllIds.size(); ++u32_ItCompare1)
   {
      for (uint32_t u32_ItCompare2 = u32_ItCompare1; u32_ItCompare2 < c_AllIds.size(); ++u32_ItCompare2)
      {
         //Check every string, but don't compare with itself
         const stw::scl::C_SclString & rc_Comp1 = c_AllIds[u32_ItCompare1];
         if ((u32_ItCompare1 != u32_ItCompare2) && (rc_Comp1 == c_AllIds[u32_ItCompare2]))
         {
            bool q_AlreadyExists = false;
            for (uint32_t u32_ItOut = 0UL; u32_ItOut < orc_DuplicateIds.size(); ++u32_ItOut)
            {
               if (rc_Comp1 == orc_DuplicateIds[u32_ItOut])
               {
                  q_AlreadyExists = true;
                  break;
               }
            }
            if (q_AlreadyExists == false)
            {
               orc_DuplicateIds.push_back(rc_Comp1);
            }
            q_Retval = false;
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefBase::Clear()
{
   this->u32_ContentVersion = 0UL;
   this->c_DeviceName = "";
   this->c_FileString = "";
   this->c_OriginalFileName = "";
   this->e_SafetyMode = eTWO_LEVELS_WITH_DROPPING;
   this->u8_NumConfigCopies = 1U;
   this->q_NvmBasedConfig = false;
   this->c_NvmNonSafeAddressOffset.clear();
   this->c_NvmSafeAddressOffset.clear();
   this->u32_NvmReservedListSizeParameters = 100UL;
   this->u32_NvmReservedListSizeInputValues = 100UL;
   this->u32_NvmReservedListSizeOutputValues = 100UL;
   this->u32_NvmReservedListSizeStatusValues = 100UL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks if the HALC configuration is clear or if it has a loaded definition

   \retval   true    Definition is cleared. No definition is loaded.
   \retval   false   The definition is not cleared. A concrete definition is loaded.
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscHalcDefBase::IsClear(void) const
{
   bool q_Return = false;

   if ((this->u32_ContentVersion == 0UL) &&
       (this->c_DeviceName == "") &&
       (this->c_FileString == "") &&
       (this->c_OriginalFileName == "") &&
       (this->e_SafetyMode == eTWO_LEVELS_WITH_DROPPING) &&
       (this->u8_NumConfigCopies == 1U) &&
       (this->q_NvmBasedConfig == false) &&
       (this->c_NvmNonSafeAddressOffset.size() == 0UL) &&
       (this->c_NvmSafeAddressOffset.size() == 0UL) &&
       (this->u32_NvmReservedListSizeParameters == 100UL) &&
       (this->u32_NvmReservedListSizeInputValues == 100UL) &&
       (this->u32_NvmReservedListSizeOutputValues == 100UL) &&
       (this->u32_NvmReservedListSizeStatusValues == 100UL))
   {
      q_Return = true;
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefBase::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_ContentVersion, sizeof(this->u32_ContentVersion), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_DeviceName.c_str(), this->c_DeviceName.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_FileString.c_str(), this->c_FileString.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_OriginalFileName.c_str(),
                                       this->c_OriginalFileName.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_SafetyMode, sizeof(this->e_SafetyMode), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_NumConfigCopies, sizeof(this->u8_NumConfigCopies), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_NvmBasedConfig, sizeof(this->q_NvmBasedConfig), oru32_HashValue);
   for (uint32_t u32_It = 0UL; u32_It < this->c_NvmNonSafeAddressOffset.size(); ++u32_It)
   {
      const uint32_t u32_Tmp = this->c_NvmNonSafeAddressOffset[u32_It];
      stw::scl::C_SclChecksums::CalcCRC32(&u32_Tmp, sizeof(u32_Tmp), oru32_HashValue);
   }
   for (uint32_t u32_It = 0UL; u32_It < this->c_NvmSafeAddressOffset.size(); ++u32_It)
   {
      const uint32_t u32_Tmp = this->c_NvmSafeAddressOffset[u32_It];
      stw::scl::C_SclChecksums::CalcCRC32(&u32_Tmp, sizeof(u32_Tmp), oru32_HashValue);
   }
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_NvmReservedListSizeParameters,
                                       sizeof(this->u32_NvmReservedListSizeParameters), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_NvmReservedListSizeInputValues,
                                       sizeof(this->u32_NvmReservedListSizeInputValues), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_NvmReservedListSizeOutputValues,
                                       sizeof(this->u32_NvmReservedListSizeOutputValues), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_NvmReservedListSizeStatusValues,
                                       sizeof(this->u32_NvmReservedListSizeStatusValues), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Aggregate ids

   \param[in]      orc_Items           Items
   \param[in,out]  orc_DuplicateIds    Duplicate ids
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefBase::mh_AggregateIds(const std::vector<C_OscHalcDefStruct> & orc_Items,
                                       std::vector<stw::scl::C_SclString> & orc_DuplicateIds)
{
   for (uint32_t u32_ItItem = 0UL; u32_ItItem < orc_Items.size(); ++u32_ItItem)
   {
      const C_OscHalcDefStruct & rc_Item = orc_Items[u32_ItItem];
      orc_DuplicateIds.push_back(rc_Item.c_Id);
      for (uint32_t u32_ItItemElem = 0UL; u32_ItItemElem < rc_Item.c_StructElements.size(); ++u32_ItItemElem)
      {
         const C_OscHalcDefElement & rc_ItemElem = rc_Item.c_StructElements[u32_ItItemElem];
         orc_DuplicateIds.push_back(rc_ItemElem.c_Id);
      }
   }
}
