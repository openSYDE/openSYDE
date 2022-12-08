//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standard color button in color picker (implementation)

   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for..

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgePubColor.hpp"
#include "C_OgeWiUtil.hpp"

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
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgePubColor::C_OgePubColor(QWidget * const opc_Parent) :
   QPushButton(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set background color

   \param[in]  orc_BackgroundColor   New background color
   \param[in]  orc_TopBorderColor    New top border color
   \param[in]  orc_RestBorderColor   New rest border color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubColor::SetColors(const QColor & orc_BackgroundColor, const QColor & orc_TopBorderColor,
                              const QColor & orc_RestBorderColor)
{
   this->setStyleSheet(
      "stw--opensyde_gui_elements--C_OgePubColor"
      "{"
      "   background-color: " + orc_BackgroundColor.name() + ";"
      "   border-top-color: " + orc_TopBorderColor.name() + ";"
      "   border-right-color: " + orc_RestBorderColor.name() + ";"
      "   border-bottom-color: " + orc_RestBorderColor.name() + ";"
      "   border-left-color: " + orc_RestBorderColor.name() + ";"
      "}");
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
bool C_OgePubColor::event(QEvent * const opc_Event)
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
               this->setMouseTracking(true);

               this->m_GetToolTip()->SetHeading(this->mc_ToolTipHeading);
               this->m_GetToolTip()->SetContent(this->mc_ToolTipContent);
               this->m_GetToolTip()->SetType(this->me_ToolTipType);
               this->m_GetToolTip()->show();
               this->m_GetToolTip()->DoMove(pc_HelpEvent->globalPos());
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
         q_Return = QPushButton::event(opc_Event);
      }
   }
   else
   {
      q_Return = QPushButton::event(opc_Event);
   }

   return q_Return;
}
