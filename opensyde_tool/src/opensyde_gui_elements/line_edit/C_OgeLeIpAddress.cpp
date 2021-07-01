//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit for editing IP addresses.

   Line edit with input mask, input validator and improved look on focus in and out events.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgeLeIpAddress.h"
#include "TGLUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeLeIpAddress::C_OgeLeIpAddress(QWidget * const opc_Parent) :
   C_OgeLeProperties(opc_Parent)
{
   // set validator for IP address (source:
   // https://stackoverflow.com/questions/23166283/how-to-set-input-mask-and-qvalidator-to-a-qlineedit-at-a-time-in-qt
   // https://forum.qt.io/topic/83468/inputmask-and-validator-not-working-together/3)
   const QString c_Range =
      "(([ 0]+)|"                                     // only spaces or zeros
      "([ 0]*[0-9] *)|"                               // one digit, e.g. "9  " or " 08" or " 1 "; but not " 10"
      "([0-9][0-9] )|([ 0][0-9][0-9])|([0-9] [0-9])|" // two digits, e.g. "99 " or " 25" or "087" or "8 3"
      "(1[0-9][0-9])|([2][0-4][0-9])|(25[0-5]))";     // three digits, e.g. "255" or "179"; but not "678"
   const QRegularExpression c_Regex("^" + c_Range +
                                    "\\." + c_Range +
                                    "\\." + c_Range +
                                    "\\." + c_Range + "$");

   this->mpc_Validator = new QRegularExpressionValidator(c_Regex, this);
   this->setValidator(mpc_Validator);

   // use a mask in addition to the validator
   this->setInputMask("000.000.000.000");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeLeIpAddress::~C_OgeLeIpAddress()
{
   delete mpc_Validator;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set IP Address

   Convert uint8 to IP Address format.

   \param[in]  opu8_IPAddress    IP address in uint8 format (must point to 4 uint8s)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeIpAddress::SetIPAddress(const uint8 * const opu8_IPAddress)
{
   QString c_HelpString;

   // set IP address (padded with blanks)
   c_HelpString = "";
   c_HelpString.append(static_cast<QString>("%1").arg(opu8_IPAddress[0], 3, 10, QChar(' ')));
   c_HelpString.append(".");
   c_HelpString.append(static_cast<QString>("%1").arg(opu8_IPAddress[1], 3, 10, QChar(' ')));
   c_HelpString.append(".");
   c_HelpString.append(static_cast<QString>("%1").arg(opu8_IPAddress[2], 3, 10, QChar(' ')));
   c_HelpString.append(".");
   c_HelpString.append(static_cast<QString>("%1").arg(opu8_IPAddress[3], 3, 10, QChar(' ')));

   this->setText(c_HelpString);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get IP Address

   Convert string to int vector format.

   \return IP Address as vector
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sint32> C_OgeLeIpAddress::GetIPAddress(void) const
{
   std::vector<sint32> c_Return;

   const QString c_HelpString = this->text();
   const QStringList c_Parts = c_HelpString.split(".", Qt::SplitBehaviorFlags::SkipEmptyParts);

   tgl_assert(c_Parts.size() == 4);
   if (c_Parts.size() == 4)
   {
      c_Return.push_back(c_Parts[0].toInt());
      c_Return.push_back(c_Parts[1].toInt());
      c_Return.push_back(c_Parts[2].toInt());
      c_Return.push_back(c_Parts[3].toInt());
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden focus in event slot

   Here: show empty string instead of "  0.  0.  0.  0."

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeIpAddress::focusInEvent(QFocusEvent * const opc_Event)
{
   if (this->text() == "0.0.0.0")
   {
      this->clear();
   }
   C_OgeLeProperties::focusInEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden focus out event slot

   Here: replace empty parts with 0 and move ciphers to the right

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeIpAddress::focusOutEvent(QFocusEvent * const opc_Event)
{
   QStringList c_Parts = this->text().split(".");

   // replace *..* with *.  0.*, *.2.* with *.  2.* and *.23.* with *. 23.*
   for (sint32 s32_Pos = 0; s32_Pos < c_Parts.size(); s32_Pos++)
   {
      if (c_Parts[s32_Pos] == "")
      {
         c_Parts[s32_Pos] = "0";
      }
      c_Parts[s32_Pos] = c_Parts[s32_Pos].rightJustified(3, ' ');
   }

   this->setText(c_Parts.join("."));

   C_OgeLeProperties::focusOutEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden key press event slot

   Here: Switch IP address part on arrow button click,
   With "part" is meant: one of the four parts of the IP Address, separated by '.'.

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeIpAddress::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   // Moving forward
   if ((opc_KeyEvent->key() == static_cast<stw_types::sintn>(Qt::Key_Down)) ||
       (opc_KeyEvent->key() == static_cast<stw_types::sintn>(Qt::Key_Right)))
   {
      if (this->cursorPosition() > 11)
      {
         // in right-most part just move cursor one forward
         this->cursorForward(false, 1);
      }
      else
      {
         tgl_assert(this->displayText().size() == 15);
         tgl_assert(this->cursorPosition() >= 0);

         // if there exists a cipher on the next two positions, move cursor there
         if (this->displayText().at(this->cursorPosition() + 1).isNumber() == true)
         {
            this->cursorForward(false, 1);
         }
         else if (this->displayText().at(this->cursorPosition() + 2).isNumber() == true)
         {
            this->cursorForward(false, 2);
            // because '.' is no number this works for all cases
         }
         // jump to next part
         else
         {
            this->cursorForward(false, 4 - (this->cursorPosition() % 4));
         }
      }
   }
   // Moving backward
   else if ((opc_KeyEvent->key() == static_cast<stw_types::sintn>(Qt::Key_Up)) ||
            (opc_KeyEvent->key() == static_cast<stw_types::sintn>(Qt::Key_Left)))
   {
      if (this->cursorPosition() < 3)
      {
         // in left-most part just move cursor one backward
         this->cursorBackward(false, 1);
      }
      else
      {
         tgl_assert(this->displayText().size() == 15);
         tgl_assert(this->cursorPosition() >= 0);

         // moving backwards is a bit more defensive with jumps, to allow moving to every position by arrow keys
         const sint32 s32_MaximumJumpWidth = static_cast<sint32>(this->cursorPosition() % 4);
         sint32 s32_RealJumpWidth = s32_MaximumJumpWidth;

         // part-position 2 or 3:
         // move to first position or move single steps, depending on existence of ciphers
         if ((s32_MaximumJumpWidth == 2) || (s32_MaximumJumpWidth == 3))
         {
            // check all characters in this part for ciphers
            for (sint32 s32_Step = 0; s32_Step <= s32_MaximumJumpWidth; s32_Step++)
            {
               const sint32 s32_Help = static_cast<sint32>(this->cursorPosition()) - s32_Step;
               if (this->displayText().at(s32_Help).isNumber() == true)
               {
                  s32_RealJumpWidth = 1;
                  break;
               }
            }
         }
         // part-position 0:
         // move to previous part either in single steps or to first position, depending on existence of ciphers
         else if (s32_MaximumJumpWidth == 0)
         {
            s32_RealJumpWidth = 4;
            // check all characters in the previous part for ciphers
            for (sint32 s32_Step = 2; s32_Step < 5; s32_Step++)
            {
               const sint32 s32_Help = static_cast<sint32>(this->cursorPosition()) - s32_Step;
               if (this->displayText().at(s32_Help).isNumber() == true)
               {
                  s32_RealJumpWidth = 1;
                  break;
               }
            }
         }
         else
         {
            // part-position 1: always jump to first position in this part (which already is default)
         }

         this->cursorBackward(false, s32_RealJumpWidth);
      }
   }
   // handle end key manually because it does not behave correctly per default
   else if (opc_KeyEvent->key() == static_cast<stw_types::sintn>(Qt::Key_End))
   {
      this->setCursorPosition(15);
   }
   else
   {
      C_OgeLeProperties::keyPressEvent(opc_KeyEvent);

      // move one step forward on delete key (Usual behavior would be that text moves one step backwards,
      // but with the input mask this is not possible. To allow deletion of next character we have to move cursor)
      if (opc_KeyEvent->key() == static_cast<stw_types::sintn>(Qt::Key_Delete))
      {
         this->cursorForward(false, 1);
      }
   }
}
