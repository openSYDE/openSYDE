//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog is shown if a encrypted service project is loaded

   Basically this is a slightly adapted implementation of C_OgeWiCustomMessage, but with a line edit, without details
   section.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "constants.hpp"
#include "stwtypes.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include <QGraphicsDropShadowEffect>
#include "C_PopPasswordDialogWidget.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "ui_C_PopPasswordDialogWidget.h"
#include "C_PuiProject.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in] opc_Parent    Parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PopPasswordDialogWidget::C_PopPasswordDialogWidget(QWidget * const opc_Parent) :
   QDialog(opc_Parent),
   mpc_Ui(new Ui::C_PopPasswordDialogWidget),
   ms32_MinHeight(QWIDGETSIZE_MAX) /*lint !e893 !e9130 !e9136*/,
   ms32_MinWidth(QWIDGETSIZE_MAX) //lint !e893 !e9130 !e9136
{
   this->mpc_Ui->setupUi(this);

   // no interaction with "background" window possible
   this->setWindowModality(Qt::ApplicationModal);

   // no frame
   this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);

   //transparency
   this->setAttribute(Qt::WA_TranslucentBackground);

   //shadow (adapted from C_OgePopUpDialog.cpp)
   {
      QColor c_Color;
      QGraphicsDropShadowEffect * pc_Shadow;

      pc_Shadow = new QGraphicsDropShadowEffect(this->mpc_Ui->pc_GroupBoxBackground);
      pc_Shadow->setBlurRadius(60.0);
      pc_Shadow->setXOffset(0.0);
      pc_Shadow->setYOffset(10.0);
      c_Color = mc_STYLE_GUIDE_COLOR_33;
      c_Color.setAlpha(180);
      pc_Shadow->setColor(c_Color);
      this->mpc_Ui->pc_GroupBoxBackground->setGraphicsEffect(pc_Shadow);
   } //lint !e429  //no memory leak because of the parent of pc_Shadow and the Qt memory management

   // set gray message box style
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_GroupBoxBackground, "Type", "QUESTION");

   // set style and behavior password line edit incl. toggle button
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditPassword, "NoRightBorder", true);
   this->mpc_Ui->pc_LineEditPassword->SetBackgroundColor(0);
   this->mpc_Ui->pc_LineEditPassword->setEchoMode(QLineEdit::Password);
   this->mpc_Ui->pc_PubTogglePwd->setIcon(QIcon("://images/main_page_and_navi_bar/Icon_password_show.svg"));
   this->mpc_Ui->pc_PubTogglePwd->SetBackgroundColor(0);

   // set key logo
   QPixmap c_Icon;
   const QString c_IconUrl = "://images/main_page_and_navi_bar/Icon_password_key.svg";
   c_Icon.load(c_IconUrl);
   this->mpc_Ui->pc_LabIcon->setPixmap(c_Icon.scaledToWidth(48, Qt::SmoothTransformation));

   InitStaticNames();

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_PopPasswordDialogWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this, &C_PopPasswordDialogWidget::m_CancelClicked);
   connect(this->mpc_Ui->pc_PubTogglePwd, &QPushButton::pressed, this,
           &C_PopPasswordDialogWidget::m_ShowPassword);
   connect(this->mpc_Ui->pc_PubTogglePwd, &QPushButton::released, this,
           &C_PopPasswordDialogWidget::m_HidePassword);

   // mouse events for box moving
   connect(this->mpc_Ui->pc_GroupBoxBackground, &C_OgeGbxMouseMove::SigMousePress, this,
           &C_PopPasswordDialogWidget::HandleMousePressEvent);
   connect(this->mpc_Ui->pc_GroupBoxBackground, &C_OgeGbxMouseMove::SigMouseMove, this,
           &C_PopPasswordDialogWidget::HandleMouseMoveEvent);
   connect(this->mpc_Ui->pc_GroupBoxBackground, &C_OgeGbxMouseMove::SigMouseRelease, this,
           &C_PopPasswordDialogWidget::HandleMouseReleaseEvent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PopPasswordDialogWidget::~C_PopPasswordDialogWidget(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Activate move window

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopPasswordDialogWidget::HandleMousePressEvent(const QMouseEvent * const opc_Event)
{
   mq_Pressed = true;
   this->mc_LastPos = this->mapToGlobal(opc_Event->pos());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  \brief   Deactivate move window

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopPasswordDialogWidget::HandleMouseReleaseEvent(const QMouseEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)
   mq_Pressed = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Move window

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopPasswordDialogWidget::HandleMouseMoveEvent(const QMouseEvent * const opc_Event)
{
   if (mq_Pressed == true)
   {
      const QPoint c_Cur = this->mapToGlobal(opc_Event->pos());
      const QPoint c_Diff = c_Cur - mc_LastPos;
      this->move(this->pos() + c_Diff);
      this->mc_LastPos = c_Cur;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopPasswordDialogWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabHeading->setText(C_GtGetText::h_GetText("SERVICE PROJECT AUTHENTIFICATION"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_PubTogglePwd->setText("");
   this->mpc_Ui->pc_LabPassword->setText(C_GtGetText::h_GetText("Password"));
   this->mpc_Ui->pc_LabProject->setText(C_GtGetText::h_GetText("Project"));
   this->mpc_Ui->pc_LabProjectName->setText(C_GtGetText::h_GetText(
                                               C_PuiProject::h_GetInstance()->GetName().toStdString().c_str()));

   // tooltip
   this->mpc_Ui->pc_LabPassword->SetToolTipInformation(C_GtGetText::h_GetText("Password"),
                                                       C_GtGetText::h_GetText(
                                                          "Enter password to gain access to Service Project."));

   // button tooltips
   this->mpc_Ui->pc_PubTogglePwd->SetToolTipInformation(C_GtGetText::h_GetText("Show/Hide Password"),
                                                        C_GtGetText::h_GetText(
                                                           "Shows/hides password when pressed down."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  returns entered password

   \return
   password as plain text
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PopPasswordDialogWidget::GetPassword() const
{
   return this->mpc_Ui->pc_LineEditPassword->text();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopPasswordDialogWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == false) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->m_OkClicked();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopPasswordDialogWidget::m_OkClicked(void)
{
   if (this->GetPassword() == "")
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Open Service Project"));
      c_Message.SetDescription(C_GtGetText::h_GetText("You must enter a password."));
      c_Message.SetOkButtonText(C_GtGetText::h_GetText("OK"));
      c_Message.SetCustomMinHeight(180, 180);

      if (c_Message.Execute() == C_OgeWiCustomMessage::eYES)
      {
         this->mpc_Ui->pc_LineEditPassword->setFocus();
      }
   }
   else
   {
      this->accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopPasswordDialogWidget::m_CancelClicked()
{
   this->reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for button show password click & hold
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopPasswordDialogWidget::m_ShowPassword()
{
   this->mpc_Ui->pc_LineEditPassword->setEchoMode(QLineEdit::Normal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for button show password release
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopPasswordDialogWidget::m_HidePassword()
{
   this->mpc_Ui->pc_LineEditPassword->setEchoMode(QLineEdit::Password);
   this->mpc_Ui->pc_LineEditPassword->setFocus();
}
