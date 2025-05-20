//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Simple data class for max char limit change report information

   Simple data class for max char limit change report information

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscSystemNameMaxCharLimitChangeReportItem.hpp"

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
C_OscSystemNameMaxCharLimitChangeReportItem::C_OscSystemNameMaxCharLimitChangeReportItem()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init constructor

   \param[in]  oc_Type           Type
   \param[in]  oc_OriginalName   Original name
   \param[in]  oc_NewName        New name
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSystemNameMaxCharLimitChangeReportItem::C_OscSystemNameMaxCharLimitChangeReportItem(
   const stw::scl::C_SclString oc_Type, const stw::scl::C_SclString oc_OriginalName,
   const stw::scl::C_SclString oc_NewName) :
   c_Type(oc_Type),
   c_OriginalName(oc_OriginalName),
   c_NewName(oc_NewName)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle name max char limit item

   \param[in]      ou32_NameMaxCharLimit  Name max char limit
   \param[in,out]  orc_Type               Type
   \param[in,out]  orc_Name               Name
   \param[in,out]  opc_ChangedItems       Changed items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSystemNameMaxCharLimitChangeReportItem::h_HandleNameMaxCharLimitItem(const uint32_t ou32_NameMaxCharLimit,
                                                                               const stw::scl::C_SclString & orc_Type,
                                                                               stw::scl::C_SclString & orc_Name,
                                                                               std::list<C_OscSystemNameMaxCharLimitChangeReportItem> * const opc_ChangedItems)
{
   if (orc_Name.Length() > ou32_NameMaxCharLimit)
   {
      const stw::scl::C_SclString c_NewNameTmp = orc_Name.SubString(1UL, ou32_NameMaxCharLimit);
      if (opc_ChangedItems != NULL)
      {
         const C_OscSystemNameMaxCharLimitChangeReportItem c_Entry(orc_Type, orc_Name, c_NewNameTmp);
         opc_ChangedItems->push_back(c_Entry);
      }
      else
      {
         orc_Name = c_NewNameTmp;
      }
   }
}
