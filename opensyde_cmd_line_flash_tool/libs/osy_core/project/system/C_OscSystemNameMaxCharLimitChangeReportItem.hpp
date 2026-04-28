//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Simple data class for max char limit change report information
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSYSTEMNAMEMAXCHARLIMITCHANGEREPORTITEM_HPP
#define C_OSCSYSTEMNAMEMAXCHARLIMITCHANGEREPORTITEM_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <list>

#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSystemNameMaxCharLimitChangeReportItem
{
public:
   C_OscSystemNameMaxCharLimitChangeReportItem();
   C_OscSystemNameMaxCharLimitChangeReportItem(const stw::scl::C_SclString oc_Type,
                                               const stw::scl::C_SclString oc_OriginalName,
                                               const stw::scl::C_SclString oc_NewName);

   stw::scl::C_SclString c_Type;
   stw::scl::C_SclString c_OriginalName;
   stw::scl::C_SclString c_NewName;

   static void h_HandleNameMaxCharLimitItem(const uint32_t ou32_NameMaxCharLimit,
                                            const stw::scl::C_SclString & orc_Type, stw::scl::C_SclString & orc_Name,
                                            std::list<C_OscSystemNameMaxCharLimitChangeReportItem> * const opc_ChangedItems);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
