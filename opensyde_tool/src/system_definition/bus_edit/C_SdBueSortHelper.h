//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for comparison operation of sorting algorithm (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUESORTHELPER_H
#define C_SDBUESORTHELPER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSdNodeCanMessage.h"
#include "C_PuiSdNodeDataPoolList.h"
#include "C_OSCCanMessageIdentificationIndices.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueSortHelper
{
public:
   C_SdBueSortHelper(void);
   bool operator ()(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_Message1,
                    const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_Message2) const;
   static bool h_CompareString(const stw_scl::C_SCLString & orc_String1, const stw_scl::C_SCLString & orc_String2);

   static stw_types::sint32 h_SortOneMessageVector(std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCMessages,
                                                   std::vector<C_PuiSdNodeCanMessage> & orc_UiMessages,
                                                   stw_opensyde_core::C_OSCNodeDataPoolList & orc_OSCList,
                                                   C_PuiSdNodeDataPoolList & orc_UiList);

private:
   static bool mh_CheckMessagesSorted(const std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCMessages);
   static stw_types::sint32 mh_SwapMessages(const stw_types::uint32 ou32_MessageIndex1,
                                            const stw_types::uint32 ou32_MessageIndex2,
                                            std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCMessages,
                                            std::vector<C_PuiSdNodeCanMessage> & orc_UiMessages,
                                            stw_opensyde_core::C_OSCNodeDataPoolList & orc_OSCList,
                                            C_PuiSdNodeDataPoolList & orc_UiList);
};

class C_SdBueSortHelperSignal
{
public:
   C_SdBueSortHelperSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_Message);
   bool operator ()(const stw_types::uint32 & oru32_Signal1, const stw_types::uint32 & oru32_Signal2) const;

private:
   const stw_opensyde_core::C_OSCCanMessageIdentificationIndices mc_Message;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
