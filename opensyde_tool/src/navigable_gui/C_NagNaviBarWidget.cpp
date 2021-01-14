//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for navigation bar

   Navigation bar for switching use cases.
   This widget is designed in a ui file.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QProcess>

#include "TGLUtils.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSvHandler.h"
#include "C_NagNaviBarWidget.h"
#include "C_GtGetText.h"
#include "ui_C_NagNaviBarWidget.h"

#include "stwtypes.h"
#include "stwerrors.h"

#include "C_UsHandler.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSvData.h"
#include "C_Uti.h"
#include "C_OSCLoggingHandler.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
static const sintn msn_MAXIMIZED_WIDTH = 290;
static const sintn msn_MINIMIZED_WIDTH = 36; //36 = min size of minimize/maximize icon

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagNaviBarWidget::C_NagNaviBarWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_NagNaviBarWidget),
   mq_Loaded(false),
   mq_NodeSectionSizeLoaded(false),
   mq_Maximized(true),
   ms32_ActiveMode(ms32_MODE_SYSDEF),
   ms32_ActiveSubMode(ms32_SUBMODE_SYSDEF_TOPOLOGY),
   mu32_ActiveIndex(0U)
{
   QPixmap c_ImgLogo;

   mpc_Ui->setupUi(this);

   //Layout splitter
   this->mpc_Ui->pc_Splitter->setCollapsible(0, false);
   this->mpc_Ui->pc_Splitter->setCollapsible(1, false);
   // if there is more space stretch bottom widget (i.e. index 1)
   this->mpc_Ui->pc_Splitter->setStretchFactor(0, 0);
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 1);

   c_ImgLogo.load(":images/STW_Logo.png");
   c_ImgLogo = c_ImgLogo.scaled((c_ImgLogo.width() / 12), (c_ImgLogo.height() / 12),
                                Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
   this->mpc_Ui->pc_StwLogo->setPixmap(c_ImgLogo);

   //Remove debug text
   this->mpc_Ui->pc_GroupBoxSD->setTitle("");

   //Set list types
   this->mpc_Ui->pc_ListViewNodes->SetTypeNode(true);
   this->mpc_Ui->pc_ListViewBuses->SetTypeNode(false);

   // configure animation
   this->mc_TimerAnimation.setInterval(msn_TIMER_INTERVAL);
   this->msn_StepSize = (msn_MAXIMIZED_WIDTH - msn_MINIMIZED_WIDTH) / msn_ANIMATION_STEPS;

   //Remove focus from topology button (otherwise escape does not get forwarded for whatever reason!)
   this->mpc_Ui->pc_PushButtonTopology->setFocusPolicy(Qt::NoFocus);

   // create the dynamic buttons (all 'sub' buttons)

   //Set left border state (both included!)
   this->mpc_Ui->pc_WidgetTabSd->SetIncludeLeftBorder(true);
   this->mpc_Ui->pc_WidgetTabSc->SetIncludeLeftBorder(true);

   //Set SVGs
   this->mpc_Ui->pc_WidgetTabSd->SetIconSvg("://images/IconSystemDefinition.svg");
   this->mpc_Ui->pc_WidgetTabSc->SetIconSvg("://images/IconSystemCommissioning.svg");
   this->mpc_Ui->pc_PushButtonAddSysView->SetCustomIcon("://images/main_page_and_navi_bar/Icon_new_project.svg",
                                                        "://images/main_page_and_navi_bar/Icon_new_project.svg");
   this->mpc_Ui->pc_PushButtonCanMonitor->SetCustomIcon(":/images/CanMonitorIcon_Navibar.png",
                                                        ":/images/CanMonitorIcon_Navibar.png");

   //Customize buttons
   this->mpc_Ui->pc_PushButtonAddSysView->setIconSize(QSize(16, 16));
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_PushButtonAddSysView, "Border", true);
   this->mpc_Ui->pc_PushButtonCanMonitor->setIconSize(QSize(18, 18));
   this->mpc_Ui->pc_PushButtonCanMonitor->SetIconPaddingLeft(22); //special padding
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_PushButtonCanMonitor, "CanMonitorButton", true);

   //Handle scroll area styling issues
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ScrollArea, "C_OgeSaNaviBar", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ScrollAreaWidgetContents, "C_OgeSaNaviBar", true);

   // connect the buttons signals
   connect(this->mpc_Ui->pc_BtnMain, &QPushButton::clicked, this, &C_NagNaviBarWidget::m_StartViewClicked);
   connect(&this->mc_TimerAnimation, &QTimer::timeout, this, &C_NagNaviBarWidget::m_AnimationTimerEvent);

   // view action
   connect(this->mpc_Ui->pc_PushButtonAddSysView, &QPushButton::clicked, this, &C_NagNaviBarWidget::m_AddViewClicked);
   connect(this->mpc_Ui->pc_WidgetTabSc, &C_OgeWiNavigationTab::released, this, &C_NagNaviBarWidget::m_TabScCLicked);
   connect(this->mpc_Ui->pc_WidgetTabSd, &C_OgeWiNavigationTab::released, this, &C_NagNaviBarWidget::m_TabSdCLicked);

   // Tree for topology and its nodes and busses
   connect(this->mpc_Ui->pc_PushButtonTopology, &C_OgePubNavigation::clicked, this,
           &C_NagNaviBarWidget::m_PbTopologyClick);
   connect(this->mpc_Ui->pc_ListViewNodes, &C_NagTopList::SigClicked, this, &C_NagNaviBarWidget::m_OnClickNode);
   connect(this->mpc_Ui->pc_ListViewBuses, &C_NagTopList::SigClicked, this, &C_NagNaviBarWidget::m_OnClickBus);

   connect(this->mpc_Ui->pc_ListViewViews, &C_NagViewList::SigMoveView, this, &C_NagNaviBarWidget::SigMoveView);
   connect(this->mpc_Ui->pc_ListViewViews, &C_NagViewList::SigSelect, this, &C_NagNaviBarWidget::m_SelectView);
   connect(this->mpc_Ui->pc_ListViewViews, &C_NagViewList::SigSetName, this,
           &C_NagNaviBarWidget::m_SysViewRenameClicked);
   connect(this->mpc_Ui->pc_ListViewViews, &C_NagViewList::SigDuplicate, this,
           &C_NagNaviBarWidget::m_SysViewDuplicateClicked);
   connect(this->mpc_Ui->pc_ListViewViews, &C_NagViewList::SigDelete, this,
           &C_NagNaviBarWidget::m_SysViewDeleteClicked);
   connect(this->mpc_Ui->pc_ListViewViews, &C_NagViewList::SigSizeChange, this,
           &C_NagNaviBarWidget::m_SysViewSizeChanged);

   //connect to nodes/bus change signals
   connect(C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigNodesChanged, this,
           &C_NagNaviBarWidget::m_NodesChanged);
   connect(C_PuiSdHandler::h_GetInstance(), &C_PuiSdHandler::SigBussesChanged,
           this, &C_NagNaviBarWidget::m_BussesChanged);

   connect(this->mpc_Ui->pc_PushButtonCanMonitor, &C_OgePubProjAction::clicked, this,
           &C_NagNaviBarWidget::m_OpenCanMonitor);

   this->SetMode(ms32_MODE_NONE);
   this->InitText();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagNaviBarWidget::~C_NagNaviBarWidget()
{
   //Store splitter position
   SaveUserSettings();

   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent all elements and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize dynamic system view page
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::InitSysView(void)
{
   // system view
   this->mpc_Ui->pc_ListViewViews->Init();

   //Check AFTER item was added to global vector
   Q_EMIT this->SigCheckSysViews();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update all view names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::UpdateNames(void) const
{
   this->mpc_Ui->pc_ListViewViews->UpdateNames();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle view icons update

   \param[in] ou32_ViewIndex       Index to specify which view changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::UpdateViewIcons(const uint32 ou32_ViewIndex) const
{
   m_UpdateViewIcons(true, ou32_ViewIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle all views icon update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::UpdateAllViewsIcons(void) const
{
   m_UpdateViewIcons(false, 0UL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle all screens icons update

   \param[in] oq_CheckAll Flag if all icons should be checked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::UpdateAllScreenIcons(const bool oq_CheckAll) const
{
   if (oq_CheckAll == true)
   {
      m_UpdateNodeErrors();
      m_UpdateBusErrors();
   }
   this->mpc_Ui->pc_ListViewViews->UpdateDeco(false, 0UL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets mode of navigation bar

   The mode defines which use case is active and the submode defines which
   function of the use case is active.

   \param[in]  os32_Mode      Index of mode
   \param[in]  os32_SubMode   Index of submode
   \param[in]  ou32_Index     Optional Index number (for example for node or bus)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::SetMode(const sint32 os32_Mode, const sint32 os32_SubMode, const uint32 ou32_Index)
{
   //remember mode
   if (os32_Mode != ms32_MODE_NONE)
   {
      this->ms32_ActiveMode = os32_Mode;
      this->ms32_ActiveSubMode = os32_SubMode;
      this->mu32_ActiveIndex = ou32_Index;
   }

   //Set active state
   if (os32_Mode == ms32_MODE_SYSDEF)
   {
      this->mpc_Ui->pc_WidgetTabSd->SetActive(true);
      this->mpc_Ui->pc_WidgetTabSc->SetActive(false);
   }
   else
   {
      this->mpc_Ui->pc_WidgetTabSd->SetActive(false);
      this->mpc_Ui->pc_WidgetTabSc->SetActive(true);
   }

   // hide everything in the first step
   this->mpc_Ui->pc_ListViewViews->setVisible(false);
   this->mpc_Ui->pc_GroupBoxSD->setVisible(false);
   this->mpc_Ui->pc_GroupBoxAddViewButtonSection->setVisible(false);
   this->mpc_Ui->pc_GroupBoxOpenCanMonitorOutside->setVisible(false);

   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_PushButtonTopology, "Active", false);

   this->mpc_Ui->pc_ListViewNodes->SetSelectedIndexAndScroll(-1);
   this->mpc_Ui->pc_ListViewBuses->SetSelectedIndexAndScroll(-1);

   this->mpc_Ui->pc_GroupBoxSD->SetSpecialBackground(false);

   switch (os32_Mode)
   {
   case ms32_MODE_NONE:
      //do nothing
      break;

   case ms32_MODE_SYSDEF:
      this->mpc_Ui->pc_GroupBoxSD->SetSpecialBackground(true);
      switch (os32_SubMode)
      {
      case ms32_SUBMODE_SYSDEF_TOPOLOGY:
         C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_PushButtonTopology, "Active", true);
         break;
      case ms32_SUBMODE_SYSDEF_NODEEDIT:
         this->mpc_Ui->pc_ListViewNodes->SetSelectedIndexAndScroll(static_cast<sint32>(this->mu32_ActiveIndex));
         break;
      case ms32_SUBMODE_SYSDEF_BUSEDIT:
         this->mpc_Ui->pc_ListViewBuses->SetSelectedIndexAndScroll(static_cast<sint32>(this->mu32_ActiveIndex));
         break;
      default:
         break;
      }

      this->mpc_Ui->pc_GroupBoxSD->setVisible(true);

      break;

   case ms32_MODE_SYSVIEW:
      this->mpc_Ui->pc_ListViewViews->setVisible(true);
      this->mpc_Ui->pc_GroupBoxAddViewButtonSection->setVisible(true);
      this->mpc_Ui->pc_GroupBoxOpenCanMonitorOutside->setVisible(true);
      //Highlight active view
      this->mpc_Ui->pc_ListViewViews->SetActive(ou32_Index, os32_SubMode);

      break;
   default:
      break;
   }
   //Always trigger size change at start (even for system defintion) to properly clean up and initialize the size
   m_SysViewSizeChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Marks the specific mode with a star for changed data in the mode

   \param[in]  oq_Changed     Flag if data was changed or not
   \param[in]  oq_All         Flag if all items of a submode shall be marked or not
   \param[in]  os32_Mode      Index of mode
   \param[in]  os32_SubMode   Index of submode
   \param[in]  ou32_Index     Optional Index number (for example for node or bus)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::MarkModeForDataChanged(const bool oq_Changed, const bool oq_All, const sint32 os32_Mode,
                                                const sint32 os32_SubMode, const uint32 ou32_Index) const
{
   //Currently deactivated
   Q_UNUSED(oq_Changed)
   Q_UNUSED(oq_All)
   Q_UNUSED(os32_Mode)
   Q_UNUSED(os32_SubMode)
   Q_UNUSED(ou32_Index)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Specific function to reset any change use-case request to its original state

   \param[in] os32_Mode Use-case to revert to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::ResetUseCaseAfterChangeFailure(const sint32 os32_Mode) const
{
   //Update active mode
   if (os32_Mode == ms32_MODE_SYSDEF)
   {
      this->mpc_Ui->pc_WidgetTabSd->SetActive(true);
      this->mpc_Ui->pc_WidgetTabSc->SetActive(false);
   }
   else
   {
      this->mpc_Ui->pc_WidgetTabSd->SetActive(false);
      this->mpc_Ui->pc_WidgetTabSc->SetActive(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::InitText(void) const
{
   this->mpc_Ui->pc_PushButtonTopology->setText(C_GtGetText::h_GetText("NETWORK TOPOLOGY"));
   this->mpc_Ui->pc_BtnMain->setText(C_GtGetText::h_GetText("MAIN"));

   this->mpc_Ui->pc_PushButtonAddSysView->setText(C_GtGetText::h_GetText("Add View"));
   this->mpc_Ui->pc_PushButtonCanMonitor->setText(C_GtGetText::h_GetText("openSYDE CAN Monitor"));

   //SetText
   this->mpc_Ui->pc_WidgetTabSd->SetText(C_GtGetText::h_GetText("SYSTEM\nDEFINITION"));
   this->mpc_Ui->pc_WidgetTabSc->SetText(C_GtGetText::h_GetText("SYSTEM\nCOMMISSIONING"));

   this->mpc_Ui->pc_PushButtonCanMonitor->SetToolTipInformation(C_GtGetText::h_GetText("openSYDE CAN Monitor"),
                                                                C_GtGetText::h_GetText(
                                                                   "Open new instance of openSYDE CAN Monitor."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load splitter size from user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::LoadUserSettings(void)
{
   this->mq_Loaded = true;
   if (this->mpc_Ui->pc_ListViewViews->isVisible() == true)
   {
      this->mpc_Ui->pc_ListViewViews->LoadUserSettings();
   }
   else
   {
      const sint32 s32_FirstSegmentWidth = C_UsHandler::h_GetInstance()->GetNaviBarNodeSectionSize();

      this->mpc_Ui->pc_Splitter->SetFirstSegment(s32_FirstSegmentWidth);
      this->mq_NodeSectionSizeLoaded = true;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save splitter user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::SaveUserSettings(void)
{
   if (this->mq_Loaded == true)
   {
      if (this->mpc_Ui->pc_ListViewViews->isVisible() == true)
      {
         this->mpc_Ui->pc_ListViewViews->SaveUserSettings();
      }
      else if (this->mq_NodeSectionSizeLoaded == true)
      {
         const QList<sintn> c_Sizes = this->mpc_Ui->pc_Splitter->sizes();

         // Save only in case of a valid value
         // If the system view was opened and the system definition not, it can happen that no valid values will be
         // saved
         if ((c_Sizes.size() > 0) &&
             (c_Sizes[0] > 0))
         {
            C_UsHandler::h_GetInstance()->SetNaviBarNodeSectionSize(c_Sizes.at(0));
         }

         // Load and save are always called in combination.
         // In case of a project switch, the setting of the previous project should not be overwritten
         // when the system definition was not opened
         this->mq_NodeSectionSizeLoaded = false;
      }
      else
      {
         // Nothing to do
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten hide event slot

   Here: Save splitter position

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::hideEvent(QHideEvent * const opc_Event)
{
   SaveUserSettings();
   QWidget::hideEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: handle the system view size

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);
   m_SysViewSizeChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::paintEvent(QPaintEvent * const opc_Event)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);

   QWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handles all visible elements on minimize / maximize
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::HandleElements()
{
   if (this->mq_Maximized == true)
   {
      this->mpc_Ui->pc_HorizontalSpacerTop1->changeSize(7, 20);
      this->mpc_Ui->pc_HorizontalSpacerTop2->changeSize(40, 20);
      this->mpc_Ui->pc_HorizontalSpacerLogoRight->changeSize(40, 20);
      this->mpc_Ui->pc_HorizontalSpacerLogoLeft->changeSize(80, 20);
      this->mpc_Ui->pc_BtnMain->setVisible(true);
      this->mpc_Ui->pc_StwLogo->setVisible(true);
      this->mpc_Ui->pc_GroupBoxSD->setVisible(true);

      //restore elements
      this->SetMode(this->ms32_ActiveMode, this->ms32_ActiveSubMode);
   }
   else
   {
      this->mpc_Ui->pc_BtnMain->setVisible(false);
      this->mpc_Ui->pc_StwLogo->setVisible(false);
      this->mpc_Ui->pc_GroupBoxSD->setVisible(false);
      this->mpc_Ui->pc_HorizontalSpacerTop1->changeSize(0, 20);
      this->mpc_Ui->pc_HorizontalSpacerTop2->changeSize(0, 20);
      this->mpc_Ui->pc_HorizontalSpacerLogoRight->changeSize(0, 20);
      this->mpc_Ui->pc_HorizontalSpacerLogoLeft->changeSize(0, 20);

      //hide elements
      this->SetMode(ms32_MODE_NONE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for button start view (main) click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_StartViewClicked()
{
   Q_EMIT this->SigStartViewClicked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for node change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_NodesChanged(void) const
{
   uint32 u32_Index;

   std::vector<QString> c_Nodes;

   //Update node count
   this->mpc_Ui->pc_LabelNodes->setText(
      static_cast<QString>(C_GtGetText::h_GetText("Nodes (%1)")).arg(C_PuiSdHandler::
                                                                     h_GetInstance()
                                                                     ->GetOSCNodesSize()));

   c_Nodes.reserve(C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize());
   //add new nodes
   for (u32_Index = 0U; u32_Index < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(); ++u32_Index)
   {
      c_Nodes.push_back(C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_Index)->c_Properties.c_Name.c_str());
   }

   //Update view
   this->mpc_Ui->pc_ListViewNodes->SetContent(c_Nodes);
   //Trigger error check
   m_UpdateNodeErrors();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for busses change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_BussesChanged(void) const
{
   uint32 u32_Index;

   std::vector<QString> c_Buses;

   //Update bus count
   this->mpc_Ui->pc_LabelBuses->setText(
      static_cast<QString>(C_GtGetText::h_GetText("Buses (%1)")).arg(C_PuiSdHandler::
                                                                     h_GetInstance()
                                                                     ->GetOSCBusesSize()));

   c_Buses.reserve(C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize());
   //add new nodes
   for (u32_Index = 0U; u32_Index < C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize(); ++u32_Index)
   {
      c_Buses.push_back(C_PuiSdHandler::h_GetInstance()->GetOSCBus(u32_Index)->c_Name.c_str());
   }
   //Update view
   this->mpc_Ui->pc_ListViewBuses->SetContent(c_Buses);
   //Trigger error check
   m_UpdateBusErrors();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle add view button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_AddViewClicked(void)
{
   const uint16 u16_TimerId = osc_write_log_performance_start();

   Q_EMIT this->SigAddViewClicked();
   m_SysViewSizeChanged();

   osc_write_log_performance_stop(u16_TimerId, "Add view");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle open CAN monitor click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_OpenCanMonitor(void)
{
   QString c_ExecutablePath = C_Uti::h_GetExePath() + "/CAN_Monitor/openSYDE_CAN_Monitor.exe";
   // Adapted working directory is necessary for the stwpeak2.ini
   bool q_Temp = QProcess::startDetached(c_ExecutablePath, QStringList(),
                                         C_Uti::h_GetExePath() + "/CAN_Monitor");
   QString c_ErrorMsg = static_cast<QString>(
      "Could not start openSYDE CAN Monitor. Reason: Most likely due to insufficient permissions or the executable"
      " \"%1\" is missing.").arg(c_ExecutablePath);

   if (q_Temp == false)
   {
      osc_write_log_error("Open openSYDE CAN Monitor Tool", c_ErrorMsg.toStdString().c_str());

      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eERROR,
                                        C_GtGetText::h_GetText(
                                           "Could not start openSYDE CAN Monitor. Reason: Most likely due to insufficient permissions or the executable is missing."));
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Open openSYDE CAN Monitor"));
      c_MessageBox.SetDetails(static_cast<QString>(C_GtGetText::h_GetText(
                                                      "Executable path: \n%1")).arg(c_ExecutablePath));
      c_MessageBox.SetCustomMinHeight(230, 270);
      c_MessageBox.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle rename view action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_SysViewRenameClicked(const stw_types::uint32 ou32_ViewIndex, const QString & orc_Name)
{
   Q_EMIT this->SigRenameView(ou32_ViewIndex, orc_Name, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle delete view action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_SysViewDeleteClicked(const stw_types::uint32 ou32_ViewIndex)
{
   Q_EMIT this->SigDeleteView(ou32_ViewIndex, this->ms32_ActiveSubMode, this->mu32_ActiveIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle duplicate view action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_SysViewDuplicateClicked(const stw_types::uint32 ou32_ViewIndex)
{
   Q_EMIT this->SigDuplicateView(ou32_ViewIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle system view size change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_SysViewSizeChanged(void) const
{
   if (this->mpc_Ui->pc_ListViewViews->isVisible() == true)
   {
      const sint32 s32_OtherItemSizes = 444;
      const sint32 s32_MinSize = 52;
      //Check if there is any available space to work with
      if (this->height() > (s32_OtherItemSizes + s32_MinSize))
      {
         const sint32 s32_AvailableSpace = this->height() - s32_OtherItemSizes;
         //Get desired size taking into account that the minimum is still there
         const sint32 s32_DesiredSize =
            std::max(this->mpc_Ui->pc_ListViewViews->GetMaximumRequiredHeight(), s32_MinSize);
         sint32 s32_FinalSize;
         //Check if the desired size is in the available space (so in this case the add button can be moved)
         if (s32_DesiredSize < s32_AvailableSpace)
         {
            s32_FinalSize = s32_DesiredSize;
         }
         else
         {
            s32_FinalSize = s32_AvailableSpace;
         }
         this->mpc_Ui->pc_ScrollArea->setMinimumHeight(s32_FinalSize);
         this->mpc_Ui->pc_ScrollArea->setMaximumHeight(s32_FinalSize);
      }
      else
      {
         //Default if size calculation returns trash
         this->mpc_Ui->pc_ScrollArea->setMinimumHeight(s32_MinSize);
         this->mpc_Ui->pc_ScrollArea->setMaximumHeight(s32_MinSize);
      }
   }
   else
   {
      //Restore defaults
      this->mpc_Ui->pc_ScrollArea->setMinimumHeight(0);
      //lint -e1960 we cannot change Qt constant but it is still better than using the hard coded magic number 16777215
      this->mpc_Ui->pc_ScrollArea->setMaximumHeight(QWIDGETSIZE_MAX);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle SC tab clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_TabScCLicked(void)
{
   //Update active mode (user feedback before triggering a "long" action,
   // and avoid setting the buttons after they were reverted by an aborted action)
   this->mpc_Ui->pc_WidgetTabSd->SetActive(false);
   this->mpc_Ui->pc_WidgetTabSc->SetActive(true);

   Q_EMIT this->SigChangeUseCase(ms32_MODE_SYSVIEW, ms32_SUBMODE_SYSVIEW_SETUP, true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle SD tab clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_TabSdCLicked(void)
{
   //Update active mode (user feedback before triggering a "long" action,
   // and avoid setting the buttons after they were reverted by an aborted action)
   this->mpc_Ui->pc_WidgetTabSd->SetActive(true);
   this->mpc_Ui->pc_WidgetTabSc->SetActive(false);

   Q_EMIT this->SigSysDefRequest(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_TOPOLOGY);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update node errors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_UpdateNodeErrors(void)  const
{
   std::vector<bool> c_ErrorsNode;
   c_ErrorsNode.reserve(C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize());
   for (uint32 u32_ItNode = 0UL; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(); ++u32_ItNode)
   {
      c_ErrorsNode.push_back(C_PuiSdHandler::h_GetInstance()->CheckNodeConflict(u32_ItNode));
   }
   this->mpc_Ui->pc_ListViewNodes->SetError(c_ErrorsNode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update bus errors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_UpdateBusErrors(void) const
{
   std::vector<bool> c_ErrorsBus;
   c_ErrorsBus.reserve(C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize());
   for (uint32 u32_ItBus = 0UL; u32_ItBus < C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize(); ++u32_ItBus)
   {
      c_ErrorsBus.push_back(C_PuiSdHandler::h_GetInstance()->CheckBusConflict(u32_ItBus));
   }
   this->mpc_Ui->pc_ListViewBuses->SetError(c_ErrorsBus);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle bus click

   \param[in] osn_Index Selected index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_OnClickBus(const sintn osn_Index)
{
   if (osn_Index >= 0)
   {
      const uint32 u32_Index = static_cast<uint32>(osn_Index);
      const stw_opensyde_core::C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(u32_Index);
      if (pc_Bus != NULL)
      {
         Q_EMIT this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_BUSEDIT, u32_Index, pc_Bus->c_Name.c_str());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle node click

   \param[in] osn_Index Selected index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_OnClickNode(const sintn osn_Index)
{
   if (osn_Index >= 0)
   {
      const uint32 u32_Index = static_cast<uint32>(osn_Index);
      const stw_opensyde_core::C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_Index);
      if (pc_Node != NULL)
      {
         Q_EMIT this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_NODEEDIT, u32_Index,
                                    pc_Node->c_Properties.c_Name.c_str());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select view

   \param[in] ou32_ViewIndex     View index
   \param[in] os32_SubMode       Sub mode
   \param[in] orc_Name           Selected view name
   \param[in] orc_SubSubItemName Selected sub sub mode name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_SelectView(const uint32 ou32_ViewIndex, const sint32 os32_SubMode, const QString & orc_Name,
                                      const QString & orc_SubSubItemName)
{
   //After change of mode we will restore user settings so we have to store the current configuration
   this->SaveUserSettings();
   Q_EMIT this->SigChangeMode(ms32_MODE_SYSVIEW, os32_SubMode, ou32_ViewIndex, orc_Name, orc_SubSubItemName);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   update all view icons (depends on current state)

   \param[in] oq_CheckOnlyThisView Flag to reduce view error check to one item (only used in view state)
   \param[in] ou32_ViewIndex       Index to specify which view changed (only used if oq_CheckOnlyThisView set)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_UpdateViewIcons(const bool oq_CheckOnlyThisView, const uint32 ou32_ViewIndex) const
{
   if (this->ms32_ActiveMode == ms32_MODE_SYSVIEW)
   {
      this->mpc_Ui->pc_ListViewViews->UpdateDeco(oq_CheckOnlyThisView, ou32_ViewIndex);
   }
   else if (this->ms32_ActiveMode == ms32_MODE_SYSDEF)
   {
      switch (this->ms32_ActiveSubMode)
      {
      case ms32_SUBMODE_SYSDEF_NODEEDIT:
         m_UpdateNodeErrors();
         //Node com data pool changes can affect connected busses (eg com data pool delete)
         m_UpdateBusErrors();
         break;
      case ms32_SUBMODE_SYSDEF_BUSEDIT:
         m_UpdateBusErrors();
         break;
      case ms32_SUBMODE_SYSDEF_TOPOLOGY:
         //Update all errors
         m_UpdateNodeErrors();
         m_UpdateBusErrors();
         break;
      default:
         //No error handling necessary
         break;
      }
   }
   else
   {
      //No error handling necessary
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for animation timer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_AnimationTimerEvent()
{
   QSize c_Size;
   sintn sn_NewWidth;

   if (this->mq_Maximized == false)
   {
      // minimize the widget
      c_Size = this->minimumSize();

      if (c_Size.width() == msn_MAXIMIZED_WIDTH)
      {
         // first call
         this->HandleElements();
      }

      if (c_Size.width() > msn_MINIMIZED_WIDTH)
      {
         //make slide step
         sn_NewWidth = c_Size.width() - this->msn_StepSize;
         if (sn_NewWidth > msn_MINIMIZED_WIDTH)
         {
            this->setMinimumSize(sn_NewWidth, 0);
         }
         else
         {
            this->setMinimumSize(msn_MINIMIZED_WIDTH, 0);
         }
      }
      else
      {
         // finished
         this->mc_TimerAnimation.stop();
      }
   }
   else
   {
      // maximize the widget
      c_Size = this->minimumSize();

      if (c_Size.width() < msn_MAXIMIZED_WIDTH)
      {
         //make slide step
         sn_NewWidth = c_Size.width() + this->msn_StepSize;
         if (sn_NewWidth < msn_MAXIMIZED_WIDTH)
         {
            this->setMinimumSize(sn_NewWidth, 0);
         }
         else
         {
            this->setMinimumSize(msn_MAXIMIZED_WIDTH, 0);
         }
      }
      else
      {
         // finished
         this->HandleElements();
         this->mc_TimerAnimation.stop();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle push button topology click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagNaviBarWidget::m_PbTopologyClick(void)
{
   const QString c_Name = C_GtGetText::h_GetText("NETWORK TOPOLOGY");
   const sint32 s32_SubMode = ms32_SUBMODE_SYSDEF_TOPOLOGY;

   Q_EMIT this->SigChangeMode(ms32_MODE_SYSDEF, s32_SubMode, 0, c_Name);
}
