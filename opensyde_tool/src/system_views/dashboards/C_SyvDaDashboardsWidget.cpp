//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboards

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_SyvDaDashboardsWidget.h"
#include "ui_C_SyvDaDashboardsWidget.h"

#include "C_Uti.h"
#include "stwerrors.h"
#include "constants.h"
#include "TGLUtils.h"
#include "C_SyvUtil.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSdHandler.h"
#include "C_OgePopUpDialog.h"
#include "C_UsHandler.h"
#include "C_SyvDaDashboardToolbox.h"
#include "C_SyvDaPeUpdateModeConfiguration.h"
#include "TGLTime.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */
const QString C_SyvDaDashboardsWidget::mhc_DarkModeEnabledIconPath = "://images/system_views/Darkmode_Enable.svg";
const QString C_SyvDaDashboardsWidget::mhc_DarkModeDisabledIconPath = "://images/system_views/Darkmode_Disable.svg";
const sintn C_SyvDaDashboardsWidget::mhsn_WidgetBorder = 25;
const sintn C_SyvDaDashboardsWidget::mhsn_ToolboxInitPosY = 150;
stw_types::uint32 C_SyvDaDashboardsWidget::mhu32_DisconnectTime = 0UL;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_ViewIndex    View index
   \param[in]     opc_ToolboxParent Optional pointer to toolbox parent
   \param[in,out] opc_Parent        Optional pointer to parent

   \created     20.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvDaDashboardsWidget::C_SyvDaDashboardsWidget(const uint32 ou32_ViewIndex, QWidget * const opc_ToolboxParent,
                                                 QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaDashboardsWidget),
   mpc_ComDriver(NULL),
   mpc_Toolbox(NULL),
   mpc_ToolboxParent(opc_ToolboxParent),
   mu32_ViewIndex(ou32_ViewIndex),
   mq_EditModeActive(false),
   mq_ConnectActive(false)
{
   mpc_Ui->setupUi(this);

   this->InitText();

   //Init view index
   this->mpc_Ui->pc_TabWidget->SetViewIndex(this->mu32_ViewIndex);

   //Hide cancel button
   this->mpc_Ui->pc_PbCancel->setVisible(false);

   // Configure timer
   this->mc_Timer.setInterval(msn_TIMER_GUI_REFRESH);
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SyvDaDashboardsWidget::m_UpdateShowValues);

   // Connect buttons
   connect(this->mpc_Ui->pc_PbConfirm, &stw_opensyde_gui_elements::C_OgePubTextWithBorderEdit::clicked,
           this, &C_SyvDaDashboardsWidget::m_ConfirmClicked);
   connect(this->mpc_Ui->pc_PbCancel, &stw_opensyde_gui_elements::C_OgePubTextWithBorderEdit::clicked,
           this, &C_SyvDaDashboardsWidget::m_CancelClicked);

   connect(this->mpc_Ui->pc_TabWidget, &C_SyvDaDashboardSelectorTabWidget::SigConfirmClicked,
           this, &C_SyvDaDashboardsWidget::m_ConfirmClicked);
   connect(this->mpc_Ui->pc_TabWidget, &C_SyvDaDashboardSelectorTabWidget::SigCancelClicked,
           this, &C_SyvDaDashboardsWidget::m_CancelClicked);
   connect(this->mpc_Ui->pc_TabWidget, &C_SyvDaDashboardSelectorTabWidget::SigChanged,
           this, &C_SyvDaDashboardsWidget::SigChanged);
   connect(this->mpc_Ui->pc_TabWidget, &C_SyvDaDashboardSelectorTabWidget::SigErrorChange,
           this, &C_SyvDaDashboardsWidget::CheckError);
   connect(this->mpc_Ui->pc_TabWidget, &C_SyvDaDashboardSelectorTabWidget::SigDataPoolWrite,
           this, &C_SyvDaDashboardsWidget::m_DataPoolWrite);
   connect(this->mpc_Ui->pc_TabWidget, &C_SyvDaDashboardSelectorTabWidget::SigDataPoolRead,
           this, &C_SyvDaDashboardsWidget::m_DataPoolRead);

   this->SetEditMode(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     20.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvDaDashboardsWidget::~C_SyvDaDashboardsWidget(void)
{
   this->m_CloseOsyDriver();

   if (this->mpc_Toolbox != NULL)
   {
      this->mpc_Toolbox->hide();
   }

   delete mpc_Ui;

   //lint -e{1579}  no memory leak of mpc_ComDriver because of calling m_CloseOsyDriver here and for mpc_Toolbox the Qt
   // memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initializes all visible strings on the widget

   \created     30.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::InitText(void) const
{
   this->mpc_Ui->pc_PbConfirm->setText(C_GtGetText::h_GetText("Confirm"));
   this->mpc_Ui->pc_PbCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle initial dark mode

   \created     02.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::LoadDarkMode(void)
{
   const C_PuiSvData * pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      if (pc_View->GetDarkModeActive() == true)
      {
         Q_EMIT this->SigSetPushButtonIcon(C_SyvDaDashboardsWidget::mhc_DarkModeEnabledIconPath);
         m_ApplyDarkMode(true);
      }
      else
      {
         Q_EMIT this->SigSetPushButtonIcon(C_SyvDaDashboardsWidget::mhc_DarkModeDisabledIconPath);
         m_ApplyDarkMode(false);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save data

   \created     30.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::Save(void) const
{
   m_CleanUpToolBox();
   this->mpc_Ui->pc_TabWidget->Save();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle dashboards settings request

   \created     27.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Handle dark mode toggle

   \created     02.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the edit mode

   \param[in]     oq_Active      Flag for edit mode

   \created     30.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::SetEditMode(const bool oq_Active)
{
   this->mq_EditModeActive = oq_Active;

   this->mpc_Ui->pc_BackgroundWidget->SetEditBackground(oq_Active);
   this->mpc_Ui->pc_GroupBoxButtons->setVisible(oq_Active);

   this->mpc_Ui->pc_TabWidget->SetEditMode(oq_Active);

   if (oq_Active == true)
   {
      //Initially create toolbox
      if (this->mpc_Toolbox == NULL)
      {
         this->m_InitToolBox();
      }
      //Show toolbox
      if (this->mpc_Toolbox != NULL)
      {
         this->mpc_Toolbox->show();
      }

      // create copy of dashboard in case of clicking cancel

      this->mpc_Ui->pc_BackgroundWidget->layout()->setContentsMargins(8, 0, 8, 8);
   }
   else
   {
      //Hide toolbox
      if (this->mpc_Toolbox != NULL)
      {
         this->mpc_Toolbox->hide();
      }

      //Also should remember the toolbox settings
      this->Save();
      this->mpc_Ui->pc_BackgroundWidget->layout()->setContentsMargins(0, 0, 0, 8);
      Q_EMIT this->SigEditModeClosed();
   }
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_GroupBox, "Edit", oq_Active);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the state of the edit mode

   \return
   true     edit mode is active
   false    edit mode is not active

   \created     30.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvDaDashboardsWidget::GetEditMode(void) const
{
   return this->mq_EditModeActive;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get connected flag

   \return
   Current connected flag

   \created     26.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SyvDaDashboardsWidget::GetConnectActive(void) const
{
   return mq_ConnectActive;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set connected flag

   \param[in] orc_Value New connected flag

   \created     26.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SyvDaDashboardsWidget::SetConnectActive(const bool oq_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   this->mq_ConnectActive = oq_Value;

   QApplication::setOverrideCursor(Qt::WaitCursor);

   if ((this->mq_ConnectActive == true) &&
       (this->mpc_ComDriver == NULL))
   {
      QString c_Message;
      QString c_MessageDetails = "";

      // Is a new connect already possible
      while ((C_SyvDaDashboardsWidget::mhu32_DisconnectTime + 5100U) > TGL_GetTickCount())
      {
         // Wait till it is possible
      }

      mpc_ComDriver = new C_SyvComDriverDiag(this->mu32_ViewIndex);
      connect(mpc_ComDriver, &C_SyvComDriverDiag::SigPollingFinished, this,
              &C_SyvDaDashboardsWidget::m_HandleManualOperationFinished);

      s32_Retval = this->m_InitOsyDriver(c_Message);

      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = this->m_InitNodes(c_Message, c_MessageDetails);
      }

      if ((s32_Retval == C_NO_ERR) ||
          (s32_Retval == C_NOACT))
      {
         // Register all widgets
         this->mpc_Ui->pc_TabWidget->RegisterWidgets(*this->mpc_ComDriver);
      }

      if (s32_Retval == C_NO_ERR)
      {
         QString c_ErrorDetails;
         std::vector<C_OSCNodeDataPoolListElementId> c_FailedIdRegisters;
         std::vector<QString> c_FailedIdErrorDetails;
         std::map<stw_types::uint32, stw_types::uint32> c_FailedNodesElementNumber;
         std::map<stw_types::uint32, stw_types::uint32> c_NodesElementNumber;

         // Start the asyn communication
         s32_Retval = this->mpc_ComDriver->SetUpCyclicTransmissions(c_ErrorDetails, c_FailedIdRegisters,
                                                                    c_FailedIdErrorDetails,
                                                                    c_FailedNodesElementNumber,
                                                                    c_NodesElementNumber);

         switch (s32_Retval)
         {
         case C_NO_ERR:
            if (c_FailedIdRegisters.size() > 0)
            {
               uintn un_Counter;
               uintn un_CurrentNode = 0xFFFFFFFFU;

               //Signal all widgets about c_FailedIdRegisters
               // ToDo: No visualization on the dashboard
               //this->mpc_Ui->pc_TabWidget->SetErrorForFailedCyclicElementIdRegistrations(c_FailedIdRegisters,
               //                                                                          c_FailedIdErrorDetails);

               // Show all error information
               c_Message =
                  QString(C_GtGetText::h_GetText("Not all cyclic transmission could be registered. (%1 failed)")).
                  arg(
                     c_FailedIdRegisters.size());

               for (un_Counter = 0; un_Counter < c_FailedIdRegisters.size(); ++un_Counter)
               {
                  const C_OSCNodeDataPoolListElementId & rc_Id = c_FailedIdRegisters[un_Counter];
                  C_OSCNodeDataPool::E_Type e_Type;
                  C_PuiSdHandler::h_GetInstance()->GetDataPoolType(rc_Id.u32_NodeIndex, rc_Id.u32_DataPoolIndex,
                                                                   e_Type);

                  if (un_CurrentNode != rc_Id.u32_NodeIndex)
                  {
                     const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(
                        rc_Id.u32_NodeIndex);

                     if (c_MessageDetails != "")
                     {
                        c_MessageDetails += "\n\n";
                     }

                     if (pc_Node != NULL)
                     {
                        const std::map<stw_types::uint32,
                                       stw_types::uint32>::const_iterator c_ItFailedNodesElementNumber =
                           c_FailedNodesElementNumber.find(rc_Id.u32_NodeIndex);

                        // This information as title for all errors of this node
                        c_MessageDetails += "Node " + QString(pc_Node->c_Properties.c_Name.c_str()) + ":\n";

                        if (c_ItFailedNodesElementNumber != c_FailedNodesElementNumber.end())
                        {
                           c_MessageDetails += QString("The node %1 can handle maximum %2 transmissions. ").
                                               arg(pc_Node->c_Properties.c_Name.c_str()).arg(c_FailedNodesElementNumber[
                                                                                                rc_Id.u32_NodeIndex] -
                                                                                             1U);
                           c_MessageDetails += QString("Tried to register %1 transmissions.\n"
                                                       "To fix this adjust 'Max number of cyclic/event driven "
                                                       "transmissions' in 'Code Generation Settings' of"
                                                       " the node properties.\n\n").
                                               arg(c_NodesElementNumber[rc_Id.u32_NodeIndex]);
                        }
                     }
                     c_MessageDetails += "Error details:\n";

                     un_CurrentNode = rc_Id.u32_NodeIndex;
                  }

                  c_MessageDetails += "The initiating of the transmission of the element ";
                  if (e_Type != C_OSCNodeDataPool::eCOM)
                  {
                     c_MessageDetails += C_PuiSdHandler::h_GetInstance()->GetNamespace(rc_Id);
                  }
                  else
                  {
                     c_MessageDetails += C_PuiSdHandler::h_GetInstance()->GetSignalNamespace(rc_Id);
                  }

                  c_MessageDetails += " failed: " + c_FailedIdErrorDetails[un_Counter] + "\n";
               }

               s32_Retval = C_RD_WR;
            }

            break;
         case C_CONFIG:
            c_Message =
               QString(C_GtGetText::h_GetText("System connection failed. Configuration error.")); //Many possibilities
                                                                                                  // for
                                                                                                  // this error
            break;
         case C_COM:
            c_Message =
               QString(C_GtGetText::h_GetText("Communication error. (%1)")).arg(c_ErrorDetails);
            break;
         default:
            c_Message = QString(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(s32_Retval));
            break;
         }
      }

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

      if (s32_Retval == C_NO_ERR)
      {
         // When connected, no drag and drop is allowed on top level
         Q_EMIT (this->SigBlockDragAndDrop(true));
         // Inform all widgets about the connection
         this->mpc_Ui->pc_TabWidget->ConnectionActiveChanged(true);
         // Start updating the dashboard widgets
         this->mc_Timer.start();
      }
      else
      {
         C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR);

         this->m_CloseOsyDriver();
         // Save the time of 'disconnect'
         C_SyvDaDashboardsWidget::mhu32_DisconnectTime = TGL_GetTickCount();

         //Restore cursor before error message
         QApplication::restoreOverrideCursor();

         // Show the message box only when all communication relevant functions closing in m_CloseOsyDriver are finished
         c_MessageBox.SetHeading(C_GtGetText::h_GetText("Dashboard connect"));
         c_MessageBox.SetDescription(c_Message);
         c_MessageBox.SetDetails(c_MessageDetails);
         c_MessageBox.SetCustomMinHeight(450);
         c_MessageBox.SetCustomMinWidth(850);
         c_MessageBox.Execute();
      }
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
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Perform error check

   \created     27.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::CheckError(void) const
{
   QString c_ErrorText;
   const bool q_ViewSetupError = C_SyvUtil::h_CheckViewSetupError(this->mu32_ViewIndex, c_ErrorText);

   if (q_ViewSetupError == true)
   {
      this->mpc_Ui->pc_ErrorLabel->SetCompleteText(c_ErrorText);
      this->mpc_Ui->pc_ErrorFrame->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_ErrorFrame->setVisible(false);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten hide event slot

   Here: hide and delete toolbox

   \param[in,out] opc_Event Event identification and information

   \created     03.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::hideEvent(QHideEvent * const opc_Event)
{
   QWidget::hideEvent(opc_Event);
   if (this->mpc_Toolbox != NULL)
   {
      this->mpc_Toolbox->hide();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten resize event

   Move the toolbox.

   \param[in,out] opc_Event Event identification and information

   \created     25.01.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   if (this->mpc_Toolbox != NULL)
   {
      QPoint c_Point = this->mpc_Toolbox->pos();
      QSize c_Size = this->mpc_Toolbox->size();
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

      this->mpc_Toolbox->setGeometry(QRect(c_Point, c_Size));
   }
}

//-----------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_ConfirmClicked(void)
{
   this->SetEditMode(false);
}

//-----------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_CancelClicked(void)
{
   // restore the saved dashboard
   // TODO

   this->SetEditMode(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply dark mode setting

   \param[in] oq_Active Dark mode value

   \created     02.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Init toolbox widget

   \created     03.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

   // create toolbox
   if (this->mpc_Toolbox == NULL)
   {
      mpc_ToolboxContent = new C_SyvDaDashboardToolbox();
      mpc_Toolbox = new C_OgeWiHover(*mpc_ToolboxContent, C_GtGetText::h_GetText(
                                        "TOOLBOX"), false, this->mpc_ToolboxParent, this->mpc_ToolboxParent);
   }

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
         this->mpc_Toolbox->setGeometry(QRect(QPoint((((this->mpc_ToolboxParent->width() - this->mpc_Toolbox->width()) -
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

   if (c_ViewSettings.GetDashboardToolboxMaximized() == false)
   {
      this->mpc_Toolbox->SetMinimize();
   }

   this->mpc_Toolbox->hide();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clean up toolbox

   \created     03.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_CleanUpToolBox(void) const
{
   if (this->mpc_Toolbox != NULL)
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

//-----------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_UpdateShowValues(void) const
{
   this->mpc_Ui->pc_TabWidget->UpdateShowValues();
}

//-----------------------------------------------------------------------------
sint32 C_SyvDaDashboardsWidget::m_InitOsyDriver(QString & orc_Message)
{
   sint32 s32_Retval;
   bool q_NameInvalid;
   bool q_PCNotConnected;
   bool q_RoutingInvalid;
   bool q_UpdateDisabledButDataBlocks;
   bool q_SysDefInvalid;
   bool q_NoNodesActive;

   s32_Retval = this->mpc_ComDriver->InitDiag();

   switch (s32_Retval)
   {
   case C_NO_ERR:
   case C_NOACT: // C_NOACT means that no node is active, but the CAN signal interpretation is although necessary
      // Starting logging starts the CAN signal interpretation too
      // TODO: Bitrate is not necessary for initialization here and the bus load calculation is not used here.
      //       If this is necessary, this bitrate must be adapted for the real value and not this dummy
      this->mpc_ComDriver->StartLogging(0U);
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

//-----------------------------------------------------------------------------
sint32 C_SyvDaDashboardsWidget::m_InitNodes(QString & orc_Message, QString & orc_MessageDetails)
{
   sint32 s32_Retval;
   QString c_ErrorDetails;

   s32_Retval = this->mpc_ComDriver->SetDiagnosticMode(c_ErrorDetails);

   switch (s32_Retval)
   {
   case C_NO_ERR:
      break;
   case C_CONFIG:
      orc_Message = QString(C_GtGetText::h_GetText("Initialization was not finished."));
      break;
   case C_NOACT:
      orc_Message = QString(C_GtGetText::h_GetText("System View is invalid. Action cannot be performed."));
      break;
   case C_TIMEOUT:
      orc_Message = QString(C_GtGetText::h_GetText("At least one node is not responding!"));
      orc_MessageDetails = QString(C_GtGetText::h_GetText("Affected node(s):\n%1")).arg(c_ErrorDetails);
      break;
   case C_BUSY:
      orc_Message = QString(C_GtGetText::h_GetText("Connecting to at least one node failed!"));
      orc_MessageDetails = QString(C_GtGetText::h_GetText("Affected node(s):\n"
                                                          "%1\n"
                                                          "\n"
                                                          "Possible scenarios:\n"
                                                          "- Wrong configured IP address\n"
                                                          "- Gateway address of node is not configured properly"))
                           .arg(c_ErrorDetails);
      break;
   case C_CHECKSUM:
      orc_Message =
         QString(C_GtGetText::h_GetText(
                    "The CRC over at least one Datapool does not match. Are the applications on all nodes "
                    "up to date?"));
      orc_MessageDetails = QString(C_GtGetText::h_GetText("Affected node(s) and Datapool(s):\n%1")).arg(c_ErrorDetails);
      break;
   case C_COM:
      orc_Message = QString(C_GtGetText::h_GetText("Service error response detected."));
      orc_MessageDetails = QString(C_GtGetText::h_GetText("Affected node(s):\n"
                                                          "%1\n"
                                                          "\n"
                                                          "Possible scenario:\n"
                                                          "If these nodes are in Flashloader (Try restarting those nodes to fix this error)"))
                           .arg(c_ErrorDetails);
      break;
   case C_RD_WR:
      orc_Message = QString(C_GtGetText::h_GetText("Unexpected protocol response detected."));
      orc_MessageDetails = QString(C_GtGetText::h_GetText("Possible scenario:\n"
                                                          "If there are multiple nodes with the same node ID connected to the same bus"));
      break;
   case C_WARN:
      orc_Message = QString(C_GtGetText::h_GetText("Protocol error response detected."));
      orc_MessageDetails = QString(C_GtGetText::h_GetText("Possible scenario:\n"
                                                          "- Node can currently not enter diagnostic session\n"
                                                          "- A Data Pool defined in the openSYDE tool is not available on the Node"));
      break;
   default:
      orc_Message = QString(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(s32_Retval));
      break;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_DataPoolWrite(const uint32 ou32_NodeIndex, const uint8 ou8_DataPoolIndex,
                                              const uint16 ou16_ListIndex, const uint16 ou16_ElementIndex)
{
   if (this->mpc_ComDriver != NULL)
   {
      const sint32 s32_Return = this->mpc_ComDriver->PollDataPoolWrite(ou32_NodeIndex, ou8_DataPoolIndex,
                                                                       ou16_ListIndex, ou16_ElementIndex);
      //Error handling
      if (s32_Return != C_NO_ERR)
      {
         const C_OSCNodeDataPoolListElementId c_Id(ou32_NodeIndex, static_cast<uint32>(ou8_DataPoolIndex),
                                                   static_cast<uint32>(ou16_ListIndex),
                                                   static_cast<uint32>(ou16_ElementIndex));
         this->mc_MissedWriteOperations.insert(c_Id);
      }
   }
}

//-----------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_DataPoolRead(const C_OSCNodeDataPoolListElementId & orc_Index)
{
   if (this->mpc_ComDriver != NULL)
   {
      const sint32 s32_Return =
         this->mpc_ComDriver->PollDataPoolRead(orc_Index.u32_NodeIndex, static_cast<uint8>(orc_Index.u32_DataPoolIndex),
                                               static_cast<uint16>(orc_Index.u32_ListIndex),
                                               static_cast<uint16>(orc_Index.u32_ElementIndex));
      //Error handling
      if (s32_Return != C_NO_ERR)
      {
         const C_OSCNodeDataPoolListElementId c_Id(orc_Index);
         this->mc_MissedReadOperations.insert(c_Id);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle manual user operation finished event

   \param[in] os32_Result Operation result
   \param[in] ou8_NRC     Negative response code, if any

   \created     09.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardsWidget::m_HandleManualOperationFinished(const sint32 os32_Result, const uint8 ou8_NRC)
{
   this->mpc_Ui->pc_TabWidget->HandleManualOperationFinished(os32_Result, ou8_NRC);
   if (this->mpc_ComDriver != NULL)
   {
      if (this->mc_MissedReadOperations.size() > 0)
      {
         const C_OSCNodeDataPoolListElementId c_Id = *this->mc_MissedReadOperations.begin();
         const sint32 s32_Return =
            this->mpc_ComDriver->PollDataPoolRead(c_Id.u32_NodeIndex, static_cast<uint8>(c_Id.u32_DataPoolIndex),
                                                  static_cast<uint16>(c_Id.u32_ListIndex),
                                                  static_cast<uint16>(c_Id.u32_ElementIndex));
         //Error handling
         if (s32_Return == C_NO_ERR)
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
            if (s32_Return == C_NO_ERR)
            {
               this->mc_MissedWriteOperations.erase(this->mc_MissedWriteOperations.begin());
            }
         }
      }
   }
}
