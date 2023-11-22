//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Adapter to convert extern data structures (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEDATAPOOLLISTADAPTER_HPP
#define C_CIEDATAPOOLLISTADAPTER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QString>

#include "stwtypes.hpp"
#include "C_CieDataPoolListStructure.hpp"
#include "C_OscCanMessage.hpp"
#include "C_CieConverter.hpp"
#include "C_PuiSdNodeDataPoolListElement.hpp"
#include "C_SclStringList.hpp"
#include "C_OscEdsDcfImportMessageGroup.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieDataPoolListAdapter
{
public:
   // getter for openSYDE data pool list structure

   static C_CieDataPoolListStructure h_GetStructureFromDbcFileImport(
      const stw::opensyde_gui_logic::C_CieConverter::C_CieNode & orc_CieNode);
   static C_CieDataPoolListStructure h_GetStructureFromDcfAndEdsFileImport(
      const stw::opensyde_core::C_OscEdsDcfImportMessageGroup & orc_OscRxMessageData,
      const stw::opensyde_core::C_OscEdsDcfImportMessageGroup & orc_OscTxMessageData,
      const std::vector<std::vector<stw::scl::C_SclString> > & orc_InfoMessagesPerMessage);
   static void h_AssignNode(const stw::opensyde_core::C_OscCanInterfaceId & orc_Id,
                            std::vector<stw::opensyde_core::C_OscCanMessage> & orc_OscMessageData);

   // to convert openSYDE data structures to DBC data structures
   static int32_t h_ConvertToDbcImportMessage(const uint32_t ou32_BusIndex,
                                              const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Type,
                                              const stw::opensyde_core::C_OscCanMessage & orc_OscCanMessage,
                                              C_CieConverter::C_CieNodeMessage & orc_CieNodeMessage,
                                              stw::scl::C_SclStringList & orc_Warnings);

private:
   // internal function called by adapters to fill Ui data structures
   static void mh_FillUpUiStructure(C_CieDataPoolListStructure & orc_DataPoolListStructure,
                                    const bool oq_ActivateAutoMinMaxForSignals,
                                    const std::vector<uint8_t> * const opc_RxSignalDefaultMinMaxValuesUsed = NULL,
                                    const std::vector<uint8_t> * const opc_TxSignalDefaultMinMaxValuesUsed = NULL);
   static void mh_FillUpUiStructureForSignals(const stw::opensyde_core::C_OscCanMessage & orc_Message,
                                              const bool oq_TxMessage,
                                              const C_PuiSdNodeDataPoolListElement & orc_DefaultUiSig,
                                              C_CieDataPoolListStructure & orc_DataPoolListStructure,
                                              const uint8_t * const opu8_SignalDefaultMinMaxValuesUsed);

   static void mh_FillUpCoreStructureByDbcValues(
      const std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> & orc_CieNodeMessages,
      std::vector<stw::opensyde_core::C_OscCanMessage> & orc_CanMessages,
      std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_CanSignalData,
      std::vector<QString> & orc_WarningMessages);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
