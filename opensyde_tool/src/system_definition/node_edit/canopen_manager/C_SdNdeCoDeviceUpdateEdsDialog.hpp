//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up dialog widget for updating the EDS file for any existing device
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECODEVICEUPDATEEDSDIALOG_HPP
#define C_SDNDECODEVICEUPDATEEDSDIALOG_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QFileInfo>

#include "stwtypes.hpp"
#include "C_OscNode.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_OscCanInterfaceId.hpp"
#include "C_CieImportDataAssignment.hpp"
#include "C_OscCanOpenManagerDeviceInfo.hpp"
#include "C_CieImportedMessageVectorData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoDeviceUpdateEdsDialog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeCoDeviceUpdateEdsDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoDeviceUpdateEdsDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                           const uint32_t ou32_ManagerNodeIndex, const uint8_t ou8_ManagerInterfaceId,
                                           const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceNodeId,
                                           const QString & orc_FilePath,
                                           const stw::opensyde_core::C_OscCanOpenManagerDeviceInfo & orc_NewConfig,
                                           const C_CieImportDataAssignment & orc_NodeAssignment,
                                           const C_CieImportDataAssignment & orc_InvalidNodeAssignment);
   ~C_SdNdeCoDeviceUpdateEdsDialog(void) noexcept override;

   void DoApplyData(void) const;
   void InitStaticNames(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeCoDeviceUpdateEdsDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const uint32_t mu32_ManagerNodeIndex;
   const uint8_t mu8_ManagerInterfaceId;
   const stw::opensyde_core::C_OscCanInterfaceId mc_DeviceNodeId;
   const QString mc_FilePath;
   const stw::opensyde_core::C_OscCanOpenManagerDeviceInfo mc_NewConfig;
   C_CieImportDataAssignment mc_ImportedNodeAssignment;
   C_CieImportDataAssignment mc_ImportedInvalidNodeAssignment;
   C_CieImportDataAssignment mc_NewNodeAssignment;
   C_CieImportDataAssignment mc_AdaptedNodeAssignment;
   C_CieImportDataAssignment mc_DeletedNodeAssignment;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_SetAllDefaultData(C_CieImportDataAssignment & orc_NodeAssignment);
   void m_SetCommonDefaultData(C_CieImportDataAssignment & orc_NodeAssignment);
   void m_ShowReport(void);
   static void mh_AddToReport(const QString & orc_Heading, const QFileInfo & orc_FileInfo,
                              const C_CieImportDataAssignment & orc_Data, QString & orc_Report);
   void m_CompareData(void);
   int32_t m_PrepareCompareData(stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_ExistingRxMessages,
                                stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_ExistingTxMessages,
                                stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_ImportedRxMessages,
                                stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_ImportedTxMessages,
                                stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_NewRxMessages,
                                stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_NewTxMessages,
                                stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_AdaptedRxMessages,
                                stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_AdaptedTxMessages,
                                stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_DeletedRxMessages,
                                stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_DeletedTxMessages);
   void m_WriteComparedData(const stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_NewRxMessages,
                            const stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_NewTxMessages,
                            const stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_AdaptedRxMessages,
                            const stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_AdaptedTxMessages,
                            const stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_DeletedRxMessages,
                            const stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_DeletedTxMessages);
   void m_CompareDataMessageVector(const stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_ExistingMessages,
                                   const stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_ImportedMessages,
                                   stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_NewMessages,
                                   stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_AdaptedMessages,
                                   stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_DeletedMessages,
                                   const bool oq_MessageIsTxInEds);
   void m_InitNewAndAdaptedMessageVector(
      const stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_ExistingMessages,
      const stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_ImportedMessages,
      stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_NewMessages,
      stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_AdaptedMessages,
      const bool oq_MessageIsTxInEds);
   void m_InitDeletedMessageVector(const stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_ExistingMessages,
                                   const stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_ImportedMessages,
                                   stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_DeletedMessages);
   void m_HandleSignalChangesForAdaptedMessageVector(
      stw::opensyde_gui_logic::C_CieImportedMessageVectorData & orc_AdaptedMessages);
   static void mh_AddNewMessage(const stw::opensyde_core::C_OscCanMessage & orc_ImportedOscMessageData,
                                const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_ImportedOscSignalData, const stw::opensyde_gui_logic::C_PuiSdNodeCanMessage & orc_ImportedUiMessageData, const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_ImportedUiSignalData, const QString & orc_ImportedWarningMessages, std::vector<stw::opensyde_core::C_OscCanMessage> & orc_NewOscMessageData, std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_NewOscSignalData, std::vector<stw::opensyde_gui_logic::C_PuiSdNodeCanMessage> & orc_NewUiMessageData, std::vector<stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_NewUiSignalData, std::vector<QString> & orc_NewWarningMessagesPerMessage);
   static void mh_AddAdaptedMessage(const stw::opensyde_core::C_OscCanOpenManagerDeviceInfo & orc_NewConfig,
                                    const stw::opensyde_core::C_OscCanMessage & orc_ExistingOscMessage,
                                    const stw::opensyde_gui_logic::C_PuiSdNodeCanMessage & orc_ExistingUiMessage,
                                    const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_ExistingOscSignalData, const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_ExistingUiSignalData, const stw::opensyde_core::C_OscCanMessage & orc_ImportedOscMessageData, const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_ImportedOscSignalData, const stw::opensyde_gui_logic::C_PuiSdNodeCanMessage & orc_ImportedUiMessageData, const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_ImportedUiSignalData, const QString & orc_ImportedWarningMessages, std::vector<stw::opensyde_core::C_OscCanMessage> & orc_AdaptedOscMessageData, std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_AdaptedOscSignalData, std::vector<stw::opensyde_gui_logic::C_PuiSdNodeCanMessage> & orc_AdaptedUiMessageData, std::vector<stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_AdaptedUiSignalData, std::vector<QString> & orc_AdaptedWarningMessagesPerMessage, const bool oq_MessageIsTxInEds);
   static void mh_TransferMessageSignals(const stw::opensyde_core::C_OscCanMessage & orc_SourceOscMessage,
                                         const stw::opensyde_gui_logic::C_PuiSdNodeCanMessage & orc_SourceUiMessage,
                                         const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_SourceOscSignalData, const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_SourceUiSignalData, stw::opensyde_core::C_OscCanMessage & orc_TargetOscMessageData, stw::opensyde_gui_logic::C_PuiSdNodeCanMessage & orc_TargetUiMessageData, std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_TargetOscSignalData, std::vector<stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_TargetUiSignalData);
   stw::opensyde_core::C_OscCanOpenManagerDeviceInfo m_AdaptConfig(
      const stw::opensyde_core::C_OscCanOpenManagerDeviceInfo & orc_NewConfig) const;
   static void mh_AdaptMessageProperties(const stw::opensyde_core::C_OscCanOpenManagerDeviceInfo & orc_NewConfig,
                                         const stw::opensyde_core::C_OscCanMessage & orc_ImportedOscMessageData,
                                         stw::opensyde_core::C_OscCanMessage & orc_NewOscMessage,
                                         const bool oq_MessageIsTxInEds, QString & orc_WarningMessages);
   void m_AdaptSignalProperties(stw::opensyde_core::C_OscCanSignal & orc_Signal,
                                stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscSignalData,
                                stw::opensyde_gui_logic::C_PuiSdNodeDataPoolListElement & orc_UiSignalData,
                                const stw::opensyde_core::C_OscCanOpenManagerMappableSignal & orc_OrgSignal,
                                QString & orc_AdaptedMessageInfo);
   int32_t m_GetDataPoolIndex(void) const;
   static QString mh_BoolToString(const bool oq_Value);
   static QString mh_GetSignalNames(const stw::opensyde_core::C_OscCanMessage & orc_Message,
                                    const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_SignalData);
   static QString mh_GetSignalNameForDisplay(const stw::scl::C_SclString & orc_Name);
   const stw::opensyde_core::C_OscCanOpenManagerMappableSignal * m_GetOrgSignal(const uint16_t ou16_ObjectIndex,
                                                                                const uint8_t ou8_SubIndex)
   const;
   void m_InitMessageNames(C_CieImportDataAssignment & orc_Messages) const;
   static void mh_InitMessageVectorNames(std::vector<stw::opensyde_core::C_OscCanMessage> & orc_Messages,
                                         std::vector<uint8_t> & orc_MessageIsSrdo,
                                         const stw::opensyde_core::C_OscNode & orc_Node, const bool oq_MessageIsTxInEds,
                                         uint32_t & oru32_SrdoIndex);

   //Avoid call
   C_SdNdeCoDeviceUpdateEdsDialog(const C_SdNdeCoDeviceUpdateEdsDialog &);
   C_SdNdeCoDeviceUpdateEdsDialog & operator =(const C_SdNdeCoDeviceUpdateEdsDialog &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
