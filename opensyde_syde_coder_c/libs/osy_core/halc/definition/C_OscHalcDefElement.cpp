//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition generic element part

   HALC definition generic element part

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscHalcDefElement.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

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
C_OscHalcDefElement::C_OscHalcDefElement(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscHalcDefElement::~C_OscHalcDefElement(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Change type

   Attempt conversion of previous value(s) to new type

   \param[in]  oe_Value    Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefElement::SetType(const C_OscNodeDataPoolContent::E_Type oe_Value)
{
   this->c_InitialValue.SetType(oe_Value);
   this->c_MinValue.SetType(oe_Value);
   this->c_MaxValue.SetType(oe_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Change array status

   \param[in]  oq_Value    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefElement::SetArray(const bool oq_Value)
{
   this->c_InitialValue.SetArray(oq_Value);
   this->c_MinValue.SetArray(oq_Value);
   this->c_MaxValue.SetArray(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set complex type

   \param[in]  oe_Type  Type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefElement::SetComplexType(const C_OscHalcDefContent::E_ComplexType oe_Type)
{
   this->c_InitialValue.SetComplexType(oe_Type);
   this->c_MinValue.SetComplexType(oe_Type);
   this->c_MaxValue.SetComplexType(oe_Type);
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
int32_t C_OscHalcDefElement::AddEnumItem(const stw::scl::C_SclString & orc_DisplayName,
                                         const C_OscNodeDataPoolContent & orc_Value)
{
   int32_t s32_Retval = this->c_InitialValue.AddEnumItem(orc_DisplayName, orc_Value);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = this->c_MinValue.AddEnumItem(orc_DisplayName, orc_Value);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = this->c_MaxValue.AddEnumItem(orc_DisplayName, orc_Value);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add bitmask item

   \param[in]  orc_Value   New bitmask item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefElement::AddBitmaskItem(const C_OscHalcDefContentBitmaskItem & orc_Value)
{
   this->c_InitialValue.AddBitmaskItem(orc_Value);
   this->c_MinValue.AddBitmaskItem(orc_Value);
   this->c_MaxValue.AddBitmaskItem(orc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get bitmask status

   \return
   Bitmask status
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscHalcDefContent::E_ComplexType C_OscHalcDefElement::GetComplexType() const
{
   return this->c_InitialValue.GetComplexType();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get type

   \return
   Type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent::E_Type C_OscHalcDefElement::GetType() const
{
   return this->c_InitialValue.GetType();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get array status

   \return
   Array status
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscHalcDefElement::GetArray() const
{
   return this->c_InitialValue.GetArray();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get enum items

   \return
   Enum items
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<std::pair<stw::scl::C_SclString,
                            C_OscNodeDataPoolContent> > & C_OscHalcDefElement::GetEnumItems() const
{
   return this->c_InitialValue.GetEnumItems();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get bitmask items

   \return
   Bitmask items
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_OscHalcDefContentBitmaskItem> & C_OscHalcDefElement::GetBitmaskItems() const
{
   return this->c_InitialValue.GetBitmaskItems();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefElement::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Id.c_str(), this->c_Id.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Display.c_str(), this->c_Display.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);

   this->c_InitialValue.CalcHash(oru32_HashValue);
   this->c_MinValue.CalcHash(oru32_HashValue);
   this->c_MaxValue.CalcHash(oru32_HashValue);

   for (uint32_t u32_It = 0UL; u32_It < this->c_UseCaseAvailabilities.size(); ++u32_It)
   {
      stw::scl::C_SclChecksums::CalcCRC32(&this->c_UseCaseAvailabilities[u32_It],
                                          sizeof(uint32_t), oru32_HashValue);
   }
}
