//-----------------------------------------------------------------------------
/*!
   \file
   \brief       System definition utility functions (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDUTIL_H
#define C_SDUTIL_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include <QString>
#include <QIcon>
#include <QLabel>
#include <QComboBox>
#include <QFileDialog>
#include "stwtypes.h"
#include "C_OSCCanMessage.h"
#include "C_OSCSystemBus.h"
#include "C_OSCNode.h"
#include "C_OSCNodeDataPoolListElementId.h"
#include "C_OSCCanMessageIdentificationIndices.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdUtil
{
public:
   C_SdUtil(void);
   static stw_types::sint32 h_GetNames(const std::vector<stw_types::uint32> & orc_NodeIndices,
                                       const std::vector<stw_types::uint32> & orc_InterfaceIndices,
                                       std::vector<QString> & orc_Names);
   static stw_types::sint32 h_GetName(const stw_types::uint32 & oru32_NodeIndex, QString & orc_Name);
   static stw_types::sint32 h_GetName(const stw_types::uint32 & oru32_NodeIndex,
                                      const stw_types::uint32 & oru32_InterfaceIndex, QString & orc_Name);
   static QIcon h_InitStaticIcon(const QString & orc_File);
   static QIcon h_InitStaticIconSvg(const QString & orc_File, const QSize & orc_Size);
   static QString h_ConvertTxMethodToName(const stw_opensyde_core::C_OSCCanMessage::E_TxMethodType & ore_Type);
   static QString h_ConvertByteOrderToName(const stw_opensyde_core::C_OSCCanSignal::E_ByteOrderType & ore_Type);
   static void h_GetErrorToolTipDataPools(const stw_types::uint32 ou32_NodeIndex,
                                          const std::vector<stw_types::uint32> & orc_Indices,
                                          const bool oq_NvmSizeInvalid, QString & orc_Heading, QString & orc_Content);

   static std::vector<stw_types::sint32> h_CreateAscendingIndexMap(
      const std::vector<stw_types::uint32> & orc_UnsortedIndices);

   static void h_SortSourceDescending(std::vector<stw_types::uint32> & orc_Source,
                                      std::vector<stw_types::uint32> & orc_Target);
   static bool h_CheckSortedDescending(const std::vector<stw_types::uint32> & orc_Indices);
   static bool h_CheckSortedAscending(const std::vector<stw_types::uint32> & orc_Indices);
   static void h_SortIndicesAscending(std::vector<stw_types::uint32> & orc_IndicesTmp);
   static stw_types::uint8 h_GetNodeIdMaximum(const stw_types::uint32 & oru32_NodeIndex);
   static stw_types::uint32 h_GetActiveNodeInterface(const QComboBox & orc_ComboBox,
                                                     const stw_types::uint32 & oru32_NodeIndex,
                                                     const stw_opensyde_core::C_OSCSystemBus::E_Type & ore_Type);
   static bool h_InitNodeInterfaceComboBox(const stw_opensyde_core::C_OSCNode & orc_Node,
                                           const stw_opensyde_core::C_OSCSystemBus::E_Type & ore_BusType,
                                           QComboBox * const opc_ComboBox,
                                           const stw_types::sint32 & ors32_SpecialInterface, bool & orq_ComProtocolL2,
                                           bool & orq_ComProtocolECeS, bool & orq_ComProtocolECoS);
   static std::vector<stw_types::uint32> h_GetUsedNodeIdsForBusUniqueAndSortedAscending(
      const stw_types::uint32 & oru32_BusIndex, const stw_types::uint32 & oru32_SpecialNodeIndex,
      const stw_types::sint32 & ors32_SpecialInterface);
   static std::vector<stw_types::uint32> h_GetUsedProcessIdsForApplicationUniqueAndSortedAscending(
      const stw_types::uint32 ou32_NodeIndex, const stw_types::sint32 os32_SpecialApplicationIndex);
   static std::vector<stw_types::uint32> h_GetUsedBusIdsUniqueAndSortedAscending(
      const stw_types::sint32 os32_SpecialBusIndex);
   static void h_AdaptMessageToProtocolType(stw_opensyde_core::C_OSCCanMessage & orc_Message,
                                            const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Type,
                                            QString * const opc_AdaptationInfos);
   static void h_InitUsedNodeIdsLabel(const std::vector<stw_types::uint32> & orc_UsedNodeIds,
                                      const QString & orc_BusName, QLabel * const opc_Label);
   static QString h_InitUsedIdsString(const std::vector<stw_types::uint32> & orc_UsedIds, const QString & orc_ItemName,
                                      const QString & orc_ItemType, const bool oq_SkipItem = false);
   static stw_types::uint32 h_GetNextFreeNodeId(
      const std::vector<stw_opensyde_core::C_OSCNodeComInterfaceSettings> & orc_Interfaces,
      const std::vector<stw_types::uint32> & orc_UsedNodeIds, const stw_types::sint32 & ors32_SpecialInterface);
   static bool h_CheckNodeInterfaceAvailable(
      const std::vector<stw_opensyde_core::C_OSCNodeComInterfaceSettings> & orc_ComInterfaces,
      const stw_opensyde_core::C_OSCSystemBus::E_Type & ore_BusType);
   static bool h_CheckDatapoolNumber(const stw_types::uint32 & oru32_NodeIndex,
                                     const stw_opensyde_core::C_OSCNodeDataPool::E_Type & ore_DataPoolType,
                                     const bool & orq_AlreadyChecked, QWidget * const opc_Parent);
   static stw_types::sint32 h_GetErrorToolTipNode(const stw_types::uint32 & oru32_NodeIndex, QString & orc_Text,
                                                  bool & orq_ErrorDetected);
   static void h_GetErrorToolTipBus(const stw_types::uint32 & oru32_BusIndex, QString & orc_Text);
   static QString h_GetToolTipContentDpList(const stw_types::uint32 & oru32_NodeIndex,
                                            const stw_types::uint32 & oru32_DatapoolIndex,
                                            const stw_types::uint32 & oru32_ListIndex);
   static QString h_GetToolTipContentDpListElement(
      const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_NodeDatapoolListElementId,
      const QString & orc_AdditionalInformation = "");
   static QString h_GetToolTipContentMessage(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   static QString h_GetToolTipContentSignal(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      const stw_types::uint32 & oru32_SignalIndex, const QString & orc_AdditionalInformation = "");
   static QString h_GetSaveFileName(QWidget * const opc_Parent, const QString & orc_Heading,
                                    const QString & orc_StartingFolder, const QString & orc_Filter,
                                    const QString & orc_DefaultFileName,
                                    const QFileDialog::Options oc_Option = QFileDialog::Options());
   template <typename T, typename U>
   static void h_SortIndicesDescendingAndSync(std::vector<stw_types::uint32> & orc_IndicesTmp,
                                              std::vector<T> & orc_OSCContentTmp, std::vector<U> & orc_UIContentTmp);

   template <typename T, typename U>
   static void h_SortIndicesAscendingAndSync(std::vector<stw_types::uint32> & orc_IndicesTmp,
                                             std::vector<T> & orc_OSCContentTmp, std::vector<U> & orc_UIContentTmp);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
