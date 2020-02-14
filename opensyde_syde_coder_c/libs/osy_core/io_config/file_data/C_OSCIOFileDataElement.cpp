//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       IO description generic element part

   IO description generic element part

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "C_OSCIOFileDataElement.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
C_OSCIOFileDataElement::C_OSCIOFileDataElement(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCIOFileDataElement::~C_OSCIOFileDataElement(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Change type

   Attempt conversion of previous value(s) to new type

   \param[in] oe_Value Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCIOFileDataElement::SetType(const C_OSCNodeDataPoolContent::E_Type oe_Value)
{
   this->c_InitialValue.SetType(oe_Value);
   this->c_MinValue.SetType(oe_Value);
   this->c_MaxValue.SetType(oe_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Change array status

   \param[in] oq_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCIOFileDataElement::SetArray(const bool oq_Value)
{
   this->c_InitialValue.SetArray(oq_Value);
   this->c_MinValue.SetArray(oq_Value);
   this->c_MaxValue.SetArray(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set complex type

   \param[in] oe_Type Type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCIOFileDataElement::SetComplexType(const C_OSCIOFileDataContent::E_ComplexType oe_Type)
{
   this->c_InitialValue.SetComplexType(oe_Type);
   this->c_MinValue.SetComplexType(oe_Type);
   this->c_MaxValue.SetComplexType(oe_Type);
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
stw_types::sint32 C_OSCIOFileDataElement::AddEnumItem(const stw_scl::C_SCLString & orc_DisplayName,
                                                      const C_OSCNodeDataPoolContent & orc_Value)
{
   stw_types::sint32 s32_Retval = this->c_InitialValue.AddEnumItem(orc_DisplayName, orc_Value);
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

   \param[in] orc_Value New bitmask item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCIOFileDataElement::AddBitmaskItem(const C_OSCIOFileDataContentBitmaskItem & orc_Value)
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
C_OSCIOFileDataContent::E_ComplexType C_OSCIOFileDataElement::GetComplexType() const
{
   return this->c_InitialValue.GetComplexType();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get type

   \return
   Type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolContent::E_Type C_OSCIOFileDataElement::GetType() const
{
   return this->c_InitialValue.GetType();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get array status

   \return
   Array status
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCIOFileDataElement::GetArray() const
{
   return this->c_InitialValue.GetArray();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get enum items

   \return
   Enum items
*/
//----------------------------------------------------------------------------------------------------------------------
const std::map<stw_scl::C_SCLString, C_OSCNodeDataPoolContent> & C_OSCIOFileDataElement::GetEnumItems() const
{
   return this->c_InitialValue.GetEnumItems();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get bitmask items

   \return
   Bitmask items
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_OSCIOFileDataContentBitmaskItem> & C_OSCIOFileDataElement::GetBitmaskItems() const
{
   return this->c_InitialValue.GetBitmaskItems();
}
