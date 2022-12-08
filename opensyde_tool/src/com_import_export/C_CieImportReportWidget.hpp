//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for system node data pool list import report (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEIMPORTREPORTWIDGET_H
#define C_CIEIMPORTREPORTWIDGET_H

#include <QWidget>
#include "stwtypes.hpp"
#include "C_OscNode.hpp"
#include "C_OscCanMessage.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_PuiSdNodeCanMessage.hpp"
#include "C_CieImportDataAssignment.hpp"
#include "C_CieDataPoolListStructure.hpp"
#include "C_OscNodeDataPoolListElement.hpp"
#include "C_PuiSdNodeDataPoolListElement.hpp"

namespace Ui
{
class C_CieImportReportWidget;
}
/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieImportReportWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CieImportReportWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                    const QString & orc_FilePath, const uint32_t ou32_BusIndex,
                                    const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType,
                                    const std::vector<C_CieImportDataAssignment > & orc_ImportDataAssigned,
                                    const std::vector<C_CieImportDataAssignment> & orc_SkippedImportDataAssigned,
                                    const stw::scl::C_SclString * const opc_NodeNameReplacement,
                                    const bool oq_IsCanOpen = false);
   ~C_CieImportReportWidget(void) override;

   void InitStaticNames(void) const;

   static QString h_GetInitialReportPart(const std::vector<C_CieImportDataAssignment> & orc_ImportDataAssignment,
                                         const QString & orc_ImportedFilePath, const bool oq_IsCanOpenCase,
                                         const bool oq_AddMessageAdaptationInfo);
   static int32_t h_GetMessageTableContent(QString & orc_ImportTable, const QString & orc_Suffix,
                                           std::vector<C_CieImportDataAssignment> & orc_ImportDataAssignment,
                                           const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType,
                                           const stw::scl::C_SclString * const opc_NodeNameReplacement,
                                           const QString & orc_Heading, const bool oq_ReplaceMessageNames);

   static const QString hc_HTML_TABLE_HEADER_START;
   static const QString hc_HTML_TABLE_DATA_START;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_CieImportReportWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const QString mc_FilePath;
   const uint32_t mu32_BusIndex;
   const stw::opensyde_core::C_OscCanProtocol::E_Type me_ProtocolType;
   std::vector<C_CieImportDataAssignment> mc_ImportedAssignedData;
   std::vector<C_CieImportDataAssignment> mc_SkippedImportedAssignedData;
   const stw::scl::C_SclString * const mpc_NodeNameReplacement;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   int32_t m_ShowReport(const QString & orc_Suffix, const bool oq_IsCanOpen);
   int32_t m_CheckMessages(const std::vector<C_CieImportDataAssignment> & orc_ImportDataAssignment) const;
   int32_t m_CheckMessageMatches(const QString & orc_Suffix,
                                 std::vector<C_CieImportDataAssignment>  & orc_ImportDataAssignment);
   int32_t m_GetMessageOverrideInfo(stw::opensyde_core::C_OscCanMessage & orc_ImportMessageToFind, const bool oq_Tx,
                                    const uint32_t ou32_OsyNodeIndex, const uint32_t ou32_OsyInterfaceIndex,
                                    std::pair<int32_t,
                                              int32_t> & orc_MessageOverrideInfo, const uint32_t ou32_MessageIndex,
                                    const QString & orc_Suffix) const;
   static void mh_GetTableWithMessageEntries(int32_t & ors32_Retval, QString & orc_ImportTable,
                                             uint32_t & oru32_MessageCount, const QString & orc_Suffix,
                                             std::vector<C_CieImportDataAssignment> & orc_ImportDataAssignment,
                                             const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType,
                                             const stw::scl::C_SclString * const opc_NodeNameReplacement,
                                             const bool oq_ReplaceMessageNames);
   void m_InsertMessages(void) const;
   static int32_t mh_GetMessageEntries(uint32_t & oru32_EntryCount, uint32_t & oru32_NewEntryCount,
                                       QString & orc_TableEntries,
                                       std::vector<stw::opensyde_core::C_OscCanMessage> & orc_MessageList,
                                       const std::vector<stw::opensyde_core::C_OscCanMessage> & orc_OscMessageData,
                                       const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscAllSignalData, const
                                       std::vector<QString> & orc_InfoMessagesPerMessage, const
                                       std::vector< std::pair<int32_t, int32_t> > & orc_MessageOverrideIndices, const
                                       uint32_t ou32_NodeIndex, const bool oq_IsTx, const QString & orc_Suffix, const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType, const stw::scl::C_SclString * const opc_NodeNameReplacement, const bool oq_ReplaceMessageNames);
   static QString mh_GetMessageEntry(const uint32_t ou32_Index,
                                     const stw::opensyde_core::C_OscCanMessage & orc_CurMessage,
                                     const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscAllSignalData, const
                                     QString & orc_InfoMessages, const uint32_t ou32_NodeIndex, const bool oq_IsTx, const uint32_t ou32_MessageIndex, const QString & orc_Suffix, const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType, const stw::scl::C_SclString * const opc_NodeNameReplacement, const bool oq_ReplaceMessageNames);

   //Avoid call
   C_CieImportReportWidget(const C_CieImportReportWidget &);
   C_CieImportReportWidget & operator =(const C_CieImportReportWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
