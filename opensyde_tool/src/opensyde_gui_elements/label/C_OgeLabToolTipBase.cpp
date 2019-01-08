//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Label with tool tip (implementation)

   Label with tool tip

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.03.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QEvent>
#include <QHelpEvent>
#include "C_OgeLabToolTipBase.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_elements;

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

   \created     27.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeLabToolTipBase::C_OgeLabToolTipBase(QWidget * const opc_Parent) :
   QLabel(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored

   \created     27.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OgeLabToolTipBase::event(QEvent * const opc_Event)
{
   bool q_Return;

   if (this->mq_ToolTipActive == true)
   {
      if (opc_Event->type() == QEvent::ToolTip)
      {
         //show tooltip
         this->m_CreateToolTip();

         if (this->mpc_ToolTip->isVisible() == false)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            QHelpEvent * const pc_HelpEvent = dynamic_cast<QHelpEvent * const>(opc_Event);

            if (pc_HelpEvent != NULL)
            {
               this->setMouseTracking(true);

               Q_EMIT this->SigLastChanceToUpdateToolTip();

               this->mpc_ToolTip->SetHeading(this->mc_ToolTipHeading);
               this->mpc_ToolTip->SetContent(this->mc_ToolTipContent);
               this->mpc_ToolTip->SetType(this->me_ToolTipType);
               this->mpc_ToolTip->show();
               this->mpc_ToolTip->DoMove(pc_HelpEvent->globalPos());
            }
         }
         //Accept event because of Qt dynamic tooltip time based on the fact if there was a tooltip in this widget
         // already
         Q_EMIT this->SigHideOtherToolTips();
         opc_Event->accept();
         q_Return = true;
      }
      else if (opc_Event->type() == QEvent::Leave)
      {
         //hide on leave
         this->m_HideToolTip();

         this->setMouseTracking(false);

         opc_Event->accept();
         q_Return = true;
      }
      else
      {
         q_Return = QLabel::event(opc_Event);
      }
   }
   else
   {
      q_Return = QLabel::event(opc_Event);
   }

   return q_Return;
}
