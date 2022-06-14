//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Adapter to convert extern data structures (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEDATAPOOLLISTADAPTER_H
#define C_CIEDATAPOOLLISTADAPTER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QString>

#include "stwtypes.h"
#include "C_CieDataPoolListStructure.h"
#include "C_OSCCanMessage.h"
#include "C_CieConverter.h"
#include "C_PuiSdNodeDataPoolListElement.h"
#include "CSCLStringList.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieDataPoolListAdapter
{
public:
   // getter for openSYDE data pool list structure

   static C_CieDataPoolListStructure h_GetStructureFromDBCFileImport(
      const stw_opensyde_gui_logic::C_CieConverter::C_CIENode & orc_CIENode);
   static C_CieDataPoolListStructure h_GetStructureFromDCFAndEDSFileImport(
      const std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCRxMessageData,
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCRxSignalData,
      const std::vector<stw_types::uint8> & orc_RxSignalDefaultMinMaxValuesUsed,
      const std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCTxMessageData,
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCTxSignalData,
      const std::vector<stw_types::uint8> & orc_TxSignalDefaultMinMaxValuesUsed,
      const std::vector<std::vector<stw_scl::C_SCLString> > & orc_InfoMessagesPerMessage);
   static void h_AssignNode(const stw_opensyde_core::C_OSCCanInterfaceId & orc_Id,
                            std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCMessageData);

   // to convert openSYDE data structures to DBC data structures
   static stw_types::sint32 h_ConvertToDBCImportMessage(const stw_types::uint32 ou32_BusIndex,
                                                        const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Type,
                                                        const stw_opensyde_core::C_OSCCanMessage & orc_OSCCanMessage,
                                                        C_CieConverter::C_CIENodeMessage & orc_CIENodeMessage,
                                                        stw_scl::C_SCLStringList & orc_Warnings);

private:
   // internal function called by adapters to fill Ui data structures
   static void mh_FillUpUiStructure(C_CieDataPoolListStructure & orc_DataPoolListStructure,
                                    const bool oq_ActivateAutoMinMaxForSignals,
                                    const std::vector<stw_types::uint8> * const opc_RxSignalDefaultMinMaxValuesUsed = NULL, const std::vector<stw_types::uint8> * const opc_TxSignalDefaultMinMaxValuesUsed = NULL);
   static void mh_FillUpUiStructureForSignals(const stw_opensyde_core::C_OSCCanMessage & orc_Message,
                                              const bool oq_TxMessage,
                                              const C_PuiSdNodeDataPoolListElement & orc_DefaultUiSig,
                                              C_CieDataPoolListStructure & orc_DataPoolListStructure,
                                              const stw_types::uint8 * const opu8_SignalDefaultMinMaxValuesUsed);

   static void mh_FillUpCoreStructureByDBCValues(
      const std::vector<stw_opensyde_gui_logic::C_CieConverter::C_CIENodeMessage> & orc_CIENodeMessages,
      std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_CanMessages,
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_CanSignalData,
      std::vector<QString> & orc_WarningMessages);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
