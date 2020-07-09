//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboards

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SyvDaDashboardsWidget.h"
#include "ui_C_SyvDaDashboardsWidget.h"

#include "C_Uti.h"
#include "stwerrors.h"
#include "constants.h"
#include "TGLUtils.h"
#include "TGLTime.h"
#include "C_SyvUtil.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSdHandler.h"
#include "C_OgePopUpDialog.h"
#include "C_UsHandler.h"
#include "C_SyvDaDashboardToolbox.h"
#include "C_SyvDaPeUpdateModeConfiguration.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvDaDashboardsWidget::mhc_DarkModeEnabledIconPath = "://images/system_views/Darkmode_Enable.svg";
const QString C_SyvDaDashboardsWidget::mhc_DarkModeDisabledIconPath = "://images/system_views/Darkmode_Disable.svg";
const sintn C_SyvDaDashboardsWidget::mhsn_WidgetBorder = 25;
const sintn C_SyvDaDashboardsWidget::mhsn_ToolboxInitPosY = 150;
stw_types::uint32 C_SyvDaDashboardsWidget::mhu32_DisconnectTime = 0UL;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_ViewIndex    View index
   \param[in]     opc_ToolboxParent Optional pointer to toolbox parent
   \param[in,out] opc_Parent        Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaDashboardsWidget::C_SyvDaDashboardsWidget(const uint32 ou32_ViewIndex, QWidget * const opc_ToolboxParent,
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
   mq_ConnectActive(false),
   me_ConnectState(eCS_DISCONNECTED),
   msn_InitToolboxCounter(0)
{
   mpc_Ui->setupUi(this);

   this->InitText();

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
   this->mc_Timer.setInterval(msn_TIMER_GUI_REFRESH);
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SyvDaDashboardsWidget::m_UpdateShowValues);

   // Connect buttons
   connect(this->mpc_Ui->pc_PbConfirm, &stw_opensyde_gui_elements::C_OgePubSystemCommissioningEdit::clicked,
           this, &C_SyvDaDashboardsWidget::m_ConfirmClicked);
   connect(this->mpc_Ui->pc_PbCancel, &stw_opensyde_gui_elements::C_OgePubSystemCommissioningEdit::clicked,
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
   if (this->mpc_Toolbox == NULL)
   {
      mpc_ToolboxContent = new C_SyvDaDashboardToolbox();
      mpc_Toolbox = new C_OgeWiHover(*mpc_ToolboxContent, C_GtGetText::h_GetText("TOOLBOX"), ":images/IconToolbox.svg",
                                     false, this->mpc_ToolboxParent, this->mpc_ToolboxParent);
   }

   // create fix minimized toolbox
   if (this->mpc_FixMinimizedToolbox == NULL)
   {
      this->mpc_FixMinimizedToolbox = new C_OgeWiFixPosition(C_GtGetText::h_GetText("TOOLBOX"),
                                                             ":images/IconToolbox.svg",
                                                             QRect(1277, 24, 190, 36), this->mpc_ToolboxParent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
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

   //lint -e{1579}  no memory leak of mpc_ComDriver because of calling m_CloseOsyDriver here and for mpc_Toolbox the Qt
   // memory management
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
   const C_PuiSvData * pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      if (pc_View->GetDarkModeActive() == true)
      {
         Q_EMIT this->SigSetDarkModePushButtonIcon(C_SyvDaDashboardsWidget::mhc_DarkModeEnabledIconPath);
         m_ApplyDarkMode(true);
      }
      else
      {
         Q_EMIT this->SigSetDarkModePushButtonIcon(C_SyvDaDashboardsWidget::mhc_DarkModeDisabledIconPath);
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
   QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_SyvDaPeUpdateModeConfiguration * pc_Dialog = new C_SyvDaPeUpdateModeConfiguration(*c_New, this->mu32_ViewIndex);

   Q_UNUSED(pc_Dialog)

   //Resize
   c_New->SetSize(QSize(1400, 809));

   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      //Signal widget update
      this->mpc_Ui->pc_TabWidget->UpdateTransmissionConfiguration();
      //Recheck error
      this->CheckError();
   }

   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
   //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle dark mode toggle
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::ToggleDarkMode(void)
{
   const C_PuiSvData * pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

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

   \param[in]     oq_Active      Flag for edit mode
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
      this->mpc_Ui->pc_PbConfirm->setIconSize(QSize(22, 22));
      this->mpc_Ui->pc_PbConfirm->SetMargins(12, 20);
      this->mpc_Ui->pc_PbConfirm->setText(C_GtGetText::h_GetText("Confirm"));

      //Initially create toolbox
      this->m_InitToolBox();

      //Show toolbox
      if ((this->mpc_Toolbox != NULL) &&
          (C_UsHandler::h_GetInstance()->GetProjSvSetupView(
              C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex)->GetName())
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
      this->mpc_Ui->pc_PbConfirm->setIconSize(QSize(24, 24));
      this->mpc_Ui->pc_PbConfirm->SetMargins(16, 25);
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
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_GroupBox, "Edit", oq_Active);
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

   \param[in] oq_Value New connected flag
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
      C_SyvDaDashboardsWidget::mhu32_DisconnectTime = TGL_GetTickCount();
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
   bool q_Return = this->me_ConnectState != eCS_CONNECTING;

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
   const bool q_ViewSetupError =
      C_SyvUtil::h_CheckViewSetupError(this->mu32_ViewIndex, c_ErrorTextHeading, c_ErrorText, c_ErrorTextTooltip);

   if (q_ViewSetupError == true)
   {
      this->mpc_Ui->pc_ErrorLabelIcon->SetToolTipInformation("", c_ErrorTextTooltip, C_NagToolTip::eERROR);
      this->mpc_Ui->pc_ErrorLabelTitle->setText(c_ErrorTextHeading);
      this->mpc_Ui->pc_ErrorLabelTitle->SetToolTipInformation("", c_ErrorTextTooltip, C_NagToolTip::eERROR);
      this->mpc_Ui->pc_ErrorLabel->SetCompleteText(c_ErrorText, c_ErrorTextTooltip);
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
      Q_EMIT this->SigSetConnectPushButtonIcon("://images/system_views/IconConnecting.svg", true);
      //Leave edit mode
      if (this->GetEditMode() == true)
      {
         this->SetEditMode(false);
      }
      //Deactivate edit & config
      this->mpc_Ui->pc_TabWidget->SetEnabled(false);
      this->mpc_Ui->pc_PbConfirm->setEnabled(false);
      Q_EMIT this->SigSetConfigurationAvailable(false);
      //While connecting display updated tool bar
      QApplication::processEvents();
      this->SetConnectActive(true);
      break;
   case eCS_CONNECTED:
      //Disconnecting
      this->me_ConnectState = eCS_DISCONNECTING;
      Q_EMIT this->SigSetConnectPushButtonIcon("://images/system_views/IconDisconnecting.svg", true);
      this->SetConnectActive(false);
      this->me_ConnectState = eCS_DISCONNECTED;
      Q_EMIT this->SigSetConnectPushButtonIcon("://images/system_views/IconDisconnected.svg", false);
      //Reactivate edit & config
      this->mpc_Ui->pc_TabWidget->SetEnabled(true);
      this->mpc_Ui->pc_PbConfirm->setEnabled(true);
      Q_EMIT this->SigSetConfigurationAvailable(true);
      break;
   case eCS_CONNECTING:
   case eCS_DISCONNECTING:
      //No action can be started
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten hide event slot

   Here: hide and delete toolbox

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::hideEvent(QHideEvent * const opc_Event)
{
   QWidget::hideEvent(opc_Event);
   if (this->mpc_Toolbox != NULL)
   {
      this->mpc_Toolbox->hide();
   }

   if (this->mpc_FixMinimizedToolbox != NULL)
   {
      this->mpc_FixMinimizedToolbox->hide();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event

   Move the toolbox.

   \param[in,out] opc_Event Event identification and information
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
      if ((this->mpc_Toolbox->x() + this->mpc_Toolbox->width() + mhsn_WidgetBorder) > pc_Widget->width())
      {
         // is the toolbox to big?
         if ((this->mpc_Toolbox->width() + (2 * mhsn_WidgetBorder)) > pc_Widget->width())
         {
            c_Size.setWidth(pc_Widget->width() - (2 * mhsn_WidgetBorder));
         }
         else
         {
            // adapt position of toolbox
            c_Point.setX((pc_Widget->width() - this->mpc_Toolbox->width()) - mhsn_WidgetBorder);
         }
      }

      // would the toolbox be outside of the widget in y direction
      if ((this->mpc_Toolbox->y() + this->mpc_Toolbox->height() + mhsn_WidgetBorder) > pc_Widget->height())
      {
         // is the toolbox to big?
         if ((this->mpc_Toolbox->height() + (2 * mhsn_WidgetBorder)) > pc_Widget->height())
         {
            c_Size.setHeight(pc_Widget->height() - (2 * mhsn_WidgetBorder));
         }
         else
         {
            // adapt position of toolbox
            c_Point.setY((pc_Widget->height() - this->mpc_Toolbox->height()) - mhsn_WidgetBorder);
         }
      }

      // adapt position of fix minimized toolbox
      // The operator '-' have to follow by the same operator, otherwise the geometry is wrong
      //lint -save -e834 -e1963
      c_PointFixMiniToolbox.setX((pc_Widget->width() - this->mpc_FixMinimizedToolbox->width()) -
                                 170 - mhsn_WidgetBorder);
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

   \param[in] oq_Active Dark mode value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_ApplyDarkMode(const bool oq_Active)
{
   this->mpc_Ui->pc_TabWidget->ApplyDarkMode(oq_Active);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_GroupBox, "DarkMode", oq_Active);
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
      c_ViewName = pc_View->GetName();
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
            this->mpc_Toolbox->move(mhsn_WidgetBorder, mhsn_ToolboxInitPosY);
         }
         else
         {
            // default value
            this->mpc_Toolbox->setGeometry(QRect(QPoint((((this->mpc_ToolboxParent->width() -
                                                           this->mpc_Toolbox->width()) -
                                                          mhsn_WidgetBorder) - static_cast<sintn> (150)),
                                                        mhsn_ToolboxInitPosY + static_cast<sintn> (50)),
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

   this->msn_InitToolboxCounter = this->msn_InitToolboxCounter + 1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up toolbox
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_CleanUpToolBox(void) const
{
   if ((this->mpc_Toolbox != NULL) && (this->msn_InitToolboxCounter > 0))
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
      if (pc_View != NULL)
      {
         QSize c_Size;
         // save position, size and state of toolbox
         c_Size.setWidth(this->mpc_Toolbox->width());
         c_Size.setHeight(this->mpc_Toolbox->GetMaximizedHeight());
         C_UsHandler::h_GetInstance()->SetProjSvDashboardToolbox(pc_View->GetName(),
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
sint32 C_SyvDaDashboardsWidget::m_InitOsyDriver(QString & orc_Message)
{
   sint32 s32_Retval;
   bool q_NameInvalid;
   bool q_PCNotConnected;
   bool q_RoutingInvalid;
   bool q_UpdateDisabledButDataBlocks;
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
         QString(C_GtGetText::h_GetText("System connect failed. Configuration error.")); //Many possibilities for this
                                                                                         // error
      break;
   case C_RD_WR:
      orc_Message =
         QString(C_GtGetText::h_GetText("Configured communication DLL does not exist or DLL could not be opened."));
      break;
   case C_OVERFLOW:
      orc_Message =
         QString(C_GtGetText::h_GetText(
                    "Unknown transport protocol or unknown diagnostic server for at least one node."));
      break;
   case C_BUSY:
      if (C_PuiSvHandler::h_GetInstance()->CheckViewError(this->mu32_ViewIndex, &q_NameInvalid,
                                                          &q_PCNotConnected, &q_RoutingInvalid,
                                                          &q_UpdateDisabledButDataBlocks,
                                                          &q_SysDefInvalid, &q_NoNodesActive, NULL) == C_NO_ERR)
      {
         if ((((((q_NameInvalid == false) && (q_PCNotConnected == false)) && (q_RoutingInvalid == false)) &&
               (q_SysDefInvalid == false)) && (q_NoNodesActive == false)) && (q_UpdateDisabledButDataBlocks == false))
         {
            orc_Message = QString(C_GtGetText::h_GetText("System View Dashboard configuration error detected. "
                                                         "Check your Dashboard configuration settings and retry."));
         }
         else
         {
            orc_Message =
               QString(C_GtGetText::h_GetText(
                          "System View is invalid. Action cannot be performed. Fix the issues and retry."));
         }
      }
      else
      {
         orc_Message = QString(C_GtGetText::h_GetText("System View check failed."));
      }
      break;
   case C_COM:
      orc_Message =
         QString(C_GtGetText::h_GetText(
                    "CAN initialization failed. Check your PC CAN interface configuration (System View setup - "
                    "double-click on PC)."));
      break;
   case C_CHECKSUM:
      orc_Message = QString(C_GtGetText::h_GetText("Internal buffer overflow detected."));
      break;
   case C_RANGE:
      orc_Message = QString(C_GtGetText::h_GetText("Routing configuration failed."));
      break;
   case C_UNKNOWN_ERR:
      orc_Message = QString(C_GtGetText::h_GetText("Wrapped error of internal function call."));
      break;
   default:
      orc_Message = QString(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(s32_Retval));
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
      this->mpc_ComDriver->SendTesterPresentWithoutDefectNodes();

      this->mpc_ComDriver->StopDiagnosisServer();

      delete this->mpc_ComDriver;
      this->mpc_ComDriver = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_DataPoolWrite(const uint32 ou32_NodeIndex, const uint8 ou8_DataPoolIndex,
                                              const uint16 ou16_ListIndex, const uint16 ou16_ElementIndex)
{
   if (this->mpc_ComDriver != NULL)
   {
      const sint32 s32_Return = this->mpc_ComDriver->PollDataPoolWrite(ou32_NodeIndex, ou8_DataPoolIndex,
                                                                       ou16_ListIndex, ou16_ElementIndex);
      //Error handling
      if (s32_Return == C_BUSY)
      {
         const C_OSCNodeDataPoolListElementId c_Id(ou32_NodeIndex, static_cast<uint32>(ou8_DataPoolIndex),
                                                   static_cast<uint32>(ou16_ListIndex),
                                                   static_cast<uint32>(ou16_ElementIndex));
         this->mc_MissedWriteOperations.insert(c_Id);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_DataPoolRead(const C_OSCNodeDataPoolListElementId & orc_Index)
{
   if (this->mpc_ComDriver != NULL)
   {
      const sint32 s32_Return =
         this->mpc_ComDriver->PollDataPoolRead(orc_Index.u32_NodeIndex, static_cast<uint8>(orc_Index.u32_DataPoolIndex),
                                               static_cast<uint16>(orc_Index.u32_ListIndex),
                                               static_cast<uint16>(orc_Index.u32_ElementIndex));
      //Error handling
      if (s32_Return == C_BUSY)
      {
         const C_OSCNodeDataPoolListElementId c_Id(orc_Index);
         this->mc_MissedReadOperations.insert(c_Id);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Attempt to start read NVM list operation

   \param[in] orc_Index List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_NvmReadList(const C_OSCNodeDataPoolListId & orc_Index)
{
   if (this->mpc_ComDriver != NULL)
   {
      const sint32 s32_Return =
         this->mpc_ComDriver->PollNvmReadList(orc_Index.u32_NodeIndex, static_cast<uint8>(orc_Index.u32_DataPoolIndex),
                                              static_cast<uint16>(orc_Index.u32_ListIndex));
      //Error handling
      if (s32_Return == C_BUSY)
      {
         this->mc_MissedReadNvmOperations.insert(orc_Index);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle manual user operation finished event

   \param[in] os32_Result Operation result
   \param[in] ou8_NRC     Negative response code, if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_HandleManualOperationFinished(const sint32 os32_Result, const uint8 ou8_NRC)
{
   this->mpc_Ui->pc_TabWidget->HandleManualOperationFinished(os32_Result, ou8_NRC);
   if (this->mpc_ComDriver != NULL)
   {
      if (this->mc_MissedReadNvmOperations.size() > 0)
      {
         const C_OSCNodeDataPoolListId c_Id = *this->mc_MissedReadNvmOperations.begin();
         const sint32 s32_Return =
            this->mpc_ComDriver->PollNvmReadList(c_Id.u32_NodeIndex, static_cast<uint8>(c_Id.u32_DataPoolIndex),
                                                 static_cast<uint16>(c_Id.u32_ListIndex));
         //Error handling
         if (s32_Return != C_BUSY)
         {
            this->mc_MissedReadNvmOperations.erase(this->mc_MissedReadNvmOperations.begin());
         }
      }
      else
      {
         if (this->mc_MissedReadOperations.size() > 0)
         {
            const C_OSCNodeDataPoolListElementId c_Id = *this->mc_MissedReadOperations.begin();
            const sint32 s32_Return =
               this->mpc_ComDriver->PollDataPoolRead(c_Id.u32_NodeIndex, static_cast<uint8>(c_Id.u32_DataPoolIndex),
                                                     static_cast<uint16>(c_Id.u32_ListIndex),
                                                     static_cast<uint16>(c_Id.u32_ElementIndex));
            //Error handling
            if (s32_Return != C_BUSY)
            {
               this->mc_MissedReadOperations.erase(this->mc_MissedReadOperations.begin());
            }
         }
         else
         {
            if (this->mc_MissedWriteOperations.size() > 0)
            {
               const C_OSCNodeDataPoolListElementId c_Id = *this->mc_MissedWriteOperations.begin();
               const sint32 s32_Return =
                  this->mpc_ComDriver->PollDataPoolWrite(c_Id.u32_NodeIndex, static_cast<uint8>(c_Id.u32_DataPoolIndex),
                                                         static_cast<uint16>(c_Id.u32_ListIndex),
                                                         static_cast<uint16>(c_Id.u32_ElementIndex));
               //Error handling
               if (s32_Return != C_BUSY)
               {
                  this->mc_MissedWriteOperations.erase(this->mc_MissedWriteOperations.begin());
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
      sint32 s32_Retval;
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
      sint32 s32_Retval;
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
      sint32 s32_Retval;
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
               QString(C_GtGetText::h_GetText("System connect failed. Configuration error.")); //Many possibilities for
                                                                                               // this error
            break;
         default:
            c_Message = QString(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(s32_Retval));
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

   \param[in] os32_Result        Connection attempt result value
   \param[in] orc_Message        Error message (if any)
   \param[in] orc_MessageDetails Error message details (if any)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_HandleConnectionResult(const sint32 os32_Result, const QString & orc_Message,
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
      C_SyvDaDashboardsWidget::mhu32_DisconnectTime = TGL_GetTickCount();

      //Reactivate edit & config
      this->mpc_Ui->pc_PbConfirm->setEnabled(true);
      Q_EMIT this->SigSetConfigurationAvailable(true);
      Q_EMIT this->SigSetConnectPushButtonIcon("://images/system_views/IconDisconnected.svg", false);

      //Restore cursor before error message
      QApplication::restoreOverrideCursor();

      // Show the message box only when all communication relevant functions closing in m_CloseOsyDriver are finished
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Dashboard connect"));
      c_MessageBox.SetDescription(orc_Message);
      c_MessageBox.SetDetails(orc_MessageDetails);
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
