#include "precomp_headers.hpp" //pre-compiled headers

#include "DiagLib_config.hpp" //diaglib configuration

#include <cstring>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglUtils.hpp" //tgl_assert
#include "C_SclChecksums.hpp"
#include "C_SclString.hpp"
#include "CKFXVariableBase.hpp"
#include "CKFXVariableLists.hpp"

//**************************************************************

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::diag_lib;

//**************************************************************

//clear (i.e. set to 0) all default sets for all variables in all lists
void C_KFXVariableLists::ClearDefaults(void)
{
   uint16_t u16_List;
   uint16_t u16_Variable;

   for (u16_List = 0U; u16_List < this->GetLength(); u16_List++)
   {
      for (u16_Variable = 0U; u16_Variable < (*this)[u16_List].VariableList.GetLength(); u16_Variable++)
      {
         (*this)[u16_List].VariableList[u16_Variable].ClearAllDefaults();
      }
   }
}

//**************************************************************

//clear (i.e. set to 0) values for all variables in all lists
void C_KFXVariableLists::ClearValues(void)
{
   uint16_t u16_List;
   uint16_t u16_Variable;

   for (u16_List = 0U; u16_List < this->GetLength(); u16_List++)
   {
      for (u16_Variable = 0U; u16_Variable < (*this)[u16_List].VariableList.GetLength(); u16_Variable++)
      {
         (*this)[u16_List].VariableList[u16_Variable].ClearValue();
      }
   }
}

//**************************************************************
//04.02.09  also include default names (strictly speaking not a part of the lists,
//                                       but we want to detect any changes)
void C_KFXVariableLists::CalcCRCOverAllLists(uint16_t & oru16_CRC, const bool oq_SkipValues) const
{
   uint16_t u16_Index;

   for (u16_Index = 0; u16_Index < this->GetLength(); u16_Index++)
   {
      (*this)[u16_Index].CalcCRCOverList(oru16_CRC, oq_SkipValues);
   }

   for (u16_Index = 0; u16_Index < this->ac_DefaultNames.GetLength(); u16_Index++)
   {
      C_SclChecksums::CalcCRC16STW(this->ac_DefaultNames[u16_Index].c_str(), this->ac_DefaultNames[u16_Index].Length(),
                                   oru16_CRC);
   }
}

//**************************************************************

bool C_KFXVariableLists::VariableExists(const uint16_t ou16_Index) const
{
   uint16_t u16_List;
   uint16_t u16_Variable;

   C_KFXVariableLists::UnpackIndex(ou16_Index, &u16_List, &u16_Variable);
   return this->VariableExists(u16_List, u16_Variable);
}

//**************************************************************

bool C_KFXVariableLists::ListExists(const uint16_t ou16_List) const
{
   if (this->GetLength() <= ou16_List)
   {
      return false;
   }
   return true;
}

//**************************************************************

bool C_KFXVariableLists::VariableExists(const uint16_t ou16_List, const uint16_t ou16_Variable) const
{
   if (this->GetLength() <= ou16_List)
   {
      return false;
   }
   else
   {
      if ((*this)[ou16_List].VariableList.GetLength() <= ou16_Variable)
      {
         return false;
      }
   }
   return true;
}

//**************************************************************

uint16_t C_KFXVariableLists::PackIndex(const uint16_t ou16_List, const uint16_t ou16_Variable)
{
   return ((static_cast<uint16_t>(ou16_List << 10U)) | (ou16_Variable & 0x03FFU));
}

//**************************************************************

void C_KFXVariableLists::UnpackIndex(const uint16_t ou16_Index, uint16_t * const opu16_List,
                                     uint16_t * const opu16_Variable)
{
   if (opu16_List != NULL)
   {
      *opu16_List  = static_cast<uint16_t>((ou16_Index & 0xFC00U) >> 10U);
   }
   if (opu16_Variable != NULL)
   {
      *opu16_Variable = static_cast<uint16_t>(ou16_Index & 0x03FFU);
   }
}

//**************************************************************

C_KFXVariableBase * C_KFXVariableLists::GetVariable(const uint16_t ou16_Index)
{
   uint16_t u16_List;
   uint16_t u16_Variable;

   C_KFXVariableLists::UnpackIndex(ou16_Index, &u16_List, &u16_Variable);
   return this->GetVariable(u16_List, u16_Variable);
}

//**************************************************************

C_KFXVariableBase * C_KFXVariableLists::GetVariable(const uint16_t ou16_List, const uint16_t ou16_Variable)
{
   if (this->VariableExists(ou16_List, ou16_Variable) == false)
   {
      return NULL;
   }
   return &(*this)[ou16_List].VariableList[ou16_Variable];
}

//**************************************************************

const C_KFXVariableBase * C_KFXVariableLists::GetVariable(const uint16_t ou16_Index) const
{
   uint16_t u16_List;
   uint16_t u16_Variable;

   C_KFXVariableLists::UnpackIndex(ou16_Index, &u16_List, &u16_Variable);
   return this->GetVariable(u16_List, u16_Variable);
}

//**************************************************************

const C_KFXVariableBase * C_KFXVariableLists::GetVariable(const uint16_t ou16_List, const uint16_t ou16_Variable) const
{
   if (this->VariableExists(ou16_List, ou16_Variable) == false)
   {
      return NULL;
   }
   return &(*this)[ou16_List].VariableList[ou16_Variable];
}

//**************************************************************

const C_KFXVariableListBase * C_KFXVariableLists::GetListByName(const C_SclString & orc_ListName) const
{
   int32_t s32_ListIndex;

   for (s32_ListIndex = 0; s32_ListIndex < this->GetLength(); s32_ListIndex++)
   {
      if ((*this)[s32_ListIndex].c_ListName == orc_ListName)
      {
         return &((*this)[s32_ListIndex]);
      }
   }
   return NULL;
}

//**************************************************************

C_KFXVariableListBase * C_KFXVariableLists::GetListByName(const C_SclString & orc_ListName)
{
   int32_t s32_ListIndex;

   for (s32_ListIndex = 0; s32_ListIndex < this->GetLength(); s32_ListIndex++)
   {
      if ((*this)[s32_ListIndex].c_ListName == orc_ListName)
      {
         return &((*this)[s32_ListIndex]);
      }
   }
   return NULL;
}

//-----------------------------------------------------------------------------
