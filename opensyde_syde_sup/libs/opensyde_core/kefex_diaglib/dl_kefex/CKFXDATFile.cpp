//Note: for multiple data sets there is no matching of data set names. It is the job of the application to care for
// consistency.
#include "precomp_headers.hpp" //pre-compiled headers

#include "DiagLib_config.hpp" //diaglib configuration

#include <cstdio>
#include <cstring>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "CKFXDATFile.hpp"
#include "C_OscZipData.hpp"
#include "C_SclString.hpp"
#include "TglFile.hpp"
#include "TglUtils.hpp"

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::diag_lib;
using namespace stw::opensyde_core;

//binary file format block 0x0100
// required information:
//  Once:
//  - block ID 0x0100          -> uint16_t
//  - devicename               -> string
//  - number of lists          -> uint16_t
//  For Each list:
//  - listname                 -> string
//  - Number of variables      -> uint16_t
//  For each variable:
//       -listname             -> string
//       -variablename         -> string
//       -variablesize         -> uint32_t
//       -value                -> aobyte

//binary file format block 0x0101
// required information:
//  Once:
//  - total size of block      -> uint32_t (starting from block-ID)
//  - block ID 0x0101          -> uint16_t
//  - number of lists          -> uint16_t (must match information from block 0x0100)
//  For Each list:
//  - Number of variables      -> uint16_t (must match information from block 0x0100)
//  - number of data sets      -> uint16_t
//  For each variable:
//       -variablesize         -> uint32_t (must match information from block 0x0100)
//       -value(s)             -> num-sets * size * uint8_t

//compatibility concept:
// block 0x0100 MUST be present
// additional blocks will be parsed if present and understood; else they will be ignored

static const uint16_t KFX_DAT_FILE_BLOCK_0100 = 0x0100U;
static const uint16_t KFX_DAT_FILE_BLOCK_0101 = 0x0101U;

//--------------------------------------------------------------

int32_t C_KFXDATFile::LoadDATAllLists(const C_SclString & orc_FileName, const C_SclString & orc_DeviceName,
                                      C_KFXVariableLists & orc_VariableLists,
                                      C_SclDynamicArray<uint8_t> * const opc_ListsLoaded)
{
   return LoadDATList(orc_FileName, orc_DeviceName, orc_VariableLists, -1, opc_ListsLoaded);
}

//--------------------------------------------------------------

void C_KFXDATFile::m_GetWordFromBuffer(uint16_t * const opu16_Value, const uint8_t ** const oppu8_Buffer)
{
   // DAT file format litte endian
   (*opu16_Value) = static_cast<uint16_t>((*oppu8_Buffer)[0] +
                                          static_cast<uint16_t>(((static_cast<uint16_t>((*oppu8_Buffer)[1])) << 8U)));
   *oppu8_Buffer += sizeof(uint16_t);
}

//--------------------------------------------------------------

void C_KFXDATFile::m_GetDWordFromBuffer(uint32_t * const opu32_Value, const uint8_t ** const oppu8_Buffer)
{
   // DAT file format litte endian
   (*opu32_Value) = (*oppu8_Buffer)[0] +
                    ((static_cast<uint32_t>((*oppu8_Buffer)[1])) << 8U) +
                    ((static_cast<uint32_t>((*oppu8_Buffer)[2])) << 16U) +
                    ((static_cast<uint32_t>((*oppu8_Buffer)[3])) << 24U);
   *oppu8_Buffer += sizeof(uint32_t);
}

//--------------------------------------------------------------

//extract a string from the stream and advance stream pointer
void C_KFXDATFile::m_GetStringFromBuffer(C_SclString & orc_String, const uint8_t ** const oppu8_Buffer)
{
   char_t * pcn_Buffer;
   uint16_t u16_Length;

   m_GetWordFromBuffer(&u16_Length, oppu8_Buffer);
   pcn_Buffer = new char_t[u16_Length + 1U];
   (void)memcpy(pcn_Buffer, *oppu8_Buffer, u16_Length);
   pcn_Buffer[u16_Length] = '\0'; //terminate
   orc_String = pcn_Buffer;
   delete[] pcn_Buffer;
   (*oppu8_Buffer) += u16_Length;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Read values from a .dat

   Sequence:
   - open .dat file and read all data
   - match data from .dat file and place it in the specified VariableList's Variable's defaults

   \param[in]     orc_FileName         File path to .dat file
   \param[in]     orc_DeviceName       Device name expected in .dat file
   \param[in,out] orc_VariableLists    Variable lists to fill with the values read from the .dat file
   \param[in]     os32_ListIndex       -1: extract values for all variable lists
                                       else: extract only values for the specified list index
   \param[out]    opc_ListsLoaded      A flag for each list in the KEFEX project
                                         0: Values for list not loaded from file (list is not in file)
                                        >0: Values for list loaded from file

   \return
   C_RANGE    -> invalid os32_ListIndex (not available in oc_VariableLists)
   C_NOACT    -> could not open file        \n
                 or: not a valid .dat file
   C_CONFIG   -> .dat file has an unknown file version  \n
                 or: .dat file is for a different project (oc_DeviceName does not match)
   C_WARN     -> .dat read, but at least one variable was not found in oc_VariableLists
   C_OVERFLOW -> could not allocate system memory
*/
//-----------------------------------------------------------------------------
int32_t C_KFXDATFile::LoadDATList(const C_SclString & orc_FileName, const C_SclString & orc_DeviceName,
                                  C_KFXVariableLists & orc_VariableLists, const int32_t os32_ListIndex,
                                  C_SclDynamicArray<uint8_t> * const opc_ListsLoaded)
{
   std::FILE * pt_FileHandle;
   int32_t s32_NumBytes;
   int32_t s32_Return;
   uint32_t u32_Return;
   uint8_t * pu8_Buffer;
   uint8_t * pu8_Zipped;
   const uint8_t * pu8_Act;
   uint32_t u32_Required;
   uint32_t u32_UnpackedSize;
   C_SclString c_Temp;
   uint16_t u16_Version;
   uint16_t u16_NumLists;
   uint8_t au8_Buffer[sizeof(uint32_t)];

   if ((os32_ListIndex != -1) && (os32_ListIndex > orc_VariableLists.GetHigh()))
   {
      return C_RANGE;
   }

   s32_NumBytes = TglFileSize(orc_FileName.c_str());

   pt_FileHandle = std::fopen(orc_FileName.c_str(), "rb");
   if (pt_FileHandle == NULL)
   {
      return C_NOACT;
   }

   u32_Return = fread(&au8_Buffer[0], 1UL, sizeof(uint32_t), pt_FileHandle);
   if (u32_Return != sizeof(uint32_t))
   {
      return C_NOACT;
   }
   pu8_Act = &au8_Buffer[0];
   m_GetDWordFromBuffer(&u32_Required, &pu8_Act);

   try
   {
      pu8_Zipped = new uint8_t[static_cast<uint32_t>(s32_NumBytes) - sizeof(uint32_t)];
   }
   catch (const std::bad_alloc &)
   {
      pu8_Zipped = NULL;
      return C_OVERFLOW; //lint !e438 //placate lint ...
   }

   u32_Return = fread(pu8_Zipped, 1UL, static_cast<uint32_t>(s32_NumBytes) - sizeof(uint32_t), pt_FileHandle);
   fclose(pt_FileHandle);
   if (u32_Return != (static_cast<uint32_t>(s32_NumBytes) - sizeof(uint32_t)))
   {
      delete[] pu8_Zipped;
      return C_NOACT;
   }

   try
   {
      pu8_Buffer = new uint8_t[u32_Required];
   }
   catch (const std::bad_alloc &)
   {
      delete[] pu8_Zipped;
      pu8_Buffer = NULL;
      return C_NOACT; //lint !e438 //placate lint ...
   }
   u32_UnpackedSize = u32_Required;
   s32_Return =
      C_OscZipData::h_Unzip(pu8_Buffer, u32_UnpackedSize, pu8_Zipped,
                            static_cast<uint32_t>(s32_NumBytes) - sizeof(uint32_t));
   delete[] pu8_Zipped;
   if ((u32_UnpackedSize != u32_Required) || (s32_Return != C_NO_ERR))
   {
      delete[] pu8_Buffer;
      return C_NOACT;
   }

   pu8_Act = &pu8_Buffer[0];

   //check version:
   m_GetWordFromBuffer(&u16_Version, &pu8_Act);
   if (u16_Version != KFX_DAT_FILE_BLOCK_0100) //first block MUST be 0100
   {
      delete[] pu8_Buffer;
      return C_CONFIG;
   }

   //everything looks fine ...
   //now copy the data ...

   m_GetStringFromBuffer(c_Temp, &pu8_Act); //get device name
   if (c_Temp != orc_DeviceName)
   {
      //not a default set for this project !
      delete[] pu8_Buffer;
      return C_CONFIG;
   }

   m_GetWordFromBuffer(&u16_NumLists, &pu8_Act); //number of lists
   u32_UnpackedSize -= static_cast<uint32_t>(pu8_Act - pu8_Buffer);

   if (os32_ListIndex == -1)
   {
      s32_Return = m_BufferToLists(pu8_Act, orc_VariableLists, u16_NumLists, u32_UnpackedSize,
                                   false, 0U, opc_ListsLoaded);
   }
   else
   {
      s32_Return = m_BufferToLists(pu8_Act, orc_VariableLists, u16_NumLists, u32_UnpackedSize,
                                   true, static_cast<uint16_t>(os32_ListIndex), opc_ListsLoaded);
   }

   delete[] pu8_Buffer;
   return s32_Return;
}

//--------------------------------------------------------------
//C_NO_ERR  -> no problems
//C_WARN    -> at least one variable in buffer does not exist in VariableLists (was ignored)
//ou16_NumListsInDat: number of lists in .dat file !
int32_t C_KFXDATFile::m_BufferToLists(const uint8_t * const opu8_Buffer, C_KFXVariableLists & orc_VariableLists,
                                      const uint16_t ou16_NumListsInDat, const uint32_t ou32_NumBytesTotal,
                                      const bool oq_SingleList, const uint16_t ou16_SingleListIndex,
                                      C_SclDynamicArray<uint8_t> * const opc_ListsLoaded)
{
   C_SclString c_List;
   C_SclString c_Var;
   uint16_t u16_Value;
   uint16_t u16_NumVars;
   uint16_t u16_ListIndex = 0U;
   int32_t i;
   int32_t s32_Variable;
   int32_t s32_List;
   int32_t j;
   int32_t s32_k = 0;
   int32_t s32_Return = C_NO_ERR;
   bool q_Found;
   uint32_t u32_Value;
   uint32_t u32_Size;
   uint16_t u16_NumDefaults;
   uint32_t u32_NumBytesLeft;
   C_KFXVariableBase * pc_Variable;
   const uint8_t * pu8_Data = opu8_Buffer;

   C_SclDynamicArray<C_SclDynamicArray<C_KFXVariableBase *> > aapc_IndexMapping;
   bool q_AtLeastOneMissing = false;

   if (opc_ListsLoaded != NULL)
   {
      (*opc_ListsLoaded).SetLength(orc_VariableLists.GetLength());
      for (s32_List = 0; s32_List < (*opc_ListsLoaded).GetLength(); s32_List++)
      {
         (*opc_ListsLoaded)[s32_List] = 0;
      }
   }

   //First clear all Defaultvalues of the Device
   if (oq_SingleList == true)
   {
      for (s32_Variable = 0; s32_Variable < orc_VariableLists[ou16_SingleListIndex].VariableList.GetLength();
           s32_Variable++)
      {
         orc_VariableLists[ou16_SingleListIndex].VariableList[s32_Variable].ClearAllDefaults();
      }
   }
   else
   {
      orc_VariableLists.ClearDefaults();
   }
   aapc_IndexMapping.SetLength(ou16_NumListsInDat); //remember mappings, so we do not need to reparse following blocks

   for (i = 0; i < ou16_NumListsInDat; i++)
   {
      m_GetStringFromBuffer(c_List, &pu8_Data); //get list name

      q_Found = false;

      if (oq_SingleList == true)
      {
         if (orc_VariableLists[ou16_SingleListIndex].c_ListName.UpperCase() == c_List.UpperCase())
         {
            q_Found = true;
            u16_ListIndex = ou16_SingleListIndex;
         }
      }
      else
      {
         for (s32_List = 0; s32_List < orc_VariableLists.GetLength(); s32_List++)
         {
            if (orc_VariableLists[s32_List].c_ListName.UpperCase() == c_List.UpperCase())
            {
               q_Found = true;
               u16_ListIndex = static_cast<uint16_t>(s32_List);
               break;
            }
         }
      }

      if (q_Found == true)
      {
         if (opc_ListsLoaded != NULL)
         {
            (*opc_ListsLoaded)[u16_ListIndex] = 1;
         }
      }

      m_GetWordFromBuffer(&u16_NumVars, &pu8_Data); //number of variables for that list

      aapc_IndexMapping[i].SetLength(u16_NumVars);
      for (j = 0; j < u16_NumVars; j++)
      {
         m_GetStringFromBuffer(c_Var, &pu8_Data); //get variable name

         if (q_Found == true)
         {
            //try to find variable within the list:
            q_Found = false;
            for (s32_k = 0; s32_k < orc_VariableLists[u16_ListIndex].VariableList.GetLength(); s32_k++)
            {
               if (orc_VariableLists[u16_ListIndex].VariableList[s32_k].c_Name.UpperCase() == c_Var.UpperCase())
               {
                  q_Found = true;
                  break;
               }
            }
         }
         m_GetDWordFromBuffer(&u32_Value, &pu8_Data); //size of entry
         if (q_Found == true)
         {
            pc_Variable = &orc_VariableLists[u16_ListIndex].VariableList[s32_k];

            u32_Size = (u32_Value > pc_Variable->GetSize()) ? pc_Variable->GetSize() : u32_Value;
            (void)memcpy(&pc_Variable->aau8_Defaults[0][0], pu8_Data, u32_Size); //data
            //remember mappings, so we do not need to reparse following blocks
            aapc_IndexMapping[i][j] = pc_Variable;
         }
         else
         {
            aapc_IndexMapping[i][j] = NULL; //not found
            q_AtLeastOneMissing = true;
         }
         pu8_Data += u32_Value;
      }
   }

   tgl_assert(pu8_Data >= opu8_Buffer);

   u32_NumBytesLeft = ou32_NumBytesTotal - static_cast<uint32_t>(pu8_Data - opu8_Buffer);
   while (u32_NumBytesLeft > (sizeof(uint32_t) + sizeof(uint16_t))) //looks like there is an additional block !
   {
      m_GetDWordFromBuffer(&u32_Size, &pu8_Data); //block size
      m_GetWordFromBuffer(&u16_Value, &pu8_Data); //block-id
      u32_NumBytesLeft -= (sizeof(uint32_t) + sizeof(uint16_t));
      switch (u16_Value)
      {
      case KFX_DAT_FILE_BLOCK_0101:
         tgl_assert(u32_NumBytesLeft > sizeof(uint16_t));
         m_GetWordFromBuffer(&u16_Value, &pu8_Data);  //number of lists !
         tgl_assert(ou16_NumListsInDat == u16_Value); //must match the one from the first block
         u32_NumBytesLeft -= sizeof(uint16_t);
         for (i = 0; i < ou16_NumListsInDat; i++)
         {
            m_GetWordFromBuffer(&u16_Value, &pu8_Data); //number of variables !
            u32_NumBytesLeft -= sizeof(uint16_t);
            tgl_assert(u16_Value == static_cast<uint16_t>(aapc_IndexMapping[i].GetLength()));

            m_GetWordFromBuffer(&u16_NumDefaults, &pu8_Data); //number of default sets !
            u32_NumBytesLeft -= sizeof(uint16_t);

            for (j = 0; j < aapc_IndexMapping[i].GetLength(); j++)
            {
               m_GetDWordFromBuffer(&u32_Value, &pu8_Data); //variable size
               u32_NumBytesLeft -= sizeof(uint32_t);

               if (aapc_IndexMapping[i][j] != NULL)
               {
                  pc_Variable = aapc_IndexMapping[i][j];
                  u32_Size = (u32_Value > pc_Variable->GetSize()) ? pc_Variable->GetSize() : u32_Value;
                  for (s32_k = 0; s32_k < u16_NumDefaults; s32_k++)
                  {
                     if (s32_k < pc_Variable->GetNumDefaults())
                     {
                        (void)memcpy(&pc_Variable->aau8_Defaults[s32_k][0], &pu8_Data[s32_k * u32_Value], u32_Size); //data
                     }
                     else
                     {
                        //we might want to flag this somehow ...
                     }
                  }
               }
               pu8_Data += u32_Value * u16_NumDefaults;
               u32_NumBytesLeft -= u32_Value * u16_NumDefaults;
            }
         }
         break;
      default:
         //nothing we can handle (yet) !
         break;
      }
   }

   if (q_AtLeastOneMissing == true)
   {
      s32_Return = C_WARN;
   }

   return s32_Return;
}

//--------------------------------------------------------------
