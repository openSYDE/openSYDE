//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       All indices to identify a data element stored in a node including an optional array and a valid flag

   All indices to identify a data element stored in a node including an optional array and a valid flag

   \copyright   Copyright 2026 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscNodeDataPoolListElementOptArrayOptValidId.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
C_OscNodeDataPoolListElementOptArrayOptValidId::C_OscNodeDataPoolListElementOptArrayOptValidId() :
   C_OscNodeDataPoolListElementOptArrayId(),
   mq_IsValid(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  orc_Base                   Standard node data pool list element identification indices
   \param[in]  oq_UseArrayElementIndex    Flag to use array element index
   \param[in]  ou32_ArrayElementIndex     Array element index
   \param[in]  oq_IsValid                 Invalid flag
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolListElementOptArrayOptValidId::C_OscNodeDataPoolListElementOptArrayOptValidId(
   const C_OscNodeDataPoolListElementId & orc_Base, const bool oq_UseArrayElementIndex,
   const uint32_t ou32_ArrayElementIndex, const bool oq_IsValid) :
   C_OscNodeDataPoolListElementOptArrayId(orc_Base, oq_UseArrayElementIndex, ou32_ArrayElementIndex),
   mq_IsValid(oq_IsValid)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  orc_Base    Standard node data pool list element identification indices
   \param[in]  oq_IsValid  Invalid flag
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolListElementOptArrayOptValidId::C_OscNodeDataPoolListElementOptArrayOptValidId(
   const C_OscNodeDataPoolListElementOptArrayId & orc_Base, const bool oq_IsValid) :
   C_OscNodeDataPoolListElementOptArrayId(orc_Base),
   mq_IsValid(oq_IsValid)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataPoolIndex         Data pool index
   \param[in]  ou32_ListIndex             List index
   \param[in]  ou32_ElementIndex          Element index
   \param[in]  oq_UseArrayElementIndex    Flag to use array element index
   \param[in]  ou32_ArrayElementIndex     Array element index
   \param[in]  oq_IsValid                 Invalid flag
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolListElementOptArrayOptValidId::C_OscNodeDataPoolListElementOptArrayOptValidId(
   const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex,
   const uint32_t ou32_ElementIndex, const bool oq_UseArrayElementIndex, const uint32_t ou32_ArrayElementIndex,
   const bool oq_IsValid) :
   C_OscNodeDataPoolListElementOptArrayId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_ElementIndex,
                                          oq_UseArrayElementIndex, ou32_ArrayElementIndex),
   mq_IsValid(oq_IsValid)
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
bool C_OscNodeDataPoolListElementOptArrayOptValidId::operator <(const C_OscNodeDataPoolId & orc_Cmp) const
{
   bool q_Retval;
   const C_OscNodeDataPoolListElementOptArrayOptValidId * const pc_NonBase =
      dynamic_cast<const C_OscNodeDataPoolListElementOptArrayOptValidId *>(&orc_Cmp);

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
         q_Retval = C_OscNodeDataPoolListElementOptArrayId::operator <(orc_Cmp);
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
bool C_OscNodeDataPoolListElementOptArrayOptValidId::operator ==(const C_OscNodeDataPoolId & orc_Cmp) const
{
   bool q_Retval = C_OscNodeDataPoolListElementOptArrayId::operator ==(orc_Cmp);

   if (q_Retval == true)
   {
      const C_OscNodeDataPoolListElementOptArrayOptValidId * const pc_NonBase =
         dynamic_cast<const C_OscNodeDataPoolListElementOptArrayOptValidId *>(&orc_Cmp);

      //Not current class, assume base comparison is correct
      if (pc_NonBase != NULL)
      {
         // In case of a false valid flag, we can not trust the indices
         if ((this->mq_IsValid == pc_NonBase->mq_IsValid) &&
             (this->mq_IsValid == true))
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
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolListElementOptArrayOptValidId::CalcHash(uint32_t & oru32_HashValue) const
{
   C_OscNodeDataPoolListElementOptArrayId::CalcHash(oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->mq_IsValid, sizeof(this->mq_IsValid), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Mark invalid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolListElementOptArrayOptValidId::MarkInvalid()
{
   this->mq_IsValid = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get is valid

   \return
   Flags

   \retval   True    Is valid
   \retval   False   Is not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolListElementOptArrayOptValidId::GetIsValid() const
{
   return mq_IsValid;
}
