//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Base class for all spin boxes (implementation)

   Base class for all spin boxes

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.06.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QHelpEvent>
#include "C_GtGetText.h"
#include "C_OgeSpxAllBase.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
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

   \created     19.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeSpxAllBase::C_OgeSpxAllBase() :
   C_OgeToolTipBase(),
   me_ToolTipAdditionalType(stw_opensyde_gui::C_NagToolTip::eDEFAULT)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set additional tooltip info

   \param[in] orc_Text     Additional text
   \param[in] oq_ErrorType New error type

   \created     20.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeSpxAllBase::SetToolTipAdditionalInfo(const QString & orc_Text,
                                               const stw_opensyde_gui::C_NagToolTip::E_Type oe_Type)
{
   this->mc_ToolTipAdditionalContent = orc_Text;
   this->me_ToolTipAdditionalType = oe_Type;
   m_ApplyToolTipInfo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Activate default range tool tip

   \created     02.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeSpxAllBase::ActivateDefaultToolTip(void)
{
   const QString c_Heading = C_GtGetText::h_GetText("Value Range");

   //Translation: 1=Minimum value, 2=Maximum value
   const QString c_Content =
      QString(C_GtGetText::h_GetText("Minimum: %1\nMaximum: %2")).arg(this->m_GetMinimum()).arg(this->m_GetMaximum());

   this->SetToolTipInformation(c_Heading, c_Content);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored

   \created     19.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OgeSpxAllBase::m_HandleEvent(QEvent * const opc_Event)
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

            if ((pc_HelpEvent != NULL) && (this->m_IsEnabled() == true))
            {
               this->m_SetMouseTracking(true);

               m_ApplyToolTipInfo();
               this->m_GetToolTip()->show();
               this->m_GetToolTip()->DoMove(pc_HelpEvent->globalPos());
            }
         }

         //Accept event because of Qt dynamic tooltip time based on the fact if there was a tooltip in this widget
         // already
         opc_Event->accept();
         q_Return = true;
      }
      else if (opc_Event->type() == QEvent::Leave)
      {
         //hide on leave
         this->m_HideToolTip();

         this->m_SetMouseTracking(false);

         opc_Event->accept();
         q_Return = true;
      }
      else
      {
         q_Return = m_CallBaseEvent(opc_Event);
      }
   }
   else
   {
      q_Return = m_CallBaseEvent(opc_Event);
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply updated tool tip info

   \created     20.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeSpxAllBase::m_ApplyToolTipInfo(void)
{
   if (this->m_GetToolTip() != NULL)
   {
      this->m_GetToolTip()->SetHeading(this->mc_ToolTipHeading);
      if (this->mc_ToolTipAdditionalContent.compare("") == 0)
      {
         this->m_GetToolTip()->SetContent(this->mc_ToolTipContent);
      }
      else
      {
         this->m_GetToolTip()->SetContent(this->mc_ToolTipAdditionalContent + "\n\n" + this->mc_ToolTipContent);
      }
      this->m_GetToolTip()->SetType(this->me_ToolTipAdditionalType);
   }
}
