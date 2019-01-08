//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for system node data pool list import report (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.04.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CIEDATAPOOLCOMLISTIMPORTREPORTWIDGET_H
#define C_CIEDATAPOOLCOMLISTIMPORTREPORTWIDGET_H

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
class C_CieDataPoolComListImportReportWidget;
}
/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_CieDataPoolComListImportReportWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CieDataPoolComListImportReportWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                   const stw_types::uint32 ou32_NodeIndex,
                                                   const stw_types::uint32 ou32_DataPoolIndex,
                                                   const stw_types::uint32 ou32_InterfaceIndex,
                                                   const QString & orc_FilePath);
   ~C_CieDataPoolComListImportReportWidget(void);

   void InitStaticNames(void) const;
   stw_types::sint32 SetMessageData(const stw_opensyde_gui_logic::C_CieDataPoolListStructure & orc_Param);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_CieDataPoolComListImportReportWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const stw_types::uint32 mu32_NodeIndex;
   const stw_types::uint32 mu32_DataPoolIndex;
   const stw_types::uint32 mu32_InterfaceIndex;
   const QString mc_FilePath;
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_ProtocolType;
   std::vector<stw_opensyde_core::C_OSCCanMessage> mc_OSCRxMessageData;
   std::vector<stw_opensyde_gui_logic::C_PuiSdNodeCanMessage> mc_UiRxMessageData;
   std::vector<stw_opensyde_core::C_OSCCanMessage> mc_OSCTxMessageData;
   std::vector<stw_opensyde_gui_logic::C_PuiSdNodeCanMessage> mc_UiTxMessageData;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> mc_OSCRxSignalData;
   std::vector<stw_opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> mc_UiRxSignalData;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> mc_OSCTxSignalData;
   std::vector<stw_opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> mc_UiTxSignalData;
   std::vector<QString> mc_WarningMessagesPerRxMessage;
   std::vector<QString> mc_WarningMessagesPerTxMessage;
   QString mc_ImportNodeName;
   std::vector<stw_types::sint32> mc_RxMessageOverrideIndices;
   std::vector<stw_types::sint32> mc_TxMessageOverrideIndices;
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
   void m_InsertMessages(void) const;
   stw_types::sint32 m_InitProtocolType(void);
   static stw_types::sint32 mh_GetMessageEntries(stw_types::uint32 & oru32_NewEntryCount,
                                                 stw_types::uint32 & oru32_UpdatedEntryCount, QString & orc_TableNew,
                                                 QString & orc_TableUpdated,
                                                 const std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCMessageData, const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCAllSignalData, const std::vector<QString> & orc_InfoMessagesPerMessage, const std::vector<stw_types::sint32> & orc_MessageOverrideIndices, const bool oq_IsTx);
   static QString mh_GetMessageEntry(const stw_types::uint32 ou32_Index,
                                     const stw_opensyde_core::C_OSCCanMessage & orc_CurMessage,
                                     const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCAllSignalData, const QString & orc_InfoMessages, const bool oq_IsTx);
   static stw_types::sint32 mh_FindMatchingMessageIndex(const stw_opensyde_core::C_OSCCanMessage & orc_OSCMessageToFind,
                                                        const std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCMessages);
   static stw_types::sint32 mh_InsertMessages(const stw_types::uint32 ou32_NodeIndex,
                                              const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Type,
                                              const stw_types::uint32 ou32_InterfaceIndex, const bool oq_MessagesAreTx,
                                              const std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCMessageData, const std::vector<stw_opensyde_gui_logic::C_PuiSdNodeCanMessage> & orc_UiMessageData, const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalData, const std::vector<stw_opensyde_gui_logic::C_PuiSdNodeDataPoolListElement> & orc_UiSignalData, const std::vector<stw_types::sint32> & orc_MessageOverrideIndices);

   //Avoid call
   C_CieDataPoolComListImportReportWidget(const C_CieDataPoolComListImportReportWidget &);
   C_CieDataPoolComListImportReportWidget & operator =(const C_CieDataPoolComListImportReportWidget &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
