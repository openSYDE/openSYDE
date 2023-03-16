//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the process of writing changed NVM elements to the server. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPAWRITEWIDGET_H
#define C_SYVDAITPAWRITEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QTimer>
#include <vector>
#include <set>

#include "stwtypes.hpp"

#include "C_OgePopUpDialog.hpp"
#include "C_SyvComDriverDiag.hpp"
#include "C_SyvDaItPaValuePairs.hpp"
#include "C_OscNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDaItPaWriteWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaItPaWriteWidget :
   public QWidget
{
   Q_OBJECT

public:
   enum E_Step
   {
      eBEFOREWRITE,
      eWRITE,
      eREADBACK,
      eCONFIRM,
      eWRITECRC,
      eSEND_NOTIFICATION,
      eFINISHED
   };

   explicit C_SyvDaItPaWriteWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                   stw::opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver,
                                   const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ChangedElements,
                                   const std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> & orc_InvalidLists);
   ~C_SyvDaItPaWriteWidget() override;

   void InitText(void);
   E_Step GetStep(void) const;
   int32_t GetChangedElements(std::map<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                       stw::opensyde_gui_logic::C_SyvDaItPaValuePairs> & orc_AffectedElementValues)
   const;

protected:
   void keyPressEvent(QKeyEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvDaItPaWriteWidget(const C_SyvDaItPaWriteWidget &);
   C_SyvDaItPaWriteWidget & operator =(const C_SyvDaItPaWriteWidget &) &;

   void m_StartWriteChangedElements(
      const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ChangedElements,
      const std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> & orc_InvalidLists);
   void m_PrepareForWrite(void);
   void m_WriteChangedElementsOfNode(void);
   void m_ReadBackElementsOfNode(void);
   void m_ShowParameterValues(const bool oq_ShowReadValues);
   void m_WriteCrcOfNode(void);
   bool m_SendNotification(void);
   void m_SendNextNotification(void);

   void m_OkClicked(void);
   void m_OnCancel(void);

   void m_ConfirmCheckBoxChanged(void) const;
   void m_ConfirmClicked(void);

   bool m_WasElementChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                            const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                            const bool oq_CheckReadValues);
   QString m_GetSuspectElementReport(void) const;

   void m_Timer(void);
   void m_ReportError(const QString & orc_FunctionName, const QString & orc_ErrorText, const int32_t os32_ErrorCode);
   void m_ReportErrorNvmSafeWriteChangedValues(const int32_t os32_ErrorCode);
   void m_ReportErrorNvmSafeReadValues(const int32_t os32_ErrorCode);
   void m_ReportErrorNvmSafeWriteCrcs(const int32_t os32_ErrorCode);
   void m_ReportErrorNvmNotifyOfChanges(const int32_t os32_ErrorCode);
   QString m_ReadAndStoreUpdatedValues(void);
   static QString mh_GetId(const stw::opensyde_core::C_OscNodeDataPoolListId & orc_Id);
   const stw::opensyde_core::C_OscNodeDataPoolListElement * m_GetReadElementById(const uint32_t ou32_NodeIndex,
                                                                                 const uint32_t ou32_DataPoolIndex,
                                                                                 const uint32_t ou32_ListIndex,
                                                                                 const uint32_t ou32_ElementIndex)
   const;

   Ui::C_SyvDaItPaWriteWidget * mpc_Ui;

   stw::opensyde_gui_elements::C_OgePopUpDialog * mpc_ParentDialog;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_logic::C_SyvComDriverDiag & mrc_ComDriver;
   std::map<stw::opensyde_core::C_OscNodeDataPoolListElementId,
            stw::opensyde_gui_logic::C_SyvDaItPaValuePairs> mc_AllAffectedValues;

   std::vector<uint32_t> mc_AllNodeIndexes;
   std::vector<const stw::opensyde_core::C_OscNode *> mc_ReadNodeValues;
   std::set<stw::opensyde_core::C_OscNodeDataPoolListId> mc_RelevantLists;
   std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> > mc_InvalidListsForEachNode;
   std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> > mc_ChangedElementsForEachNode;

   std::set<stw::opensyde_core::C_OscNodeDataPoolListId>::const_iterator mc_NotificationIterator;
   std::vector<bool> mc_NotificationResult;

   QTimer mc_Timer;
   E_Step me_Step;
   uint32_t mu32_CurrentNode;
   bool mq_RequestPending;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
