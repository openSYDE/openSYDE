//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Content including enum
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFCONTENT_H
#define C_OSCHALCDEFCONTENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "CSCLString.h"
#include "C_OSCNodeDataPoolContent.h"
#include "C_OSCHalcDefContentBitmaskItem.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcDefContent :
   public C_OSCNodeDataPoolContent
{
public:
   enum E_ComplexType
   {
      eCT_PLAIN,
      eCT_ENUM,
      eCT_BIT_MASK
   };

   C_OSCHalcDefContent();

   void SetComplexType(const E_ComplexType oe_Type);
   E_ComplexType GetComplexType(void) const;
   stw_types::sint32 AddEnumItem(const stw_scl::C_SCLString & orc_DisplayName,
                                 const C_OSCNodeDataPoolContent & orc_Value);
   stw_types::sint32 SetEnumValue(const stw_scl::C_SCLString & orc_DisplayName);
   stw_types::sint32 GetEnumValue(stw_scl::C_SCLString & orc_DisplayName);
   const C_OSCNodeDataPoolContent * FindEnumItem(const stw_scl::C_SCLString & orc_DisplayName) const;
   const std::vector<std::pair<stw_scl::C_SCLString, C_OSCNodeDataPoolContent> > & GetEnumItems(void) const;

   void AddBitmaskItem(const C_OSCHalcDefContentBitmaskItem & orc_Value);
   const std::vector<C_OSCHalcDefContentBitmaskItem> & GetBitmaskItems(void) const;
   void GetBitmaskStatusValues(std::vector<stw_scl::C_SCLString> * const opc_Displays,
                               std::vector<bool> * const opc_Values) const;
   stw_types::sint32 GetBitmask(const stw_scl::C_SCLString & orc_DisplayName, bool & orq_Value) const;
   stw_types::sint32 SetBitmask(const stw_scl::C_SCLString & orc_DisplayName, const bool oq_Value);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   virtual void CalcHashElement(stw_types::uint32 & oru32_HashValue, const stw_types::uint32 ou32_Index) const;
   virtual void CalcHashStructure(stw_types::uint32 & oru32_HashValue) const;

private:
   E_ComplexType me_ComplexType;
   std::vector<std::pair<stw_scl::C_SCLString, C_OSCNodeDataPoolContent> > mc_EnumItems; ///< All known enum values
   std::vector<C_OSCHalcDefContentBitmaskItem> mc_BitmaskItems;                          ///< All known bitmask values
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
