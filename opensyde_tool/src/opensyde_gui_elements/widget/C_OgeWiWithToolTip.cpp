//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with tool tip functionality (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QEvent>
#include <QHelpEvent>

#include "C_OgeWiWithToolTip.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_elements;

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
C_OgeWiWithToolTip::C_OgeWiWithToolTip(QWidget * const opc_Parent) :
   QWidget(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hides the actual tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiWithToolTip::HideToolTip(void)
{
   this->m_HideToolTip();

   this->setMouseTracking(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeWiWithToolTip::event(QEvent * const opc_Event)
{
   bool q_Return;

   if (this->mq_ToolTipActive == true)
   {
      if (opc_Event->type() == QEvent::ToolTip)
      {
         //show tooltip
         if (this->m_GetToolTip()->isVisible() == false)
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            QHelpEvent * const pc_HelpEvent = dynamic_cast<QHelpEvent * const>(opc_Event);

            if (pc_HelpEvent != NULL)
            {
               this->setMouseTracking(true);

               this->m_GetToolTip()->SetHeading(this->mc_ToolTipHeading);
               this->m_GetToolTip()->SetContent(this->mc_ToolTipContent);
               this->m_GetToolTip()->SetType(this->me_ToolTipType);
               this->m_GetToolTip()->show();
               this->m_GetToolTip()->DoMove(this->mapToGlobal(pc_HelpEvent->pos()));
            }
         }

         Q_EMIT this->SigHideOtherToolTips();
         opc_Event->accept();
         q_Return = true;
      }
      else if (opc_Event->type() == QEvent::Leave)
      {
         //hide on leave
         this->HideToolTip();

         opc_Event->accept();
         q_Return = true;
      }
      else
      {
         q_Return = QWidget::event(opc_Event);
      }
   }
   else
   {
      q_Return = QWidget::event(opc_Event);
   }

   return q_Return;
}
