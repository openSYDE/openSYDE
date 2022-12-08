//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all spin boxes (implementation)

   Base class for all spin boxes

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QHelpEvent>
#include "C_GtGetText.hpp"
#include "C_OgeSpxAllBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSpxAllBase::C_OgeSpxAllBase() :
   C_OgeToolTipBase(),
   me_ToolTipAdditionalType(stw::opensyde_gui::C_NagToolTip::eDEFAULT)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set additional tooltip info

   \param[in]  orc_Text    Additional text
   \param[in]  oe_Type     New error type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxAllBase::SetToolTipAdditionalInfo(const QString & orc_Text,
                                               const stw::opensyde_gui::C_NagToolTip::E_Type oe_Type)
{
   this->mc_ToolTipAdditionalContent = orc_Text;
   this->me_ToolTipAdditionalType = oe_Type;
   m_ApplyToolTipInfo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activate default range tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpxAllBase::ActivateDefaultToolTip(void)
{
   const QString c_Heading = C_GtGetText::h_GetText("Value Range");

   //Translation: 1=Minimum value, 2=Maximum value
   const QString c_Content =
      static_cast<QString>(C_GtGetText::h_GetText("Minimum: %1\nMaximum: %2")).arg(this->m_GetMinimumString()).arg(
         this->m_GetMaximumString());

   this->SetToolTipInformation(c_Heading, c_Content);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get minimum string

   \return
    Minimum string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeSpxAllBase::m_GetMinimumString() const
{
   QString c_Retval;

   if (this->mc_UserMinValString.isEmpty())
   {
      c_Retval = this->m_GetMinimumRawString();
   }
   else
   {
      c_Retval = this->mc_UserMinValString;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get maximum string

   \return
   Maximum string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeSpxAllBase::m_GetMaximumString() const
{
   QString c_Retval;

   if (this->mc_UserMaxValString.isEmpty())
   {
      c_Retval = this->m_GetMaximumRawString();
   }
   else
   {
      c_Retval = this->mc_UserMaxValString;
   }
   return c_Retval;
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
            QHelpEvent * const pc_HelpEvent = dynamic_cast<QHelpEvent * const>(opc_Event);

            if (pc_HelpEvent != NULL)
            {
               if ((this->mq_ShowToolTipWhenDisabled == true) ||
                   (this->m_IsEnabled() == true))
               {
                  this->m_SetMouseTracking(true);

                  m_ApplyToolTipInfo();
                  this->m_GetToolTip()->show();
                  this->m_GetToolTip()->DoMove(pc_HelpEvent->globalPos());
               }
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply updated tool tip info
*/
//----------------------------------------------------------------------------------------------------------------------
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
