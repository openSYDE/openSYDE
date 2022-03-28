//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data element tree item
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLTREDATAELEMENTITEM_H
#define C_TBLTREDATAELEMENTITEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblTreItem.h"
#include "C_OSCNodeDataPoolListElement.h"
#include "C_PuiSvDbNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreDataElementItem :
   public C_TblTreItem
{
public:
   C_TblTreDataElementItem(const bool oq_IsArrayItem, const bool oq_IsArray, const QString & orc_OriginalName,
                           const stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type oe_Type,
                           const stw_opensyde_core::C_OSCNodeDataPoolListElement::E_Access oe_Access,
                           const bool oq_IsString, const C_PuiSvDbNodeDataPoolListElementId & orc_Id);

   void ConfigureDynamicName(const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                             const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                             const std::vector<C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);

   const C_PuiSvDbNodeDataPoolListElementId & GetId(void) const;

private:
   const bool mq_IsArrayItem;
   const bool mq_IsArray;
   const QString mc_OriginalName;
   const stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type me_Type;
   const stw_opensyde_core::C_OSCNodeDataPoolListElement::E_Access me_Access;
   const bool mq_IsString;
   const C_PuiSvDbNodeDataPoolListElementId mc_Id;

   static const QString mhc_ADDITIONAL_64BIT_INFO;
   static const QString mhc_ADDITIONAL_ARRAY_INFO;
   static const QString mhc_ADDITIONAL_WRITE_ONLY_INFO;
   static const QString mhc_ADDITIONAL_ARRAY_INDEX_INFO;
   static const QString mhc_ADDITIONAL_ARRAY_STRING_INFO;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
