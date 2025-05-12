//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class with functions for comm import export (header)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEUTIL_HPP
#define C_CIEUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"
#include "C_CieConverter.hpp"
#include "C_CieImportDataAssignment.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieUtil
{
public:
   C_CieUtil(void);

   static int32_t h_ImportFile(const uint32_t ou32_BusIndex,
                               const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType,
                               QWidget * const opc_Parent, std::vector<uint32_t> & orc_NodeIndexes,
                               std::vector<uint32_t> & orc_InterfaceIndexes);
   static int32_t h_ExportFile(const stw::opensyde_gui_logic::C_CieConverter::C_CieCommDefinition & orc_CommDef,
                               QWidget * const opc_Parent, const uint32_t ou32_NumOfNodes,
                               const uint32_t ou32_NumOfMessages, const uint32_t ou32_NumOfSignals);
   static void h_AdaptName(stw::scl::C_SclString & orc_Name, stw::scl::C_SclString & orc_Comment,
                           const bool oq_AlwaysAppendNameInComment);
   static void h_AdaptImportMessages(
      std::vector<stw::opensyde_gui::C_CieImportDataAssignment> & orc_ImportDataAssignment,
      const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType,
      const bool oq_AlwaysAppendNameInComment);
   static void h_InsertMessages(
      const std::vector<stw::opensyde_gui::C_CieImportDataAssignment> & orc_ImportDataAssignment,
      const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType, const bool oq_UniqueAddRequested);
   static int32_t h_GetDeviceInfo(const uint32_t ou32_DeviceNodeIndex, const uint32_t ou32_DeviceNodeInterfaceIndex,
                                  const uint32_t ou32_ManagerNodeIndex, const uint8_t ou8_ManagerNodeInterfaceNumber,
                                  const QString & orc_EdsPath,
                                  stw::opensyde_gui::C_CieImportDataAssignment & orc_NodeAssignment,
                                  stw::opensyde_gui::C_CieImportDataAssignment & orc_InvalidNodeAssignment,
                                  QString & orc_ParsingError);
   static void h_ReportEdsImportError(QWidget * const opc_ParentWidget, const int32_t os32_ImportResult,
                                      const QString & orc_ParsingError, const uint8_t ou8_NodeId);
   static QString h_GetMessageName(const stw::opensyde_core::C_OscNode & orc_Node, const bool oq_IsTx,
                                   const uint32_t ou32_MessageIndex,
                                   const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType,
                                   const stw::scl::C_SclString * const opc_NodeNameReplacement, const bool oq_IsSrdo,
                                   uint32_t & oru32_SrdoIndex);
   static bool h_GetIsSrdoFromMessageIsSrdoVector(const std::vector<uint8_t> & orc_MessageIsSrdo,
                                                  const uint32_t ou32_MessageIndex);

private:
   static int32_t mh_ImportDbcFile(const uint32_t ou32_BusIndex,
                                   const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType,
                                   const QString & orc_FullFilePath, QWidget * const opc_Parent,
                                   std::vector<uint32_t> & orc_NodeIndexes,
                                   std::vector<uint32_t> & orc_InterfaceIndexes);
   static int32_t mh_ImportDcfEdsFile(const uint32_t ou32_BusIndex,
                                      const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType,
                                      const QString & orc_FullFilePath, QWidget * const opc_Parent,
                                      std::vector<uint32_t> & orc_NodeIndexes,
                                      std::vector<uint32_t> & orc_InterfaceIndexes);
   static void mh_AdaptMessageNames(
      std::vector<stw::opensyde_gui::C_CieImportDataAssignment> & orc_ImportDataAssignment,
      const bool oq_AlwaysAppendNameInComment);
   static void mh_AdaptMessagesToProtocolType(
      std::vector<stw::opensyde_gui::C_CieImportDataAssignment> & orc_ImportDataAssignment,
      const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType);
   static int32_t mh_InsertMessages(const uint32_t ou32_NodeIndex,
                                    const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Type,
                                    const uint32_t ou32_InterfaceIndex, const uint32_t ou32_DatapoolIndexForNewContent,
                                    const bool oq_MessagesAreTx,
                                    const std::vector<stw::opensyde_core::C_OscCanMessage> & orc_OscMessageData, const
                                    std::vector<stw::opensyde_gui_logic::C_PuiSdNodeCanMessage> & orc_UiMessageData, const
                                    std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalData, const
                                    std::vector<stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_UiSignalData, const
                                    std::vector<std::pair<int32_t, int32_t> > & orc_MessageOverrideIndices, const bool oq_UniqueAddRequested);
   static QString mh_ConstructMessageName(const stw::scl::C_SclString & orc_Nodename,
                                          const stw::scl::C_SclString & orc_TpdoRpdo, const uint32_t ou32_MessageIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
