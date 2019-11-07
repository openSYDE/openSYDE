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
#include "stwtypes.h"
#include "C_OSCCanMessage.h"
#include "C_OgePopUpDialog.h"
#include "C_OSCCanProtocol.h"
#include "C_PuiSdNodeCanMessage.h"
#include "C_CieDataPoolListStructure.h"
#include "C_OSCNodeDataPoolListElement.h"
#include "C_PuiSdNodeDataPoolListElement.h"

namespace Ui
{
class C_CieImportReportWidget;
}
/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_CieImportReportWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CieImportReportWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                    const stw_types::uint32 ou32_NodeIndex,
                                    const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_ProtocolType,
                                    const stw_types::sint32 os32_DataPoolIndex,
                                    const stw_types::uint32 ou32_InterfaceIndex, const QString & orc_FilePath);
   ~C_CieImportReportWidget(void);

   void InitStaticNames(void) const;
   stw_types::sint32 SetMessageData(const stw_opensyde_gui_logic::C_CieDataPoolListStructure & orc_Param);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_CieImportReportWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const stw_types::uint32 mu32_NodeIndex;
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_ProtocolType;
   const stw_types::sint32 ms32_DataPoolIndex;
   const stw_types::uint32 mu32_InterfaceIndex;
   const QString mc_FilePath;
   stw_types::uint32 mu32_NewMessageCount;
   stw_opensyde_gui_logic::C_CieDataPoolListStructure mc_ImportedData;
   // message override indices:
   // pair of Datapool index (in node) and message index (in Message Containers Tx/Rx messages)
   // negative means no match found resp. nothing to override resp. add new
   std::vector< std::pair<stw_types::sint32, stw_types::sint32> > mc_RxMessageOverrideIndices;
   std::vector< std::pair<stw_types::sint32, stw_types::sint32> > mc_TxMessageOverrideIndices;
   static const QString mhc_HTML_TABLE_HEADER_START;
   static const QString mhc_HTML_TABLE_DATA_START;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_FixSignalCount(void);
   void m_AdaptMessageNames(void);
   void m_AdaptMessagesToProtocolType(void);
   static void mh_CutSignalCount(std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCRxMessageData,
                                 std::vector<stw_opensyde_gui_logic::C_PuiSdNodeCanMessage> & orc_UiRxMessageData,
                                 std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCRxSignalData,
                                 std::vector<stw_opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_UiRxSignalData, std::vector<QString> & orc_WarningMessagesPerRxMessage);
   static bool mh_CheckAscendingSignalIndices(
      const std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCRxMessageData);
   static void mh_RenumberMessageSignalIndicesAscending(
      std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCRxMessageData);
   static void mh_AdaptName(stw_scl::C_SCLString & orc_Name, stw_scl::C_SCLString & orc_Comment);
   stw_types::sint32 m_ShowReport(void);
   stw_types::sint32 m_CheckMessages(void) const;
   stw_types::sint32 m_CheckMessageMatches(void);
   stw_types::sint32 m_GetMessageOverrideInfo(const stw_opensyde_core::C_OSCCanMessage & orc_DBCMessageToFind,
                                              const bool oq_Tx, std::pair<stw_types::sint32,
                                                                          stw_types::sint32> & orc_MessageOverrideInfo)
   const;
   void m_InsertMessages(const stw_types::uint32 ou32_DatpoolIndexForNewContent) const;
   static stw_types::sint32 mh_GetMessageEntries(stw_types::uint32 & oru32_NewEntryCount,
                                                 stw_types::uint32 & oru32_UpdatedEntryCount, QString & orc_TableNew,
                                                 QString & orc_TableUpdated,
                                                 const std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCMessageData, const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCAllSignalData, const std::vector<QString> & orc_InfoMessagesPerMessage, const std::vector< std::pair<stw_types::sint32, stw_types::sint32> > & orc_MessageOverrideIndices, const bool oq_IsTx);
   static QString mh_GetMessageEntry(const stw_types::uint32 ou32_Index,
                                     const stw_opensyde_core::C_OSCCanMessage & orc_CurMessage,
                                     const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCAllSignalData, const QString & orc_InfoMessages, const bool oq_IsTx);
   static stw_types::sint32 mh_InsertMessages(const stw_types::uint32 ou32_NodeIndex,
                                              const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Type,
                                              const stw_types::uint32 ou32_InterfaceIndex,
                                              const stw_types::uint32 ou32_DatapoolIndexForNewContent,
                                              const bool oq_MessagesAreTx,
                                              const std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCMessageData, const std::vector<stw_opensyde_gui_logic::C_PuiSdNodeCanMessage> & orc_UiMessageData, const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalData, const std::vector<stw_opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_UiSignalData, const std::vector<std::pair<stw_types::sint32, stw_types::sint32> > & orc_MessageOverrideIndices);

   //Avoid call
   C_CieImportReportWidget(const C_CieImportReportWidget &);
   C_CieImportReportWidget & operator =(const C_CieImportReportWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
