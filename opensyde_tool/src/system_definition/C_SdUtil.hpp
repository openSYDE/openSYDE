//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System definition utility functions (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDUTIL_HPP
#define C_SDUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <QString>
#include <QIcon>
#include <QLabel>
#include <QComboBox>
#include "stwtypes.hpp"
#include "C_OscCanMessage.hpp"
#include "C_OscSystemBus.hpp"
#include "C_OscNode.hpp"
#include "C_PuiSdNodeDataPoolListElement.hpp"
#include "C_OscNodeDataPoolListElementId.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_PuiSdNodeCanMessage.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdUtil
{
public:
   C_SdUtil(void);
   static int32_t h_GetNames(const std::vector<uint32_t> & orc_NodeIndices,
                             const std::vector<uint32_t> & orc_InterfaceIndices, std::vector<QString> & orc_Names,
                             const bool oq_NameWithInterfaceAlways,
                             const std::vector<uint32_t> * const opc_DatapoolIndices = NULL,
                             std::vector<QString> * const opc_DatapoolNames = NULL);
   static int32_t h_GetName(const uint32_t & oru32_NodeIndex, QString & orc_Name);
   static int32_t h_GetName(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_InterfaceIndex,
                            QString & orc_Name);
   static QIcon h_InitStaticIcon(const QString & orc_File);
   static QIcon h_InitStaticIconSvg(const QString & orc_File, const QSize & orc_Size);
   static QString h_ConvertTxMethodToName(const stw::opensyde_core::C_OscCanMessage::E_TxMethodType & ore_Type);
   static QString h_ConvertByteOrderToName(const stw::opensyde_core::C_OscCanSignal::E_ByteOrderType & ore_Type);
   static void h_GetErrorToolTipDataPools(const uint32_t ou32_NodeIndex, const std::vector<uint32_t> & orc_Indices,
                                          const bool oq_NvmSizeInvalid, const bool oq_NvmOverlapDetected,
                                          QString & orc_Heading, QString & orc_Content);

   static void h_SortSourceDescending(std::vector<uint32_t> & orc_Source, std::vector<uint32_t> & orc_Target);
   static bool h_CheckSortedDescending(const std::vector<uint32_t> & orc_Indices);
   static void h_SortIndicesAscending(std::vector<uint32_t> & orc_IndicesTmp);
   static uint8_t h_GetNodeIdMaximum(const uint32_t & oru32_NodeIndex);
   static uint32_t h_GetActiveNodeInterface(const QComboBox & orc_ComboBox, const uint32_t & oru32_NodeIndex,
                                            const stw::opensyde_core::C_OscSystemBus::E_Type & ore_Type);
   static bool h_InitNodeInterfaceComboBox(const stw::opensyde_core::C_OscNode & orc_Node,
                                           const stw::opensyde_core::C_OscSystemBus::E_Type & ore_BusType,
                                           QComboBox * const opc_ComboBox, const int32_t & ors32_SpecialInterface);
   static std::vector<uint32_t> h_GetUsedNodeIdsForBusUniqueAndSortedAscending(const uint32_t & oru32_BusIndex,
                                                                               const uint32_t & oru32_SpecialNodeIndex,
                                                                               const int32_t & ors32_SpecialInterface);
   static std::vector<uint32_t> h_GetUsedProcessIdsForApplicationUniqueAndSortedAscending(const uint32_t ou32_NodeIndex,
                                                                                          const int32_t os32_SpecialApplicationIndex);
   static std::vector<uint32_t> h_GetUsedBusIdsUniqueAndSortedAscending(const int32_t os32_SpecialBusIndex);
   static std::vector<uint32_t> h_GetUsedIpAddressesForBusUniqueAndSortedAscending(const uint32_t & oru32_BusIndex,
                                                                                   const uint32_t & oru32_SpecialNodeIndex, const int32_t & ors32_SpecialInterface);
   static std::vector<std::vector<uint8_t> > h_GetAllUsedIpAddressesForBus(const uint32_t & oru32_BusIndex,
                                                                           const uint32_t & oru32_SpecialNodeIndex,
                                                                           const int32_t & ors32_SpecialInterface);
   static void h_AdaptMessageToProtocolType(stw::opensyde_core::C_OscCanMessage & orc_Message,
                                            stw::opensyde_gui_logic::C_PuiSdNodeCanMessage * const opc_UiMessage,
                                            std::vector<opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalListElements, const std::vector<opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_UiSignalListElements, const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Type, QStringList * const opc_AdaptationInfos, const bool oq_IncludeSignalUpdate = true);
   static void h_AdaptSignalToProtocolType(stw::opensyde_core::C_OscCanSignal & orc_Signal,
                                           opensyde_core::C_OscNodeDataPoolListElement & orc_OscSignalListElement,
                                           const stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement & orc_UiSignalListElement, const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Type, QStringList * const opc_AdaptationInfos);
   static QString h_InitUsedIdsString(const std::vector<uint32_t> & orc_UsedIds, const QString & orc_ItemName,
                                      const QString & orc_ItemType, const bool oq_SkipItem = false);
   static QString h_InitUsedIpsString(const std::vector<std::vector<uint8_t> > & orc_UsedIps,
                                      const QString & orc_ItemName, const QString & orc_ItemType,
                                      const bool oq_SkiptItem = false);
   static QString h_IpAddressAsString(const std::vector<uint8_t> & orc_Ip);
   static uint32_t h_GetNextFreeNodeProperty(
      const std::vector<stw::opensyde_core::C_OscNodeComInterfaceSettings> & orc_Interfaces,
      const std::vector<uint32_t> & orc_UsedNodeProperties, const int32_t & ors32_SpecialInterface,
      const bool oq_GenerateId);
   static bool h_HasConnectionType(const stw::opensyde_core::C_OscNode & orc_Node,
                                   const stw::opensyde_core::C_OscSystemBus::E_Type oe_Type);
   static bool h_CheckNodeInterfaceAvailable(
      const std::vector<stw::opensyde_core::C_OscNodeComInterfaceSettings> & orc_ComInterfaces,
      const stw::opensyde_core::C_OscSystemBus::E_Type & ore_BusType);
   static bool h_CheckNodeInterfaceConnected(
      const std::vector<stw::opensyde_core::C_OscNodeComInterfaceSettings> & orc_ComInterfaces,
      const stw::opensyde_core::C_OscSystemBus::E_Type & ore_BusType);
   static bool h_CheckDatapoolNumber(const uint32_t & oru32_NodeIndex, const bool & orq_AlreadyChecked,
                                     QWidget * const opc_Parent);
   static int32_t h_GetErrorToolTipNode(const uint32_t & oru32_NodeIndex, QString & orc_Text, bool & orq_ErrorDetected,
                                        const bool oq_CheckAll);
   static void h_GetErrorToolTipBus(const uint32_t & oru32_BusIndex, QString & orc_Text);
   static QString h_GetToolTipContentDpList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DatapoolIndex,
                                            const uint32_t & oru32_ListIndex);
   static QString h_GetToolTipContentDpListElement(
      const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_NodeDatapoolListElementId,
      const QString & orc_AdditionalInformation = "");
   static QString h_GetToolTipContentMessage(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   static QString h_GetToolTipContentSignal(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
      const uint32_t & oru32_SignalIndex, const QString & orc_AdditionalInformation = "");
   template <typename T, typename U>
   static void h_SortIndicesDescendingAndSync(std::vector<uint32_t> & orc_IndicesTmp,
                                              std::vector<T> & orc_OscContentTmp, std::vector<U> & orc_UiContentTmp);

   template <typename T, typename U>
   static void h_SortIndicesAscendingAndSync(std::vector<uint32_t> & orc_IndicesTmp, std::vector<T> & orc_OscContentTmp,
                                             std::vector<U> & orc_UiContentTmp);
   static QString h_GetEdsFileDetails(const stw::opensyde_core::C_OscCanOpenObjectDictionary oc_CanOpenObjDictionary);

   static int32_t h_GetMessageCountOfNode(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                          const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol,
                                          const stw::opensyde_core::C_OscCanInterfaceId * const opc_CoDeviceInterfaceId,
                                          uint32_t & oru32_RxMessageCount, uint32_t & oru32_TxMessageCount,
                                          uint32_t * const opu32_SignalCount);
   static QString h_GetCanOpenSignalObjectIndex(const uint32_t ou32_ObjectIndex, const uint32_t ou32_ObjectSubIndex);
   static void h_NodeIdToBeChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                   QWidget * const opc_Parent);

private:
   static void mh_WriteEtherCanLogMessage(const opensyde_core::C_OscNodeProperties * const opc_NodeProperties,
                                          const opensyde_core::C_OscDeviceDefinition * const opc_DevDef);
   static void mh_AdaptSignalToUnsignedType(opensyde_core::C_OscNodeDataPoolListElement & orc_SignalListElement,
                                            QStringList & orc_AdaptationInfos);
   static void mh_AdaptSignalMaxToUnsignedType(opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                               QStringList & orc_AdaptationInfos, const uint16_t ou16_BitLength,
                                               const QString & orc_SignalName);
   static void mh_AdaptDataElementToUnsignedType(opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                                 QStringList & orc_AdaptationInfos, const QString & orc_ContentType,
                                                 const QString & orc_SignalName);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
