//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboards

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SyvDaDashboardsWidget.hpp"
#include "ui_C_SyvDaDashboardsWidget.h"

#include "C_Uti.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "TglUtils.hpp"
#include "TglTime.hpp"
#include "C_SyvUtil.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_UsHandler.hpp"
#include "C_SyvDaDashboardToolbox.hpp"
#include "C_SyvDaPeUpdateModeConfiguration.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvDaDashboardsWidget::mhc_DARK_MODE_ENABLED_ICON_PATH = "://images/system_views/Darkmode_Disable.svg";
const QString C_SyvDaDashboardsWidget::mhc_DARK_MODE_DISABLED_ICON_PATH = "://images/system_views/Darkmode_Enable.svg";
const int32_t C_SyvDaDashboardsWidget::mhs32_WIDGET_BORDER = 11;
const int32_t C_SyvDaDashboardsWidget::mhs32_TOOLBOX_INIT_POS_Y = 150;
uint32_t C_SyvDaDashboardsWidget::mhu32_DisconnectTime = 0UL;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      ou32_ViewIndex      View index
   \param[in]      opc_ToolboxParent   Optional pointer to toolbox parent
   \param[in,out]  opc_Parent          Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaDashboardsWidget::C_SyvDaDashboardsWidget(const uint32_t ou32_ViewIndex, QWidget * const opc_ToolboxParent,
                                                 QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaDashboardsWidget),
   mpc_ComDriver(NULL),
   mpc_ConnectionThread(new C_SyvComDriverDiagConnect(this)),
   mpc_Toolbox(NULL),
   mpc_FixMinimizedToolbox(NULL),
   mpc_ToolboxParent(opc_ToolboxParent),
   mu32_ViewIndex(ou32_ViewIndex),
   mq_EditModeActive(false),
   mq_DarkModeActive(false),
   mq_ConnectActive(false),
   me_ConnectState(eCS_DISCONNECTED),
   ms32_InitToolboxCounter(0)
{
   const bool q_ServiceModeActive = C_PuiSvHandler::h_GetInstance()->GetServiceModeActive();

   mpc_Ui->setupUi(this);

   this->InitText();

   // Style
   this->mpc_Ui->pc_WidgetWhite->SetBackgroundColor(0);

   //Style error label
   this->mpc_Ui->pc_ErrorLabelTitle->SetForegroundColor(24);
   this->mpc_Ui->pc_ErrorLabelTitle->SetFontPixel(14, false, true);
   this->mpc_Ui->pc_ErrorLabelIcon->SetSvg("://images/Error_iconV2.svg");

   //Remove debug label
   this->mpc_Ui->pc_GroupBoxButtons->setTitle("");

   //Init view index
   this->mpc_Ui->pc_TabWidget->SetViewIndex(this->mu32_ViewIndex);

   //Hide cancel button
   this->mpc_Ui->pc_PbCancel->setVisible(false);

   // Configure timer
   this->mc_Timer.setInterval(ms32_TIMER_GUI_REFRESH);
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SyvDaDashboardsWidget::m_UpdateShowValues);

   // Handle the service mode
   this->mpc_Ui->pc_PbConfirm->setEnabled(!q_ServiceModeActive);

   // Connect buttons
   connect(this->mpc_Ui->pc_PbConfirm, &stw::opensyde_gui_elements::C_OgePubSystemCommissioningEdit::clicked,
           this, &C_SyvDaDashboardsWidget::m_ConfirmClicked);
   connect(this->mpc_Ui->pc_PbCancel, &stw::opensyde_gui_elements::C_OgePubSystemCommissioningEdit::clicked,
           this, &C_SyvDaDashboardsWidget::m_CancelClicked);

   connect(this->mpc_Ui->pc_TabWidget, &C_SyvDaDashboardSelectorTabWidget::SigConfirmClicked,
           this, &C_SyvDaDashboardsWidget::m_ConfirmClicked);
   connect(this->mpc_Ui->pc_TabWidget, &C_SyvDaDashboardSelectorTabWidget::SigCancelClicked,
           this, &C_SyvDaDashboardsWidget::m_CancelClicked);
   connect(this->mpc_Ui->pc_TabWidget, &C_SyvDaDashboardSelectorTabWidget::SigChanged,
           this, &C_SyvDaDashboardsWidget::SigChanged);
   connect(this->mpc_Ui->pc_TabWidget, &C_SyvDaDashboardSelectorTabWidget::SigNumberDashboardsChanged,
           this, &C_SyvDaDashboardsWidget::SigNumberDashboardsChanged);
   connect(this->mpc_Ui->pc_TabWidget, &C_SyvDaDashboardSelectorTabWidget::SigErrorChange,
           this, &C_SyvDaDashboardsWidget::CheckError);
   connect(this->mpc_Ui->pc_TabWidget, &C_SyvDaDashboardSelectorTabWidget::SigDataPoolWrite,
           this, &C_SyvDaDashboardsWidget::m_DataPoolWrite);
   connect(this->mpc_Ui->pc_TabWidget, &C_SyvDaDashboardSelectorTabWidget::SigDataPoolRead,
           this, &C_SyvDaDashboardsWidget::m_DataPoolRead);
   connect(this->mpc_Ui->pc_TabWidget, &C_SyvDaDashboardSelectorTabWidget::SigNvmReadList,
           this, &C_SyvDaDashboardsWidget::m_NvmReadList);

   connect(this->mpc_ConnectionThread, &C_SyvComDriverDiagConnect::finished, this,
           &C_SyvDaDashboardsWidget::m_ConnectStepFinished);

   this->SetEditMode(false);

   // create toolbox
   this->mpc_ToolboxContent = new C_SyvDaDashboardToolbox();
   this->mpc_Toolbox = new C_OgeWiHover(*mpc_ToolboxContent,
                                        C_GtGetText::h_GetText("TOOLBOX"), ":images/IconToolbox.svg",
                                        false, this->mpc_ToolboxParent, this->mpc_ToolboxParent);

   // create fix minimized toolbox
   this->mpc_FixMinimizedToolbox = new C_OgeWiFixPosition(C_GtGetText::h_GetText("TOOLBOX"),
                                                          ":images/IconToolbox.svg",
                                                          QRect(1277, 14, 190, 36), this->mpc_ToolboxParent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak of mpc_ComDriver because of calling m_CloseOsyDriver here and for mpc_Toolbox the Qt
// memory management
C_SyvDaDashboardsWidget::~C_SyvDaDashboardsWidget(void)
{
   this->m_CloseOsyDriver();

   if (this->mpc_Toolbox != NULL)
   {
      this->mpc_Toolbox->hide();
   }

   if (this->mpc_FixMinimizedToolbox != NULL)
   {
      this->mpc_FixMinimizedToolbox->hide();
   }

   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::InitText(void) const
{
   this->mpc_Ui->pc_PbCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle initial dark mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::LoadDarkMode(void)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      if (pc_View->GetDarkModeActive() == true)
      {
         Q_EMIT (this->SigSetDarkModePushButtonIcon(C_SyvDaDashboardsWidget::mhc_DARK_MODE_ENABLED_ICON_PATH));
         m_ApplyDarkMode(true);
      }
      else
      {
         Q_EMIT (this->SigSetDarkModePushButtonIcon(C_SyvDaDashboardsWidget::mhc_DARK_MODE_DISABLED_ICON_PATH));
         m_ApplyDarkMode(false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::Save(void) const
{
   m_CleanUpToolBox();
   this->mpc_Ui->pc_TabWidget->Save();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle dashboards settings request
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::OpenSettings(void)
{
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_SyvDaPeUpdateModeConfiguration * const pc_Dialog = new C_SyvDaPeUpdateModeConfiguration(*c_New,
                                                                                             this->mu32_ViewIndex);

   Q_UNUSED(pc_Dialog)

   //Resize
   c_New->SetSize(QSize(1400, 809));

   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      //Signal widget update
      this->mpc_Ui->pc_TabWidget->UpdateTransmissionConfiguration();
      //Recheck error
      this->CheckError();
   }

   if (c_New != NULL)
   {
      c_New->HideOverlay();
      c_New->deleteLater();
   }
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle dark mode toggle
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::ToggleDarkMode(void)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      if (pc_View->GetDarkModeActive() == true)
      {
         C_PuiSvHandler::h_GetInstance()->SetViewDarkModeActive(this->mu32_ViewIndex, false);
      }
      else
      {
         C_PuiSvHandler::h_GetInstance()->SetViewDarkModeActive(this->mu32_ViewIndex, true);
      }
   }
   this->LoadDarkMode();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the edit mode

   \param[in]  oq_Active   Flag for edit mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::SetEditMode(const bool oq_Active)
{
   this->mq_EditModeActive = oq_Active;

   this->mpc_Ui->pc_BackgroundWidget->SetEditBackground(oq_Active);

   this->mpc_Ui->pc_TabWidget->SetEditMode(oq_Active);

   if (oq_Active == true)
   {
      //Handle button
      this->mpc_Ui->pc_PbConfirm->SetSvg("://images/system_views/IconConfirm.svg");
      this->mpc_Ui->pc_PbConfirm->SetMargins(10, 20);
      this->mpc_Ui->pc_PbConfirm->setText(C_GtGetText::h_GetText("Confirm"));

      //Initially create toolbox
      this->m_InitToolBox();

      //Show toolbox
      if ((this->mpc_Toolbox != NULL) &&
          (C_UsHandler::h_GetInstance()->GetProjSvSetupView(
              C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex)->GetName().c_str())
           .GetDashboardToolboxMaximized() != false))
      {
         this->mpc_Toolbox->show();
      }
      else
      {
         this->mpc_FixMinimizedToolbox->show();
      }

      // create copy of dashboard in case of clicking cancel
   }
   else
   {
      //Handle button
      this->mpc_Ui->pc_PbConfirm->SetSvg("://images/main_page_and_navi_bar/IconEdit.svg",
                                         "://images/IconEditDisabledBright.svg");
      this->mpc_Ui->pc_PbConfirm->SetMargins(10, 28); // put a bit more space in between
      this->mpc_Ui->pc_PbConfirm->setText(C_GtGetText::h_GetText("Edit"));

      //Hide toolbox
      if (this->mpc_Toolbox != NULL)
      {
         this->mpc_Toolbox->hide();
      }

      // Hide minimized toolbox
      if (this->mpc_FixMinimizedToolbox != NULL)
      {
         this->mpc_FixMinimizedToolbox->hide();
      }

      //Also should remember the toolbox settings
      this->Save();
   }

   // style
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_GroupBox, "Edit", oq_Active);
   this->m_AdaptSpaceHolderWidgetColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the state of the edit mode

   \return
   true     edit mode is active
   false    edit mode is not active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardsWidget::GetEditMode(void) const
{
   return this->mq_EditModeActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get connected flag

   \return
   Current connected flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardsWidget::GetConnectActive(void) const
{
   return mq_ConnectActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set connected flag

   \param[in]  oq_Value    New connected flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::SetConnectActive(const bool oq_Value)
{
   this->mq_ConnectActive = oq_Value;

   QApplication::setOverrideCursor(Qt::WaitCursor);

   if ((this->mq_ConnectActive == true) &&
       (this->mpc_ComDriver == NULL))
   {
      this->mpc_ConnectionThread->SetWaitingStepParameters(C_SyvDaDashboardsWidget::mhu32_DisconnectTime);
      this->mpc_ConnectionThread->start();
   }
   else
   {
      this->mc_Timer.stop();
      // Inform all widgets about the connection
      this->mpc_Ui->pc_TabWidget->ConnectionActiveChanged(false);

      this->m_CloseOsyDriver();

      Q_EMIT (this->SigBlockDragAndDrop(false));

      // Save the time of disconnect
      C_SyvDaDashboardsWidget::mhu32_DisconnectTime = TglGetTickCount();
   }

   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to prepare closing the widget

   \return
   true     Can be closed.
   false    Cannot be closed.
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardsWidget::PrepareToClose(void) const
{
   const bool q_Return = (this->me_ConnectState != eCS_CONNECTING);

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Perform error check
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::CheckError(void) const
{
   QString c_ErrorText;
   QString c_ErrorTextHeading;
   QString c_ErrorTextTooltip;

   C_NagToolTip::E_Type e_ToolTipType;
   QString c_IconPath;
   int32_t s32_ColorId;
   const bool q_ViewSetupError = C_SyvUtil::h_GetViewStatusLabelInfo(
      this->mu32_ViewIndex, ms32_SUBMODE_SYSVIEW_DASHBOARD, c_ErrorTextHeading, c_ErrorText, c_ErrorTextTooltip,
      e_ToolTipType,
      c_IconPath, s32_ColorId);

   if (q_ViewSetupError == true)
   {
      this->mpc_Ui->pc_ErrorLabelIcon->SetSvg(c_IconPath);
      this->mpc_Ui->pc_ErrorLabelIcon->SetToolTipInformation(C_GtGetText::h_GetText("Invalid"),
                                                             c_ErrorTextTooltip, e_ToolTipType);
      this->mpc_Ui->pc_ErrorLabelTitle->SetForegroundColor(s32_ColorId);
      this->mpc_Ui->pc_ErrorLabelTitle->setText(c_ErrorTextHeading);
      this->mpc_Ui->pc_ErrorLabelTitle->SetToolTipInformation(C_GtGetText::h_GetText("Invalid"),
                                                              c_ErrorTextTooltip, e_ToolTipType);
      this->mpc_Ui->pc_ErrorLabel->SetForegroundColor(s32_ColorId);
      this->mpc_Ui->pc_ErrorLabel->SetCompleteText(c_ErrorText, c_ErrorTextTooltip, e_ToolTipType);
      this->mpc_Ui->pc_GroupBoxErrorContent->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxErrorContent->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle push button connect press
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::OnPushButtonConnectPress(void)
{
   switch (this->me_ConnectState)
   {
   case eCS_DISCONNECTED:
      //Connecting
      this->me_ConnectState = eCS_CONNECTING;
      Q_EMIT (this->SigSetConnectPushButtonIcon("://images/system_views/IconConnecting.svg", true));
      //Leave edit mode
      if (this->GetEditMode() == true)
      {
         this->SetEditMode(false);
      }
      //Deactivate edit & config
      this->mpc_Ui->pc_TabWidget->SetEnabled(false);
      this->mpc_Ui->pc_PbConfirm->setEnabled(false);
      Q_EMIT (this->SigSetConfigurationAvailable(false));
      //While connecting display updated tool bar
      QApplication::processEvents();
      this->SetConnectActive(true);
      break;
   case eCS_CONNECTED:
      //Disconnecting
      this->me_ConnectState = eCS_DISCONNECTING;
      Q_EMIT (this->SigSetConnectPushButtonIcon("://images/system_views/IconDisconnecting.svg", true));
      this->SetConnectActive(false);
      this->me_ConnectState = eCS_DISCONNECTED;
      Q_EMIT (this->SigSetConnectPushButtonIcon("://images/system_views/IconDisconnected.svg", false));
      //Reactivate edit & config
      this->mpc_Ui->pc_TabWidget->SetEnabled(true);
      if (C_PuiSvHandler::h_GetInstance()->GetServiceModeActive() == false)
      {
         this->mpc_Ui->pc_PbConfirm->setEnabled(true);
      }
      Q_EMIT (this->SigSetConfigurationAvailable(true));
      break;
   case eCS_CONNECTING:
   case eCS_DISCONNECTING:
      //No action can be started
      break;
   default:
      // Nothing to do
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten hide event slot

   Here: hide and delete toolbox

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::hideEvent(QHideEvent * const opc_Event)
{
   QWidget::hideEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event

   Move the toolbox.

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   if ((this->mpc_Toolbox != NULL) && (this->mpc_FixMinimizedToolbox != NULL))
   {
      QPoint c_Point = this->mpc_Toolbox->pos();
      QSize c_Size = this->mpc_Toolbox->size();

      QPoint c_PointFixMiniToolbox = this->mpc_FixMinimizedToolbox->pos();

      QWidget * pc_Widget = this->mpc_ToolboxParent;

      Q_UNUSED(opc_Event)

      if (pc_Widget == NULL)
      {
         // if no parent exist use this widget
         pc_Widget = this;
      }

      // would the toolbox be outside of the widget in x direction
      if ((this->mpc_Toolbox->x() + this->mpc_Toolbox->width() + mhs32_WIDGET_BORDER) > pc_Widget->width())
      {
         // is the toolbox to big?
         if ((this->mpc_Toolbox->width() + (2 * mhs32_WIDGET_BORDER)) > pc_Widget->width())
         {
            c_Size.setWidth(pc_Widget->width() - (2 * mhs32_WIDGET_BORDER));
         }
         else
         {
            // adapt position of toolbox
            c_Point.setX((pc_Widget->width() - this->mpc_Toolbox->width()) - mhs32_WIDGET_BORDER);
         }
      }

      // would the toolbox be outside of the widget in y direction
      if ((this->mpc_Toolbox->y() + this->mpc_Toolbox->height() + mhs32_WIDGET_BORDER) > pc_Widget->height())
      {
         // is the toolbox to big?
         if ((this->mpc_Toolbox->height() + (2 * mhs32_WIDGET_BORDER)) > pc_Widget->height())
         {
            c_Size.setHeight(pc_Widget->height() - (2 * mhs32_WIDGET_BORDER));
         }
         else
         {
            // adapt position of toolbox
            c_Point.setY((pc_Widget->height() - this->mpc_Toolbox->height()) - mhs32_WIDGET_BORDER);
         }
      }

      // adapt position of fix minimized toolbox
      // The operator '-' have to follow by the same operator, otherwise the geometry is wrong
      //lint -save -e834
      c_PointFixMiniToolbox.setX((pc_Widget->width() - this->mpc_FixMinimizedToolbox->width()) -
                                 170 - mhs32_WIDGET_BORDER);
      //lint -restore

      this->mpc_Toolbox->setGeometry(QRect(c_Point, c_Size));
      this->mpc_FixMinimizedToolbox->setGeometry(QRect(c_PointFixMiniToolbox, this->mpc_FixMinimizedToolbox->size()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_ConfirmClicked(void)
{
   this->SetEditMode(!this->GetEditMode());
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_CancelClicked(void)
{
   // restore the saved dashboard
   // TODO

   this->SetEditMode(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply dark mode setting

   \param[in]  oq_Active   Dark mode value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_ApplyDarkMode(const bool oq_Active)
{
   this->mq_DarkModeActive = oq_Active;
   this->mpc_Ui->pc_TabWidget->ApplyDarkMode(oq_Active);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_GroupBox, "DarkMode", oq_Active);
   this->m_AdaptSpaceHolderWidgetColor();

   if (this->mpc_Toolbox != NULL)
   {
      this->mpc_Toolbox->ApplyDarkMode(oq_Active);
      if (this->mpc_ToolboxContent != NULL)
      {
         this->mpc_ToolboxContent->ApplyDarkMode(oq_Active);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init toolbox widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_InitToolBox(void)
{
   QString c_ViewName = "";

   C_UsSystemView c_ViewSettings;
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      c_ViewName = pc_View->GetName().c_str();
   }
   c_ViewSettings = C_UsHandler::h_GetInstance()->GetProjSvSetupView(c_ViewName);

   if (this->mpc_Toolbox != NULL)
   {
      //Dark mode
      if (pc_View != NULL)
      {
         this->mpc_Toolbox->ApplyDarkMode(pc_View->GetDarkModeActive());
         if (this->mpc_ToolboxContent != NULL)
         {
            this->mpc_ToolboxContent->ApplyDarkMode(pc_View->GetDarkModeActive());
         }
      }
      else
      {
         this->mpc_Toolbox->ApplyDarkMode(false);
         if (this->mpc_ToolboxContent != NULL)
         {
            this->mpc_ToolboxContent->ApplyDarkMode(false);
         }
      }

      // check for saved default values for toolbox
      if (c_ViewSettings.GetDashboardToolboxPos().x() < 0)
      {
         if (this->mpc_ToolboxParent == NULL)
         {
            // default value in this error case
            this->mpc_Toolbox->move(mhs32_WIDGET_BORDER, mhs32_TOOLBOX_INIT_POS_Y);
         }
         else
         {
            // default value
            this->mpc_Toolbox->setGeometry(QRect(QPoint((((this->mpc_ToolboxParent->width() -
                                                           this->mpc_Toolbox->width()) -
                                                          mhs32_WIDGET_BORDER) - static_cast<int32_t> (150)),
                                                        mhs32_TOOLBOX_INIT_POS_Y + static_cast<int32_t> (50)),
                                                 QSize(449, 429)));

            this->mpc_Toolbox->SetMaximizedHeight(429);
         }
      }
      else
      {
         // restore last position and size
         this->mpc_Toolbox->setGeometry(QRect(c_ViewSettings.GetDashboardToolboxPos(),
                                              c_ViewSettings.GetDashboardToolboxSize()));
         this->mpc_Toolbox->SetMaximizedHeight(c_ViewSettings.GetDashboardToolboxSize().height());
      }
   }

   if (c_ViewSettings.GetDashboardToolboxMaximized() == false)
   {
      this->m_WiHoverMinBtnClicked();
   }

   // Hide toolbox
   if (this->mpc_Toolbox != NULL)
   {
      this->mpc_Toolbox->hide();
   }

   // Hide minimizied toolbox
   if (this->mpc_FixMinimizedToolbox != NULL)
   {
      this->mpc_FixMinimizedToolbox->hide();
   }

   connect(this->mpc_Toolbox, &C_OgeWiHover::SigWiHoverMinBtnClicked,
           this, &C_SyvDaDashboardsWidget::m_WiHoverMinBtnClicked);
   connect(this->mpc_FixMinimizedToolbox, &C_OgeWiFixPosition::SigWiFixPosMaxBtnClicked,
           this, &C_SyvDaDashboardsWidget::m_WiFixPosMaxBtnClicked);

   this->ms32_InitToolboxCounter = this->ms32_InitToolboxCounter + 1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up toolbox
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_CleanUpToolBox(void) const
{
   if ((this->mpc_Toolbox != NULL) && (this->ms32_InitToolboxCounter > 0))
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
      if (pc_View != NULL)
      {
         QSize c_Size;
         // save position, size and state of toolbox
         c_Size.setWidth(this->mpc_Toolbox->width());
         c_Size.setHeight(this->mpc_Toolbox->GetMaximizedHeight());
         C_UsHandler::h_GetInstance()->SetProjSvDashboardToolbox(pc_View->GetName().c_str(),
                                                                 this->mpc_Toolbox->pos(), c_Size,
                                                                 this->mpc_Toolbox->GetMaximized());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_UpdateShowValues(void) const
{
   this->mpc_Ui->pc_TabWidget->UpdateShowValues();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize C_SyvComDriverDiag

   \param[in,out]  orc_Message   Message

   \return
   C_NO_ERR      Operation success
   C_NOACT       No active nodes
   C_CONFIG      Invalid system definition/view configuration or mpc_ComDriver is NULL
   C_RD_WR       Configured communication DLL does not exist
   C_OVERFLOW    Unknown transport protocol or unknown diagnostic server for at least one node
   C_BUSY        System view error detected
   C_COM         CAN initialization failed
   C_CHECKSUM    Internal buffer overflow detected
   C_RD_WR       Active bus index invalid
   C_RANGE       Routing configuration failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvDaDashboardsWidget::m_InitOsyDriver(QString & orc_Message)
{
   int32_t s32_Retval;
   bool q_NameInvalid;
   bool q_PcNotConnected;
   bool q_RoutingInvalid;
   bool q_SysDefInvalid;
   bool q_NoNodesActive;

   if (this->mpc_ComDriver != NULL)
   {
      s32_Retval = this->mpc_ComDriver->InitDiag();
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   switch (s32_Retval)
   {
   case C_NO_ERR:
   case C_NOACT: // C_NOACT means that no node is active, but the CAN signal interpretation is although necessary
      // Starting logging starts the CAN signal interpretation too
      // TODO: Bitrate is not necessary for initialization here and the bus load calculation is not used here.
      //       If this is necessary, this bitrate must be adapted for the real value and not this dummy
      if (this->mpc_ComDriver != NULL)
      {
         this->mpc_ComDriver->StartLogging(0U);
      }
      break;
   case C_CONFIG:
      orc_Message =
         static_cast<QString>(C_GtGetText::h_GetText("System connect failed. Configuration error.")); //Many
                                                                                                      // possibilities
                                                                                                      // for this
      // error
      break;
   case C_RD_WR:
      orc_Message =
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "Configured communication DLL does not exist or DLL could not be opened."));
      break;
   case C_OVERFLOW:
      orc_Message =
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "Unknown transport protocol or unknown diagnostic server for at least one node."));
      break;
   case C_BUSY:
      if (C_PuiSvHandler::h_GetInstance()->CheckViewError(this->mu32_ViewIndex, &q_NameInvalid,
                                                          &q_PcNotConnected, &q_RoutingInvalid,
                                                          NULL, NULL,
                                                          &q_SysDefInvalid, &q_NoNodesActive, NULL, NULL) == C_NO_ERR)
      {
         if ((q_NameInvalid == false) && (q_PcNotConnected == false) && (q_RoutingInvalid == false) &&
             (q_SysDefInvalid == false) && (q_NoNodesActive == false))
         {
            orc_Message =
               static_cast<QString>(C_GtGetText::h_GetText("System View Dashboard configuration error detected. "
                                                           "Check your Dashboard configuration settings and retry."));
         }
         else
         {
            orc_Message =
               static_cast<QString>(C_GtGetText::h_GetText(
                                       "System View is invalid. Action cannot be performed. Fix the issues and retry."));
         }
      }
      else
      {
         orc_Message = static_cast<QString>(C_GtGetText::h_GetText("System View check failed."));
      }
      break;
   case C_COM:
      orc_Message =
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "CAN initialization failed. Check your PC CAN interface configuration (System View setup - "
                                 "double-click on PC)."));
      break;
   case C_CHECKSUM:
      orc_Message = static_cast<QString>(C_GtGetText::h_GetText("Internal buffer overflow detected."));
      break;
   case C_RANGE:
      orc_Message = static_cast<QString>(C_GtGetText::h_GetText("Routing configuration failed."));
      break;
   case C_UNKNOWN_ERR:
      orc_Message = static_cast<QString>(C_GtGetText::h_GetText("Wrapped error of internal function call."));
      break;
   default:
      orc_Message =
         static_cast<QString>(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(s32_Retval));
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_CloseOsyDriver(void)
{
   if (this->mpc_ComDriver != NULL)
   {
      // Stop further cycles
      this->mpc_ComDriver->StopCycling();

      // Send a last time the tester present to make sure the close function can send all necessary stop messages
      this->mpc_ComDriver->SendTesterPresentToActiveNodes();

      this->mpc_ComDriver->StopDiagnosisServer();

      delete this->mpc_ComDriver;
      this->mpc_ComDriver = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_DataPoolWrite(const uint32_t ou32_NodeIndex, const uint8_t ou8_DataPoolIndex,
                                              const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex)
{
   if (this->mpc_ComDriver != NULL)
   {
      const int32_t s32_Return = this->mpc_ComDriver->PollDataPoolWrite(ou32_NodeIndex, ou8_DataPoolIndex,
                                                                        ou16_ListIndex, ou16_ElementIndex);
      //Error handling
      if (s32_Return == C_BUSY)
      {
         const C_OscNodeDataPoolListElementId c_Id(ou32_NodeIndex, static_cast<uint32_t>(ou8_DataPoolIndex),
                                                   static_cast<uint32_t>(ou16_ListIndex),
                                                   static_cast<uint32_t>(ou16_ElementIndex));
         this->mc_MissedWriteOperations.insert(c_Id);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read a Datapool element from server

   \param[in]       orc_Index             Detailed input parameter description
   \param[in]       opc_DashboardWidget   Optional pointer to dashboard widget data element
                                             Valid pointer: read value only for this widget with this Datapool element
                                             NULL pointer:  read value for all widgets with this Datapool element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_DataPoolRead(const C_OscNodeDataPoolListElementId & orc_Index,
                                             stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DashboardWidget)
{
   if (this->mpc_ComDriver != NULL)
   {
      const int32_t s32_Return =
         this->mpc_ComDriver->PollDataPoolRead(orc_Index.u32_NodeIndex,
                                               static_cast<uint8_t>(orc_Index.u32_DataPoolIndex),
                                               static_cast<uint16_t>(orc_Index.u32_ListIndex),
                                               static_cast<uint16_t>(orc_Index.u32_ElementIndex),
                                               opc_DashboardWidget);
      //Error handling
      if (s32_Return == C_BUSY)
      {
         const C_MissedReadOperation c_MissedOperation(orc_Index, opc_DashboardWidget);
         this->mc_MissedReadOperations.insert(c_MissedOperation);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Attempt to start read NVM list operation

   \param[in]  orc_Index   List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_NvmReadList(const C_OscNodeDataPoolListId & orc_Index)
{
   if (this->mpc_ComDriver != NULL)
   {
      const int32_t s32_Return =
         this->mpc_ComDriver->PollNvmReadList(orc_Index.u32_NodeIndex,
                                              static_cast<uint8_t>(orc_Index.u32_DataPoolIndex),
                                              static_cast<uint16_t>(orc_Index.u32_ListIndex));
      //Error handling
      if (s32_Return == C_BUSY)
      {
         this->mc_MissedReadNvmOperations.insert(orc_Index);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle manual user operation finished event

   \param[in]  os32_Result    Operation result
   \param[in]  ou8_Nrc        Negative response code, if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc)
{
   this->mpc_Ui->pc_TabWidget->HandleManualOperationFinished(os32_Result, ou8_Nrc);
   if (this->mpc_ComDriver != NULL)
   {
      if (this->mc_MissedReadNvmOperations.size() > 0)
      {
         const C_OscNodeDataPoolListId c_Id = *this->mc_MissedReadNvmOperations.begin();
         const int32_t s32_Return =
            this->mpc_ComDriver->PollNvmReadList(c_Id.u32_NodeIndex, static_cast<uint8_t>(c_Id.u32_DataPoolIndex),
                                                 static_cast<uint16_t>(c_Id.u32_ListIndex));
         //Error handling
         if (s32_Return != C_BUSY)
         {
            this->mc_MissedReadNvmOperations.erase(this->mc_MissedReadNvmOperations.begin());
         }
      }
      else
      {
         if (this->mc_MissedWriteOperations.size() > 0)
         {
            const C_OscNodeDataPoolListElementId c_Id = *this->mc_MissedWriteOperations.begin();
            const int32_t s32_Return =
               this->mpc_ComDriver->PollDataPoolWrite(c_Id.u32_NodeIndex,
                                                      static_cast<uint8_t>(c_Id.u32_DataPoolIndex),
                                                      static_cast<uint16_t>(c_Id.u32_ListIndex),
                                                      static_cast<uint16_t>(c_Id.u32_ElementIndex));
            //Error handling
            if (s32_Return != C_BUSY)
            {
               this->mc_MissedWriteOperations.erase(this->mc_MissedWriteOperations.begin());
            }
         }
         else
         {
            if (this->mc_MissedReadOperations.size() > 0)
            {
               const C_MissedReadOperation c_MissedOperation = *this->mc_MissedReadOperations.begin();
               const C_OscNodeDataPoolListElementId & rc_Id = c_MissedOperation.c_ElementId;
               const int32_t s32_Return =
                  this->mpc_ComDriver->PollDataPoolRead(rc_Id.u32_NodeIndex,
                                                        static_cast<uint8_t>(rc_Id.u32_DataPoolIndex),
                                                        static_cast<uint16_t>(rc_Id.u32_ListIndex),
                                                        static_cast<uint16_t>(rc_Id.u32_ElementIndex),
                                                        c_MissedOperation.pc_DashboardWidget);
               //Error handling
               if (s32_Return != C_BUSY)
               {
                  this->mc_MissedReadOperations.erase(this->mc_MissedReadOperations.begin());
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle signal of any connect step finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_ConnectStepFinished(void)
{
   if (this->mpc_ConnectionThread->GetStep() == C_SyvComDriverDiagConnect::eCDCS_WAITING_FINISHED)
   {
      int32_t s32_Retval;
      QString c_Message;

      mpc_ComDriver = new C_SyvComDriverDiag(this->mu32_ViewIndex);
      connect(mpc_ComDriver, &C_SyvComDriverDiag::SigPollingFinished, this,
              &C_SyvDaDashboardsWidget::m_HandleManualOperationFinished);

      s32_Retval = this->m_InitOsyDriver(c_Message);

      if ((s32_Retval == C_NO_ERR) ||
          (s32_Retval == C_NOACT))
      {
         //Continue with set diagnostic mode
         this->mpc_ConnectionThread->SetSetDiagnosticModeParameters(this->mpc_ComDriver);
         this->mpc_ConnectionThread->start();
      }
      else
      {
         //Default error handling
         this->m_HandleConnectionResult(s32_Retval, c_Message, "");
      }
   }
   else if ((this->mpc_ConnectionThread->GetStep() == C_SyvComDriverDiagConnect::eCDCS_SET_DIAGNOSTIC_MODE_FINISHED) &&
            (this->mpc_ComDriver != NULL))
   {
      int32_t s32_Retval;
      QString c_Message;
      QString c_MessageDetails;
      //Get result of set diagnostic mode
      this->mpc_ConnectionThread->GetLastOperationResult(s32_Retval, c_Message, c_MessageDetails);

      if ((s32_Retval == C_NO_ERR) ||
          (s32_Retval == C_NOACT))
      {
         // Register all widgets
         this->mpc_Ui->pc_TabWidget->RegisterWidgets(*this->mpc_ComDriver);
      }

      if (s32_Retval == C_NO_ERR)
      {
         //Continue with set up cyclic transmission parameters
         this->mpc_ConnectionThread->SetSetUpCyclicTransmissionsParameters(this->mpc_ComDriver);
         this->mpc_ConnectionThread->start();
      }
      else
      {
         //Default error handling
         this->m_HandleConnectionResult(s32_Retval, c_Message, c_MessageDetails);
      }
   }
   else if ((this->mpc_ConnectionThread->GetStep() ==
             C_SyvComDriverDiagConnect::eCDCS_SET_UP_CYCLIC_TRANSMISSIONS_FINISHED) &&
            (this->mpc_ComDriver != NULL))
   {
      int32_t s32_Retval;
      QString c_Message;
      QString c_MessageDetails;
      //Get result of set up cyclic transmissions
      this->mpc_ConnectionThread->GetLastOperationResult(s32_Retval, c_Message, c_MessageDetails);

      if ((s32_Retval == C_NO_ERR) ||
          (s32_Retval == C_NOACT))
      {
         s32_Retval = this->mpc_ComDriver->StartCycling();
         switch (s32_Retval)
         {
         case C_NO_ERR:
            break;
         case C_CONFIG:
            c_Message =
               static_cast<QString>(C_GtGetText::h_GetText("System connect failed. Configuration error.")); //Many
                                                                                                            // possibilities
                                                                                                            // for
            // this error
            break;
         default:
            c_Message =
               static_cast<QString>(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(s32_Retval));
            break;
         }
      }
      this->m_HandleConnectionResult(s32_Retval, c_Message, c_MessageDetails);
   }
   else
   {
      //Step unknown
      tgl_assert(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle connection attempt result value

   \param[in]  os32_Result          Connection attempt result value
   \param[in]  orc_Message          Error message (if any)
   \param[in]  orc_MessageDetails   Error message details (if any)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_HandleConnectionResult(const int32_t os32_Result, const QString & orc_Message,
                                                       const QString & orc_MessageDetails)
{
   if (os32_Result == C_NO_ERR)
   {
      //Success
      // When connected, no drag and drop is allowed on top level
      Q_EMIT (this->SigBlockDragAndDrop(true));
      // Inform all widgets about the connection
      this->mpc_Ui->pc_TabWidget->ConnectionActiveChanged(true);
      // Start updating the dashboard widgets
      this->mc_Timer.start();
      //Update UI
      this->me_ConnectState = eCS_CONNECTED;
      Q_EMIT this->SigSetConnectPushButtonIcon("://images/system_views/IconConnected.svg", false);
   }
   else
   {
      //Failure
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR);

      this->m_CloseOsyDriver();
      // Save the time of 'disconnect'
      C_SyvDaDashboardsWidget::mhu32_DisconnectTime = TglGetTickCount();

      //Reactivate edit & config
      if (C_PuiSvHandler::h_GetInstance()->GetServiceModeActive() == false)
      {
         this->mpc_Ui->pc_PbConfirm->setEnabled(true);
      }
      Q_EMIT this->SigSetConfigurationAvailable(true);
      Q_EMIT this->SigSetConnectPushButtonIcon("://images/system_views/IconDisconnected.svg", false);

      //Restore cursor before error message
      QApplication::restoreOverrideCursor();
      QString c_MessageDetails = orc_MessageDetails;
      c_MessageDetails.replace("\n", "<br>");
      if (!c_MessageDetails.contains("For more information see"))
      {
         c_MessageDetails += C_GtGetText::h_GetText("<br>For more information see ");
         c_MessageDetails += C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLE_GUIDE_COLOR_LINK,
                                              C_OscLoggingHandler::h_GetCompleteLogFileLocation().
                                              c_str());
         c_MessageDetails += C_GtGetText::h_GetText(".");
      }

      // Show the message box only when all communication relevant functions closing in m_CloseOsyDriver are finished
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Dashboard connect"));
      c_MessageBox.SetDescription(orc_Message);
      c_MessageBox.SetDetails(c_MessageDetails);
      c_MessageBox.SetCustomMinHeight(220, 400);
      c_MessageBox.SetCustomMinWidth(650);
      c_MessageBox.Execute();
      //Reset internal state AFTER user confirmation to block any screen switches until this point
      this->me_ConnectState = eCS_DISCONNECTED;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function of fix minimized toolbox widget for button maximizing click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_WiFixPosMaxBtnClicked(void)
{
   // Show toolbox and set maximized true
   if (this->mpc_Toolbox != NULL)
   {
      this->mpc_Toolbox->setVisible(true);
      this->mpc_Toolbox->SetMaximized(true);
   }

   // Don't show minimized toolbox
   if (this->mpc_FixMinimizedToolbox != NULL)
   {
      this->mpc_FixMinimizedToolbox->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function of hover widget for button minimizing click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_WiHoverMinBtnClicked(void)
{
   // Don't show toolbox and set maximized false
   if (this->mpc_Toolbox != NULL)
   {
      this->mpc_Toolbox->setVisible(false);
      this->mpc_Toolbox->SetMaximized(false);
   }

   // Show minimized toolbox
   if (this->mpc_FixMinimizedToolbox != NULL)
   {
      this->mpc_FixMinimizedToolbox->setVisible(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Helper function to adapt space holder widget color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_AdaptSpaceHolderWidgetColor(void) const
{
   if ((this->mq_EditModeActive == true) && (this->mq_DarkModeActive == true))
   {
      this->mpc_Ui->pc_WidgetWhite->SetBackgroundColor(52);
   }
   else
   {
      this->mpc_Ui->pc_WidgetWhite->SetBackgroundColor(0);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current smaller than orc_Cmp

   Warning: Due to array comparison no comparison function may be combined.

   \param[in]  orc_Cmp  Compared instance

   \return
   Current smaller than orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardsWidget::C_MissedReadOperation::operator <(const C_MissedReadOperation & orc_Cmp) const
{
   bool q_Return;

   if (this->c_ElementId < orc_Cmp.c_ElementId)
   {
      q_Return = true;
   }
   else if (this->c_ElementId == orc_Cmp.c_ElementId)
   {
      //lint -e{946}  Operator necessary for std::set, no further interpretation
      if (this->pc_DashboardWidget < orc_Cmp.pc_DashboardWidget)
      {
         q_Return = true;
      }
      else
      {
         q_Return = false;
      }
   }
   else
   {
      q_Return = false;
   }

   return q_Return;
}
