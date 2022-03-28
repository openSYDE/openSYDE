//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition generic element part
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFELEMENT_H
#define C_OSCHALCDEFELEMENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLString.h"
#include "C_OSCHalcDefContent.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcDefElement
{
public:
   C_OSCHalcDefElement(void);
   virtual ~C_OSCHalcDefElement(void);

   void SetType(const C_OSCNodeDataPoolContent::E_Type oe_Value);
   void SetArray(const bool oq_Value);
   void SetComplexType(const C_OSCHalcDefContent::E_ComplexType oe_Type);
   stw_types::sint32 AddEnumItem(const stw_scl::C_SCLString & orc_DisplayName,
                                 const C_OSCNodeDataPoolContent & orc_Value);
   void AddBitmaskItem(const C_OSCHalcDefContentBitmaskItem & orc_Value);

   C_OSCHalcDefContent::E_ComplexType GetComplexType(void) const;
   C_OSCNodeDataPoolContent::E_Type GetType(void) const;
   bool GetArray(void) const;
   const std::vector<std::pair<stw_scl::C_SCLString, C_OSCNodeDataPoolContent> > & GetEnumItems(void) const;
   const std::vector<C_OSCHalcDefContentBitmaskItem> & GetBitmaskItems(void) const;

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   stw_scl::C_SCLString c_Id;                              ///< Unique ID for later reference of this element
   stw_scl::C_SCLString c_Display;                         ///< Name to display to user
   stw_scl::C_SCLString c_Comment;                         ///< Description to display to user
   C_OSCHalcDefContent c_InitialValue;                     ///< Initial value of this element
   C_OSCHalcDefContent c_MinValue;                         ///< Minimum value of this element
   C_OSCHalcDefContent c_MaxValue;                         ///< Maximum value of this element
   std::vector<stw_types::uint32> c_UseCaseAvailabilities; ///< Availability for this element (use-case index)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
