//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to store all indices to identify a data element in the system view (implementation)

   Utility class to store all indices to identify a data element in the system view

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_PuiSvDbNodeDataPoolListElementId.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

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
   C_OSCNodeDataPoolListElementId(),
   me_Type(eDATAPOOL_ELEMENT),
   mq_IsValid(true),
   me_InvalidTypePlaceholder(C_OSCNodeDataPool::eDIAG)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in] orc_Base                   Standard node data pool list element identification indices
   \param[in] oe_Type                    Enum to denote the source type
   \param[in] oq_IsValid                 Invalid flag
   \param[in] oe_InvalidTypePlaceholder  Type placeholder to be used in case of invalid state
   \param[in] orc_InvalidNamePlaceholder Name placeholder to be used in case of invalid state
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbNodeDataPoolListElementId::C_PuiSvDbNodeDataPoolListElementId(const C_OSCNodeDataPoolListElementId & orc_Base,
                                                                       const E_Type oe_Type, const bool oq_IsValid,
                                                                       const C_OSCNodeDataPool::E_Type oe_InvalidTypePlaceholder,
                                                                       const QString & orc_InvalidNamePlaceholder) :
   C_OSCNodeDataPoolListElementId(orc_Base.u32_NodeIndex, orc_Base.u32_DataPoolIndex, orc_Base.u32_ListIndex,
                                  orc_Base.u32_ElementIndex),
   me_Type(oe_Type),
   mq_IsValid(oq_IsValid),
   me_InvalidTypePlaceholder(oe_InvalidTypePlaceholder),
   mc_InvalidNamePlaceholder(orc_InvalidNamePlaceholder)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in] ou32_NodeIndex             Node index
   \param[in] ou32_DataPoolIndex         Data pool index
   \param[in] ou32_ListIndex             List index
   \param[in] ou32_ElementIndex          Element index
   \param[in] oe_Type                    Enum to denote the source type
   \param[in] oq_IsValid                 Invalid flag
   \param[in] oe_InvalidTypePlaceholder  Type placeholder to be used in case of invalid state
   \param[in] orc_InvalidNamePlaceholder Name placeholder to be used in case of invalid state
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbNodeDataPoolListElementId::C_PuiSvDbNodeDataPoolListElementId(const stw_types::uint32 ou32_NodeIndex,
                                                                       const stw_types::uint32 ou32_DataPoolIndex,
                                                                       const stw_types::uint32 ou32_ListIndex,
                                                                       const stw_types::uint32 ou32_ElementIndex,
                                                                       const E_Type oe_Type, const bool oq_IsValid,
                                                                       const C_OSCNodeDataPool::E_Type oe_InvalidTypePlaceholder,
                                                                       const QString & orc_InvalidNamePlaceholder) :
   C_OSCNodeDataPoolListElementId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, ou32_ElementIndex),
   me_Type(oe_Type),
   mq_IsValid(oq_IsValid),
   me_InvalidTypePlaceholder(oe_InvalidTypePlaceholder),
   mc_InvalidNamePlaceholder(orc_InvalidNamePlaceholder)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Less operator.

   \param[in] orc_Cmp Compared instance

   \return
   True  Current smaller than orc_Cmp
   False Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbNodeDataPoolListElementId::operator <(const C_OSCNodeDataPoolId & orc_Cmp) const
{
   bool q_Retval;
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
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
         if (this->me_Type > pc_NonBase->GetType())
         {
            q_Retval = false;
         }
         else if (this->me_Type == pc_NonBase->GetType())
         {
            q_Retval = C_OSCNodeDataPoolListElementId::operator <(orc_Cmp);
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
      q_Retval = C_OSCNodeDataPoolListElementId::operator <(orc_Cmp);
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Equal operator.

   \param[in] orc_Cmp Compared instance

   \return
   True  Current equal to orc_Cmp
   False Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbNodeDataPoolListElementId::operator ==(const C_OSCNodeDataPoolId & orc_Cmp) const
{
   bool q_Retval = C_OSCNodeDataPoolListElementId::operator ==(orc_Cmp);

   if (q_Retval == true)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
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
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbNodeDataPoolListElementId::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   C_OSCNodeDataPoolListElementId::CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Mark element as invalid

   \param[in] oe_InvalidTypePlaceholder New invalid type placeholder
   \param[in] orc_InvalidName           New invalid name placeholder
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbNodeDataPoolListElementId::MarkInvalid(const C_OSCNodeDataPool::E_Type oe_InvalidTypePlaceholder,
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
/*! \brief   Get type placeholder for invalid state

   \return
   Current type placeholder for invalid state
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPool::E_Type C_PuiSvDbNodeDataPoolListElementId::GetInvalidTypePlaceholder(void) const
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

   \param[in] oe_Type New type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbNodeDataPoolListElementId::SetType(const C_PuiSvDbNodeDataPoolListElementId::E_Type oe_Type)
{
   this->me_Type = oe_Type;
}
