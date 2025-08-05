//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       resource string handling class

   ANSI C++ resource string handling class.
   For details cf. documentation in .h file.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers

#include "stwtypes.hpp"
#include "C_SclResourceStrings.hpp"

using namespace stw::scl;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

C_SCLResourceStrings::C_SCLResourceStrings(void) :
   mq_Dirty(false)
{
   this->Clear();
}

//----------------------------------------------------------------------------------------------------------------------

void C_SCLResourceStrings::Clear(void)
{
   mac_Strings.SetLength(0);
   mq_Dirty = true;
}

//----------------------------------------------------------------------------------------------------------------------
//do a binary search in the sorted table:
C_SclString C_SCLResourceStrings::LoadStr(const uint16_t ou16_Index)
{
   uint16_t u16_Pos;
   uint16_t u16_First;
   uint16_t u16_Last;

   if (mq_Dirty == true)
   {
      return "C_SCLResourceStrings::LoadStr: string table not initialized !"; //most probably not sorted !
   }

   u16_First = 0U;
   u16_Last  = static_cast<uint16_t>(mac_Strings.GetHigh());

   while (u16_First <= u16_Last)
   {
      u16_Pos = static_cast<uint16_t>((u16_First + u16_Last) / 2U);
      if (ou16_Index > mac_Strings[u16_Pos].u16_Index)
      {
         u16_First = static_cast<uint16_t>(u16_Pos + 1U);
      }
      else if (ou16_Index < mac_Strings[u16_Pos].u16_Index)
      {
         u16_Last = static_cast<uint16_t>(u16_Pos - 1U);
      }
      else
      {
         //found !!
         return mac_Strings[u16_Pos].c_Text;
      }
   }
   return ("C_SCLResourceStrings::LoadStr: String " + C_SclString::IntToStr(ou16_Index) + "not defined !");
}

//----------------------------------------------------------------------------------------------------------------------

void C_SCLResourceStrings::AddString(const uint16_t ou16_Index, const C_SclString & orc_Text)
{
   mac_Strings.IncLength();
   mac_Strings[mac_Strings.GetHigh()].u16_Index = ou16_Index;
   mac_Strings[mac_Strings.GetHigh()].c_Text    = orc_Text;
   mq_Dirty = true;
}

//----------------------------------------------------------------------------------------------------------------------

void C_SCLResourceStrings::SortByIndex(void)
{
   int32_t s32_Index;    //needs to be signed !
   int32_t s32_Position; //needs to be signed !
   C_SclResourceString t_Key;

   for (s32_Index = 1; s32_Index < static_cast<int32_t>(mac_Strings.GetLength()); s32_Index++)
   {
      t_Key = mac_Strings[s32_Index];
      s32_Position = s32_Index - 1;
      while ((s32_Position >= 0) && (mac_Strings[s32_Position].u16_Index > t_Key.u16_Index))
      {
         mac_Strings[s32_Position + 1] = mac_Strings[s32_Position];
         s32_Position--;
      }
      mac_Strings[s32_Position + 1] = t_Key;
   }
   mq_Dirty = false;
}

//----------------------------------------------------------------------------------------------------------------------

void C_SCLResourceStrings::SetStringTable(const C_SclResourceString * const opc_Strings, const uint16_t ou16_NumStrings)
{
   uint16_t u16_Index;

   try
   {
      mac_Strings.SetLength(ou16_NumStrings);
   }
   catch (...)
   {
      throw ("C_SCLResourceStrings::SetStringTable: could not allocate memory for ressource string table !");
   }

   for (u16_Index = 0U; u16_Index < ou16_NumStrings; u16_Index++)
   {
      mac_Strings[u16_Index] = opc_Strings[u16_Index];
   }
   this->SortByIndex();
}

//----------------------------------------------------------------------------------------------------------------------
