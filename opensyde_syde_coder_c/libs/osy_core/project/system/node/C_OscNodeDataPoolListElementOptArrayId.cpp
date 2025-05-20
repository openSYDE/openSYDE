//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Store all indices to identify a data element stored in a node including an optional array

   Store all indices to identify a data element stored in a node including an optional array

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SclChecksums.hpp"
#include "C_OscNodeDataPoolListElementOptArrayId.hpp"

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
C_OscNodeDataPoolListElementOptArrayId::C_OscNodeDataPoolListElementOptArrayId() :
   C_OscNodeDataPoolListElementId(),
   mq_UseArrayElementIndex(false),
   mu32_ArrayElementIndex(0UL)
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
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolListElementOptArrayId::C_OscNodeDataPoolListElementOptArrayId(const uint32_t ou32_NodeIndex,
                                                                               const uint32_t ou32_DataPoolIndex,
                                                                               const uint32_t ou32_ListIndex,
                                                                               const uint32_t ou32_ElementIndex,
                                                                               const bool oq_UseArrayElementIndex,
                                                                               const uint32_t ou32_ArrayElementIndex) :
   C_OscNodeDataPoolListElementId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_ElementIndex),
   mq_UseArrayElementIndex(oq_UseArrayElementIndex),
   mu32_ArrayElementIndex(ou32_ArrayElementIndex)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]  orc_Base                   Standard node data pool list element identification indices
   \param[in]  oq_UseArrayElementIndex    Flag to use array element index
   \param[in]  ou32_ArrayElementIndex     Array element index
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolListElementOptArrayId::C_OscNodeDataPoolListElementOptArrayId(
   const C_OscNodeDataPoolListElementId & orc_Base, const bool oq_UseArrayElementIndex,
   const uint32_t ou32_ArrayElementIndex) :
   C_OscNodeDataPoolListElementId(orc_Base.u32_NodeIndex, orc_Base.u32_DataPoolIndex, orc_Base.u32_ListIndex,
                                  orc_Base.u32_ElementIndex),
   mq_UseArrayElementIndex(oq_UseArrayElementIndex),
   mu32_ArrayElementIndex(ou32_ArrayElementIndex)
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
bool C_OscNodeDataPoolListElementOptArrayId::operator <(const C_OscNodeDataPoolId & orc_Cmp) const
{
   bool q_Retval;
   const C_OscNodeDataPoolListElementOptArrayId * const pc_NonBase =
      dynamic_cast<const C_OscNodeDataPoolListElementOptArrayId *>(&orc_Cmp);

   //Not current class, assume base comparison is correct
   if (pc_NonBase != NULL)
   {
      if (this->GetUseArrayElementIndex() == false)
      {
         if (this->GetUseArrayElementIndex() == pc_NonBase->GetUseArrayElementIndex())
         {
            //Ignore array element index
            q_Retval = C_OscNodeDataPoolListElementId::operator <(orc_Cmp);
         }
         else
         {
            //Just define an order for inequality case
            q_Retval = false;
         }
      }
      else
      {
         if (this->GetUseArrayElementIndex() == pc_NonBase->GetUseArrayElementIndex())
         {
            //Check array element index
            if (this->GetArrayElementIndex() > pc_NonBase->GetArrayElementIndex())
            {
               q_Retval = false;
            }
            else if (this->GetArrayElementIndex() == pc_NonBase->GetArrayElementIndex())
            {
               if (this->GetHalChannelName() == pc_NonBase->GetHalChannelName())
               {
                  q_Retval = C_OscNodeDataPoolListElementId::operator <(orc_Cmp);
               }
               else
               {
                  q_Retval = this->GetHalChannelName() < pc_NonBase->GetHalChannelName();
               }
            }
            else
            {
               q_Retval = true;
            }
         }
         else
         {
            //Just define an order for inequality case
            q_Retval = true;
         }
      }
   }
   else
   {
      q_Retval = C_OscNodeDataPoolListElementId::operator <(orc_Cmp);
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
bool C_OscNodeDataPoolListElementOptArrayId::operator ==(const C_OscNodeDataPoolId & orc_Cmp) const
{
   bool q_Retval = C_OscNodeDataPoolListElementId::operator ==(orc_Cmp);

   if (q_Retval == true)
   {
      const C_OscNodeDataPoolListElementOptArrayId * const pc_NonBase =
         dynamic_cast<const C_OscNodeDataPoolListElementOptArrayId *>(&orc_Cmp);

      //Not current class, assume base comparison is correct
      if (pc_NonBase != NULL)
      {
         q_Retval = false;

         if (this->GetUseArrayElementIndex() == pc_NonBase->GetUseArrayElementIndex())
         {
            //Only check array element index if relevant
            if (this->GetUseArrayElementIndex())
            {
               if (this->GetArrayElementIndex() == pc_NonBase->GetArrayElementIndex())
               {
                  if (this->GetHalChannelName() == pc_NonBase->GetHalChannelName())
                  {
                     q_Retval = true;
                  }
               }
            }
            else
            {
               q_Retval = true;
            }
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
void C_OscNodeDataPoolListElementOptArrayId::CalcHash(uint32_t & oru32_HashValue) const
{
   C_OscNodeDataPoolListElementId::CalcHash(oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->mq_UseArrayElementIndex, sizeof(this->mq_UseArrayElementIndex),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->mu32_ArrayElementIndex, sizeof(this->mu32_ArrayElementIndex),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->mc_HalChannelName.c_str(),
                                       this->mc_HalChannelName.Length(), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get HAL channel name

   \return
   Current HAL channel name
*/
//----------------------------------------------------------------------------------------------------------------------
stw::scl::C_SclString C_OscNodeDataPoolListElementOptArrayId::GetHalChannelName(void) const
{
   return this->mc_HalChannelName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set HAL channel name

   \param[in]  orc_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolListElementOptArrayId::SetHalChannelName(const stw::scl::C_SclString & orc_Value)
{
   this->mc_HalChannelName = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get array element index

   \return
   Current array element index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscNodeDataPoolListElementOptArrayId::GetArrayElementIndex() const
{
   return this->mu32_ArrayElementIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Combine array index valid check with return of array index, with default zero if index invalid

   \return
   Index to use
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscNodeDataPoolListElementOptArrayId::GetArrayElementIndexOrZero(void) const
{
   uint32_t u32_Retval;

   if (this->mq_UseArrayElementIndex)
   {
      u32_Retval = this->mu32_ArrayElementIndex;
   }
   else
   {
      u32_Retval = 0UL;
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get array element index usage flag

   \return
   Current array element index usage flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolListElementOptArrayId::GetUseArrayElementIndex(void) const
{
   return this->mq_UseArrayElementIndex;
}
