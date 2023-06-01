#include "precomp_headers.hpp" //pre-compiled headers

#include "DiagLib_config.hpp" //diaglib configuration

#include <cstring>
#include "stwtypes.hpp"
#include "C_SclChecksums.hpp"
#include "TglUtils.hpp"
#include "C_SclString.hpp"
#include "CKFXVariableListBase.hpp"

//**************************************************************

using namespace stw::scl;
using namespace stw::diag_lib;

//**************************************************************

C_KFXVariableListBase::C_KFXVariableListBase(void)
{
   u8_ListType = KFX_VARIABLE_TYPE_RAM; //RAM,EEPROM,ROM
   q_CheckSummed = false;
   u32_CheckSumAddress = 0U;
   c_ListName = "";
   mu16_NumDefaults = 0U;
   u64_CRCServer = 0U;
}

//**************************************************************

C_KFXVariableListBase::~C_KFXVariableListBase(void)
{
}

//**************************************************************

C_KFXVariableListBase::C_KFXVariableListBase(const C_KFXVariableListBase & orc_Source)
{
   this->operator =(orc_Source);
}

//**************************************************************

C_KFXVariableListBase & C_KFXVariableListBase::operator =(const C_KFXVariableListBase & orc_Source)
{
   if (this == &orc_Source)
   {
      return *this;
   }

   this->u8_ListType         = orc_Source.u8_ListType;
   this->q_CheckSummed       = orc_Source.q_CheckSummed;
   this->u32_CheckSumAddress = orc_Source.u32_CheckSumAddress;
   this->c_ListName          = orc_Source.c_ListName;
   this->VariableList.operator =(orc_Source.VariableList);
   this->SetNumDefaults(orc_Source.GetNumDefaults());
   this->u64_CRCServer       = orc_Source.u64_CRCServer;

   return (*this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculate CRC over list definition

   Uses CalcCRC16STW algorithm.

   \param[in,out] oru16_CRC              calculated CRC (caller must/may set start value)
   \param[in]     oq_SkipValues          true: include variable values; false: don't include values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_KFXVariableListBase::CalcCRCOverList(uint16_t & oru16_CRC, const bool oq_SkipValues) const
{
   int32_t i;
   uint8_t au8_Data[4];

   C_SclChecksums::CalcCRC16STW(this->c_ListName.c_str(),   this->c_ListName.Length(),         oru16_CRC);
   C_SclChecksums::CalcCRC16STW(&this->u8_ListType,         sizeof(this->u8_ListType),         oru16_CRC);
   au8_Data[0] = static_cast<uint8_t>(this->q_CheckSummed);
   C_SclChecksums::CalcCRC16STW(&au8_Data[0],       1U,       oru16_CRC);
   au8_Data[0] = static_cast<uint8_t>(this->u32_CheckSumAddress);
   au8_Data[1] = static_cast<uint8_t>(this->u32_CheckSumAddress >> 8U);
   au8_Data[2] = static_cast<uint8_t>(this->u32_CheckSumAddress >> 16U);
   au8_Data[3] = static_cast<uint8_t>(this->u32_CheckSumAddress >> 24U);
   C_SclChecksums::CalcCRC16STW(&au8_Data[0], sizeof(au8_Data), oru16_CRC);

   for (i = 0; i < this->VariableList.GetLength(); i++)
   {
      this->VariableList[i].CalcCRCOverEntry(oru16_CRC, oq_SkipValues);
   }
}

//**************************************************************

uint16_t C_KFXVariableListBase::CountChangedValues(void) const
{
   int32_t i;
   uint16_t u16_Changed = 0U;

   for (i = 0; i < this->VariableList.GetLength(); i++)
   {
      if (VariableList[i].q_ValueChanged == true)
      {
         u16_Changed++;
      }
   }
   return u16_Changed;
}

//**************************************************************

void C_KFXVariableListBase::SetNumDefaults(const uint16_t ou16_NumDefaults)
{
   int32_t i;

   if (mu16_NumDefaults != ou16_NumDefaults)
   {
      for (i = 0; i < this->VariableList.GetLength(); i++)
      {
         VariableList[i].SetNumDefaults(ou16_NumDefaults);
      }
      mu16_NumDefaults = ou16_NumDefaults;
   }
}

//**************************************************************

uint16_t C_KFXVariableListBase::GetNumDefaults(void) const
{
   return mu16_NumDefaults;
}

//**************************************************************

const C_KFXVariableBase * C_KFXVariableListBase::GetVariableByName(const C_SclString & orc_VariableName) const
{
   int32_t s32_VarIndex;

   for (s32_VarIndex = 0; s32_VarIndex < VariableList.GetLength(); s32_VarIndex++)
   {
      if (VariableList[s32_VarIndex].c_Name == orc_VariableName)
      {
         return &VariableList[s32_VarIndex];
      }
   }
   return NULL;
}

//**************************************************************

C_KFXVariableBase * C_KFXVariableListBase::GetVariableByName(const C_SclString & orc_VariableName)
{
   int32_t s32_VarIndex;

   for (s32_VarIndex = 0; s32_VarIndex < VariableList.GetLength(); s32_VarIndex++)
   {
      if (VariableList[s32_VarIndex].c_Name == orc_VariableName)
      {
         return &VariableList[s32_VarIndex];
      }
   }
   return NULL;
}

//**************************************************************
