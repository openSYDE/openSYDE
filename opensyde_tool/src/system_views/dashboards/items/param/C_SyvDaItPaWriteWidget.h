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

#include "stwtypes.h"

#include "C_OgePopUpDialog.h"
#include "C_SyvComDriverDiag.h"
#include "C_SyvDaItPaValuePairs.h"
#include "C_OSCNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDaItPaWriteWidget;
}

namespace stw_opensyde_gui
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

   explicit C_SyvDaItPaWriteWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                   stw_opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver,
                                   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ChangedElements,
                                   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_InvalidLists);
   virtual ~C_SyvDaItPaWriteWidget();

   void InitText(void);
   E_Step GetStep(void) const;
   stw_types::sint32 GetChangedElements(std::map<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                 stw_opensyde_gui_logic::C_SyvDaItPaValuePairs> & orc_AffectedElementValues)
   const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SyvDaItPaWriteWidget(const C_SyvDaItPaWriteWidget &);
   C_SyvDaItPaWriteWidget & operator =(const C_SyvDaItPaWriteWidget &);

   void m_StartWriteChangedElements(
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ChangedElements,
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_InvalidLists);
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

   bool m_WasElementChanged(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex,
                            const stw_types::uint32 ou32_ListIndex, const stw_types::uint32 ou32_ElementIndex,
                            const bool oq_CheckReadValues);
   QString m_GetSuspectElementReport(void) const;

   void m_Timer(void);
   void m_ReportError(const QString & orc_FunctionName, const QString & orc_ErrorText,
                      const stw_types::sint32 os32_ErrorCode);
   void m_ReportErrorNvmSafeWriteChangedValues(const stw_types::sint32 os32_ErrorCode);
   void m_ReportErrorNvmSafeReadValues(const stw_types::sint32 os32_ErrorCode);
   void m_ReportErrorNvmSafeWriteCrcs(const stw_types::sint32 os32_ErrorCode);
   void m_ReportErrorNvmNotifyOfChanges(const stw_types::sint32 os32_ErrorCode);
   QString m_ReadAndStoreUpdatedValues(void);
   static QString mh_GetId(const stw_opensyde_core::C_OSCNodeDataPoolListId & orc_Id);
   const stw_opensyde_core::C_OSCNodeDataPoolListElement * m_GetReadElementById(const stw_types::uint32 ou32_NodeIndex,
                                                                                const stw_types::uint32 ou32_DataPoolIndex, const stw_types::uint32 ou32_ListIndex, const stw_types::uint32 ou32_ElementIndex)
   const;

   Ui::C_SyvDaItPaWriteWidget * mpc_Ui;

   stw_opensyde_gui_elements::C_OgePopUpDialog * mpc_ParentDialog;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_logic::C_SyvComDriverDiag & mrc_ComDriver;
   std::map<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
            stw_opensyde_gui_logic::C_SyvDaItPaValuePairs> mc_AllAffectedValues;

   std::vector<stw_types::uint32> mc_AllNodeIndexes;
   std::vector<const stw_opensyde_core::C_OSCNode *> mc_ReadNodeValues;
   std::set<stw_opensyde_core::C_OSCNodeDataPoolListId> mc_RelevantLists;
   std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> > mc_InvalidListsForEachNode;
   std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> > mc_ChangedElementsForEachNode;

   std::set<stw_opensyde_core::C_OSCNodeDataPoolListId>::const_iterator mc_NotificationIterator;
   std::vector<bool> mc_NotificationResult;

   QTimer mc_Timer;
   E_Step me_Step;
   stw_types::uint32 mu32_CurrentNode;
   bool mq_RequestPending;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
