//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit for list header (implementation)

   Text edit for list header

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QHelpEvent>
#include "C_OgeTedListHeaderBase.h"
#include "C_Uti.h"
#include "constants.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QFont C_OgeTedListHeaderBase::mhc_Font = mc_STYLE_GUIDE_FONT_REGULAR_12;

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
C_OgeTedListHeaderBase::C_OgeTedListHeaderBase(QWidget * const opc_Parent) :
   C_OgeTedContextMenuBase(opc_Parent),
   mq_JustGotFocus(false),
   mpc_ToolTip(NULL)
{
   this->installEventFilter(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTedListHeaderBase::~C_OgeTedListHeaderBase(void)
{
   delete mpc_ToolTip;
   mpc_ToolTip = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten focus in event slot

   Here: Signal, revert text and remember

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTedListHeaderBase::focusInEvent(QFocusEvent * const opc_Event)
{
   QTextEdit::focusInEvent(opc_Event);
   Q_EMIT this->SigFocus(true);
   m_ShowFullText();
   mq_JustGotFocus = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Here: Select all if necessary

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTedListHeaderBase::mousePressEvent(QMouseEvent * const opc_Event)
{
   QTextEdit::mousePressEvent(opc_Event);
   if (this->mq_JustGotFocus == true)
   {
      this->mq_JustGotFocus = false;
      this->selectAll();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten focus in event slot

   Here: Signal and reduce text

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTedListHeaderBase::focusOutEvent(QFocusEvent * const opc_Event)
{
   QTextEdit::focusOutEvent(opc_Event);
   Q_EMIT this->SigFocus(false);
   HideFullText();
   Q_EMIT this->SigEditFinished();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out]  opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeTedListHeaderBase::event(QEvent * const opc_Event)
{
   if (opc_Event->type() == QEvent::ToolTip)
   {
      //show tooltip
      if (mpc_ToolTip == NULL)
      {
         mpc_ToolTip = new stw_opensyde_gui::C_NagToolTip();
      }

      if (mpc_ToolTip->isVisible() == false)
      {
         if (this->toolTip().compare("") != 0)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            QHelpEvent * const pc_HelpEvent = dynamic_cast<QHelpEvent * const>(opc_Event);
            if (pc_HelpEvent != NULL)
            {
               mpc_ToolTip->move(this->mapToGlobal(pc_HelpEvent->pos()));
               mpc_ToolTip->SetContent(this->toolTip());
               mpc_ToolTip->show();
            }
         }
      }

      opc_Event->accept();
      return true;
   }
   else if (opc_Event->type() == QEvent::Leave)
   {
      //hide on leave
      if (this->mpc_ToolTip != NULL)
      {
         mpc_ToolTip->hide();
      }

      return QTextEdit::event(opc_Event);
   }
   else
   {
      return QTextEdit::event(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get complete text (text edit may contain only reduced comment)

   \return
   Complete user comment
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeTedListHeaderBase::GetFullText(void) const
{
   return mc_CompleteText;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save complete comment and replace by one line only

   \param[in]  oq_CheckForNewText   Flag if text needs an update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTedListHeaderBase::HideFullText(const bool oq_CheckForNewText)
{
   sintn sn_CharCount = 0;
   QString c_Text;
   const QFontMetrics c_FontMetrics(C_OgeTedListHeaderBase::mhc_Font);

   if (oq_CheckForNewText == true)
   {
      this->mc_CompleteText = this->toPlainText();
   }
   c_Text = C_Uti::h_AdaptStringToSize(this->mc_CompleteText, c_FontMetrics, static_cast<float64>(this->width()));
   //Reduce to one line
   for (QString::const_iterator pc_ItText = c_Text.begin(); pc_ItText != c_Text.end(); ++pc_ItText)
   {
      if (*pc_ItText == '\n')
      {
         c_Text.resize(sn_CharCount);
         c_Text += "...";
         break;
      }
      else
      {
         ++sn_CharCount;
      }
   }
   //Set reduced string
   this->setText(c_Text);
   //ToolTip
   if (c_Text.compare(this->mc_CompleteText) != 0)
   {
      this->setToolTip(this->mc_CompleteText);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reinsert complete comment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTedListHeaderBase::m_ShowFullText(void)
{
   this->setText(mc_CompleteText);
   //ToolTip
   this->setToolTip("");
}
