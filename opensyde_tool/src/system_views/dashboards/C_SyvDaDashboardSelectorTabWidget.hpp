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
#include "stwtypes.hpp"

#include <QTabWidget>
#include <QList>

#include "C_OgePubIconText.hpp"
#include "C_PuiSvDashboard.hpp"
#include "C_SyvDaDashboardWidget.hpp"
#include "C_SyvDaDashboardSelectorTabBar.hpp"
#include "C_SyvDaTearOffWidget.hpp"
#include "C_OgeWiDashboardTab.hpp"
#include "C_SyvComDriverDiag.hpp"
#include "C_PuiSvReadDataConfiguration.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardSelectorTabWidget :
   public QTabWidget
{
   Q_OBJECT

public:
   C_SyvDaDashboardSelectorTabWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SyvDaDashboardSelectorTabWidget(void);

   void TearOffWidget(const int32_t os32_Index, const QPoint & orc_Pos);

   void SetEditMode(const bool oq_Active);
   void SetEnabled(const bool oq_Enabled);
   void SetViewIndex(const uint32_t ou32_Value);

   void Save(void);
   void ApplyDarkMode(const bool oq_Active);

   void RegisterWidgets(stw::opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver) const;
   void ConnectionActiveChanged(const bool oq_Active);
   void UpdateShowValues(void) const;
   void UpdateTransmissionConfiguration(void);
   int32_t GetDashboardIndexForTabIndex(const int32_t os32_TabIndex, uint32_t & oru32_DashboardIndex) const;
   void HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigConfirmClicked(void);
   void SigCancelClicked(void);
   void SigChanged(void);
   void SigErrorChange(void);
   void SigNumberDashboardsChanged(void);
   void SigDataPoolWrite(const uint32_t ou32_NodeIndex, const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                         const uint16_t ou16_ElementIndex);
   void SigDataPoolRead(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Index);
   void SigNvmReadList(const stw::opensyde_core::C_OscNodeDataPoolListId & orc_Id);
   void SigEnabledDisabled(bool oq_Connected);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   void resizeEvent(QResizeEvent * const opc_Event) override;
   void hideEvent(QHideEvent * const opc_Event) override;
   void showEvent(QShowEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvDaDashboardSelectorTabWidget(const C_SyvDaDashboardSelectorTabWidget &);
   C_SyvDaDashboardSelectorTabWidget & operator =(const C_SyvDaDashboardSelectorTabWidget &) &;

   void m_WidgetComeBack(C_SyvDaTearOffWidget * const opc_Widget);
   void m_MakeScreenshot(const int32_t os32_Index);
   void m_InitTabStyle(const uint32_t ou32_Index, const QString & orc_Name, const QString & orc_Comment,
                       const bool oq_Active, const bool oq_Dark);
   void m_CurrentChanged(const int32_t os32_CurrentIndex) const;
   void m_OnAddClicked(void);
   void m_RepositionAddButton(void);
   void m_HandleChangeSelection(const int32_t os32_Index);
   int32_t m_MapDataIndexToTabIndex(const uint32_t ou32_DataIndex) const;
   void m_AddSpecificTab(const uint32_t ou32_DataIndex, const int32_t os32_TabIndex = -1,
                         C_SyvDaDashboardWidget * const opc_Widget = NULL);
   void m_SyncDashboardDeleteDataIndex(const uint32_t ou32_DataIndex);
   void m_OnNameChange(stw::opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source, const QString & orc_NewName);
   void m_OnActiveChange(const stw::opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source,
                         const bool oq_Active);
   void m_OnCopyAction(const stw::opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source) const;
   void m_OnCutAction(const stw::opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source);
   void m_OnPasteAction(void);
   void m_OnDeleteAction(const stw::opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source);
   void m_OnCloseAction(const stw::opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source);
   int32_t m_GetTabIndex(const stw::opensyde_gui_elements::C_OgeWiDashboardTab * const opc_DrawingWidget) const;
   void m_OnUndock(const stw::opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source);
   void m_OnEditProperties(stw::opensyde_gui_elements::C_OgeWiDashboardTab * const opc_Source);
   void m_TearOffWidget(const uint32_t ou32_DataIndex, const QPoint & orc_Pos,
                        C_SyvDaDashboardWidget * const opc_Widget = NULL, const QSize & orc_Size = QSize(1000, 800),
                        const bool oq_TornOffWindowMinimized = false, const bool oq_TornOffWindowMaximized = false,
                        const bool oq_Load = false);
   void m_AddTab(const stw::opensyde_gui_logic::C_PuiSvDashboard & orc_Data,
                 const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                            stw::opensyde_gui_logic::C_PuiSvReadDataConfiguration> * const opc_Rails);
   void m_DeleteTab(const int32_t os32_TabIndex);
   void m_CopyTab(const uint32_t ou32_DataIndex) const;
   bool m_EditTab(const uint32_t & oru32_DataIndex, QString & orc_Name, QString & orc_Comment);
   void m_StoreUserSettings(void);
   void m_Connect(const C_SyvDaDashboardWidget * const opc_Widget) const;
   void m_Disconnect(const C_SyvDaDashboardWidget * const opc_Widget) const;
   void m_OnTabChanged(const int32_t os32_Index) const;

   stw::opensyde_gui_elements::C_OgePubIconText * mpc_PushButton;
   C_SyvDaDashboardSelectorTabBar * mpc_TabBar;
   QList<C_SyvDaTearOffWidget *> mc_TearedOffWidgets;
   bool mq_EditMode;
   bool mq_Connected;
   uint32_t mu32_ViewIndex;
   static const QTabBar::ButtonPosition mhe_TAB_CONTENT_POSITION;
   static const QString mhc_ADD_ICON_DARK;
   static const QString mhc_ADD_ICON_LIGHT;
   stw::opensyde_gui_logic::C_PuiSvDashboard::E_TabType me_DashboardTabType;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
