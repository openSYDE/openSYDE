//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to store all indices to identify a data element in the system view (implementation)

   Utility class to store all indices to identify a data element in the system view

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_GtGetText.hpp"
#include "C_SclChecksums.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbNodeDataPoolListElementId::C_PuiSvDbNodeDataPoolListElementId(void) :
   C_OscNodeDataPoolListElementOptArrayId(),
   me_Type(eDATAPOOL_ELEMENT),
   mq_IsValid(true),
   me_InvalidTypePlaceholder(C_OscNodeDataPool::eDIAG)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  orc_Base                      Standard node data pool list element identification indices
   \param[in]  oe_Type                       Enum to denote the source type
   \param[in]  oq_UseArrayElementIndex       Flag to use array element index
   \param[in]  ou32_ArrayElementIndex        Array element index
   \param[in]  oq_IsValid                    Invalid flag
   \param[in]  oe_InvalidTypePlaceholder     Type placeholder to be used in case of invalid state
   \param[in]  orc_InvalidNamePlaceholder    Name placeholder to be used in case of invalid state
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbNodeDataPoolListElementId::C_PuiSvDbNodeDataPoolListElementId(const C_OscNodeDataPoolListElementId & orc_Base,
                                                                       const E_Type oe_Type,
                                                                       const bool oq_UseArrayElementIndex,
                                                                       const uint32_t ou32_ArrayElementIndex,
                                                                       const bool oq_IsValid,
                                                                       const C_OscNodeDataPool::E_Type oe_InvalidTypePlaceholder,
                                                                       const QString & orc_InvalidNamePlaceholder) :
   C_OscNodeDataPoolListElementOptArrayId(orc_Base, oq_UseArrayElementIndex, ou32_ArrayElementIndex),
   me_Type(oe_Type),
   mq_IsValid(oq_IsValid),
   me_InvalidTypePlaceholder(oe_InvalidTypePlaceholder),
   mc_InvalidNamePlaceholder(orc_InvalidNamePlaceholder)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  orc_Base                      Standard node data pool list element identification indices
   \param[in]  oe_Type                       Enum to denote the source type
   \param[in]  oq_IsValid                    Invalid flag
   \param[in]  oe_InvalidTypePlaceholder     Type placeholder to be used in case of invalid state
   \param[in]  orc_InvalidNamePlaceholder    Name placeholder to be used in case of invalid state
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbNodeDataPoolListElementId::C_PuiSvDbNodeDataPoolListElementId(
   const C_OscNodeDataPoolListElementOptArrayId & orc_Base, const E_Type oe_Type, const bool oq_IsValid,
   const C_OscNodeDataPool::E_Type oe_InvalidTypePlaceholder, const QString & orc_InvalidNamePlaceholder) :
   C_OscNodeDataPoolListElementOptArrayId(orc_Base),
   me_Type(oe_Type),
   mq_IsValid(oq_IsValid),
   me_InvalidTypePlaceholder(oe_InvalidTypePlaceholder),
   mc_InvalidNamePlaceholder(orc_InvalidNamePlaceholder)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  ou32_NodeIndex                Node index
   \param[in]  ou32_DataPoolIndex            Data pool index
   \param[in]  ou32_ListIndex                List index
   \param[in]  ou32_ElementIndex             Element index
   \param[in]  oe_Type                       Enum to denote the source type
   \param[in]  oq_UseArrayElementIndex       Flag to use array element index
   \param[in]  ou32_ArrayElementIndex        Array element index
   \param[in]  oq_IsValid                    Invalid flag
   \param[in]  oe_InvalidTypePlaceholder     Type placeholder to be used in case of invalid state
   \param[in]  orc_InvalidNamePlaceholder    Name placeholder to be used in case of invalid state
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbNodeDataPoolListElementId::C_PuiSvDbNodeDataPoolListElementId(const uint32_t ou32_NodeIndex,
                                                                       const uint32_t ou32_DataPoolIndex,
                                                                       const uint32_t ou32_ListIndex,
                                                                       const uint32_t ou32_ElementIndex,
                                                                       const E_Type oe_Type,
                                                                       const bool oq_UseArrayElementIndex,
                                                                       const uint32_t ou32_ArrayElementIndex,
                                                                       const bool oq_IsValid,
                                                                       const C_OscNodeDataPool::E_Type oe_InvalidTypePlaceholder,
                                                                       const QString & orc_InvalidNamePlaceholder) :
   C_OscNodeDataPoolListElementOptArrayId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_ElementIndex,
                                          oq_UseArrayElementIndex, ou32_ArrayElementIndex),
   me_Type(oe_Type),
   mq_IsValid(oq_IsValid),
   me_InvalidTypePlaceholder(oe_InvalidTypePlaceholder),
   mc_InvalidNamePlaceholder(orc_InvalidNamePlaceholder)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Less operator.

   \param[in]  orc_Cmp  Compared instance

   \return
   True  Current smaller than orc_Cmp
   False Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbNodeDataPoolListElementId::operator <(const C_OscNodeDataPoolId & orc_Cmp) const
{
   bool q_Retval;
   const C_PuiSvDbNodeDataPoolListElementId * const pc_NonBase =
      dynamic_cast<const C_PuiSvDbNodeDataPoolListElementId *>(&orc_Cmp);

   //Not current class, assume base comparison is correct
   if (pc_NonBase != NULL)
   {
      if ((this->mq_IsValid != pc_NonBase->mq_IsValid) && (this->mq_IsValid == true))
      {
         // Instance is 'bigger'. Definition valid elements are bigger
         q_Retval = false;
      }
      else if (this->mq_IsValid == pc_NonBase->mq_IsValid)
      {
         //Check type
         if (this->me_Type > pc_NonBase->GetType())
         {
            q_Retval = false;
         }
         else if (this->me_Type == pc_NonBase->GetType())
         {
            q_Retval = C_OscNodeDataPoolListElementOptArrayId::operator <(orc_Cmp);
         }
         else
         {
            q_Retval = true;
         }
      }
      else
      {
         q_Retval = true;
      }
   }
   else
   {
      q_Retval = C_OscNodeDataPoolListElementOptArrayId::operator <(orc_Cmp);
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Equal operator.

   \param[in]  orc_Cmp  Compared instance

   \return
   True  Current equal to orc_Cmp
   False Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbNodeDataPoolListElementId::operator ==(const C_OscNodeDataPoolId & orc_Cmp) const
{
   bool q_Retval = C_OscNodeDataPoolListElementOptArrayId::operator ==(orc_Cmp);

   if (q_Retval == true)
   {
      const C_PuiSvDbNodeDataPoolListElementId * const pc_NonBase =
         dynamic_cast<const C_PuiSvDbNodeDataPoolListElementId *>(&orc_Cmp);

      //Not current class, assume base comparison is correct
      if (pc_NonBase != NULL)
      {
         // In case of a false valid flag, we can not trust the indices
         if ((this->mq_IsValid == pc_NonBase->mq_IsValid) &&
             (this->mq_IsValid == true) &&
             (this->me_Type == pc_NonBase->GetType()))
         {
            q_Retval = true;
         }
         else
         {
            q_Retval = false;
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if both elements use the same data element ID as a base

   Array index is will not be checked in this function. Only base information is used for comparison.

   \param[in]  orc_OtherId    Compared instance

   \return
   True  Current base data element ID equal to orc_OtherId
   False Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbNodeDataPoolListElementId::CheckSameDataElement(const C_OscNodeDataPoolListElementId & orc_OtherId)
const
{
   return C_OscNodeDataPoolListElementId::operator ==(orc_OtherId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if both elements use the same data element ID as a base

   Array index is will not be checked in this function. Only base information is used for comparison.

   \param[in]  orc_OtherId    Compared instance

   \return
   True  Current base data element ID equal to orc_OtherId
   False Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbNodeDataPoolListElementId::CheckSameDataElement(const C_PuiSvDbNodeDataPoolListElementId & orc_OtherId)
const
{
   bool q_Retval;

   if ((this->GetIsValid() == false) || (orc_OtherId.GetIsValid() == false))
   {
      q_Retval = false;
   }
   else
   {
      if ((this->GetUseArrayElementIndex() == false) && (orc_OtherId.GetUseArrayElementIndex() == false))
      {
         //Same result as comparison
         q_Retval = *this == orc_OtherId;
      }
      else
      {
         if (this->me_Type == orc_OtherId.GetType())
         {
            //Same result as base comparison
            q_Retval = C_OscNodeDataPoolListElementId::operator ==(orc_OtherId);
         }
         else
         {
            q_Retval = false;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbNodeDataPoolListElementId::CalcHash(uint32_t & oru32_HashValue) const
{
   C_OscNodeDataPoolListElementOptArrayId::CalcHash(oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->me_Type, sizeof(this->me_Type), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->mq_IsValid, sizeof(this->mq_IsValid), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Mark element as invalid

   \param[in]  oe_InvalidTypePlaceholder  New invalid type placeholder
   \param[in]  orc_InvalidName            New invalid name placeholder
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbNodeDataPoolListElementId::MarkInvalid(const C_OscNodeDataPool::E_Type oe_InvalidTypePlaceholder,
                                                     const QString & orc_InvalidName)
{
   this->mq_IsValid = false;
   this->me_InvalidTypePlaceholder = oe_InvalidTypePlaceholder;
   this->mc_InvalidNamePlaceholder = orc_InvalidName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get valid flag

   \return
   True  Valid
   False Invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbNodeDataPoolListElementId::GetIsValid(void) const
{
   return this->mq_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name placeholder for invalid state

   \return
   Current name placeholder for invalid state
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDbNodeDataPoolListElementId::GetInvalidNamePlaceholder(void) const
{
   return this->mc_InvalidNamePlaceholder;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get invalid item placeholder tooltip

   \return
   Invalid item placeholder tooltip
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiSvDbNodeDataPoolListElementId::GetInvalidItemPlaceholderTooltip(void) const
{
   QString c_Retval;

   //Complete tooltip
   c_Retval = C_GtGetText::h_GetText("Data element was deleted in SYSTEM DEFINITION\n");
   //Additional reasons only
   if ((this->me_InvalidTypePlaceholder == C_OscNodeDataPool::eHALC) ||
       (this->me_InvalidTypePlaceholder == C_OscNodeDataPool::eHALC_NVM))
   {
      c_Retval += C_GtGetText::h_GetText(
         "\n"
         "Potential reason(s):\n"
         "- HAL was changed and corresponding datapool was generated and all variables were replaced\n"
         "- HAL was removed\n"
         "\n"
         "Next steps:\n"
         "Delete or replace this item by a valid one");
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type placeholder for invalid state

   \return
   Current type placeholder for invalid state
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPool::E_Type C_PuiSvDbNodeDataPoolListElementId::GetInvalidTypePlaceholder(void) const
{
   return this->me_InvalidTypePlaceholder;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type

   \return
   Current type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbNodeDataPoolListElementId::E_Type C_PuiSvDbNodeDataPoolListElementId::GetType() const
{
   return this->me_Type;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set type

   \param[in]  oe_Type  New type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbNodeDataPoolListElementId::SetType(const C_PuiSvDbNodeDataPoolListElementId::E_Type oe_Type)
{
   this->me_Type = oe_Type;
}
