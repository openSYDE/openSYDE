//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       IO description generic element part
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOFILEDATAELEMENT_H
#define C_OSCIOFILEDATAELEMENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLString.h"
#include "C_OSCIOFileDataContent.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOFileDataElement
{
public:
   C_OSCIOFileDataElement(void);
   virtual ~C_OSCIOFileDataElement(void);

   void SetType(const C_OSCNodeDataPoolContent::E_Type oe_Value);
   void SetArray(const bool oq_Value);
   void SetComplexType(const C_OSCIOFileDataContent::E_ComplexType oe_Type);
   stw_types::sint32 AddEnumItem(const stw_scl::C_SCLString & orc_DisplayName,
                                 const C_OSCNodeDataPoolContent & orc_Value);
   void AddBitmaskItem(const C_OSCIOFileDataContentBitmaskItem & orc_Value);

   C_OSCIOFileDataContent::E_ComplexType GetComplexType(void) const;
   C_OSCNodeDataPoolContent::E_Type GetType(void) const;
   bool GetArray(void) const;
   const std::map<stw_scl::C_SCLString, C_OSCNodeDataPoolContent> & GetEnumItems(void) const;
   const std::vector<C_OSCIOFileDataContentBitmaskItem> & GetBitmaskItems(void) const;

   stw_scl::C_SCLString c_Id;                              ///< Unique ID for later reference of this element
   stw_scl::C_SCLString c_Display;                         ///< Name to display to user
   stw_scl::C_SCLString c_Comment;                         ///< Description to display to user
   C_OSCIOFileDataContent c_InitialValue;                  ///< Initial value of this element
   C_OSCIOFileDataContent c_MinValue;                      ///< Minimum value of this element
   C_OSCIOFileDataContent c_MaxValue;                      ///< Maximum value of this element
   std::vector<stw_types::uint32> c_UseCaseAvailabilities; ///< Availability for this element (use-case index)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
