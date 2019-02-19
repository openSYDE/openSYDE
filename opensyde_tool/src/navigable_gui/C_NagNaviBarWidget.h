//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for navigation bar

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

#ifndef C_NAGNAVIBARWIDGET_H
#define C_NAGNAVIBARWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>
#include <QTimer>
#include <QVector>
#include <set>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QHBoxLayout>

#include "C_OgeGbxTransparent.h"
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_NagNaviBarWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_NagNaviBarWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagNaviBarWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_NagNaviBarWidget();

   void InitSysView(void);
   void UpdateNames(void) const;
   void UpdateViewIcons(const stw_types::uint32 ou32_ViewIndex) const;
   void UpdateAllViewsIcons(void) const;
   void UpdateAllScreenIcons(const bool oq_CheckAll) const;

   void SetMode(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode = 0,
                const stw_types::uint32 ou32_Index = 0U);
   void MarkModeForDataChanged(const bool oq_Changed, const bool oq_All, const stw_types::sint32 os32_Mode,
                               const stw_types::sint32 os32_SubMode = 0, const stw_types::uint32 ou32_Index = 0U) const;

   void HandleElements(void);
   void InitText(void) const;
   void LoadUserSettings(void);
   void SaveUserSettings(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigStartViewClicked(void);
   void SigAddViewClicked(void);
   void SigCheckSysViews(void);
   void SigDuplicateView(const stw_types::uint32 ou32_Index);
   void SigMoveView(const stw_types::uint32 ou32_SourceIndex, const stw_types::uint32 ou32_TargetIndex);
   void SigRenameView(const stw_types::uint32 ou32_Index, const QString & orc_Name, const bool oq_UpdateDisplayName);
   void SigDeleteView(const stw_types::uint32 ou32_Index, const stw_types::sint32 os32_SelectedSubMode,
                      const stw_types::uint32 ou32_SelectedIndex);
   void SigChangeUseCase(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode);
   void SigChangeMode(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode = 0,
                      const stw_types::uint32 ou32_Index = 0U, const QString & orc_Name = "",
                      const QString & orc_SubSubItemName = "", const stw_types::uint32 ou32_Flag = 0U);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void hideEvent(QHideEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_NagNaviBarWidget(const C_NagNaviBarWidget &);
   C_NagNaviBarWidget & operator =(const C_NagNaviBarWidget &);

   Ui::C_NagNaviBarWidget * mpc_Ui;
   QTimer mc_TimerAnimation;
   bool mq_Loaded;
   bool mq_Maximized;
   stw_types::sintn msn_StepSize;

   // slots
   void m_MinmaxClicked(void);
   void m_StartViewClicked(void);
   void m_AnimationTimerEvent(void);

   void m_PbTopologyClick(void);
   void m_NodesChanged(void) const;
   void m_BussesChanged(void) const;

   void m_AddViewClicked(void);
   void m_OpenCanMonitor(void);
   void m_SysViewRenameClicked(const stw_types::uint32 ou32_ViewIndex, const QString & orc_Name);
   void m_SysViewDeleteClicked(const stw_types::uint32 ou32_ViewIndex);
   void m_SysViewDuplicateClicked(const stw_types::uint32 ou32_ViewIndex);
   void m_SysViewSizeChanged(void) const;

   void m_TabScCLicked(void);
   void m_TabSdCLicked(void);

   void m_UpdateNodeErrors(void) const;
   void m_UpdateBusErrors(void) const;

   void m_OnClickBus(const stw_types::sintn osn_Index);
   void m_OnClickNode(const stw_types::sintn osn_Index);
   void m_SelectView(const stw_types::uint32 ou32_ViewIndex, const stw_types::sint32 os32_SubMode,
                     const QString & orc_Name, const QString & orc_SubSubItemName);
   void m_UpdateViewIcons(const bool oq_CheckOnlyThisView, const stw_types::uint32 ou32_ViewIndex) const;

   stw_types::sint32 ms32_ActiveMode;
   stw_types::sint32 ms32_ActiveSubMode;
   stw_types::uint32 mu32_ActiveIndex;

   // set to save information about changed elements
   std::set<stw_types::uint32> mc_SetChangedNodes;
   std::set<stw_types::uint32> mc_SetChangedBusses;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
