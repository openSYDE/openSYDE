//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Structure for imported data assignment
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEIMPORTDATAASSIGNMENT_HPP
#define C_CIEIMPORTDATAASSIGNMENT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_CieDataPoolListStructure.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieImportDataAssignment
{
public:
   C_CieImportDataAssignment();

   stw::opensyde_gui_logic::C_CieDataPoolListStructure c_ImportData;
   uint32_t u32_OsyNodeIndex;
   uint32_t u32_OsyInterfaceIndex;
   // message override indices:
   // pair of Datapool index (in node) and message index (in Message Containers Tx/Rx messages)
   // negative means no match found resp. nothing to override resp. add new
   std::vector< std::pair<int32_t, int32_t> > c_RxMessageOverrideIndices;
   std::vector< std::pair<int32_t, int32_t> > c_TxMessageOverrideIndices;
   int32_t s32_DatapoolIndexForNew;
   uint32_t u32_NewMessageCount;

   void CheckInternalConsistency(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
