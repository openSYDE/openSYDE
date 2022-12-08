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

#include "C_SclString.hpp"
#include "C_OscNodeDataPoolContent.hpp"
#include "C_OscHalcDefContentBitmaskItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcDefContent :
   public C_OscNodeDataPoolContent
{
public:
   enum E_ComplexType
   {
      eCT_PLAIN,
      eCT_ENUM,
      eCT_BIT_MASK
   };

   C_OscHalcDefContent();

   void SetComplexType(const E_ComplexType oe_Type);
   E_ComplexType GetComplexType(void) const;
   int32_t AddEnumItem(const stw::scl::C_SclString & orc_DisplayName, const C_OscNodeDataPoolContent & orc_Value);
   int32_t SetEnumValue(const stw::scl::C_SclString & orc_DisplayName);
   int32_t GetEnumValue(stw::scl::C_SclString & orc_DisplayName);
   const C_OscNodeDataPoolContent * FindEnumItem(const stw::scl::C_SclString & orc_DisplayName) const;
   const std::vector<std::pair<stw::scl::C_SclString, C_OscNodeDataPoolContent> > & GetEnumItems(void) const;

   void AddBitmaskItem(const C_OscHalcDefContentBitmaskItem & orc_Value);
   const std::vector<C_OscHalcDefContentBitmaskItem> & GetBitmaskItems(void) const;
   void GetBitmaskStatusValues(std::vector<stw::scl::C_SclString> * const opc_Displays,
                               std::vector<bool> * const opc_Values) const;
   int32_t GetBitmask(const stw::scl::C_SclString & orc_DisplayName, bool & orq_Value) const;
   int32_t SetBitmask(const stw::scl::C_SclString & orc_DisplayName, const bool oq_Value);

   virtual void CalcHash(uint32_t & oru32_HashValue) const;
   virtual void CalcHashElement(uint32_t & oru32_HashValue, const uint32_t ou32_Index) const;
   virtual void CalcHashStructure(uint32_t & oru32_HashValue) const;

private:
   E_ComplexType me_ComplexType;
   std::vector<std::pair<stw::scl::C_SclString, C_OscNodeDataPoolContent> > mc_EnumItems; ///< All known enum values
   std::vector<C_OscHalcDefContentBitmaskItem> mc_BitmaskItems;                           ///< All known bitmask values
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
