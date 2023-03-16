//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data element tree item

   Data element tree item

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GtGetText.hpp"

#include "C_TblTreDataElementItem.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_TblTreDataElementItem::mhc_ADDITIONAL_64BIT_INFO = " (Not supported, 64 bit value)";
const QString C_TblTreDataElementItem::mhc_ADDITIONAL_ARRAY_INFO = " (Not supported, array or string type)";
const QString C_TblTreDataElementItem::mhc_ADDITIONAL_WRITE_ONLY_INFO = " (Not supported, read only)";
const QString C_TblTreDataElementItem::mhc_ADDITIONAL_ARRAY_INDEX_INFO = " (Not supported, array index)";
const QString C_TblTreDataElementItem::mhc_ADDITIONAL_ARRAY_STRING_INFO = " (Not supported, string type)";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in]  oq_IsArrayItem    Flag for if item is an array
   \param[in]  oq_IsArray        Is array
   \param[in]  orc_OriginalName  Original name
   \param[in]  oe_Type           Type
   \param[in]  oe_Access         Access
   \param[in]  oq_IsString       Is string
   \param[in]  orc_Id            Id
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreDataElementItem::C_TblTreDataElementItem(const bool oq_IsArrayItem, const bool oq_IsArray,
                                                 const QString & orc_OriginalName,
                                                 const C_OscNodeDataPoolContent::E_Type oe_Type,
                                                 const C_OscNodeDataPoolListElement::E_Access oe_Access,
                                                 const bool oq_IsString,
                                                 const C_PuiSvDbNodeDataPoolListElementId & orc_Id) :
   C_TblTreItem(),
   mq_IsArrayItem(oq_IsArrayItem),
   mq_IsArray(oq_IsArray),
   mc_OriginalName(orc_OriginalName),
   me_Type(oe_Type),
   me_Access(oe_Access),
   mq_IsString(oq_IsString),
   mc_Id(orc_Id)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Configure dynamic name

   \param[in]  oq_ShowOnlyWriteElements   Optional flag to show only writable elements
   \param[in]  oq_ShowArrayElements       Optional flag to hide all array elements (if false)
   \param[in]  oq_ShowArrayIndexElements  Optional flag to hide all array index elements (if false)
   \param[in]  oq_Show64BitValues         Optional flag to hide all 64 bit elements (if false)
   \param[in]  opc_AlreasyUsedElements    Optional pointer to vector with already used elements. All added elements
                                          will be marked as used an will be disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementItem::ConfigureDynamicName(const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                                   const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                                   const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements)
{
   // Set the name always here, in case of an earlier update, the name was adapted with the explanation
   this->c_Name = this->mc_OriginalName;
   if (((this->mq_IsArrayItem) && (oq_ShowArrayIndexElements == true)) ||
       ((this->mq_IsArrayItem == false) && ((this->mq_IsArray == false) || (oq_ShowArrayElements == true))))
   {
      if ((((this->me_Type != C_OscNodeDataPoolContent::eFLOAT64) &&
            (this->me_Type != C_OscNodeDataPoolContent::eUINT64)) &&
           (this->me_Type != C_OscNodeDataPoolContent::eSINT64)) ||
          (oq_Show64BitValues == true))
      {
         if ((this->me_Access == C_OscNodeDataPoolListElement::eACCESS_RW) ||
             (oq_ShowOnlyWriteElements == false))
         {
            //Valid
            this->q_Enabled = true;
            this->q_Selectable = true;
         }
         else
         {
            this->q_Enabled = false;
            this->q_Selectable = false;
            //Explanation
            this->c_Name += C_TblTreDataElementItem::mhc_ADDITIONAL_WRITE_ONLY_INFO;
         }
      }
      else
      {
         this->q_Enabled = false;
         this->q_Selectable = false;
         //Explanation
         this->c_Name += C_TblTreDataElementItem::mhc_ADDITIONAL_64BIT_INFO;
      }
   }
   else
   {
      if (this->mq_IsArrayItem)
      {
         this->q_Enabled = false;
         this->q_Selectable = false;
         //Explanation
         this->c_Name += C_TblTreDataElementItem::mhc_ADDITIONAL_ARRAY_INDEX_INFO;
      }
      else
      {
         if (this->mq_IsString)
         {
            this->q_Enabled = false;
            this->q_Selectable = false;
            //Explanation
            this->c_Name += C_TblTreDataElementItem::mhc_ADDITIONAL_ARRAY_STRING_INFO;
         }
         else
         {
            //If there are select-able child items don't disable the parent node
            if (oq_ShowArrayIndexElements)
            {
               this->q_Enabled = true;
            }
            else
            {
               this->q_Enabled = false;
            }
            this->q_Selectable = false;
            //If there are select-able child items don't add explanation
            if (oq_ShowArrayIndexElements == false)
            {
               //Explanation
               this->c_Name += C_TblTreDataElementItem::mhc_ADDITIONAL_ARRAY_INFO;
            }
         }
      }
   }

   if (opc_AlreasyUsedElements != NULL)
   {
      // Check for already used elements is active
      // Check if element is already used
      uint32_t u32_Counter;
      for (u32_Counter = 0; u32_Counter < opc_AlreasyUsedElements->size(); ++u32_Counter)
      {
         const C_PuiSvDbNodeDataPoolListElementId & rc_ExistingId = (*opc_AlreasyUsedElements)[u32_Counter];

         if (rc_ExistingId == this->mc_Id)
         {
            this->c_Name += C_GtGetText::h_GetText(" (Already used)");
            this->q_Selectable = false;
            this->q_Enabled = false;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get id

   \return
   Id
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbNodeDataPoolListElementId & C_TblTreDataElementItem::GetId(void) const
{
   return this->mc_Id;
}
