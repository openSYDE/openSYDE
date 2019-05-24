//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for a list in a data pool (implementation)

   Data class for a list in a data pool

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "C_OSCNodeDataPoolList.h"
#include "C_OSCUtils.h"
#include "CSCLChecksums.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_errors;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//-----------------------------------------------------------------------------
C_OSCNodeDataPoolList::C_OSCNodeDataPoolList(void) :
   c_Name("NewList"),
   c_Comment(""),
   q_NvMCRCActive(false),
   u32_NvMCRC(0),
   u32_NvMStartAddress(0),
   u32_NvMSize(100),
   c_Elements(),
   c_DataSets()
{
   c_Elements.resize(1);
}

//-----------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out] oru32_HashValue    Hash value with initial [in] value and result [out] value
*/
//-----------------------------------------------------------------------------
void C_OSCNodeDataPoolList::CalcHash(uint32 & oru32_HashValue) const
{
   uint32 u32_Counter;

   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_NvMCRC, sizeof(this->u32_NvMCRC), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_NvMStartAddress, sizeof(this->u32_NvMStartAddress), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_NvMSize, sizeof(this->u32_NvMSize), oru32_HashValue);

   for (u32_Counter = 0U; u32_Counter < this->c_Elements.size(); ++u32_Counter)
   {
      this->c_Elements[u32_Counter].CalcHash(oru32_HashValue);
   }

   for (u32_Counter = 0U; u32_Counter < this->c_DataSets.size(); ++u32_Counter)
   {
      this->c_DataSets[u32_Counter].CalcHash(oru32_HashValue);
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Recalculate data pool list element addresses
*/
//-----------------------------------------------------------------------------
void C_OSCNodeDataPoolList::RecalculateAddress(void)
{
   uint32 u32_Offset = this->u32_NvMStartAddress;

   //If there is a CRC two bytes are reserved at the start
   if (this->q_NvMCRCActive == true)
   {
      u32_Offset += 2;
   }

   for (uint32 u32_ItDataElement = 0; u32_ItDataElement < this->c_Elements.size(); ++u32_ItDataElement)
   {
      C_OSCNodeDataPoolListElement & rc_CurElem = this->c_Elements[u32_ItDataElement];
      rc_CurElem.u32_NvMStartAddress = u32_Offset;
      u32_Offset += rc_CurElem.GetSizeByte();
   }
}
//-----------------------------------------------------------------------------
/*! \brief   Move list in data pool

   \param[in] oru32_Start  Start index
   \param[in] oru32_Target Target index
*/
//-----------------------------------------------------------------------------
void C_OSCNodeDataPoolList::MoveElement(const stw_types::uint32 & oru32_Start, const stw_types::uint32 & oru32_Target)
{
   if ((oru32_Start < this->c_Elements.size()) && (oru32_Target < this->c_Elements.size()))
   {
      //Copy
      C_OSCNodeDataPoolListElement c_ListElementData = this->c_Elements[oru32_Start];
      //Erase
      this->c_Elements.erase(this->c_Elements.begin() + oru32_Start);
      //Insert
      this->c_Elements.insert(this->c_Elements.begin() + oru32_Target, c_ListElementData);
      RecalculateAddress();
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Get number of bytes occupied by variables (including CRC space)

   \return
   Number of bytes occupied by variables (including CRC space)
*/
//-----------------------------------------------------------------------------
uint32 C_OSCNodeDataPoolList::GetNumBytesUsed(void) const
{
   uint32 u32_Retval = 0;

   if (this->q_NvMCRCActive == true)
   {
      u32_Retval += 2;
   }
   for (uint32 u32_ItListElement = 0; u32_ItListElement < this->c_Elements.size(); ++u32_ItListElement)
   {
      const C_OSCNodeDataPoolListElement & rc_NodeDataPoolListElement = this->c_Elements[u32_ItListElement];
      u32_Retval += rc_NodeDataPoolListElement.GetSizeByte();
   }
   return u32_Retval;
}

//-----------------------------------------------------------------------------
/*! \brief   Get number of bytes not occupied by variables

   \return
   Number of bytes not occupied by variables
*/
//-----------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolList::GetFreeBytes(void) const
{
   return static_cast<sint32>(static_cast<sint64>(u32_NvMSize) - static_cast<sint64>(GetNumBytesUsed()));
}

//-----------------------------------------------------------------------------
/*! \brief   Check error for specified data set

   \param[in]  oru32_DataSetIndex  Node data pool list data set index
   \param[out] opq_NameConflict    Name conflict
   \param[out] opq_NameInvalid     Name not usable as variable
*/
//-----------------------------------------------------------------------------
void C_OSCNodeDataPoolList::CheckErrorDataSet(const uint32 & oru32_DataSetIndex, bool * const opq_NameConflict,
                                              bool * const opq_NameInvalid) const
{
   //Init
   if (oru32_DataSetIndex < this->c_DataSets.size())
   {
      const C_OSCNodeDataPoolDataSet & rc_CurrentElement = this->c_DataSets[oru32_DataSetIndex];
      //Check variable name
      if (opq_NameInvalid != NULL)
      {
         if (C_OSCUtils::h_CheckValidCName(rc_CurrentElement.c_Name) == false)
         {
            *opq_NameInvalid = true;
         }
         else
         {
            *opq_NameInvalid = false;
         }
      }
      //Name conflict
      if (opq_NameConflict != NULL)
      {
         *opq_NameConflict = false;
         for (uint32 u32_ItDataSet = 0; (u32_ItDataSet < this->c_DataSets.size()) && (*opq_NameConflict == false);
              ++u32_ItDataSet)
         {
            if (u32_ItDataSet != oru32_DataSetIndex)
            {
               const C_OSCNodeDataPoolDataSet & rc_DataSet = this->c_DataSets[u32_ItDataSet];
               if (rc_CurrentElement.c_Name.LowerCase() == rc_DataSet.c_Name.LowerCase())
               {
                  *opq_NameConflict = true;
               }
            }
         }
      }
   }
   else
   {
      if (opq_NameConflict != NULL)
      {
         *opq_NameConflict = false;
      }
      if (opq_NameInvalid != NULL)
      {
         *opq_NameInvalid = false;
      }
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Check error for specified element

   \param[in]  oru32_ElementIndex         Node data pool list element index
   \param[out] opq_NameConflict           Name conflict
   \param[out] opq_NameInvalid            Name not usable as variable
   \param[out] opq_MinOverMax             Minimum value over maximum value
   \param[out] opq_DataSetValueInvalid    Data set value out of range
   \param[out] opc_InvalidDataSetIndices  List of indexes of troublesome data sets
*/
//-----------------------------------------------------------------------------
void C_OSCNodeDataPoolList::CheckErrorElement(const uint32 & oru32_ElementIndex, bool * const opq_NameConflict,
                                              bool * const opq_NameInvalid, bool * const opq_MinOverMax,
                                              bool * const opq_DataSetValueInvalid,
                                              std::vector<uint32> * const opc_InvalidDataSetIndices) const
{
   //Init
   if (oru32_ElementIndex < this->c_Elements.size())
   {
      const C_OSCNodeDataPoolListElement & rc_CurrentElement = this->c_Elements[oru32_ElementIndex];
      //Check variable name
      if (opq_NameInvalid != NULL)
      {
         if (C_OSCUtils::h_CheckValidCName(rc_CurrentElement.c_Name) == false)
         {
            *opq_NameInvalid = true;
         }
         else
         {
            *opq_NameInvalid = false;
         }
      }
      //Name conflict
      if (opq_NameConflict != NULL)
      {
         *opq_NameConflict = false;
         for (uint32 u32_ItElement = 0; (u32_ItElement < this->c_Elements.size()) && (*opq_NameConflict == false);
              ++u32_ItElement)
         {
            if (u32_ItElement != oru32_ElementIndex)
            {
               const C_OSCNodeDataPoolListElement & rc_ListElement = this->c_Elements[u32_ItElement];
               if (rc_CurrentElement.c_Name.LowerCase() == rc_ListElement.c_Name.LowerCase())
               {
                  *opq_NameConflict = true;
               }
            }
         }
      }
      //Check min max
      if (opq_MinOverMax != NULL)
      {
         if (rc_CurrentElement.c_MinValue <= rc_CurrentElement.c_MaxValue)
         {
            *opq_MinOverMax = false;
         }
         else
         {
            *opq_MinOverMax = true;
         }
      }
      //Check data sets
      if (opq_DataSetValueInvalid != NULL)
      {
         *opq_DataSetValueInvalid = false;
         for (uint32 u32_ItDataSet = 0; u32_ItDataSet < rc_CurrentElement.c_DataSetValues.size(); ++u32_ItDataSet)
         {
            bool q_ValueBelowMin = false;
            bool q_ValueOverMax = false;
            this->CheckErrorDataSetValue(oru32_ElementIndex, u32_ItDataSet, &q_ValueBelowMin, &q_ValueOverMax, NULL);
            if ((q_ValueBelowMin == true) || (q_ValueOverMax == true))
            {
               *opq_DataSetValueInvalid = true;
               if (opc_InvalidDataSetIndices != NULL)
               {
                  opc_InvalidDataSetIndices->push_back(u32_ItDataSet);
               }
            }
         }
      }
   }
   else
   {
      if (opq_NameConflict != NULL)
      {
         *opq_NameConflict = false;
      }
      if (opq_NameInvalid != NULL)
      {
         *opq_NameInvalid = false;
      }
      if (opq_MinOverMax != NULL)
      {
         *opq_MinOverMax = false;
      }
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Check error for specified data set value

   \param[in]  oru32_ElementIndex Node data pool list element index
   \param[in]  oru32_DataSetIndex Node data pool list data set index
   \param[out] opq_ValueBelowMin  Data set value below minimum
   \param[out] opq_ValueOverMax   Data set value over maximum
   \param[in] opu32_ArrayIndex   Optional parameter to check only a single data set array index
*/
//-----------------------------------------------------------------------------
void C_OSCNodeDataPoolList::CheckErrorDataSetValue(const uint32 & oru32_ElementIndex, const uint32 & oru32_DataSetIndex,
                                                   bool * const opq_ValueBelowMin, bool * const opq_ValueOverMax,
                                                   const uint32 * const opu32_ArrayIndex) const
{
   if (opq_ValueBelowMin != NULL)
   {
      *opq_ValueBelowMin = false;
   }
   if (opq_ValueOverMax != NULL)
   {
      *opq_ValueOverMax = false;
   }
   if (oru32_ElementIndex < this->c_Elements.size())
   {
      const C_OSCNodeDataPoolListElement & rc_CurrentElement = this->c_Elements[oru32_ElementIndex];
      if (oru32_DataSetIndex < rc_CurrentElement.c_DataSetValues.size())
      {
         const C_OSCNodeDataPoolContent & rc_DataSetValue = rc_CurrentElement.c_DataSetValues[oru32_DataSetIndex];
         if (opq_ValueBelowMin != NULL)
         {
            *opq_ValueBelowMin = false;
            if (opu32_ArrayIndex == NULL)
            {
               if ((rc_DataSetValue >= rc_CurrentElement.c_MinValue) == false)
               {
                  *opq_ValueBelowMin = true;
               }
            }
            else
            {
               if (rc_DataSetValue.CompareArrayGreaterOrEqual(rc_CurrentElement.c_MinValue, *opu32_ArrayIndex) == false)
               {
                  *opq_ValueBelowMin = true;
               }
            }
         }
         if (opq_ValueOverMax != NULL)
         {
            *opq_ValueOverMax = false;
            if (opu32_ArrayIndex == NULL)
            {
               if ((rc_DataSetValue <= rc_CurrentElement.c_MaxValue) == false)
               {
                  *opq_ValueOverMax = true;
               }
            }
            else
            {
               if (rc_DataSetValue.CompareArrayGreater(rc_CurrentElement.c_MaxValue, *opu32_ArrayIndex) == true)
               {
                  *opq_ValueOverMax = true;
               }
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Set CRC from big endian BLOB

   Convert BLOB to CRC.
   Warning: 2 Byte CRC expected

   \param[in]     orc_Data    data to set

   \return
   C_NO_ERR   value set
   C_RANGE    size of orc_Data does not match our size
*/
//-----------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolList::SetCRCFromBigEndianBlob(const std::vector<uint8> & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_Data.size() == 2)
   {
      this->u32_NvMCRC = static_cast<uint32>((static_cast<uint32>(orc_Data[0]) << 8U)) + orc_Data[1];
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*! \brief   Set CRC from little endian BLOB

   Convert BLOB to CRC.
   Warning: 2 Byte CRC expected

   \param[in]     orc_Data    data to set

   \return
   C_NO_ERR   value set
   C_RANGE    size of orc_Data does not match our size
*/
//-----------------------------------------------------------------------------
sint32 C_OSCNodeDataPoolList::SetCRCFromLittleEndianBlob(const std::vector<uint8> & orc_Data)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_Data.size() == 2)
   {
      this->u32_NvMCRC = static_cast<uint32>((static_cast<uint32>(orc_Data[1]) << 8U)) + orc_Data[0];
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*! \brief   Get CRC as big endian BLOB

   Convert content CRC to BLOB.
   Warning: 2 Byte CRC expected

   \param[out]     orc_Data    data to set
*/
//-----------------------------------------------------------------------------
void C_OSCNodeDataPoolList::GetCRCAsBigEndianBlob(std::vector<uint8> & orc_Data) const
{
   const uint16 u16_Value = static_cast<uint16>(this->u32_NvMCRC);

   orc_Data.resize(2);
   orc_Data[0] = static_cast<uint8>(u16_Value >> 8U);
   orc_Data[1] = static_cast<uint8>(u16_Value);
}

//-----------------------------------------------------------------------------
/*! \brief   Get CRC as little endian BLOB

   Convert content CRC to BLOB.
   Warning: 2 Byte CRC expected

   \param[out]     orc_Data    data to set
*/
//-----------------------------------------------------------------------------
void C_OSCNodeDataPoolList::GetCRCAsLittleEndianBlob(std::vector<uint8> & orc_Data) const
{
   const uint16 u16_Value = static_cast<uint16>(this->u32_NvMCRC);

   orc_Data.resize(2);
   orc_Data[1] = static_cast<uint8>(u16_Value >> 8U);
   orc_Data[0] = static_cast<uint8>(u16_Value);
}
