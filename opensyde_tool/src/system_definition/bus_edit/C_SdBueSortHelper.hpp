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
#include "C_PuiSdNodeCanMessage.hpp"
#include "C_PuiSdNodeDataPoolList.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueSortHelper
{
public:
   C_SdBueSortHelper(void);
   bool operator ()(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_Message1,
                    const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_Message2) const;
   static bool h_CompareString(const stw::scl::C_SclString & orc_String1, const stw::scl::C_SclString & orc_String2);

   static int32_t h_SortOneMessageVector(std::vector<stw::opensyde_core::C_OscCanMessage> & orc_OscMessages,
                                         std::vector<C_PuiSdNodeCanMessage> & orc_UiMessages,
                                         stw::opensyde_core::C_OscNodeDataPoolList & orc_OscList,
                                         C_PuiSdNodeDataPoolList & orc_UiList);

private:
   static bool mh_CheckMessagesSorted(const std::vector<stw::opensyde_core::C_OscCanMessage> & orc_OscMessages);
   static int32_t mh_SwapMessages(const uint32_t ou32_MessageIndex1, const uint32_t ou32_MessageIndex2,
                                  std::vector<stw::opensyde_core::C_OscCanMessage> & orc_OscMessages,
                                  std::vector<C_PuiSdNodeCanMessage> & orc_UiMessages,
                                  stw::opensyde_core::C_OscNodeDataPoolList & orc_OscList,
                                  C_PuiSdNodeDataPoolList & orc_UiList);
};

class C_SdBueSortHelperSignal
{
public:
   C_SdBueSortHelperSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_Message);
   bool operator ()(const uint32_t & oru32_Signal1, const uint32_t & oru32_Signal2) const;

private:
   const stw::opensyde_core::C_OscCanMessageIdentificationIndices mc_Message;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
