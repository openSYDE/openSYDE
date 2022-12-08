//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for selecting and managing dashboards (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_SyvDaDashboardSelectorTabWidget.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeGbxTransparent.hpp"
#include "C_UsHandler.hpp"
#include "C_PuiProject.hpp"
#include "C_SyvClipBoardHelper.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_SyvDaDashboardTabProperties.hpp"
#include "C_SyvDaCopyPasteManager.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QTabBar::ButtonPosition C_SyvDaDashboardSelectorTabWidget::mhe_TAB_CONTENT_POSITION = QTabBar::LeftSide;
const QString C_SyvDaDashboardSelectorTabWidget::mhc_ADD_ICON_DARK = "://images/IconAddDisabled.svg";
const QString C_SyvDaDashboardSelectorTabWidget::mhc_ADD_ICON_LIGHT = "://images/IconAddEnabled.svg";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Parent widget
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaDashboardSelectorTabWidget::C_SyvDaDashboardSelectorTabWidget(QWidget * const opc_Parent) :
   QTabWidget(opc_Parent),
   mpc_PushButton(NULL),
   mpc_TabBar(NULL),
   mq_EditMode(false),
   mq_Connected(false),
   me_DashboardTabType(C_PuiSvDashboard::eSCENE)
{
   const bool q_ServiceModeActive = C_PuiSvHandler::h_GetInstance()->GetServiceModeActive();

   //Add button
   mpc_PushButton = new C_OgePubIconText(this);
   this->mpc_PushButton->SetIconSize(24);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_PushButton->setIcon(QIcon(C_SyvDaDashboardSelectorTabWidget::mhc_ADD_ICON_LIGHT));
   this->mpc_PushButton->SetToolTipInformation(C_GtGetText::h_GetText("Add"),
                                               C_GtGetText::h_GetText("Add new Dashboard tab."));
   connect(this->mpc_PushButton, &QPushButton::clicked, this, &C_SyvDaDashboardSelectorTabWidget::m_OnAddClicked);

   // Handle service mode
   this->mpc_PushButton->setEnabled(!q_ServiceModeActive);

   //Tabs
   this->mpc_TabBar = new C_SyvDaDashboardSelectorTabBar(this);
   this->setTabBar(this->mpc_TabBar);
   connect(this->mpc_TabBar, &C_SyvDaDashboardSelectorTabBar::SigTabTearOff, this,
           &C_SyvDaDashboardSelectorTabWidget::TearOffWidget);
   connect(this->mpc_TabBar, &C_SyvDaDashboardSelectorTabBar::SigNeedScreenshot, this,
           &C_SyvDaDashboardSelectorTabWidget::m_MakeScreenshot);
   connect(this->mpc_TabBar, &C_SyvDaDashboardSelectorTabBar::SigReselect, this,
           &C_SyvDaDashboardSelectorTabWidget::m_HandleChangeSelection);
   connect(this, &C_SyvDaDashboardSelectorTabWidget::currentChanged, this,
           &C_SyvDaDashboardSelectorTabWidget::m_CurrentChanged);
   connect(this->mpc_TabBar, &C_SyvDaDashboardSelectorTabBar::SigResizeHappened, this,
           &C_SyvDaDashboardSelectorTabWidget::m_RepositionAddButton);
   connect(this, &C_SyvDaDashboardSelectorTabWidget::currentChanged, this,
           &C_SyvDaDashboardSelectorTabWidget::m_OnTabChanged);

   this->setMovable(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1579}  no memory leak because of the parent of mpc_TabBar and mpc_PushButton and the Qt memory management
C_SyvDaDashboardSelectorTabWidget::~C_SyvDaDashboardSelectorTabWidget(void)
{
   m_StoreUserSettings();
   //Delete torn off widgets
   for (QList<C_SyvDaTearOffWidget *>::const_iterator c_ItItem = this->mc_TearedOffWidgets.begin();
        c_ItItem != this->mc_TearedOffWidgets.end(); ++c_ItItem)
   {
      C_SyvDaTearOffWidget * const pc_Widget = *c_ItItem;
      if (pc_Widget != NULL)
      {
         //Delete
         pc_Widget->deleteLater();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show tear off widget for specified tab

   \param[in]  os32_Index  Tab index
   \param[in]  orc_Pos     Widget position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::TearOffWidget(const int32_t os32_Index, const QPoint & orc_Pos)
{
   //Block tear off of last tab
   if (this->count() > 1)
   {
      C_SyvDaDashboardWidget * const pc_WidgetRef =
         dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(os32_Index));

      if (pc_WidgetRef != NULL)
      {
         m_Disconnect(pc_WidgetRef);
         //Remove widget
         this->removeTab(os32_Index);

         //Tear off after tab widget was cleaned up
         m_TearOffWidget(pc_WidgetRef->GetDashboardIndex(), this->mapToGlobal(orc_Pos), pc_WidgetRef);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the edit mode

   \param[in]  oq_Active   Flag for edit mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::SetEditMode(const bool oq_Active)
{
   int32_t s32_Counter;

   QList<C_SyvDaTearOffWidget *>::const_iterator c_ItItem;

   C_SyvDaDashboardSelectorTabBar * const pc_TabBar =
      dynamic_cast<C_SyvDaDashboardSelectorTabBar * const>(this->tabBar());

   this->mq_EditMode = oq_Active;

   for (s32_Counter = 0; s32_Counter < this->count(); ++s32_Counter)
   {
      C_SyvDaDashboardWidget * const pc_WidgetRef =
         dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(s32_Counter));

      if (pc_WidgetRef != NULL)
      {
         pc_WidgetRef->SetEditMode(oq_Active);
      }
   }

   for (c_ItItem = this->mc_TearedOffWidgets.begin(); c_ItItem != this->mc_TearedOffWidgets.end(); ++c_ItItem)
   {
      (*c_ItItem)->SetEditMode(oq_Active);
   }

   if (pc_TabBar != NULL)
   {
      C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(pc_TabBar, "Edit", oq_Active);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the confirm button in tear off widget enabled or disabled

   \param[in]  oq_Enabled  Flag for enabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::SetEnabled(const bool oq_Enabled)
{
   for (QList<C_SyvDaTearOffWidget *>::const_iterator c_ItItem = this->mc_TearedOffWidgets.begin();
        c_ItItem != this->mc_TearedOffWidgets.end(); ++c_ItItem)
   {
      (*c_ItItem)->SetEnabled(oq_Enabled);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set view index

   \param[in]  ou32_Value  New view index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::SetViewIndex(const uint32_t ou32_Value)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(ou32_Value);

   this->mu32_ViewIndex = ou32_Value;
   //Load tabs
   if (pc_View != NULL)
   {
      const std::vector<C_PuiSvDashboard> & rc_Dashboards = pc_View->GetDashboards();
      std::vector<uint32_t> c_LeftToAdd;
      const C_UsSystemView c_ViewUserSettings = C_UsHandler::h_GetInstance()->GetProjSvSetupView(pc_View->GetName());
      c_LeftToAdd.reserve(rc_Dashboards.size());
      //Initially add all indices
      for (uint32_t u32_It = 0; u32_It < rc_Dashboards.size(); ++u32_It)
      {
         c_LeftToAdd.push_back(u32_It);
      }
      do
      {
         uint32_t u32_LastIteration;
         //Copy all indices to add
         std::vector<uint32_t> c_Iteration = c_LeftToAdd;
         u32_LastIteration = c_LeftToAdd.size();
         //Reset next indices to add
         c_LeftToAdd.clear();
         for (uint32_t u32_ItDashboard = 0; u32_ItDashboard < c_Iteration.size(); ++u32_ItDashboard)
         {
            //Check index
            tgl_assert(c_Iteration[u32_ItDashboard] < rc_Dashboards.size());
            if (c_Iteration[u32_ItDashboard] < rc_Dashboards.size())
            {
               const C_PuiSvDashboard & rc_CurDashboard = rc_Dashboards[c_Iteration[u32_ItDashboard]];
               const C_UsSystemViewDashboard c_DashboardUserSettings = c_ViewUserSettings.GetDashboardSettings(
                  rc_CurDashboard.GetName());
               //User settings
               if (c_DashboardUserSettings.q_TornOff == false)
               {
                  //Check if index is currently available
                  if (rc_CurDashboard.GetTabIndex() <= this->count())
                  {
                     //Check if index valid at all
                     if (rc_CurDashboard.GetTabIndex() < 0)
                     {
                        m_AddSpecificTab(c_Iteration[u32_ItDashboard]);
                     }
                     else
                     {
                        m_AddSpecificTab(c_Iteration[u32_ItDashboard], rc_CurDashboard.GetTabIndex());
                     }
                  }
                  else
                  {
                     c_LeftToAdd.push_back(c_Iteration[u32_ItDashboard]);
                  }
               }
               else
               {
                  m_TearOffWidget(c_Iteration[u32_ItDashboard], c_DashboardUserSettings.c_TornOffWindowPosition, NULL,
                                  c_DashboardUserSettings.c_TornOffWindowSize,
                                  c_DashboardUserSettings.q_TornOffWindowMinimized,
                                  c_DashboardUserSettings.q_TornOffWindowMaximized,
                                  true);
               }
            }
         }
         //Check for endless loop
         if (u32_LastIteration == c_LeftToAdd.size())
         {
            //Ignore tab restoration
            for (uint32_t u32_ItDashboard = 0; u32_ItDashboard < c_LeftToAdd.size(); ++u32_ItDashboard)
            {
               m_AddSpecificTab(c_LeftToAdd[u32_ItDashboard]);
            }
            break;
         }
      }
      while (c_LeftToAdd.size() > 0);
      //Handle current
      if ((c_ViewUserSettings.GetDashboardSelectedTabIndex() < 0) ||
          (c_ViewUserSettings.GetDashboardSelectedTabIndex() >= this->count()))
      {
         //Default
         m_CurrentChanged(0);
      }
      else
      {
         //Change current
         this->setCurrentIndex(c_ViewUserSettings.GetDashboardSelectedTabIndex());
         m_CurrentChanged(c_ViewUserSettings.GetDashboardSelectedTabIndex());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::Save(void)
{
   //Tabs
   for (int32_t s32_ItTab = 0; s32_ItTab < this->count(); ++s32_ItTab)
   {
      C_SyvDaDashboardWidget * const pc_WidgetRef =
         dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(s32_ItTab));

      if (pc_WidgetRef != NULL)
      {
         pc_WidgetRef->Save();
      }
   }
   //Tear off
   for (QList<C_SyvDaTearOffWidget *>::const_iterator c_ItItem = this->mc_TearedOffWidgets.begin();
        c_ItItem != this->mc_TearedOffWidgets.end(); ++c_ItItem)
   {
      C_SyvDaTearOffWidget * const pc_Widget = *c_ItItem;
      if (pc_Widget != NULL)
      {
         //Delete
         pc_Widget->Save();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply current dark mode setting

   \param[in]  oq_Active   Dark mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::ApplyDarkMode(const bool oq_Active)
{
   int32_t s32_Counter;

   QList<C_SyvDaTearOffWidget *>::const_iterator c_ItItem;

   C_SyvDaDashboardSelectorTabBar * const pc_TabBar =
      dynamic_cast<C_SyvDaDashboardSelectorTabBar * const>(this->tabBar());

   for (s32_Counter = 0; s32_Counter < this->count(); ++s32_Counter)
   {
      C_SyvDaDashboardWidget * const pc_WidgetRef =
         dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(s32_Counter));

      if (pc_WidgetRef != NULL)
      {
         pc_WidgetRef->SetDarkMode(oq_Active);
      }
   }

   for (s32_Counter = 0; s32_Counter < this->count(); ++s32_Counter)
   {
      C_OgeWiDashboardTab * const pc_DrawingWidget =
         dynamic_cast<C_OgeWiDashboardTab * const>(this->tabBar()->tabButton(s32_Counter,
                                                                             C_SyvDaDashboardSelectorTabWidget::
                                                                             mhe_TAB_CONTENT_POSITION));
      if (pc_DrawingWidget != NULL)
      {
         pc_DrawingWidget->SetDarkMode(oq_Active);
      }
   }

   for (c_ItItem = this->mc_TearedOffWidgets.begin(); c_ItItem != this->mc_TearedOffWidgets.end(); ++c_ItItem)
   {
      (*c_ItItem)->SetDarkMode(oq_Active);
   }

   if (pc_TabBar != NULL)
   {
      C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(pc_TabBar, "DarkMode", oq_Active);
   }

   if (this->mpc_PushButton != NULL)
   {
      if (oq_Active == true)
      {
         this->mpc_PushButton->setIcon(QIcon(C_SyvDaDashboardSelectorTabWidget::mhc_ADD_ICON_DARK));
      }
      else
      {
         this->mpc_PushButton->setIcon(QIcon(C_SyvDaDashboardSelectorTabWidget::mhc_ADD_ICON_LIGHT));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Registers all relevant dashboard widgets at the associated data dealer

   \param[in,out]  orc_ComDriver    Communication driver
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::RegisterWidgets(C_SyvComDriverDiag & orc_ComDriver) const
{
   int32_t s32_Counter;

   for (s32_Counter = 0; s32_Counter < this->count(); ++s32_Counter)
   {
      C_SyvDaDashboardWidget * const pc_WidgetRef =
         dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(s32_Counter));

      if (pc_WidgetRef != NULL)
      {
         pc_WidgetRef->RegisterWidgets(orc_ComDriver);
      }
   }

   for (QList<C_SyvDaTearOffWidget *>::const_iterator c_ItItem = this->mc_TearedOffWidgets.begin();
        c_ItItem != this->mc_TearedOffWidgets.end(); ++c_ItItem)
   {
      (*c_ItItem)->RegisterWidgets(orc_ComDriver);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active   Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::ConnectionActiveChanged(const bool oq_Active)
{
   int32_t s32_Counter;

   this->mq_Connected = oq_Active;

   for (s32_Counter = 0; s32_Counter < this->count(); ++s32_Counter)
   {
      C_SyvDaDashboardWidget * const pc_WidgetRef =
         dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(s32_Counter));

      if (pc_WidgetRef != NULL)
      {
         pc_WidgetRef->ConnectionActiveChanged(oq_Active);
      }
   }

   for (QList<C_SyvDaTearOffWidget *>::const_iterator c_ItItem = this->mc_TearedOffWidgets.begin();
        c_ItItem != this->mc_TearedOffWidgets.end(); ++c_ItItem)
   {
      (*c_ItItem)->ConnectionActiveChanged(oq_Active, false);
   }

   for (s32_Counter = 0; s32_Counter < this->count(); ++s32_Counter)
   {
      C_OgeWiDashboardTab * const pc_DrawingWidget =
         dynamic_cast<C_OgeWiDashboardTab * const>(this->tabBar()->tabButton(s32_Counter,
                                                                             C_SyvDaDashboardSelectorTabWidget::
                                                                             mhe_TAB_CONTENT_POSITION));
      if (pc_DrawingWidget != NULL)
      {
         pc_DrawingWidget->SetInteractive(!oq_Active);
      }
   }
   if (this->mpc_PushButton != NULL)
   {
      this->mpc_PushButton->setVisible(!oq_Active);
   }
   //Trigger tab changed signal once all widgets are definitely initialized
   this->m_OnTabChanged(this->currentIndex());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates all values of all dashboard widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::UpdateShowValues(void) const
{
   int32_t s32_Counter;

   for (s32_Counter = 0; s32_Counter < this->count(); ++s32_Counter)
   {
      C_SyvDaDashboardWidget * const pc_WidgetRef =
         dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(s32_Counter));

      if (pc_WidgetRef != NULL)
      {
         pc_WidgetRef->UpdateShowValues();
      }
   }

   for (QList<C_SyvDaTearOffWidget *>::const_iterator c_ItItem = this->mc_TearedOffWidgets.begin();
        c_ItItem != this->mc_TearedOffWidgets.end(); ++c_ItItem)
   {
      (*c_ItItem)->UpdateShowValues();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle changes of transmission mode for any data element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::UpdateTransmissionConfiguration()
{
   int32_t s32_Counter;

   for (s32_Counter = 0; s32_Counter < this->count(); ++s32_Counter)
   {
      C_SyvDaDashboardWidget * const pc_WidgetRef =
         dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(s32_Counter));

      if (pc_WidgetRef != NULL)
      {
         pc_WidgetRef->UpdateTransmissionConfiguration();
      }
   }

   for (QList<C_SyvDaTearOffWidget *>::const_iterator c_ItItem = this->mc_TearedOffWidgets.begin();
        c_ItItem != this->mc_TearedOffWidgets.end(); ++c_ItItem)
   {
      (*c_ItItem)->UpdateTransmissionConfiguration();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert tab index to dashboard index

   \param[in]   os32_TabIndex          Visible tab index
   \param[out]  oru32_DashboardIndex   Internal dashboard data index

   \return
   C_NO_ERR Valid result found
   C_RANGE  Tab not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaDashboardSelectorTabWidget::GetDashboardIndexForTabIndex(const int32_t os32_TabIndex,
                                                                        uint32_t & oru32_DashboardIndex) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (os32_TabIndex < this->count())
   {
      C_SyvDaDashboardWidget * const pc_Widget =
         dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(os32_TabIndex));
      if (pc_Widget != NULL)
      {
         oru32_DashboardIndex = pc_Widget->GetDashboardIndex();
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle manual user operation finished event

   \param[in]  os32_Result    Operation result
   \param[in]  ou8_Nrc        Negative response code, if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc)
{
   for (int32_t s32_Counter = 0; s32_Counter < this->count(); ++s32_Counter)
   {
      C_SyvDaDashboardWidget * const pc_WidgetRef =
         dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(s32_Counter));

      if (pc_WidgetRef != NULL)
      {
         pc_WidgetRef->HandleManualOperationFinished(os32_Result, ou8_Nrc);
      }
   }

   for (QList<C_SyvDaTearOffWidget *>::const_iterator c_ItItem = this->mc_TearedOffWidgets.begin();
        c_ItItem != this->mc_TearedOffWidgets.end(); ++c_ItItem)
   {
      (*c_ItItem)->HandleManualOperationFinished(os32_Result, ou8_Nrc);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::paintEvent(QPaintEvent * const opc_Event)
{
   stw::opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);

   QTabWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   Here: Restrict tab bar size

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   const int32_t s32_BUTTON_WIDTH = 40;

   QTabWidget::resizeEvent(opc_Event);

   //Restrict size
   this->tabBar()->setMaximumWidth(std::max((this->width() - s32_BUTTON_WIDTH) - 50, 0));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten hide event slot

   Here: Store all dashboard user settings

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::hideEvent(QHideEvent * const opc_Event)
{
   m_StoreUserSettings();
   //Call hide event (afterwards, otherwise geometries may be invalid)
   QTabWidget::hideEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Reshow all hidden widgets

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::showEvent(QShowEvent * const opc_Event)
{
   QTabWidget::showEvent(opc_Event);
   //Also show all widgets!
   for (QList<C_SyvDaTearOffWidget *>::const_iterator c_ItItem = this->mc_TearedOffWidgets.begin();
        c_ItItem != this->mc_TearedOffWidgets.end(); ++c_ItItem)
   {
      (*c_ItItem)->show();
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_WidgetComeBack(C_SyvDaTearOffWidget * const opc_Widget)
{
   if (opc_Widget != NULL)
   {
      const uint32_t u32_Index = opc_Widget->GetIndex();
      m_AddSpecificTab(u32_Index, -1, opc_Widget->GetWidget());

      // Remove the widget
      this->mc_TearedOffWidgets.removeOne(opc_Widget);

      disconnect(opc_Widget, &C_SyvDaTearOffWidget::SigWidgetComeBack, this,
                 &C_SyvDaDashboardSelectorTabWidget::m_WidgetComeBack);

      disconnect(opc_Widget, &C_SyvDaTearOffWidget::SigConfirmClicked, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigConfirmClicked);
      disconnect(opc_Widget, &C_SyvDaTearOffWidget::SigCancelClicked, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigCancelClicked);
      disconnect(opc_Widget, &C_SyvDaTearOffWidget::SigDataPoolWrite, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigDataPoolWrite);
      disconnect(opc_Widget, &C_SyvDaTearOffWidget::SigDataPoolRead, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigDataPoolRead);
      disconnect(opc_Widget, &C_SyvDaTearOffWidget::SigNvmReadList, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigNvmReadList);
      disconnect(opc_Widget, &C_SyvDaTearOffWidget::SigTriggerUpdateTransmissionConfiguration, this,
                 &C_SyvDaDashboardSelectorTabWidget::UpdateTransmissionConfiguration);
      //Error
      disconnect(opc_Widget, &C_SyvDaTearOffWidget::SigErrorChange, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigErrorChange);

      //Pop up
      opc_Widget->deleteLater();
   }
}

//----------------------------------------------------------------------------------------------------------------------

void C_SyvDaDashboardSelectorTabWidget::m_MakeScreenshot(const int32_t os32_Index)
{
   QWidget * const pc_Widget = this->widget(os32_Index);

   if ((pc_Widget != NULL) &&
       (this->mpc_TabBar != NULL))
   {
      QPixmap c_Screenshot(pc_Widget->size());
      // create the screenshot
      pc_Widget->render(&c_Screenshot, QPoint(), static_cast<QRegion>(pc_Widget->rect()));
      // resize the screenshot
      c_Screenshot = c_Screenshot.scaledToWidth(200, Qt::FastTransformation);

      this->mpc_TabBar->SetScreenshot(c_Screenshot);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init tab styling widget

   \param[in]  ou32_Index     Tab index
   \param[in]  orc_Name       Tab name
   \param[in]  orc_Comment    Tab comment
   \param[in]  oq_Active      Tab active
   \param[in]  oq_Dark        Dark mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_InitTabStyle(const uint32_t ou32_Index, const QString & orc_Name,
                                                       const QString & orc_Comment, const bool oq_Active,
                                                       const bool oq_Dark)
{
   C_OgeWiDashboardTab * const pc_DrawingWidget = new C_OgeWiDashboardTab(NULL, true);
   C_OgeGbxTransparent * const pc_GroupBox = new C_OgeGbxTransparent();

   switch (this->me_DashboardTabType)
   {
   case C_PuiSvDashboard::eSCENE:
      pc_DrawingWidget->SetTabIconVisibility(false);
      break;
   case C_PuiSvDashboard::eCHART:
      pc_DrawingWidget->SetTabIconVisibility(true);
      break;
   default:
      break;
   }

   pc_GroupBox->resize(40, 1);

   //Space
   //"RightSide": We need to modify this if we decide to change sides:
   this->tabBar()->setTabButton(static_cast<int32_t>(ou32_Index), QTabBar::RightSide, pc_GroupBox);

   connect(pc_DrawingWidget, &C_OgeWiDashboardTab::SigActiveChanged, this,
           &C_SyvDaDashboardSelectorTabWidget::m_OnActiveChange);
   connect(pc_DrawingWidget, &C_OgeWiDashboardTab::SigCopyAction, this,
           &C_SyvDaDashboardSelectorTabWidget::m_OnCopyAction);
   connect(pc_DrawingWidget, &C_OgeWiDashboardTab::SigCutAction, this,
           &C_SyvDaDashboardSelectorTabWidget::m_OnCutAction);
   connect(pc_DrawingWidget, &C_OgeWiDashboardTab::SigPasteAction, this,
           &C_SyvDaDashboardSelectorTabWidget::m_OnPasteAction);
   connect(pc_DrawingWidget, &C_OgeWiDashboardTab::SigDeleteAction, this,
           &C_SyvDaDashboardSelectorTabWidget::m_OnDeleteAction);
   connect(pc_DrawingWidget, &C_OgeWiDashboardTab::SigCloseAction, this,
           &C_SyvDaDashboardSelectorTabWidget::m_OnCloseAction);
   connect(pc_DrawingWidget, &C_OgeWiDashboardTab::SigUndockAction, this,
           &C_SyvDaDashboardSelectorTabWidget::m_OnUndock);
   connect(pc_DrawingWidget, &C_OgeWiDashboardTab::SigEditPropertiesAction, this,
           &C_SyvDaDashboardSelectorTabWidget::m_OnEditProperties);

   pc_DrawingWidget->SetDarkMode(oq_Dark);
   this->tabBar()->setTabButton(static_cast<int32_t>(ou32_Index),
                                C_SyvDaDashboardSelectorTabWidget::mhe_TAB_CONTENT_POSITION, pc_DrawingWidget);
   //After add (AND display!)
   pc_DrawingWidget->SetText(orc_Name);
   pc_DrawingWidget->SetToolTip(orc_Name, orc_Comment);
   pc_DrawingWidget->SetActive(oq_Active);
   pc_DrawingWidget->SetInteractive(!this->mq_Connected);
   this->setTabText(static_cast<int32_t>(ou32_Index), "");
} //lint !e429  //no memory leak because of the parent of pc_DrawingWidget and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle current change event

   \param[in]  os32_CurrentIndex  Current index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_CurrentChanged(const int32_t os32_CurrentIndex) const
{
   //Highlighting
   for (int32_t s32_ItTab = 0; s32_ItTab < this->count(); ++s32_ItTab)
   {
      C_OgeWiDashboardTab * const pc_DrawingWidget =
         dynamic_cast<C_OgeWiDashboardTab * const>(this->tabBar()->tabButton(s32_ItTab,
                                                                             C_SyvDaDashboardSelectorTabWidget::
                                                                             mhe_TAB_CONTENT_POSITION));
      if (pc_DrawingWidget != NULL)
      {
         if (s32_ItTab == os32_CurrentIndex)
         {
            pc_DrawingWidget->SetCurrent(true);
         }
         else
         {
            pc_DrawingWidget->SetCurrent(false);
         }
      }
   }
   //Focus

   C_SyvDaDashboardWidget * const pc_Widget =
      dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(os32_CurrentIndex));
   if (pc_Widget != NULL)
   {
      pc_Widget->SetFocus();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle add button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_OnAddClicked(void)
{
   // show popup dialog
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_SyvDaDashboardTabProperties * const pc_Dialog = new C_SyvDaDashboardTabProperties(*c_New, "DASHBOARD TAB",
                                                                                       0U,
                                                                                       this->mu32_ViewIndex,
                                                                                       true);

   Q_UNUSED(pc_Dialog)

   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      C_PuiSvDashboard c_NewDashboard;

      c_NewDashboard.SetName(pc_Dialog->GetDashboardTabName());
      c_NewDashboard.SetComment(pc_Dialog->GetDashboardTabComment());
      c_NewDashboard.SetType(pc_Dialog->GetDashboardTabType());

      this->me_DashboardTabType = pc_Dialog->GetDashboardTabType();

      m_AddTab(c_NewDashboard, NULL);
   }

   if (c_New != NULL)
   {
      c_New->HideOverlay();
      c_New->deleteLater();
   }
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle add button repositioning
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_RepositionAddButton(void)
{
   if (this->mpc_PushButton != NULL)
   {
      this->mpc_PushButton->move(this->tabBar()->width() + 58, 8);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle selection change request

   \param[in]  os32_Index  Last selected item index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_HandleChangeSelection(const int32_t os32_Index)
{
   int32_t s32_NewSelection = -1;
   bool q_Found = false;

   //Select other tab
   //First check higher index tabs
   for (int32_t s32_ItTab = os32_Index + 1L; s32_ItTab < this->count(); ++s32_ItTab)
   {
      if (this->isTabEnabled(s32_ItTab) == true)
      {
         q_Found = true;
         s32_NewSelection = s32_ItTab;
         break;
      }
   }
   if (q_Found == false)
   {
      //Then check lower index tabs
      for (int32_t s32_ItTab = 0; s32_ItTab < os32_Index; ++s32_ItTab)
      {
         if (this->isTabEnabled(s32_ItTab) == true)
         {
            q_Found = true;
            s32_NewSelection = s32_ItTab;
            break;
         }
      }
   }
   if (q_Found == true)
   {
      //Select new tab if any valid tab found
      this->setCurrentIndex(s32_NewSelection);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Map data index to tab index

   \param[in]  ou32_DataIndex    Data index

   \return
   Tab index
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaDashboardSelectorTabWidget::m_MapDataIndexToTabIndex(const uint32_t ou32_DataIndex) const
{
   int32_t s32_Retval = -1;

   for (int32_t s32_ItTab = 0; s32_ItTab < this->count(); ++s32_ItTab)
   {
      C_SyvDaDashboardWidget * const pc_Widget =
         dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(s32_ItTab));
      if (pc_Widget != NULL)
      {
         if (pc_Widget->GetDashboardIndex() == ou32_DataIndex)
         {
            s32_Retval = s32_ItTab;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add specific tab at end

   \param[in]      ou32_DataIndex   Dashboard data index
   \param[in]      os32_TabIndex    Dashboard tab index
   \param[in,out]  opc_Widget       Optional dashboard widget to integrate instead of creating a new one
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_AddSpecificTab(const uint32_t ou32_DataIndex, const int32_t os32_TabIndex,
                                                         C_SyvDaDashboardWidget * const opc_Widget)
{
   const uint16_t u16_TimerId = osc_write_log_performance_start();

   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(ou32_DataIndex);
      if (pc_Dashboard != NULL)
      {
         int32_t s32_Index;
         C_SyvDaDashboardWidget * pc_Widget;

         if (opc_Widget != NULL)
         {
            pc_Widget = opc_Widget;
         }
         else
         {
            pc_Widget = new C_SyvDaDashboardWidget(this->mu32_ViewIndex, ou32_DataIndex,
                                                   pc_Dashboard->GetName(), /* pc_Dashboard->GetComment(),*/ false,
                                                   this);
         }

         pc_Widget->SetEditMode(this->mq_EditMode);
         pc_Widget->SetDarkMode(pc_View->GetDarkModeActive());

         //Check for insert vs add action
         if ((os32_TabIndex >= 0) && (os32_TabIndex <= this->count()))
         {
            s32_Index = this->insertTab(os32_TabIndex, pc_Widget, "");
            tgl_assert(s32_Index == os32_TabIndex);
         }
         else
         {
            this->addTab(pc_Widget, "");
            s32_Index = this->count() - 1;
         }
         this->me_DashboardTabType = pc_Dashboard->GetType();
         m_InitTabStyle(static_cast<uint32_t>(s32_Index), pc_Dashboard->GetName(), pc_Dashboard->GetComment(),
                        pc_Dashboard->GetActive(), pc_View->GetDarkModeActive());
         m_Connect(pc_Widget);
      } //lint !e429  //no memory leak because of the parent of pc_Widget and the Qt memory management
   }

   osc_write_log_performance_stop(u16_TimerId, "Load dashboard");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync dashboard data indices

   \param[in]  ou32_DataIndex    Dashboard data index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_SyncDashboardDeleteDataIndex(const uint32_t ou32_DataIndex)
{
   //Sync tabs
   for (int32_t s32_ItTab = 0; s32_ItTab < this->count(); ++s32_ItTab)
   {
      C_SyvDaDashboardWidget * const pc_Widget = dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(s32_ItTab));
      if (pc_Widget != NULL)
      {
         const uint32_t u32_CurDataIndex = pc_Widget->GetDashboardIndex();
         if (u32_CurDataIndex >= ou32_DataIndex)
         {
            pc_Widget->SetDashboardIndex(u32_CurDataIndex - 1);
         }
      }
   }
   //Sync tear off widgets
   for (QList<C_SyvDaTearOffWidget *>::const_iterator c_ItTearOffWidgets = this->mc_TearedOffWidgets.begin();
        c_ItTearOffWidgets != this->mc_TearedOffWidgets.end(); ++c_ItTearOffWidgets)
   {
      C_SyvDaTearOffWidget * const pc_CurTearOffWidget = *c_ItTearOffWidgets;
      if (pc_CurTearOffWidget != NULL)
      {
         const uint32_t u32_CurDataIndex = pc_CurTearOffWidget->GetIndex();
         if (u32_CurDataIndex >= ou32_DataIndex)
         {
            pc_CurTearOffWidget->SetDataIndex(u32_CurDataIndex - 1);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle active flag changed

   \param[in]  opc_Source  Signal source widget
   \param[in]  oq_Active   New active flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_OnActiveChange(const C_OgeWiDashboardTab * const opc_Source,
                                                         const bool oq_Active)
{
   const int32_t s32_Match = m_GetTabIndex(opc_Source);

   if (s32_Match >= 0)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      C_SyvDaDashboardWidget * const pc_Widget =
         dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(s32_Match));
      if ((pc_Widget != NULL) && (pc_View != NULL))
      {
         const uint32_t u32_DataIndex = pc_Widget->GetDashboardIndex();
         if (C_PuiSvHandler::h_GetInstance()->SetDashboardActive(this->mu32_ViewIndex, u32_DataIndex,
                                                                 oq_Active) == C_NO_ERR)
         {
            Q_EMIT this->SigChanged();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle copy action request

   \param[in]  opc_Source  Signal source widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_OnCopyAction(const C_OgeWiDashboardTab * const opc_Source) const
{
   const int32_t s32_TabIndex = this->m_GetTabIndex(opc_Source);

   C_SyvDaDashboardWidget * const pc_Widget =
      dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(static_cast<int32_t>(s32_TabIndex)));

   if (pc_Widget != NULL)
   {
      pc_Widget->Save();
      m_CopyTab(pc_Widget->GetDashboardIndex());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle cut action request

   \param[in]  opc_Source  Signal source widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_OnCutAction(const C_OgeWiDashboardTab * const opc_Source)
{
   const int32_t s32_TabIndex = this->m_GetTabIndex(opc_Source);

   C_SyvDaDashboardWidget * const pc_Widget =
      dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(static_cast<int32_t>(s32_TabIndex)));

   if (pc_Widget != NULL)
   {
      pc_Widget->Save();
      m_CopyTab(pc_Widget->GetDashboardIndex());
      m_DeleteTab(s32_TabIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle paste action request
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_OnPasteAction(void)
{
   C_PuiSvDashboard c_Dashboard;

   QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_Rails;
   QMap<C_PuiSvDbNodeDataPoolListElementId,
        C_PuiSvDbElementIdCrcGroup> c_ElementIdGroups;

   if (C_SyvClipBoardHelper::h_LoadDashboardFromClipboard(c_Dashboard, c_Rails, c_ElementIdGroups, "Tab") == C_NO_ERR)
   {
      // handle invalid data element indices and param widget duplicates
      C_SyvDaCopyPasteManager::h_AdaptCopyDataForPaste(c_Dashboard, c_Rails, c_ElementIdGroups, this->mu32_ViewIndex,
                                                       this);

      // add tab
      this->m_AddTab(c_Dashboard, &c_Rails);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle delete action request

   \param[in]  opc_Source  Signal source widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_OnDeleteAction(const C_OgeWiDashboardTab * const opc_Source)
{
   const int32_t s32_TabIndex = this->m_GetTabIndex(opc_Source);

   C_SyvDaDashboardWidget * const pc_Widget =
      dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(static_cast<int32_t>(s32_TabIndex)));

   if (pc_Widget != NULL)
   {
      m_DeleteTab(s32_TabIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle close action request

   \param[in]  opc_Source  Signal source widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_OnCloseAction(const C_OgeWiDashboardTab * const opc_Source)
{
   const int32_t s32_Match = m_GetTabIndex(opc_Source);

   if ((s32_Match >= 0) && (this->count() > 1))
   {
      this->removeTab(s32_Match);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Search for specified tab drawing widget and return index if found

   \param[in]  opc_DrawingWidget    Tab drawing widget to serach for

   \return
   -1   No matching tab found
   Else Matching tab index
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaDashboardSelectorTabWidget::m_GetTabIndex(const C_OgeWiDashboardTab * const opc_DrawingWidget) const
{
   int32_t s32_Retval = -1;

   for (int32_t s32_ItTab = 0; s32_ItTab < this->count(); ++s32_ItTab)
   {
      const C_OgeWiDashboardTab * const pc_DrawingWidget =
         dynamic_cast<const C_OgeWiDashboardTab * const>(this->tabBar()->tabButton(s32_ItTab,
                                                                                   C_SyvDaDashboardSelectorTabWidget::
                                                                                   mhe_TAB_CONTENT_POSITION));
      if ((pc_DrawingWidget != NULL) && (pc_DrawingWidget == opc_DrawingWidget))
      {
         s32_Retval = s32_ItTab;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle undock action

   \param[in]  opc_Source  Signal source widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_OnUndock(const C_OgeWiDashboardTab * const opc_Source)
{
   const int32_t s32_TabIndex = this->m_GetTabIndex(opc_Source);

   if (s32_TabIndex >= 0)
   {
      this->TearOffWidget(s32_TabIndex, this->mapFromGlobal(QCursor::pos()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Open popup to edit dashboard tab properties

   \param[in,out]  opc_Source    Source widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_OnEditProperties(C_OgeWiDashboardTab * const opc_Source)
{
   const int32_t s32_TabIndex = this->m_GetTabIndex(opc_Source);

   C_SyvDaDashboardWidget * const pc_Widget =
      dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(static_cast<int32_t>(s32_TabIndex)));

   if (pc_Widget != NULL)
   {
      QString c_Name;
      QString c_Comment;

      if (m_EditTab(pc_Widget->GetDashboardIndex(), c_Name, c_Comment) == true)
      {
         Q_EMIT this->SigChanged();
         //Apply text in GUI
         opc_Source->SetText(c_Name);
         opc_Source->SetToolTip(c_Name, c_Comment);
         //Use QT function to update tab size appropriately
         this->setTabText(s32_TabIndex, "");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show tear off widget for specified dashboard

   \param[in]      ou32_DataIndex               Dashboard data index
   \param[in]      orc_Pos                      Widget position
   \param[in,out]  opc_Widget                   Optional dashboard widget to integrate instead of creating a new one
   \param[in]      orc_Size                     Widget size
   \param[in]      oq_TornOffWindowMinimized    Window minimized flag
   \param[in]      oq_TornOffWindowMaximized    Window maximized flag
   \param[in]      oq_Load                      Flag if tear of widget was loaded or user teared it of
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_TearOffWidget(const uint32_t ou32_DataIndex, const QPoint & orc_Pos,
                                                        C_SyvDaDashboardWidget * const opc_Widget,
                                                        const QSize & orc_Size, const bool oq_TornOffWindowMinimized,
                                                        const bool oq_TornOffWindowMaximized, const bool oq_Load)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_DashBoard = pc_View->GetDashboard(ou32_DataIndex);
      if (pc_DashBoard != NULL)
      {
         QPoint c_Pos = orc_Pos;
         QSize c_Size = orc_Size;
         //Pop up
         C_SyvDaTearOffWidget * const pc_Widget = new C_SyvDaTearOffWidget(this->mu32_ViewIndex, ou32_DataIndex,
                                                                           pc_DashBoard->GetName(), opc_Widget);

         connect(pc_Widget, &C_SyvDaTearOffWidget::SigWidgetComeBack, this,
                 &C_SyvDaDashboardSelectorTabWidget::m_WidgetComeBack);
         connect(pc_Widget, &C_SyvDaTearOffWidget::SigConfirmClicked, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigConfirmClicked);
         connect(pc_Widget, &C_SyvDaTearOffWidget::SigCancelClicked, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigCancelClicked);
         connect(pc_Widget, &C_SyvDaTearOffWidget::SigChanged, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigChanged);
         connect(pc_Widget, &C_SyvDaTearOffWidget::SigDataPoolWrite, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigDataPoolWrite);
         connect(pc_Widget, &C_SyvDaTearOffWidget::SigDataPoolRead, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigDataPoolRead);
         connect(pc_Widget, &C_SyvDaTearOffWidget::SigNvmReadList, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigNvmReadList);
         connect(pc_Widget, &C_SyvDaTearOffWidget::SigTriggerUpdateTransmissionConfiguration, this,
                 &C_SyvDaDashboardSelectorTabWidget::UpdateTransmissionConfiguration);
         //Error
         connect(pc_Widget, &C_SyvDaTearOffWidget::SigErrorChange, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigErrorChange);

         pc_Widget->setWindowFlags(Qt::Window);
         if (oq_Load == true)
         {
            // Only check the coordinates when loading the tear off state. It can be possible, that the
            // monitor setup of the user has changed
            C_OgeWiUtil::h_CheckAndFixDialogPositionAndSize(c_Pos, c_Size, QSize(1000, 800));
         }
         pc_Widget->setGeometry(QRect(c_Pos, c_Size));
         pc_Widget->SetEditMode(this->mq_EditMode);
         pc_Widget->SetEnabled(!this->mq_Connected);
         pc_Widget->ConnectionActiveChanged(this->mq_Connected, true);
         pc_Widget->SetDarkMode(pc_View->GetDarkModeActive());
         //Window title
         pc_Widget->setWindowTitle("openSYDE - " + pc_DashBoard->GetName());
         if (oq_TornOffWindowMinimized == true)
         {
            pc_Widget->showMinimized();
         }
         else
         {
            if (oq_TornOffWindowMaximized == true)
            {
               pc_Widget->showMaximized();
            }
            else
            {
               pc_Widget->show();
            }
         }
         // Add the widget to the container
         this->mc_TearedOffWidgets.push_back(pc_Widget);
      } //lint !e429  //no memory leak because of the parent of pc_Widget and the Qt memory
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add tab and dashboard data

   \param[in]  orc_Data    New dashboard data
   \param[in]  opc_Rails   Read rail item configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_AddTab(const C_PuiSvDashboard & orc_Data,
                                                 const QMap<C_OscNodeDataPoolListElementId,
                                                            C_PuiSvReadDataConfiguration> * const opc_Rails)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const uint32_t u32_Index = pc_View->GetDashboards().size();
      if (C_PuiSvHandler::h_GetInstance()->InsertDashboard(this->mu32_ViewIndex, u32_Index, orc_Data, true,
                                                           opc_Rails) == C_NO_ERR)
      {
         m_AddSpecificTab(u32_Index);
         Q_EMIT this->SigChanged();
      }
   }
   //Select new one
   this->setCurrentIndex(this->count() - 1);

   //Signal change
   Q_EMIT this->SigNumberDashboardsChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete tab and content

   \param[in]  os32_TabIndex  Tab index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_DeleteTab(const int32_t os32_TabIndex)
{
   C_SyvDaDashboardWidget * const pc_Widget =
      dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(static_cast<int32_t>(os32_TabIndex)));
   const bool q_LastDashboard = (this->count() == 1);

   if (pc_Widget != NULL)
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
      C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;

      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Dashboard delete"));
      c_MessageBox.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                          "Do you really want to delete dashboard \"%1\"?")).arg(
                                     pc_Widget->GetName()));
      c_MessageBox.SetOkButtonText(C_GtGetText::h_GetText("Delete"));
      c_MessageBox.SetNoButtonText(C_GtGetText::h_GetText("Keep"));
      c_MessageBox.SetCustomMinHeight(180, 180);
      e_ReturnMessageBox = c_MessageBox.Execute();
      if (e_ReturnMessageBox == C_OgeWiCustomMessage::eYES)
      {
         const uint32_t u32_DataIndex = pc_Widget->GetDashboardIndex();
         //Default remove action
         m_Disconnect(pc_Widget);
         if (C_PuiSvHandler::h_GetInstance()->DeleteDashboard(this->mu32_ViewIndex, u32_DataIndex) == C_NO_ERR)
         {
            this->removeTab(os32_TabIndex);
            this->m_SyncDashboardDeleteDataIndex(u32_DataIndex);

            //Handle last dashboard
            if (q_LastDashboard)
            {
               const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
               if (pc_View != NULL)
               {
                  //Use free index (there might be other dashboards!)
                  const uint32_t u32_FreeIndex = pc_View->GetDashboards().size();
                  //Insert one after the last one (least conflict potential)
                  C_PuiSvDashboard c_DefaultDashboard;
                  c_DefaultDashboard.SetName(C_GtGetText::h_GetText("Dashboard"));
                  if (C_PuiSvHandler::h_GetInstance()->InsertDashboard(this->mu32_ViewIndex, u32_FreeIndex,
                                                                       c_DefaultDashboard,
                                                                       true, NULL) == C_NO_ERR)
                  {
                     m_AddSpecificTab(u32_FreeIndex);
                  }
               }
               //Select new one
               this->setCurrentIndex(0L);
               //Signal might not be sent
               this->m_CurrentChanged(0);
            }

            Q_EMIT this->SigChanged();
            Q_EMIT (this->SigErrorChange());
            //Signal change
            Q_EMIT this->SigNumberDashboardsChanged();
         }
         else
         {
            tgl_assert(false);
         }
      }
   }
   else
   {
      C_OgeWiCustomMessage c_MessageBox(this);

      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Dashboard delete"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("You can't delete the last dashboard."));
      c_MessageBox.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy tab to clip board

   \param[in]  ou32_DataIndex    Dashboard data index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_CopyTab(const uint32_t ou32_DataIndex) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(ou32_DataIndex);
      if (pc_Dashboard != NULL)
      {
         QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> c_Rails;
         pc_View->GetRelevantReadRailAssigmentsForDashboard(*pc_Dashboard, c_Rails);
         C_SyvClipBoardHelper::h_StoreDashboardToClipboard(*pc_Dashboard, c_Rails, "Tab");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Edit tab properties (name and comment)

   \param[in]      oru32_DataIndex  Dashboard data index
   \param[in,out]  orc_Name         Updated name
   \param[in,out]  orc_Comment      Updated comment

   \return
   true  success
   false failure/abort
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardSelectorTabWidget::m_EditTab(const uint32_t & oru32_DataIndex, QString & orc_Name,
                                                  QString & orc_Comment)
{
   bool q_Return = false;

   // show popup dialog
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_SyvDaDashboardTabProperties * const pc_Dialog = new C_SyvDaDashboardTabProperties(*c_New, "Dashboard Tab",
                                                                                       oru32_DataIndex,
                                                                                       this->mu32_ViewIndex);

   Q_UNUSED(pc_Dialog)

   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      if ((C_PuiSvHandler::h_GetInstance()->SetDashboardName(
              this->mu32_ViewIndex, oru32_DataIndex, pc_Dialog->GetDashboardTabName()) == C_NO_ERR) &&
          (C_PuiSvHandler::h_GetInstance()->SetDashboardComment(
              this->mu32_ViewIndex, oru32_DataIndex, pc_Dialog->GetDashboardTabComment()) == C_NO_ERR))
      {
         orc_Name = pc_Dialog->GetDashboardTabName();
         orc_Comment = pc_Dialog->GetDashboardTabComment();
         q_Return = true;
      }
   }

   if (c_New != NULL)
   {
      c_New->HideOverlay();
      c_New->deleteLater();
   }
   return q_Return; //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Store tab user settings information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_StoreUserSettings(void)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      //Save torn off widgets
      for (QList<C_SyvDaTearOffWidget *>::const_iterator c_ItItem = this->mc_TearedOffWidgets.begin();
           c_ItItem != this->mc_TearedOffWidgets.end(); ++c_ItItem)
      {
         C_SyvDaTearOffWidget * const pc_Widget = *c_ItItem;
         if (pc_Widget != NULL)
         {
            const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(pc_Widget->GetIndex());
            if (pc_Dashboard != NULL)
            {
               C_UsHandler::h_GetInstance()->SetProjSvDashboardTearOffPosition(pc_View->GetName(),
                                                                               pc_Dashboard->GetName(),
                                                                               pc_Widget->geometry().topLeft(),
                                                                               pc_Widget->geometry().size(),
                                                                               pc_Widget->isMinimized(),
                                                                               pc_Widget->isMaximized());
            }
            //Manually hide
            pc_Widget->hide();
         }
      }
      //Save tabs
      for (int32_t s32_Counter = 0; s32_Counter < this->count(); ++s32_Counter)
      {
         C_SyvDaDashboardWidget * const pc_Widget =
            dynamic_cast<C_SyvDaDashboardWidget * const>(this->widget(s32_Counter));

         if (pc_Widget != NULL)
         {
            const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(pc_Widget->GetDashboardIndex());
            if (pc_Dashboard != NULL)
            {
               //User settings
               C_UsHandler::h_GetInstance()->SetProjSvDashboardMainTab(pc_View->GetName(),
                                                                       pc_Dashboard->GetName());
               //Data
               C_PuiSvHandler::h_GetInstance()->SetDashboardTabIndex(this->mu32_ViewIndex,
                                                                     pc_Widget->GetDashboardIndex(), s32_Counter);
            }
         }
      }
      //Selection
      C_UsHandler::h_GetInstance()->SetProjSvDashboardSelectedTabIndex(pc_View->GetName(), this->currentIndex());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle dashboard connections

   \param[in,out]  opc_Widget    Dashboard widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_Connect(const C_SyvDaDashboardWidget * const opc_Widget) const
{
   if (opc_Widget != NULL)
   {
      connect(opc_Widget, &C_SyvDaDashboardWidget::SigErrorChange, this,
              &C_SyvDaDashboardSelectorTabWidget::SigErrorChange);
      connect(opc_Widget, &C_SyvDaDashboardWidget::SigTriggerUpdateTransmissionConfiguration, this,
              &C_SyvDaDashboardSelectorTabWidget::UpdateTransmissionConfiguration);
      connect(opc_Widget, &C_SyvDaDashboardWidget::SigDataPoolWrite, this,
              &C_SyvDaDashboardSelectorTabWidget::SigDataPoolWrite);
      connect(opc_Widget, &C_SyvDaDashboardWidget::SigDataPoolRead, this,
              &C_SyvDaDashboardSelectorTabWidget::SigDataPoolRead);
      connect(opc_Widget, &C_SyvDaDashboardWidget::SigNvmReadList, this,
              &C_SyvDaDashboardSelectorTabWidget::SigNvmReadList);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle dashboard disconnections

   \param[in,out]  opc_Widget    Dashboard widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_Disconnect(const C_SyvDaDashboardWidget * const opc_Widget) const
{
   if (opc_Widget != NULL)
   {
      disconnect(opc_Widget, &C_SyvDaDashboardWidget::SigErrorChange, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigErrorChange);
      disconnect(opc_Widget, &C_SyvDaDashboardWidget::SigTriggerUpdateTransmissionConfiguration, this,
                 &C_SyvDaDashboardSelectorTabWidget::UpdateTransmissionConfiguration);
      disconnect(opc_Widget, &C_SyvDaDashboardWidget::SigDataPoolWrite, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigDataPoolWrite);
      disconnect(opc_Widget, &C_SyvDaDashboardWidget::SigDataPoolRead, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigDataPoolRead);
      disconnect(opc_Widget, &C_SyvDaDashboardWidget::SigNvmReadList, this,
                 &C_SyvDaDashboardSelectorTabWidget::SigNvmReadList);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle tab change

   \param[in]  os32_Index   New selected tab index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardSelectorTabWidget::m_OnTabChanged(const int32_t os32_Index) const
{
   for (int32_t s32_Counter = 0; s32_Counter < this->count(); ++s32_Counter)
   {
      const C_SyvDaDashboardWidget * const pc_Widget =
         dynamic_cast<const C_SyvDaDashboardWidget * const>(this->widget(s32_Counter));

      if (pc_Widget != NULL)
      {
         pc_Widget->SetDrawingActive(os32_Index == s32_Counter);
      }
   }
}
