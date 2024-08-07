//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for navigation bar

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_NAGNAVIBARWIDGET_HPP
#define C_NAGNAVIBARWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QTimer>
#include <QVector>
#include <set>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QHBoxLayout>

#include "C_OgeGbxTransparent.hpp"
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_NagNaviBarWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagNaviBarWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagNaviBarWidget(QWidget * const opc_Parent = NULL);
   ~C_NagNaviBarWidget() override;

   void InitSysView(void);
   void UpdateNames(void) const;
   void UpdateViewIcons(const uint32_t ou32_ViewIndex) const;
   void UpdateAllViewsIcons(void) const;
   void UpdateAllScreenIcons(const bool oq_CheckAll) const;

   void SetMode(const int32_t os32_Mode, const int32_t os32_SubMode = 0, const uint32_t ou32_Index = 0U);
   void MarkModeForDataChanged(const bool oq_Changed, const bool oq_All, const int32_t os32_Mode,
                               const int32_t os32_SubMode = 0, const uint32_t ou32_Index = 0U) const;
   void ResetUseCaseAfterChangeFailure(const int32_t os32_Mode) const;

   void HandleElements(void);
   void InitText(void) const;
   void LoadUserSettings(void);
   void SaveUserSettings(void);
   void HandleServiceMode(void) const;
   void UpdateNodesAndBusesNames();

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigStartViewClicked(void);
   void SigAddViewClicked(void);
   void SigCheckSysViews(void);
   void SigDuplicateView(const uint32_t ou32_Index);
   void SigMoveView(const uint32_t ou32_SourceIndex, const uint32_t ou32_TargetIndex);
   void SigRenameView(const uint32_t ou32_Index, const QString & orc_Name, const bool oq_UpdateDisplayName);
   void SigDeleteView(const uint32_t ou32_Index, const int32_t os32_SelectedSubMode, const uint32_t ou32_SelectedIndex);
   void SigChangeUseCase(const int32_t os32_Mode, const int32_t os32_SubMode, const bool oq_ChangeUseCase = true);
   void SigSysDefRequest(const int32_t os32_Mode, const int32_t os32_SubMode);
   void SigChangeMode(const int32_t os32_Mode, const int32_t os32_SubMode = 0, const uint32_t ou32_Index = 0U,
                      const QString & orc_Name = "", const QString & orc_SubSubItemName = "",
                      const uint32_t ou32_Flag = 0U, const bool oq_ChangeUseCase = false);

protected:
   void hideEvent(QHideEvent * const opc_Event) override;
   void resizeEvent(QResizeEvent * const opc_Event) override;
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   //Avoid call
   C_NagNaviBarWidget(const C_NagNaviBarWidget &);
   C_NagNaviBarWidget & operator =(const C_NagNaviBarWidget &) &;

   Ui::C_NagNaviBarWidget * mpc_Ui;
   QTimer mc_TimerAnimation;
   bool mq_Loaded;
   bool mq_NodeSectionSizeLoaded;
   bool mq_Maximized;
   int32_t ms32_StepSize;

   // slots
   void m_StartViewClicked(void);
   void m_AnimationTimerEvent(void);

   void m_PbTopologyClick(void);
   void m_NodesChanged(void) const;
   void m_NodeChanged(const uint32_t ou32_NodeIndex) const;
   void m_BussesChanged(void) const;
   void m_BusChanged(const uint32_t ou32_BusIndex) const;

   void m_AddViewClicked(void);
   void m_OpenCanMonitor(void);
   void m_SysViewRenameClicked(const uint32_t ou32_ViewIndex, const QString & orc_Name);
   void m_SysViewDeleteClicked(const uint32_t ou32_ViewIndex);
   void m_SysViewDuplicateClicked(const uint32_t ou32_ViewIndex);
   void m_SysViewSizeChanged(void) const;

   void m_TabScClicked(void);
   void m_TabSdClicked(void);

   void m_UpdateNodeErrors(void) const;
   void m_UpdateBusErrors(void) const;

   void m_OnClickBus(const int32_t os32_Index);
   void m_OnClickNode(const int32_t os32_Index);
   void m_SelectView(const uint32_t ou32_ViewIndex, const int32_t os32_SubMode, const QString & orc_Name,
                     const QString & orc_SubSubItemName);
   void m_UpdateViewIcons(const bool oq_CheckOnlyThisView, const uint32_t ou32_ViewIndex) const;

   int32_t ms32_ActiveMode;
   int32_t ms32_ActiveSubMode;
   uint32_t mu32_ActiveIndex;

   // set to save information about changed elements
   std::set<uint32_t> mc_SetChangedNodes;
   std::set<uint32_t> mc_SetChangedBusses;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
