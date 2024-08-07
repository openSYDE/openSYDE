//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing system view setup

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#include "precomp_headers.hpp"

#include "stwerrors.hpp"

#include "C_SyvSeSetupWidget.hpp"

#include "ui_C_SyvSeSetupWidget.h"

#include "constants.hpp"
#include "C_SyvUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_UsHandler.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscNode.hpp"
#include "C_SyvDcWidget.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      ou32_ViewIndex   View index
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvSeSetupWidget::C_SyvSeSetupWidget(const uint32_t ou32_ViewIndex, QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvSeSetupWidget),
   mpc_Scene(NULL),
   mu32_ViewIndex(ou32_ViewIndex),
   mq_EditModeActive(false),
   mq_IgnoreSelectAllCheckboxChanges(false)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   const bool q_ServiceModeActive = C_PuiSvHandler::h_GetInstance()->GetServiceModeActive();
   QString c_Name;

   mpc_Ui->setupUi(this);

   //Style error label
   this->mpc_Ui->pc_ErrorLabelTitle->SetForegroundColor(24);
   this->mpc_Ui->pc_ErrorLabelTitle->SetFontPixel(14, false, true);
   this->mpc_Ui->pc_ErrorLabelIcon->SetSvg("://images/Error_iconV2.svg");

   //Handle button icons
   this->mpc_Ui->pc_PbCancel->SetSvg("://images/system_views/IconTabCloseClickedDark.svg");
   this->mpc_Ui->pc_PbCancel->setIconSize(QSize(14, 14));
   this->mpc_Ui->pc_PbCancel->SetMargins(12, 21);

   //Remove debug label
   this->mpc_Ui->pc_GroupBoxButtons->setTitle("");

   // create scene for graphics view
   this->mpc_Scene = new C_SyvSeScene(mu32_ViewIndex);
   //And set it again after creation. Some changes have to be done on scene too.
   this->SetEditMode(false, false);

   // configure scene
   this->mpc_Scene->setSceneRect(0.0, 0.0,
                                 static_cast<float64_t>(this->mpc_Ui->pc_GraphicsView->width()),
                                 static_cast<float64_t>(this->mpc_Ui->pc_GraphicsView->height()));
   this->mpc_Ui->pc_GraphicsView->SetSceneAndConnect(this->mpc_Scene);

   this->InitText();

   // Handle the service mode
   this->mpc_Ui->pc_PbConfirm->setEnabled(!q_ServiceModeActive);

   // make all generic connects
   //Connect for tool tip
   connect(this->mpc_Scene, &C_SebScene::SigShowToolTip, this->mpc_Ui->pc_GraphicsView,
           &C_SebGraphicsView::ShowToolTip);
   connect(this->mpc_Scene, &C_SebScene::SigHideToolTip, this->mpc_Ui->pc_GraphicsView,
           &C_SebGraphicsView::HideToolTip);
   connect(this->mpc_Ui->pc_GraphicsView, &C_SebGraphicsView::SigShowToolTip, this->mpc_Scene,
           &C_SebScene::DisplayToolTip);
   //Interaction point scaling
   connect(this->mpc_Scene, &C_SebScene::SigTriggerUpdateTransform, this->mpc_Ui->pc_GraphicsView,
           &C_SebGraphicsView::UpdateTransform);
   // Connect buttons
   connect(this->mpc_Ui->pc_CheckBoxSelectAll,
           &stw::opensyde_gui_elements::C_OgeChxSystemCommisioningEdit::stateChanged,
           this, &C_SyvSeSetupWidget::m_SelectAllStateChanged);
   connect(this->mpc_Ui->pc_PbConfirm, &stw::opensyde_gui_elements::C_OgePubSystemCommissioningEdit::clicked,
           this, &C_SyvSeSetupWidget::m_ConfirmClicked);
   connect(this->mpc_Ui->pc_PbCancel, &stw::opensyde_gui_elements::C_OgePubSystemCommissioningEdit::clicked,
           this, &C_SyvSeSetupWidget::m_CancelClicked);
   // Scene forwarding
   connect(this->mpc_Scene, &C_SebScene::SigChanged, this, &C_SyvSeSetupWidget::m_ViewChanged);
   connect(this->mpc_Scene, &C_SyvSeScene::SigErrorCheck, this, &C_SyvSeSetupWidget::m_CheckViewForErrorGeneral);
   connect(this->mpc_Scene, &C_SyvSeScene::SigConnectionChange, this, &C_SyvSeSetupWidget::m_OnViewConnectionChange);

   //Update all items with initial zoom & pos value
   if (pc_View != NULL)
   {
      c_Name = pc_View->GetName().c_str();
   }
   else
   {
      c_Name = "";
   }
   this->mpc_Ui->pc_GraphicsView->SetZoomValue(C_UsHandler::h_GetInstance()->GetProjSvSetupView(
                                                  c_Name).s32_SetupViewZoom, true);
   this->mpc_Ui->pc_GraphicsView->SetViewPos(C_UsHandler::h_GetInstance()->GetProjSvSetupView(c_Name).c_SetupViewPos);
   this->m_CheckViewForError();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvSeSetupWidget::~C_SyvSeSetupWidget(void) noexcept
{
   delete mpc_Ui;
   delete mpc_Scene;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::InitText(void) const
{
   this->mpc_Ui->pc_CheckBoxSelectAll->setText(C_GtGetText::h_GetText("Select All"));
   this->mpc_Ui->pc_PbCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::Save(void) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   // store configuration of the view
   if (pc_View != NULL)
   {
      C_UsHandler::h_GetInstance()->SetProjSvSetupViewZoom(pc_View->GetName().c_str(),
                                                           this->mpc_Ui->pc_GraphicsView->GetZoomValue());
      C_UsHandler::h_GetInstance()->SetProjSvSetupViewPos(pc_View->GetName().c_str(),
                                                          this->mpc_Ui->pc_GraphicsView->GetViewPos());
   }
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->Save();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delayed load of scene
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::LoadScene(void)
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->Load();

      this->SetEditMode(false);
      this->m_CheckViewForError(false);
      this->m_OnViewConnectionChange();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle abort action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::Abort(void)
{
   m_CancelClicked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the edit mode

   \param[in]  oq_Active      Flag for edit mode
   \param[in]  oq_WithSave    Flag if changes should get saved
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::SetEditMode(const bool oq_Active, const bool oq_WithSave)
{
   this->mq_EditModeActive = oq_Active;

   this->mpc_Ui->pc_BackgroundWidget->SetEditBackground(oq_Active);
   this->mpc_Ui->pc_PbCancel->setVisible(oq_Active);
   this->mpc_Ui->pc_CheckBoxSelectAll->setVisible(oq_Active);

   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetDrawingBackground(oq_Active);
      this->mpc_Scene->SetEditMode(oq_Active);
   }

   this->mpc_Ui->pc_GraphicsView->SetDrawingBackground(oq_Active);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_DrawFrame, "NoBorder", oq_Active);

   if (oq_Active == true)
   {
      //Handle button
      this->mpc_Ui->pc_PbConfirm->SetSvg("://images/system_views/IconConfirm.svg");
      this->mpc_Ui->pc_PbConfirm->SetMargins(10, 20);
      this->mpc_Ui->pc_PbConfirm->setText(C_GtGetText::h_GetText("Confirm"));

      // create copy of view in case of clicking cancel
      this->mc_ViewCopy = *C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   }
   else
   {
      //Handle button
      this->mpc_Ui->pc_PbConfirm->SetSvg("://images/main_page_and_navi_bar/IconEdit.svg",
                                         "://images/IconEditDisabledBright.svg");
      this->mpc_Ui->pc_PbConfirm->SetMargins(10, 28); // put a bit more space in between
      this->mpc_Ui->pc_PbConfirm->setText(C_GtGetText::h_GetText("Edit"));

      if (oq_WithSave == true)
      {
         this->Save();
      }
      if (this->mpc_Scene != NULL)
      {
         this->mpc_Scene->ClearUndoStack();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the state of the edit mode

   \return
   true     edit mode is active
   true     edit mode is not active
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvSeSetupWidget::GetEditMode(void) const
{
   return this->mq_EditModeActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts the device configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::StartDeviceConfiguration(void)
{
   bool q_DeviceConfigPossible = true;

   // Check for special case of a automatically deactivated sub node
   std::vector<uint8_t> c_NodeActiveFlagsWithDeactivatedSubNodes;
   const int32_t s32_Retval = C_PuiSvHandler::h_GetInstance()->GetNodeActiveFlagsWithSquadAdaptions(
      this->mu32_ViewIndex,
      c_NodeActiveFlagsWithDeactivatedSubNodes, false);
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if ((pc_View != NULL) &&
       (s32_Retval == C_NO_ERR))
   {
      const std::vector<uint8_t> & rc_NodeActiveFlags = pc_View->GetNodeActiveFlags();
      uint32_t u32_Counter;

      tgl_assert(rc_NodeActiveFlags.size() == c_NodeActiveFlagsWithDeactivatedSubNodes.size());
      for (u32_Counter = 0U; u32_Counter < rc_NodeActiveFlags.size(); ++u32_Counter)
      {
         if (rc_NodeActiveFlags[u32_Counter] != c_NodeActiveFlagsWithDeactivatedSubNodes[u32_Counter])
         {
            C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR);
            QString c_Message;

            c_Message = C_GtGetText::h_GetText("At least one sub node is not connected or can not"
                                               " reached on the local bus.\n"
                                               "The device configuration is only possible when all sub nodes"
                                               " of one device can be reached on one interface.");

            c_MessageBox.SetCustomMinHeight(230, 230);
            c_MessageBox.SetDescription(c_Message);
            c_MessageBox.Execute();

            // Difference detected
            q_DeviceConfigPossible = false;

            break;
         }
      }
   }

   if (q_DeviceConfigPossible == true)
   {
      QPointer<C_OgePopUpDialog> c_New;
      C_SyvDcWidget * pc_Dialog;

      //Confirm if necessary
      if (this->GetEditMode() == true)
      {
         this->SetEditMode(false);
      }

      //Show dialog
      c_New = new C_OgePopUpDialog(this, this);
      pc_Dialog = new C_SyvDcWidget(*c_New, this->mu32_ViewIndex);

      Q_UNUSED(pc_Dialog)

      //Resize
      c_New->SetSize(QSize(1100, 743));

      c_New->exec();

      if (c_New != NULL)
      {
         pc_Dialog->CleanUp();
         c_New->HideOverlay();
         c_New->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to prepare closing the scene
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::PrepareToClose(void)
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->PrepareToClose();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden key press event

   \param[in,out]  opc_Event  Pointer to key event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::keyPressEvent(QKeyEvent * const opc_Event)
{
   if ((opc_Event->key() == static_cast<int32_t>(Qt::Key_Escape)) &&
       (this->mq_EditModeActive == true))
   {
      this->m_CancelClicked();
   }
   else
   {
      QWidget::keyPressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::m_ViewChanged(void)
{
   Q_EMIT (this->SigChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle error check trigger
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::m_CheckViewForErrorGeneral(void) const
{
   m_CheckViewForError();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::m_CheckViewForError(const bool oq_SendError) const
{
   QString c_ErrorText;
   QString c_ErrorTextHeading;
   QString c_ErrorTextTooltip;

   C_NagToolTip::E_Type e_ToolTipType;
   QString c_IconPath;
   int32_t s32_ColorId;
   const bool q_ViewSetupError = C_SyvUtil::h_GetViewStatusLabelInfo(
      this->mu32_ViewIndex, ms32_SUBMODE_SYSVIEW_SETUP, c_ErrorTextHeading, c_ErrorText, c_ErrorTextTooltip,
      e_ToolTipType, c_IconPath, s32_ColorId);

   if (q_ViewSetupError == true)
   {
      QString c_ToolTipHeading;

      if (e_ToolTipType == C_NagToolTip::eERROR)
      {
         c_ToolTipHeading = C_GtGetText::h_GetText("Invalid");
         Q_EMIT (this->SigEnableConfiguration(false));
      }
      else
      {
         c_ToolTipHeading = C_GtGetText::h_GetText("Note");
         Q_EMIT (this->SigEnableConfiguration(true));
      }

      this->mpc_Ui->pc_ErrorLabelIcon->SetSvg(c_IconPath);
      this->mpc_Ui->pc_ErrorLabelIcon->SetToolTipInformation(c_ToolTipHeading,
                                                             c_ErrorTextTooltip, e_ToolTipType);
      this->mpc_Ui->pc_ErrorLabelTitle->SetForegroundColor(s32_ColorId);
      this->mpc_Ui->pc_ErrorLabelTitle->setText(c_ErrorTextHeading);
      this->mpc_Ui->pc_ErrorLabelTitle->SetToolTipInformation(c_ToolTipHeading,
                                                              c_ErrorTextTooltip, e_ToolTipType);
      this->mpc_Ui->pc_ErrorLabel->SetForegroundColor(s32_ColorId);
      this->mpc_Ui->pc_ErrorLabel->SetCompleteText(c_ErrorText, c_ErrorTextTooltip, e_ToolTipType);
      this->mpc_Ui->pc_GroupBoxErrorContent->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxErrorContent->setVisible(false);
      Q_EMIT (this->SigEnableConfiguration(true));
   }
   if (oq_SendError == true)
   {
      Q_EMIT (this->SigCheckViewError(this->mu32_ViewIndex));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle change of any node connection state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::m_OnViewConnectionChange(void)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      // Get original active flags
      const std::vector<uint8_t> & rc_ActiveFlags = pc_View->GetNodeActiveFlags();
      bool q_AllActive = true;
      for (uint32_t u32_ItActive = 0UL; u32_ItActive < rc_ActiveFlags.size(); ++u32_ItActive)
      {
         if (rc_ActiveFlags[u32_ItActive] == 0U)
         {
            q_AllActive = false;
         }
      }
      this->mq_IgnoreSelectAllCheckboxChanges = true;
      this->mpc_Ui->pc_CheckBoxSelectAll->setChecked(q_AllActive);
      this->mq_IgnoreSelectAllCheckboxChanges = false;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle state change for select all checkbox
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::m_SelectAllStateChanged(void)
{
   //Flag to ignore automated changes triggering this function
   if ((this->mq_IgnoreSelectAllCheckboxChanges == false) && (this->mpc_Scene != NULL))
   {
      const uint16_t u16_Timer = osc_write_log_performance_start();
      if (this->mpc_Ui->pc_CheckBoxSelectAll->isChecked())
      {
         this->mpc_Scene->SetAllNodesConnected(true);
         osc_write_log_performance_stop(u16_Timer, "System Commisioning Setup Check All");
      }
      else
      {
         this->mpc_Scene->SetAllNodesConnected(false);
         osc_write_log_performance_stop(u16_Timer, "System Commisioning Setup Uncheck All");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::m_ConfirmClicked(void)
{
   this->SetEditMode(!this->GetEditMode());
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::m_CancelClicked(void)
{
   // restore the saved view
   C_PuiSvHandler::h_GetInstance()->SetView(this->mu32_ViewIndex, this->mc_ViewCopy);
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->ReloadViewData();
      this->mpc_Scene->ClearUndoStack();
   }

   this->SetEditMode(false);
}
