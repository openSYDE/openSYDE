//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing system view setup

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
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

// TODO BAY
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     19.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvSeSetupWidget::C_SyvSeSetupWidget(const uint32 ou32_ViewIndex, QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvSeSetupWidget),
   mpc_Scene(NULL),
   mu32_ViewIndex(ou32_ViewIndex),
   mq_EditModeActive(false)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   QString c_Name;

   mpc_Ui->setupUi(this);

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
   connect(this->mpc_Ui->pc_PbCheckAll, &stw_opensyde_gui_elements::C_OgePubTextOnlyEdit::clicked,
           this, &C_SyvSeSetupWidget::m_CheckAllClicked);
   connect(this->mpc_Ui->pc_PbCheckNone, &stw_opensyde_gui_elements::C_OgePubTextOnlyEdit::clicked,
           this, &C_SyvSeSetupWidget::m_CheckNoneClicked);
   connect(this->mpc_Ui->pc_PbConfirm, &stw_opensyde_gui_elements::C_OgePubTextWithBorderEdit::clicked,
           this, &C_SyvSeSetupWidget::m_ConfirmClicked);
   connect(this->mpc_Ui->pc_PbCancel, &stw_opensyde_gui_elements::C_OgePubTextWithBorderEdit::clicked,
           this, &C_SyvSeSetupWidget::m_CancelClicked);
   // Scene forwarding
   connect(this->mpc_Scene, &C_SebScene::SigChanged, this, &C_SyvSeSetupWidget::m_ViewChanged);
   connect(this->mpc_Scene, &C_SyvSeScene::SigErrorCheck, this, &C_SyvSeSetupWidget::m_CheckViewForErrorGeneral);

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

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     19.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvSeSetupWidget::~C_SyvSeSetupWidget(void)
{
   delete mpc_Ui;
   delete mpc_Scene;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initializes all visible strings on the widget

   \created     21.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvSeSetupWidget::InitText(void) const
{
   this->mpc_Ui->pc_PbCheckAll->setText(C_GtGetText::h_GetText("Check All"));
   this->mpc_Ui->pc_PbCheckNone->setText(C_GtGetText::h_GetText("Uncheck All"));
   this->mpc_Ui->pc_PbConfirm->setText(C_GtGetText::h_GetText("Confirm"));
   this->mpc_Ui->pc_PbCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save data

   \created     28.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Delayed load of scene

   \created     04.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvSeSetupWidget::LoadScene(void)
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->Load();

      this->SetEditMode(false);
      this->m_CheckViewForError(false);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle abort action

   \created     27.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvSeSetupWidget::Abort(void)
{
   m_CancelClicked();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the edit mode

   \param[in]     oq_Active      Flag for edit mode

   \created     20.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvSeSetupWidget::SetEditMode(const bool oq_Active, const bool oq_WithSave)
{
   this->mq_EditModeActive = oq_Active;

   this->mpc_Ui->pc_BackgroundWidget->SetEditBackground(oq_Active);
   this->mpc_Ui->pc_GroupBoxButtons->setVisible(oq_Active);

   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetDrawingBackground(oq_Active);
      this->mpc_Scene->SetEditMode(oq_Active);
   }

   this->mpc_Ui->pc_GraphicsView->SetDrawingBackground(oq_Active);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_DrawFrame, "NoBorder", oq_Active);

   if (oq_Active == true)
   {
      // create copy of view in case of clicking cancel
      this->mc_ViewCopy = *C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
      this->mpc_Ui->pc_BackgroundWidget->layout()->setContentsMargins(8, 0, 8, 8);
   }
   else
   {
      if (oq_WithSave == true)
      {
         this->Save();
      }
      if (this->mpc_Scene != NULL)
      {
         this->mpc_Scene->ClearUndoStack();
      }
      this->mpc_Ui->pc_BackgroundWidget->layout()->setContentsMargins(0, 0, 0, 8);
      Q_EMIT this->SigEditModeClosed();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the state of the edit mode

   \return
   true     edit mode is active
   true     edit mode is not active

   \created     20.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvSeSetupWidget::GetEditMode(void) const
{
   return this->mq_EditModeActive;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Starts the device configuration

   \created     23.11.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvSeSetupWidget::StartDeviceConfiguration(void)
{
   // TODO BAY: No Ethernet configuration available. The check must be removed again
   // Start Check for Ethernet
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
   bool q_CanInterface = false;

   if (pc_View != NULL)
   {
      const uint32 u32_BusIndex = pc_View->GetPcData().GetBusIndex();
      const stw_opensyde_core::C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(u32_BusIndex);

      if ((pc_Bus != NULL) &&
          (pc_Bus->e_Type == stw_opensyde_core::C_OSCSystemBus::eCAN))
      {
         q_CanInterface = true;
      }
   }
   // End Check for Ethernet

   if (q_CanInterface == false)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eWARNING);
      c_Message.SetHeading(C_GtGetText::h_GetText("Device Configuration via Ethernet is not available"));
      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "This tool version does not support the Ethernet device configuration. "
                                  "Will be supported soon. Please use a CAN bus for device configuration."));
      c_Message.Execute();
   }
   else
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
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function to prepare closing the scene

   \created     24.01.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvSeSetupWidget::PrepareToClose(void)
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->PrepareToClose();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overrided key press event

   \param[in,out] opc_event  Pointer to key event

   \created     04.11.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
void C_SyvSeSetupWidget::m_ViewChanged(void)
{
   this->m_CheckViewForError();
   Q_EMIT this->SigChanged();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle error check trigger

   \created     20.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvSeSetupWidget::m_CheckViewForErrorGeneral(void) const
{
   m_CheckViewForError();
}

//-----------------------------------------------------------------------------
void C_SyvSeSetupWidget::m_CheckViewForError(const bool oq_SendError) const
{
   QString c_ErrorText;
   const bool q_ViewSetupError = C_SyvUtil::h_CheckViewSetupError(this->mu32_ViewIndex, c_ErrorText);

   if (q_ViewSetupError == true)
   {
      this->mpc_Ui->pc_ErrorLabel->SetCompleteText(c_ErrorText);
      this->mpc_Ui->pc_ErrorFrame->setVisible(true);
      Q_EMIT this->SigEnableConfiguration(false);
   }
   else
   {
      this->mpc_Ui->pc_ErrorFrame->setVisible(false);
      Q_EMIT this->SigEnableConfiguration(true);
   }
   if (oq_SendError == true)
   {
      Q_EMIT this->SigCheckViewError(this->mu32_ViewIndex);
   }
}

//-----------------------------------------------------------------------------
void C_SyvSeSetupWidget::m_CheckAllClicked(void)
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetAllNodesConnected(true);
   }
}

//-----------------------------------------------------------------------------
void C_SyvSeSetupWidget::m_CheckNoneClicked(void)
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetAllNodesConnected(false);
   }
}

//-----------------------------------------------------------------------------
void C_SyvSeSetupWidget::m_ConfirmClicked(void)
{
   this->SetEditMode(false);
}

//-----------------------------------------------------------------------------
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
