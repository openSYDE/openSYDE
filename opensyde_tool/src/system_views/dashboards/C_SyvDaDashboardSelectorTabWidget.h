//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for selecting and managing dashboards (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDSELECTORTABWIDGET_H
#define C_SYVDADASHBOARDSELECTORTABWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include <QTabWidget>
#include <QList>

#include "C_OgePubIconText.h"
#include "C_SyvDaDashboardWidget.h"
#include "C_SyvDaDashboardSelectorTabBar.h"
#include "C_SyvDaTearOffWidget.h"
#include "C_OgeWiDashboardTab.h"
#include "C_SyvComDriverDiag.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardSelectorTabWidget :
   public QTabWidget
{
   Q_OBJECT

public:
   C_SyvDaDashboardSelectorTabWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDaDashboardSelectorTabWidget(void);

   void TearOffWidget(const stw_types::sint32 os32_Index, const QPoint & orc_Pos);

   void SetEditMode(const bool oq_Active);
   void SetEnabled(const bool oq_Enabled);
   void SetViewIndex(const stw_types::uint32 ou32_Value);

   void Save(void);
   void ApplyDarkMode(const bool oq_Active);

   void RegisterWidgets(stw_opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver) const;
   void ConnectionActiveChanged(const bool oq_Active);
   void UpdateShowValues(void) const;
   void UpdateTransmissionConfiguration(void);
   stw_types::sint32 GetDashboardIndexForTabIndex(const stw_types::sint32 os32_TabIndex,
                                                  stw_types::uint32 & oru32_DashboardIndex) const;
   void HandleManualOperationFinished(const stw_types::sint32 os32_Result, const stw_types::uint8 ou8_NRC);
   void SetErrorForFailedCyclicElementIdRegistrations(
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_FailedIdRegisters,
      const std::vector<QString> & orc_FailedIdErrorDetails);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigConfirmClicked(void);
   void SigCancelClicked(void);
   void SigChanged(void);
   void SigErrorChange(void);
   void SigNumberDashboardsChanged(void);
   void SigDataPoolWrite(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_DataPoolIndex,
                         const stw_types::uint16 ou16_ListIndex, const stw_types::uint16 ou16_ElementIndex);
   void SigDataPoolRead(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Index);
   void SigNvmReadList(const stw_opensyde_core::C_OSCNodeDataPoolListId & orc_Id);
   void SigEnabledDisabled(bool oq_Connected);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void hideEvent(QHideEvent * const opc_Event) override;
   virtual void showEvent(QShowEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SyvDaDashboardSelectorTabWidget(const C_SyvDaDashboardSelectorTabWidget &);
   C_SyvDaDashboardSelectorTabWidget & operator =(const C_SyvDaDashboardSelectorTabWidget &);

   void m_WidgetComeBack(C_SyvDaTearOffWidget * const opc_Widget);
   void m_MakeScreenshot(const stw_types::sint32 s32_Index);
   void m_InitTabStyle(const stw_types::uint32 ou32_Index, const QString & orc_Name, const QString & orc_Comment,
                       const bool oq_Active, const bool oq_Dark);
   void m_CurrentChanged(const stw_types::sintn osn_CurrentIndex) const;
   void m_OnAddClicked(void);
   void m_RepositionAddButton(void);
   void m_HandleChangeSelection(const stw_types::sint32 os32_Index);
   stw_types::sint32 m_MapDataIndexToTabIndex(const stw_types::uint32 ou32_DataIndex) const;
   void m_AddSpecificTab(const stw_types::uint32 ou32_DataIndex, const stw_types::sint32 os32_TabIndex = -1,
                         C_SyvDaDashboardWidget * const opc_Widget = NULL);
   void m_SyncDashboardDeleteDataIndex(const stw_types::uint32 ou32_DataIndex);
   void m_OnNameChange(stw_opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source, const QString & orc_NewName);
   void m_OnActiveChange(const stw_opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source, const bool oq_Active);
   void m_OnCopyAction(const stw_opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source) const;
   void m_OnCutAction(const stw_opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source);
   void m_OnPasteAction(void);
   void m_OnDeleteAction(const stw_opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source);
   void m_OnCloseAction(const stw_opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source);
   stw_types::sint32 GetTabIndex(const stw_opensyde_gui_elements::C_OgeWiDashboardTab * const opc_DrawingWidget) const;
   void m_OnUndock(const stw_opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source);
   void m_OnEditProperties(stw_opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source);
   void m_TearOffWidget(const stw_types::uint32 ou32_DataIndex, const QPoint & orc_Pos,
                        C_SyvDaDashboardWidget * const opc_Widget = NULL, const QSize & orc_Size = QSize(1000, 800),
                        const bool oq_TornOffWindowMinimized = false, const bool oq_TornOffWindowMaximized = false,
                        const bool oq_Load = false);
   void m_AddTab(const stw_opensyde_gui_logic::C_PuiSvDashboard & orc_Data);
   void m_DeleteTab(const stw_types::sint32 os32_TabIndex);
   void m_CopyTab(const stw_types::uint32 ou32_DataIndex) const;
   bool m_EditTab(const stw_types::uint32 & oru32_DataIndex, QString & orc_Name, QString & orc_Comment);
   void m_StoreUserSettings(void);
   void m_Connect(C_SyvDaDashboardWidget * const opc_Widget);
   void m_Disconnect(C_SyvDaDashboardWidget * const opc_Widget);
   void m_OnTabChanged(const stw_types::sintn osn_Index) const;

   stw_opensyde_gui_elements::C_OgePubIconText * mpc_PushButton;
   C_SyvDaDashboardSelectorTabBar * mpc_TabBar;
   QList<C_SyvDaTearOffWidget *> mc_TearedOffWidgets;
   bool mq_EditMode;
   bool mq_Connected;
   stw_types::uint32 mu32_ViewIndex;
   static const QTabBar::ButtonPosition mhe_TabContentPosition;
   static const QString mhc_AddIconDark;
   static const QString mhc_AddIconLight;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
