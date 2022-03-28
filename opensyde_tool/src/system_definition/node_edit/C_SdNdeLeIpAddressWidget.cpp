//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for editing one single IPv4 address.

   This widget holds four separate line edits and three labels to build up an IPv4 address.

   The appropriate source code is from:
   https://stackoverflow.com/questions/9306335/an-ip-address-widget-for-qt-similar-to-mfcs-ip-address-control

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdNdeLeIpAddressWidget.h"
#include "ui_C_SdNdeLeIpAddressWidget.h"
#include <QRegularExpression>
#include "TGLUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_types;

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
C_SdNdeLeIpAddressWidget::C_SdNdeLeIpAddressWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeLeIpAddressWidget)
{
   this->mpc_Ui->setupUi(this);
   this->mpc_Ui->pc_GroupBox->setTitle("");

   // assign line edits
   this->mapc_LineEdit[0] = this->mpc_Ui->pc_LineEditIpPart1;
   this->mapc_LineEdit[1] = this->mpc_Ui->pc_LineEditIpPart2;
   this->mapc_LineEdit[2] = this->mpc_Ui->pc_LineEditIpPart3;
   this->mapc_LineEdit[3] = this->mpc_Ui->pc_LineEditIpPart4;

   for (uint32 u32_Pos = 0; u32_Pos < static_cast<uint32>(mhsn_IPV4SIZE); ++u32_Pos)
   {
      this->mapc_LineEdit[u32_Pos]->setFocusPolicy(Qt::StrongFocus);
      this->mapc_LineEdit[u32_Pos]->installEventFilter(this);

      this->mapc_LineEdit[u32_Pos]->setFrame(false);
      this->mapc_LineEdit[u32_Pos]->setAlignment(Qt::AlignCenter);

      // set validator for ip address
      const QString c_Range =
         "(([ 0]+)|"                                     // only spaces or zeros
         "([ 0]*[0-9] *)|"                               // one digit, e.g. "9  " or " 08" or " 1 "; but not " 10"
         "([0-9][0-9] )|([ 0][0-9][0-9])|([0-9] [0-9])|" // two digits, e.g. "99 " or " 25" or "087" or "8 3"
         "(1[0-9][0-9])|([2][0-4][0-9])|(25[0-5]))";     // three digits, e.g. "255" or "179"; but not "678"
      const QRegularExpression c_Regex("^" + c_Range + "$");
      this->mapc_Validator[u32_Pos] = new QRegularExpressionValidator(c_Regex, this->mapc_LineEdit[u32_Pos]);
      this->mapc_LineEdit[u32_Pos]->setValidator(this->mapc_Validator[u32_Pos]);

      // signal connects
      connect(dynamic_cast<C_OgeLeIpAddress *>(this->mapc_LineEdit[u32_Pos]),
              &C_OgeLeIpAddress::SignalFocusIn,
              this, &C_SdNdeLeIpAddressWidget::m_FocusInSlot);
      connect(dynamic_cast<C_OgeLeIpAddress *>(this->mapc_LineEdit[u32_Pos]), &C_OgeLeIpAddress::SignalFocusOut,
              this, &C_SdNdeLeIpAddressWidget::m_FocusOutSlot);
   }

   this->mpc_Ui->pc_LabelDot1->SetFontPixel(13, false, false);
   this->mpc_Ui->pc_LabelDot2->SetFontPixel(13, false, false);
   this->mpc_Ui->pc_LabelDot3->SetFontPixel(13, false, false);
   this->SetError(false);

   // signal connects
   connect(this, &C_SdNdeLeIpAddressWidget::SignalTextChanged,
           this, &C_SdNdeLeIpAddressWidget::SlotTextChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeLeIpAddressWidget::~C_SdNdeLeIpAddressWidget()
{
   for (uint32 u32_Pos = 0; u32_Pos < mhsn_IPV4SIZE; ++u32_Pos)
   {
      delete this->mapc_Validator[u32_Pos];
   }
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Implements cursor position of IPv4 address field behaviour when user is typing.

   \param[in]       opc_LineEdit     Currently edited line edit. Detailed input parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeLeIpAddressWidget::SlotTextChanged(const QLineEdit * const opc_LineEdit)
{
   for (uint32 u32_Pos = 0; u32_Pos != mhsn_IPV4SIZE; ++u32_Pos)
   {
      if (opc_LineEdit == this->mapc_LineEdit[u32_Pos])
      {
         if (((opc_LineEdit->text().size() + 1) >= mhsn_IPV4MAXDIGITS) && // +1 since we are hanging one digit behind
             (opc_LineEdit->text().size() == opc_LineEdit->cursorPosition()) &&
             (opc_LineEdit->hasSelectedText() == false)) // we only want to move further if line edit is not selected
         {
            // auto-move to next item
            if ((u32_Pos + 1) < mhsn_IPV4SIZE)
            {
               this->mapc_LineEdit[u32_Pos + 1]->setFocus();
               this->mapc_LineEdit[u32_Pos + 1]->selectAll();
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns all four line edits of this ip address widget

   \return
   vector of QLineEdit pointers of this widget
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<QLineEdit *> C_SdNdeLeIpAddressWidget::GetLineEdits(void)
{
   std::vector<QLineEdit *> c_LineEdits;

   for (uint32 u32_Pos = 0; u32_Pos != mhsn_IPV4SIZE; ++u32_Pos)
   {
      c_LineEdits.push_back(this->mapc_LineEdit[u32_Pos]);
   }

   return c_LineEdits;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks if given line edit is part of this widget

   \param[in]       opc_LineEdit     Pointer of QLineEdit to check if it is part of this widget.

   \return
   bool

   \retval   true             Given line edit is part of this widget.
   \retval   false                  -"-     is not part   -"-
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeLeIpAddressWidget::IsLineEditPartOfWidget(const QLineEdit * const opc_LineEdit) const
{
   bool q_IsPartOfWidget = false;

   for (uint32 u32_Pos = 0; u32_Pos != mhsn_IPV4SIZE; ++u32_Pos)
   {
      if (this->mapc_LineEdit[u32_Pos] == opc_LineEdit)
      {
         q_IsPartOfWidget = true;
         break;
      }
   }

   return q_IsPartOfWidget;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Focus in slot to show empty string instead of "  0.  0.  0.  0"

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeLeIpAddressWidget::m_FocusInSlot(void)
{
   bool q_FlagZero = true;

   for (uint32 u32_Pos = 0; u32_Pos < mhsn_IPV4SIZE; ++u32_Pos)
   {
      if (this->mapc_LineEdit[u32_Pos]->text().trimmed() != "0")
      {
         q_FlagZero = false;
      }
   }

   if (q_FlagZero == true)
   {
      for (uint32 u32_Pos = 0; u32_Pos < mhsn_IPV4SIZE; ++u32_Pos)
      {
         // clear entry
         this->mapc_LineEdit[u32_Pos]->setText("");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Focus out slot to replace empty parts with "0"

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeLeIpAddressWidget::m_FocusOutSlot(void)
{
   bool q_FlagHasFocus = false;

   for (uint32 u32_Pos = 0; u32_Pos < mhsn_IPV4SIZE; ++u32_Pos)
   {
      if (this->mapc_LineEdit[u32_Pos]->hasFocus() == true)
      {
         q_FlagHasFocus = true;
      }
   }

   if (q_FlagHasFocus == false)
   {
      for (uint32 u32_Pos = 0; u32_Pos < mhsn_IPV4SIZE; ++u32_Pos)
      {
         this->m_RemoveLeadingZeroes(this->mapc_LineEdit[u32_Pos]);
         if (this->mapc_LineEdit[u32_Pos]->text().trimmed() == "")
         {
            this->mapc_LineEdit[u32_Pos]->setText("0");
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Removes leading zeroes of one cell.

   E.g. "007" --> "7"

   \param[in]       opc_LineEdit     Current line edit to remove leading zeroes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeLeIpAddressWidget::m_RemoveLeadingZeroes(QLineEdit * const opc_LineEdit)
{
   // remove leading zeros '0' from current cell
   QString c_Tmp =  opc_LineEdit->text();

   c_Tmp = c_Tmp.remove(QRegExp("^[0]*"));
   if (c_Tmp.isEmpty() == true)
   {
      // leave at least one '0' left
      c_Tmp = "0";
   }
   opc_LineEdit->setText(c_Tmp);
}

// eventFilter is inherited therefore QObject and QEvent pointer is not const
//lint -save -e952
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden event filter needed to implement special behaviour of line edits for IPv4 address.

   \param[in]       opc_Obj    Watched QObject of event filter.
   \param[in,out]   opc_Event  Filtered Qt event.

   \return
   bool

   \retval   true             Further event processing stopped, later event filters don't see the event at all.
   \retval   false            Further event processing allowed, the event is sent to the target object itself.
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeLeIpAddressWidget::eventFilter(QObject * opc_Obj, QEvent * opc_Event)
{
   const bool q_Result = QWidget::eventFilter(opc_Obj, opc_Event);

   if (opc_Event->type() == QEvent::KeyPress)
   {
      QKeyEvent * const pc_KeyEvent = dynamic_cast<QKeyEvent *>(opc_Event);
      if (pc_KeyEvent != NULL)
      {
         for (uint32 u32_Pos = 0; u32_Pos < mhsn_IPV4SIZE; ++u32_Pos)
         {
            QLineEdit * const pc_LineEdit = this->mapc_LineEdit[u32_Pos];

            if (pc_LineEdit == opc_Obj)
            {
               if (pc_KeyEvent->matches(QKeySequence::SelectAll) == true)
               {
                  this->mapc_LineEdit[u32_Pos]->selectAll();
               }
               else
               {
                  // all other keys
                  switch (pc_KeyEvent->key())
                  {
                  case Qt::Key_Left:
                     if (pc_LineEdit->cursorPosition() == 0)
                     {
                        // user wants to move to previous item
                        m_MovePrevLineEdit(u32_Pos);
                     }
                     break;

                  case Qt::Key_Right:
                     if (pc_LineEdit->text().isEmpty() || (pc_LineEdit->text().size() == pc_LineEdit->cursorPosition()))
                     {
                        // user wants to move to next item
                        m_MoveNextLineEdit(u32_Pos);
                     }
                     break;
                  case Qt::Key_Backspace:
                     if (pc_LineEdit->text().isEmpty() || (pc_LineEdit->cursorPosition() == 0))
                     {
                        // user wants to move to previous item
                        m_MovePrevLineEdit(u32_Pos);
                     }
                     break;
                  case Qt::Key_Tab:
                  case Qt::Key_Control:
                     // do nothing here
                     break;
                  case Qt::Key_Comma:
                  case Qt::Key_Period:
                     m_MoveNextLineEdit(u32_Pos);
                     break;
                  default:
                     Q_EMIT SignalTextChanged(pc_LineEdit);
                     break;
                  }
               }
            }
         }
      }
   }

   return q_Result;
}
//lint -restore

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets error color of labels for dots of IPv4 address.

   \param[in]       oq_Error   Error flag, true means error occurred.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeLeIpAddressWidget::SetError(const bool oq_Error) const
{
   if (oq_Error == true)
   {
      this->mpc_Ui->pc_LabelDot1->SetForegroundColor(24);
      this->mpc_Ui->pc_LabelDot2->SetForegroundColor(24);
      this->mpc_Ui->pc_LabelDot3->SetForegroundColor(24);
   }
   else
   {
      this->mpc_Ui->pc_LabelDot1->SetForegroundColor(6);
      this->mpc_Ui->pc_LabelDot2->SetForegroundColor(6);
      this->mpc_Ui->pc_LabelDot3->SetForegroundColor(6);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Moving to next line edit of IPv4 address.

   \param[in]       ou32_Pos   one of the first three IPv4 address line edits, ranging from 0 to 2.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeLeIpAddressWidget::m_MoveNextLineEdit(const uint32 ou32_Pos)
{
   if ((ou32_Pos + 1) < mhsn_IPV4SIZE)
   {
      this->mapc_LineEdit[ou32_Pos + 1]->setFocus();
      this->mapc_LineEdit[ou32_Pos + 1]->setCursorPosition(0);
      this->mapc_LineEdit[ou32_Pos + 1]->selectAll();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Moving to previous line edit of IPv4 address.

   \param[in]       ou32_Pos   one of the last three IPv4 address line edits, ranging from 1 to 3.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeLeIpAddressWidget::m_MovePrevLineEdit(const uint32 ou32_Pos)
{
   if (ou32_Pos > 0)
   {
      // pointer out of bounds is secured by if-clause
      //lint -save -e661
      this->mapc_LineEdit[ou32_Pos - 1]->setFocus();
      this->mapc_LineEdit[ou32_Pos - 1]->setCursorPosition(this->mapc_LineEdit[ou32_Pos - 1]->text().size());
      //lint -restore
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set IP Address

   Convert uint8 to IP Address format.

   \param[in]  opu8_IPAddress    IP address in uint8 format (must point to 4 uint8s)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeLeIpAddressWidget::SetIPAddress(const uint8 * const opu8_IPAddress)
{
   for (uint32 u32_Pos = 0; u32_Pos < mhsn_IPV4SIZE; ++u32_Pos)
   {
      this->mapc_LineEdit[u32_Pos]->setText(static_cast<QString>("%1").arg(opu8_IPAddress[u32_Pos], 0, 10, QChar(' ')));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get IP Address

   Convert string to int vector format.

   \return IP Address as vector
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sint32> C_SdNdeLeIpAddressWidget::GetIPAddress(void) const
{
   std::vector<sint32> c_Return;

   for (uint32 u32_Pos = 0; u32_Pos < mhsn_IPV4SIZE; ++u32_Pos)
   {
      c_Return.push_back(this->mapc_LineEdit[u32_Pos]->text().toInt());
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Jump to one of four IPv4 address line edits by given position from 0 to 3.
 *
 *  \param[in]  ou32_Pos        Position of one of the four IPv4 address line edits starting from 0 to 3.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeLeIpAddressWidget::SetCursorPosition(const uint32 ou32_Pos)
{
   if (ou32_Pos < mhsn_IPV4SIZE)
   {
      this->mapc_LineEdit[ou32_Pos]->setFocus();
      this->mapc_LineEdit[ou32_Pos]->setCursorPosition(0);
      this->mapc_LineEdit[ou32_Pos]->selectAll();
   }
}
