//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List widget with tool tip (implementation)

   List widget with tool tip

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QEvent>
#include <QHelpEvent>
#include "constants.h"
#include "C_OgeListWidgetToolTipBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
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
C_OgeListWidgetToolTipBase::C_OgeListWidgetToolTipBase(QWidget * const opc_Parent) :
   QListWidget(opc_Parent),
   ms32_HoveredRow(-1),
   ms32_HoveredCol(-1)
{
   this->setMouseTracking(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Handle tooltip if necessary

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeListWidgetToolTipBase::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   QListWidget::mouseMoveEvent(opc_Event);
   m_HandleMouseMoveToolTip(opc_Event->globalPos());
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
bool C_OgeListWidgetToolTipBase::event(QEvent * const opc_Event)
{
   bool q_Return;

   if (opc_Event->type() == QEvent::ToolTip)
   {
      //show tooltip
      if (this->m_GetToolTip()->isVisible() == false)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         QHelpEvent * const pc_HelpEvent = dynamic_cast<QHelpEvent * const>(opc_Event);

         if (pc_HelpEvent != NULL)
         {
            //Check table last!
            const QModelIndex c_Index = this->indexAt(this->viewport()->mapFromGlobal(pc_HelpEvent->globalPos()));
            if (c_Index.isValid() == true)
            {
               const sint32 s32_ToolTipRow = c_Index.row();
               const sint32 s32_ToolTipCol = c_Index.column();
               if ((s32_ToolTipRow >= 0) && (s32_ToolTipCol >= 0))
               {
                  const QString c_Heading = c_Index.data(msn_USER_ROLE_TOOL_TIP_HEADING).value<QString>();
                  const QString c_Content = c_Index.data(msn_USER_ROLE_TOOL_TIP_CONTENT).value<QString>();
                  if ((c_Heading.compare("") != 0) || (c_Content.compare("") != 0))
                  {
                     this->ms32_HoveredRow = s32_ToolTipRow;
                     this->ms32_HoveredCol = s32_ToolTipCol;
                     //Update text
                     this->setMouseTracking(true);
                     this->m_GetToolTip()->SetHeading(c_Heading);
                     this->m_GetToolTip()->SetContent(c_Content);
                     this->m_GetToolTip()->SetType(this->me_ToolTipType);
                     this->m_GetToolTip()->show();
                     this->m_GetToolTip()->DoMove(pc_HelpEvent->globalPos());
                  }
               }
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
      m_HideToolTip();

      this->setMouseTracking(false);

      opc_Event->accept();
      q_Return = true;
   }
   else
   {
      q_Return = QListWidget::event(opc_Event);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide tooltip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeListWidgetToolTipBase::m_HideToolTip(void)
{
   C_OgeToolTipBase::m_HideToolTip();
   this->ms32_HoveredRow = -1;
   this->ms32_HoveredCol = -1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle mouse move (tool tip related)

   \param[in] orc_GlobalPos Global mouse pos
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeListWidgetToolTipBase::m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos)
{
   if ((this->ms32_HoveredRow >= 0) && (this->ms32_HoveredCol >= 0))
   {
      const QModelIndex c_Index = this->indexAt(this->viewport()->mapFromGlobal(orc_GlobalPos));
      if (c_Index.isValid() == true)
      {
         if ((this->ms32_HoveredRow != c_Index.row()) || (this->ms32_HoveredCol != c_Index.column()))
         {
            //Hide Tooltip
            m_HideToolTip();
         }
      }
      else
      {
         //Hide Tooltip
         m_HideToolTip();
      }
   }
}
