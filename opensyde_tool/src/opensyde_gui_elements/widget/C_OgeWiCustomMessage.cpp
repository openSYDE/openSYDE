//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Dialog window for error, information, question and warning.

   Create a QDialog that can be used like a QMessageBox.
   Instead of one text-field there are three:
      heading: title of the box in bold capital letters
      description: actual message
      details: optional details in a read-only text edit

   Example usage:

   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
   c_Message.SetHeading(C_GtGetText::h_GetText("Please confirm: "));
   c_Message.SetDescription(C_GtGetText::h_GetText("Do you really want to steal the apple for eating it?"));
   C_OgeWiCustomMessage::E_Outputs e_Output;
   e_Output = c_Message.Execute();
   switch(e_Output)
   {
   case C_OgeWiCustomMessage::eYES:
      // Steal the apple and eat it.
      break;
   case C_OgeWiCustomMessage::eNO:
      // Steal the apple but do not eat it.
      break;
   case C_OgeWiCustomMessage::eCANCEL:
      // Do not steal the apple.
      break;
   default:
      break;
   }

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.04.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------
/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <cmath>
#include <QGraphicsDropShadowEffect>

#include "C_OgeWiCustomMessage.h"
#include "ui_C_OgeWiCustomMessage.h"

#include "constants.h"
#include "C_GtGetText.h"
#include "C_OgeWiUtil.h"
#include "C_OgeOverlay.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;
using namespace std;
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

   \created     16.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------

C_OgeWiCustomMessage::C_OgeWiCustomMessage(QWidget * const opc_Parent, const E_Type & ore_MessageType,
                                           const QString & orc_Description) :
   QDialog(opc_Parent),
   mpc_Ui(new Ui::C_OgeWiCustomMessage),
   mq_Pressed(false)
{
   mpc_Ui->setupUi(this);

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
      pc_Shadow->setBlurRadius(30.0);
      c_Color = mc_STYLE_GUIDE_COLOR_34;
      c_Color.setAlpha(128);
      pc_Shadow->setColor(c_Color);
      this->mpc_Ui->pc_GroupBoxBackground->setGraphicsEffect(pc_Shadow);
      //lint -e{429}  no memory leak because of the parent of pc_Shadow and the Qt memory management
   }

   // Mouse Events for Box moving
   connect(this->mpc_Ui->pc_GroupBoxBackground, &C_OgeGbxMouseMove::SigMousePress, this,
           &C_OgeWiCustomMessage::HandleMousePressEvent);
   connect(this->mpc_Ui->pc_GroupBoxBackground, &C_OgeGbxMouseMove::SigMouseMove, this,
           &C_OgeWiCustomMessage::HandleMouseMoveEvent);
   connect(this->mpc_Ui->pc_GroupBoxBackground, &C_OgeGbxMouseMove::SigMouseRelease, this,
           &C_OgeWiCustomMessage::HandleMouseReleaseEvent);

   // Initialize Buttons and Co
   m_InitButtons();

   // Initialize the message type (error, information, question, warning)
   me_MessageType = ore_MessageType;
   m_SetMessageType(me_MessageType);

   // Use the provided texts (heading, description, details)
   mc_Heading = "";
   mc_Description = orc_Description;
   mc_Details = "";

   // Initialize Text (header, description, details)
   m_SetText(mc_Heading, mc_Description, mc_Details);

   //set default min width
   this->SetCustomMinWidth(ms32_MESSAGE_BOX_DEFAULT_WIDTH);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     16.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
C_OgeWiCustomMessage::~C_OgeWiCustomMessage(void)
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Activate move window

   \param[in,out] opc_Event Event identification and information

   \created     17.04.2018  STW/G.Scupin adapted from preexisting code in C_OgePopUpDialog
*/
//-----------------------------------------------------------------------------

void C_OgeWiCustomMessage::HandleMousePressEvent(const QMouseEvent * const opc_Event)
{
   mq_Pressed = true;
   this->mc_LastPos = this->mapToGlobal(opc_Event->pos());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Deactivate move window

   \param[in,out] opc_Event Event identification and information

   \created     17.04.2018  STW/G.Scupin adapted from preexisting code in C_OgePopUpDialog
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::HandleMouseReleaseEvent(QMouseEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)
   mq_Pressed = false;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Move window

   \param[in,out] opc_Event Event identification and information

   \created     17.04.2018  STW/G.Scupin adapted from preexisting code in C_OgePopUpDialog
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::HandleMouseMoveEvent(const QMouseEvent * const opc_Event)
{
   if (mq_Pressed == true)
   {
      const QPoint c_Cur = this->mapToGlobal(opc_Event->pos());
      const QPoint c_Diff = c_Cur - mc_LastPos;
      this->move(this->pos() + c_Diff);
      this->mc_LastPos = c_Cur;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information

   \created     17.04.2018  STW/G.Scupin adapted from preexisting code in C_SyvUpNodePropertiesDialog
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->accept();
         me_Output = E_Outputs::eOK;
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

   // close window when escape key is pressed (did not work without this, and now safe cancel is called)
   if (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Escape))
   {
      me_Output = E_Outputs::eCANCEL;
      this->reject();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize the buttons.

   Initialize the buttons (visibility, connections, icons).

   \created     17.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::m_InitButtons(void)
{
   // Initialize the buttons text
   this->mpc_Ui->pc_ButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_ButtonNo->setText(C_GtGetText::h_GetText("NO"));
   this->mpc_Ui->pc_ButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_LabelDetails->setText(C_GtGetText::h_GetText("DETAILS"));

   // Initialize the arrow icon paths
   mc_PathExpandIcon = ":/images/BtnWidgetMax.png";
   mc_PathMinimizeIcon = ":/images/BtnWidgetMin.png";
   QIcon c_ExpandIcon = QIcon(mc_PathExpandIcon);

   // hide the cancel and no button (only needed for question)
   this->mpc_Ui->pc_ButtonNo->hide();
   this->mpc_Ui->pc_ButtonCancel->hide();
   mq_ShowCancelButton = false;

   // make details read only
   this->mpc_Ui->pc_TebDetails->setReadOnly(true);
   // hide the details at start
   this->mpc_Ui->pc_TebDetails->hide();
   this->mpc_Ui->pc_ButtonDetails->setIcon(c_ExpandIcon);

   // Show and hide the Details
   connect(this->mpc_Ui->pc_ButtonDetails, &QPushButton::clicked, this, &C_OgeWiCustomMessage::m_ExpandCollapseDetails);

   // Make the buttons behave similar to QToolButtonBox or QMessageBox
   me_Output = E_Outputs::eINVALID; // gets overwritten by a valid value like eOK or eNO when box is closed (by
                                    // button-click)
   connect(this->mpc_Ui->pc_ButtonOk, &QPushButton::clicked, this, &C_OgeWiCustomMessage::m_OKClicked);
   connect(this->mpc_Ui->pc_ButtonNo, &QPushButton::clicked, this, &C_OgeWiCustomMessage::m_NOClicked);
   connect(this->mpc_Ui->pc_ButtonCancel, &QPushButton::clicked, this, &C_OgeWiCustomMessage::m_CancelClicked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set color and icon according to the message type.

   Choose the icon and corresponding color for one of the four message types
   "error", "information", "question" and "warning". Furthermore show
   two or three buttons in case of question type.

   \param[in]     ore_MessageType         type of the message

   \created     17.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::m_SetMessageType(const E_Type & ore_MessageType)
{
   // reset to default
   mc_TypeString = "";
   this->mpc_Ui->pc_ButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_ButtonNo->hide();
   this->mpc_Ui->pc_ButtonCancel->hide();

   switch (ore_MessageType)
   {
   case E_Type::eERROR:
      mc_PixmapIcon = QPixmap(":/images/Error_Icon_MessageBox.svg");
      this->mpc_Ui->pc_WidgetFrame->SetBackgroundColor(24);
      mc_TypeString = "ERROR";
      break;

   case E_Type::eINFORMATION:
      mc_PixmapIcon = QPixmap(":/images/Info_Icon_MessageBox.svg");
      this->mpc_Ui->pc_WidgetFrame->SetBackgroundColor(79);
      mc_TypeString = "INFORMATION";
      break;

   case E_Type::eQUESTION:
      mc_PixmapIcon = QPixmap(":/images/Question_Icon_MessageBox.svg");
      this->mpc_Ui->pc_WidgetFrame->SetBackgroundColor(8);
      mc_TypeString = "QUESTION";

      // show the Cancel and NO Button and change text of OK button
      this->mpc_Ui->pc_ButtonOk->setText(C_GtGetText::h_GetText("YES"));
      this->mpc_Ui->pc_ButtonNo->show();
      if (mq_ShowCancelButton == true)
      {
         this->mpc_Ui->pc_ButtonCancel->show();
      }
      break;

   case E_Type::eWARNING:
      mc_PixmapIcon = QPixmap(":/images/Warning_Icon_MessageBox.svg");
      this->mpc_Ui->pc_WidgetFrame->SetBackgroundColor(179);
      mc_TypeString = "WARNING";
      break;
   }

   // set the icon
   this->mpc_Ui->pc_LabelIcon->setPixmap(mc_PixmapIcon.scaledToWidth(135, Qt::SmoothTransformation));

   // set the style
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_GroupBoxBackground, "Type", mc_TypeString);

   // reload the text, especially the heading (when no heading is provided, the default heading is the type and
   // therefore should change too)
   m_SetText(mc_Heading, mc_Description, mc_Details);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set the text of header, description and details.

   \param[in]     orc_Header           Header (e.g. INFORMATION: UNKNOWN INITIALIZATION PARAMETER!)
   \param[in]     orc_Description      Description
   \param[in]     orc_Details          Details (only shown if expanded)

   \created     17.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::m_SetText(const QString & orc_Heading, const QString & orc_Description,
                                     const QString & orc_Details)
{
   QString c_Heading = orc_Heading;
   // for retaining size of text edit if details are provided and no retaining else
   QSizePolicy c_SizePolicyRetain = this->mpc_Ui->pc_TebDetails->sizePolicy();

   // Take message type as heading if no header is provided or update heading to right type
   if ((c_Heading == "") || (c_Heading == "ERROR") || (c_Heading == "INFORMATION") ||
       (c_Heading == "QUESTION") || (c_Heading == "WARNING"))
   {
      c_Heading = mc_TypeString;
   }

   // Capitalize the header
   c_Heading = c_Heading.toUpper();
   mc_Heading = c_Heading;

   // set the window title and labels text
   this->setWindowTitle("openSYDE - " + c_Heading);
   this->mpc_Ui->pc_LabelHeading->setText(c_Heading);
   this->mpc_Ui->pc_LabelDescription->setText(orc_Description);

   // show details and expand-button only if there are any details
   if (orc_Details == "")
   {
      // do not retain size of text edit
      c_SizePolicyRetain.setRetainSizeWhenHidden(false);
      this->mpc_Ui->pc_TebDetails->setSizePolicy(c_SizePolicyRetain);
      this->mpc_Ui->pc_TebDetails->hide(); // actually not necessary
      this->mpc_Ui->pc_LabelDetails->hide();
      this->mpc_Ui->pc_ButtonDetails->hide();
   }
   else
   {
      // do retain size of text edit
      c_SizePolicyRetain.setRetainSizeWhenHidden(true);
      this->mpc_Ui->pc_TebDetails->setSizePolicy(c_SizePolicyRetain);
      this->mpc_Ui->pc_TebDetails->setText(orc_Details);
      this->mpc_Ui->pc_LabelDetails->show();
      this->mpc_Ui->pc_ButtonDetails->show();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Functionality of the arrow button (show/hide details)

   Show the details or hide them.

   \created     17.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::m_ExpandCollapseDetails(void) const
{
   // show or hide the details according to previous state
   if (this->mpc_Ui->pc_TebDetails->isHidden() == true)
   {
      const QIcon c_MinimizeIcon = QIcon(mc_PathMinimizeIcon);
      this->mpc_Ui->pc_TebDetails->show();
      this->mpc_Ui->pc_ButtonDetails->setIcon(c_MinimizeIcon);
   }
   else
   {
      const QIcon c_ExpandIcon = QIcon(mc_PathExpandIcon);
      this->mpc_Ui->pc_TebDetails->hide();
      this->mpc_Ui->pc_ButtonDetails->setIcon(c_ExpandIcon);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call execute and return more specific values.

   Call execute and return more specific values.
   The return value of exec() only returns 0 (rejected) or 1 (accepted), but
   we need three different return values (OK/YES, NO, Cancel).

   \return
   Returns one of the E_Output values.

   \created     23.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
C_OgeWiCustomMessage::E_Outputs C_OgeWiCustomMessage::Execute(void)
{
   const QWidget * pc_TopWidget = C_OgeOverlay::h_GetTopWidget();

   // store recent override cursor shape
   sintn sn_CursorShape = -1;

   if (QApplication::overrideCursor() != NULL)
   {
      sn_CursorShape = static_cast<sintn>(QApplication::overrideCursor()->shape());
      QApplication::restoreOverrideCursor();
   }

   //Center dialog
   if (pc_TopWidget != NULL)
   {
      QPoint c_DialogCenter = mapToGlobal(this->rect().center());
      QPoint c_ParentWindowCenter = pc_TopWidget->window()->mapToGlobal(pc_TopWidget->window()->rect().center());
      this->move(c_ParentWindowCenter - c_DialogCenter);
   }

   // Execute
   this->exec();

   if (sn_CursorShape != -1)
   {
      QApplication::setOverrideCursor(QCursor(static_cast<Qt::CursorShape>(sn_CursorShape)));
   }

   return me_Output;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot for OK or YES Button

   \created     23.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::m_OKClicked(void)
{
   this->accept();
   me_Output = E_Outputs::eOK;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot for NO Button

   \created     23.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::m_NOClicked(void)
{
   this->reject();
   me_Output = E_Outputs::eNO;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot for Cancel Button

   \created     23.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::m_CancelClicked(void)
{
   this->reject();
   me_Output = E_Outputs::eCANCEL;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set a new message type (error, information, question, warning)

   \param[in]     ore_MessageType       type of the message

   \created     24.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::SetType(const E_Type & ore_MessageType)
{
   me_MessageType = ore_MessageType;
   m_SetMessageType(me_MessageType);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set a new heading.

   Analog to QMessageBox::setText, but here we have 3 different text fields
   (heading, description, details).

   \param[in]     orc_Text         heading string

   \created     24.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::SetHeading(const QString & orc_Text)
{
   mc_Heading = orc_Text;
   m_SetText(mc_Heading, mc_Description, mc_Details);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set a new description.

   Analog to QMessageBox::setText, but here we have 3 different text fields
   (heading, description, details).

   \param[in]     orc_Text         description string

   \created     24.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::SetDescription(const QString & orc_Text)
{
   mc_Description = orc_Text;
   m_SetText(mc_Heading, mc_Description, mc_Details);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new details.

   Analog to QMessageBox::setText, but here we have 3 different text fields
   (heading, description, details).

   \param[in]     orc_Text         details string

   \created     24.04.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::SetDetails(const QString & orc_Text)
{
   mc_Details = orc_Text;
   m_SetText(mc_Heading, mc_Description, mc_Details);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new button text of OK button.

   \param[in]     orc_Text         button text string

   \return
   possible return value(s) and description

   \created     04.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::SetOKButtonText(const QString & orc_Text) const
{
   this->mpc_Ui->pc_ButtonOk->setText(orc_Text);
   //   this->mpc_Ui->pc_ButtonOk->setStyleSheet("padding-left: 15px; padding-right: 15px");
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new button text of NO-button and show button.

   \param[in]     orc_Text         button text string

   \return
   possible return value(s) and description

   \created     04.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::SetNOButtonText(const QString & orc_Text) const
{
   this->mpc_Ui->pc_ButtonNo->setText(orc_Text);
   this->mpc_Ui->pc_ButtonNo->setStyleSheet("padding-left: 15px; padding-right: 15px");

   // if a special button text is set, the button should be shown too
   this->mpc_Ui->pc_ButtonNo->show();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new button text of Cancel-button and show button.

   \param[in]     orc_Text         button text string

   \return
   possible return value(s) and description

   \created     04.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::SetCancelButtonText(const QString & orc_Text)
{
   this->mpc_Ui->pc_ButtonCancel->setText(orc_Text);
   this->mpc_Ui->pc_ButtonCancel->setStyleSheet("padding-left: 15px; padding-right: 15px");

   // make the button visible
   ShowCancelButton();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set the Cancel-button visible.

   The cancel button is per default not shown, because in most cases it has the
   same function as the NO-button.

   \created     07.05.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::ShowCancelButton(void)
{
   mq_ShowCancelButton = true;
   this->mpc_Ui->pc_ButtonCancel->show();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set custom minimum width (and so custom width) of message box.

   This can be used for message boxes where the caller already knows
   that there will be much text. Please check resulting message box if
   using this method, because you may break the layout.

   \param[in]     ors32_MaxWidth        new minimum width

   \created     19.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::SetCustomMinWidth(const sint32 & ors32_MinWidth) const
{
   this->mpc_Ui->pc_GroupBoxBackground->setMinimumWidth(ors32_MinWidth);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set custom minimum height (and so custom height) of message box.

   This can be used for message boxes where the caller already knows
   that there will be much text. Please check resulting message box if
   using this method, because you may break the layout.

   \param[in]     ors32_MaxHeight        new minimum height

   \created     19.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_OgeWiCustomMessage::SetCustomMinHeight(const sint32 & ors32_MinHeight) const
{
   this->mpc_Ui->pc_GroupBoxBackground->setMinimumHeight(ors32_MinHeight);

   // reset maximum height of details field so it can use the whole space
   //lint -e1960 we can not change Qt constant but it is still better than using the hard coded magic number 16777215
   this->mpc_Ui->pc_TebDetails->setMaximumHeight(QWIDGETSIZE_MAX);
}
