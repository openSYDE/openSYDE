//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing system view setup

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#include "precomp_headers.h"

#include "stwerrors.h"

#include "C_SyvSeSetupWidget.h"

#include "ui_C_SyvSeSetupWidget.h"

#include "C_SyvUtil.h"
#include "C_GtGetText.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSvHandler.h"
#include "C_UsHandler.h"
#include "C_PuiSdHandler.h"
#include "C_OSCNode.h"
#include "C_SyvDcWidget.h"
#include "C_OgePopUpDialog.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

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
C_SyvSeSetupWidget::C_SyvSeSetupWidget(const uint32 ou32_ViewIndex, QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvSeSetupWidget),
   mpc_Scene(NULL),
   mu32_ViewIndex(ou32_ViewIndex),
   mq_EditModeActive(false),
   mq_IgnoreSelectAllCheckboxChanges(false)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   QString c_Name;

   mpc_Ui->setupUi(this);

   //Style error label
   this->mpc_Ui->pc_ErrorLabelTitle->SetForegroundColor(24);
   this->mpc_Ui->pc_ErrorLabelTitle->SetFontPixel(14, false, true);
   this->mpc_Ui->pc_ErrorLabelIcon->SetSvg("://images/Error_iconV2.svg");

   //Handle button icons
   this->mpc_Ui->pc_PbCancel->SetSvg("://images/system_views/IconTabCloseClickedDark.svg");
   this->mpc_Ui->pc_PbCancel->setIconSize(QSize(16, 16));
   this->mpc_Ui->pc_PbCancel->SetMargins(14, 23);

   //Remove debug label
   this->mpc_Ui->pc_GroupBoxButtons->setTitle("");

   // create scene for graphics view
   this->mpc_Scene = new C_SyvSeScene(mu32_ViewIndex);
   //And set it again after creation. Some changes have to be done on scene too.
   this->SetEditMode(false, false);

   // configure scene
   this->mpc_Scene->setSceneRect(0.0, 0.0,
                                 static_cast<float64>(this->mpc_Ui->pc_GraphicsView->width()),
                                 static_cast<float64>(this->mpc_Ui->pc_GraphicsView->height()));
   this->mpc_Ui->pc_GraphicsView->SetSceneAndConnect(this->mpc_Scene);

   this->InitText();

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
   connect(this->mpc_Ui->pc_CheckBoxSelectAll, &stw_opensyde_gui_elements::C_OgeChxSystemCommisioningEdit::stateChanged,
           this, &C_SyvSeSetupWidget::m_SelectAllStateChanged);
   connect(this->mpc_Ui->pc_PbConfirm, &stw_opensyde_gui_elements::C_OgePubSystemCommissioningEdit::clicked,
           this, &C_SyvSeSetupWidget::m_ConfirmClicked);
   connect(this->mpc_Ui->pc_PbCancel, &stw_opensyde_gui_elements::C_OgePubSystemCommissioningEdit::clicked,
           this, &C_SyvSeSetupWidget::m_CancelClicked);
   // Scene forwarding
   connect(this->mpc_Scene, &C_SebScene::SigChanged, this, &C_SyvSeSetupWidget::m_ViewChanged);
   connect(this->mpc_Scene, &C_SyvSeScene::SigErrorCheck, this, &C_SyvSeSetupWidget::m_CheckViewForErrorGeneral);
   connect(this->mpc_Scene, &C_SyvSeScene::SigConnectionChange, this, &C_SyvSeSetupWidget::m_OnViewConnectionChange);

   //Update all items with initial zoom & pos value
   if (pc_View != NULL)
   {
      c_Name = pc_View->GetName();
   }
   else
   {
      c_Name = "";
   }
   this->mpc_Ui->pc_GraphicsView->SetZoomValue(C_UsHandler::h_GetInstance()->GetProjSvSetupView(
                                                  c_Name).sn_SetupViewZoom, true);
   this->mpc_Ui->pc_GraphicsView->SetViewPos(C_UsHandler::h_GetInstance()->GetProjSvSetupView(c_Name).c_SetupViewPos);
   this->m_CheckViewForError();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvSeSetupWidget::~C_SyvSeSetupWidget(void)
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
      C_UsHandler::h_GetInstance()->SetProjSvSetupViewZoom(
         pc_View->GetName(), this->mpc_Ui->pc_GraphicsView->GetZoomValue());
      C_UsHandler::h_GetInstance()->SetProjSvSetupViewPos(pc_View->GetName(),
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

   \param[in]     oq_Active      Flag for edit mode
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
      this->mpc_Ui->pc_PbConfirm->setIconSize(QSize(22, 22));
      this->mpc_Ui->pc_PbConfirm->SetMargins(12, 20);
      this->mpc_Ui->pc_PbConfirm->setText(C_GtGetText::h_GetText("Confirm"));

      // create copy of view in case of clicking cancel
      this->mc_ViewCopy = *C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   }
   else
   {
      //Handle button
      this->mpc_Ui->pc_PbConfirm->SetSvg("://images/main_page_and_navi_bar/IconEdit.svg");
      this->mpc_Ui->pc_PbConfirm->setIconSize(QSize(24, 24));
      this->mpc_Ui->pc_PbConfirm->SetMargins(16, 25);
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
   c_New->SetSize(QSize(900, 743));

   c_New->exec();

   if (c_New != NULL)
   {
      pc_Dialog->CleanUp();
      c_New->HideOverlay();
   }
   //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
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
/*! \brief   Overrided key press event

   \param[in,out] opc_event  Pointer to key event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::keyPressEvent(QKeyEvent * const opc_Event)
{
   if ((opc_Event->key() == static_cast<sintn>(Qt::Key_Escape)) &&
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
   Q_EMIT this->SigChanged();
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
   const bool q_ViewSetupError =
      C_SyvUtil::h_CheckViewSetupError(this->mu32_ViewIndex, c_ErrorTextHeading, c_ErrorText, c_ErrorTextTooltip);

   if (q_ViewSetupError == true)
   {
      this->mpc_Ui->pc_ErrorLabelIcon->SetToolTipInformation("", c_ErrorTextTooltip, C_NagToolTip::eERROR);
      this->mpc_Ui->pc_ErrorLabelTitle->setText(c_ErrorTextHeading);
      this->mpc_Ui->pc_ErrorLabelTitle->SetToolTipInformation("", c_ErrorTextTooltip, C_NagToolTip::eERROR);
      this->mpc_Ui->pc_ErrorLabel->SetCompleteText(c_ErrorText, c_ErrorTextTooltip);
      this->mpc_Ui->pc_GroupBoxErrorContent->setVisible(true);
      Q_EMIT this->SigEnableConfiguration(false);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxErrorContent->setVisible(false);
      Q_EMIT this->SigEnableConfiguration(true);
   }
   if (oq_SendError == true)
   {
      Q_EMIT this->SigCheckViewError(this->mu32_ViewIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle change of any node connectionn state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeSetupWidget::m_OnViewConnectionChange(void)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const std::vector<uint8> & rc_ActiveFlags = pc_View->GetNodeActiveFlags();
      bool q_AllActive = true;
      for (uint32 u32_ItActive = 0UL; u32_ItActive < rc_ActiveFlags.size(); ++u32_ItActive)
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
      if (this->mpc_Ui->pc_CheckBoxSelectAll->isChecked())
      {
         this->mpc_Scene->SetAllNodesConnected(true);
      }
      else
      {
         this->mpc_Scene->SetAllNodesConnected(false);
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
