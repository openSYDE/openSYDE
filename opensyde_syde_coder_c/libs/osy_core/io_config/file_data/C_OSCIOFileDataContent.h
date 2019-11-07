//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Content including enum
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOFILEDATACONTENT_H
#define C_OSCIOFILEDATACONTENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <map>

#include "CSCLString.h"
#include "C_OSCNodeDataPoolContent.h"
#include "C_OSCIOFileDataContentBitmaskItem.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOFileDataContent :
   public C_OSCNodeDataPoolContent
{
public:
   enum E_ComplexType
   {
      eCT_PLAIN,
      eCT_ENUM,
      eCT_BIT_MASK
   };

   C_OSCIOFileDataContent();

   void SetComplexType(const E_ComplexType oe_Type);
   E_ComplexType GetComplexType(void) const;
   stw_types::sint32 AddEnumItem(const stw_scl::C_SCLString & orc_DisplayName,
                                 const C_OSCNodeDataPoolContent & orc_Value);
   stw_types::sint32 SetEnumValue(const stw_scl::C_SCLString & orc_DisplayName);
   const std::map<stw_scl::C_SCLString, C_OSCNodeDataPoolContent> & GetEnumItems(void) const;

   void AddBitmaskItem(const C_OSCIOFileDataContentBitmaskItem & orc_Value);
   const std::vector<C_OSCIOFileDataContentBitmaskItem> & GetBitmaskItems(void) const;

private:
   E_ComplexType me_ComplexType;
   std::map<stw_scl::C_SCLString, C_OSCNodeDataPoolContent> mc_EnumItems; ///< All known enum values
   std::vector<C_OSCIOFileDataContentBitmaskItem> mc_BitmaskItems;        ///< All known bitmask values
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
