//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tab widget with tool tip (implementation)

   Tab widget with tool tip

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QEvent>
#include <QTabBar>
#include <QHelpEvent>
#include "C_OgeTawToolTipBase.h"

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
C_OgeTawToolTipBase::C_OgeTawToolTipBase(QWidget * const opc_Parent) :
   QTabWidget(opc_Parent),
   mpc_ToolTip(NULL),
   ms32_CurrentHoveredIndex(-1)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTawToolTipBase::~C_OgeTawToolTipBase()
{
   delete mpc_ToolTip;
   mpc_ToolTip = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set tool tip

   \param[in] ou32_Index  Tab index (ID)
   \param[in] orc_Heading Tool tip heading
   \param[in] orc_Content Tool tip content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTawToolTipBase::SetToolTipInformation(const stw_types::uint32 ou32_Index, const QString & orc_Heading,
                                                const QString & orc_Content)
{
   C_ToolTipContent c_ToolTip;

   c_ToolTip.c_Heading = orc_Heading;
   c_ToolTip.c_Content = orc_Content;
   this->mc_ToolTips[static_cast<stw_types::sint32>(ou32_Index)] = c_ToolTip;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Handle tooltip if necessary

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTawToolTipBase::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   QTabWidget::mouseMoveEvent(opc_Event);
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
bool C_OgeTawToolTipBase::event(QEvent * const opc_Event)
{
   bool q_Return;

   if (opc_Event->type() == QEvent::ToolTip)
   {
      //show tooltip
      if (mpc_ToolTip == NULL)
      {
         mpc_ToolTip = new stw_opensyde_gui::C_NagToolTip();
      }

      if (this->mpc_ToolTip->isVisible() == false)
      {
         
         QHelpEvent * const pc_HelpEvent = dynamic_cast<QHelpEvent * const>(opc_Event);

         if (pc_HelpEvent != NULL)
         {
            const stw_types::sint32 s32_TabIndex =
               this->tabBar()->tabAt(this->tabBar()->mapFromGlobal(pc_HelpEvent->globalPos()));
            if (s32_TabIndex >= 0)
            {
               std::map<stw_types::sint32, C_ToolTipContent>::const_iterator c_It;
               c_It = this->mc_ToolTips.find(s32_TabIndex);
               if (c_It != this->mc_ToolTips.end())
               {
                  this->setMouseTracking(true);

                  this->mpc_ToolTip->SetHeading(c_It->second.c_Heading);
                  this->mpc_ToolTip->SetContent(c_It->second.c_Content);
                  this->mpc_ToolTip->show();
                  this->mpc_ToolTip->DoMove(pc_HelpEvent->globalPos());

                  //Accept event because of Qt dynamic tooltip time based on the fact if there was a tooltip in this
                  // widget
                  // already
                  opc_Event->accept();
                  q_Return = true;
               }
               else
               {
                  m_HideToolTip();

                  q_Return = QTabWidget::event(opc_Event);
               }
            }
            else
            {
               m_HideToolTip();

               q_Return = QTabWidget::event(opc_Event);
            }
         }
         else
         {
            m_HideToolTip();

            q_Return = QTabWidget::event(opc_Event);
         }
      }
      else
      {
         m_HideToolTip();

         q_Return = QTabWidget::event(opc_Event);
      }
   }
   else if (opc_Event->type() == QEvent::Leave)
   {
      m_HideToolTip();

      q_Return = QTabWidget::event(opc_Event);
   }
   else
   {
      q_Return = QTabWidget::event(opc_Event);
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide tooltip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTawToolTipBase::m_HideToolTip()
{
   if (this->mpc_ToolTip != NULL)
   {
      this->mpc_ToolTip->hide();
   }
   this->ms32_CurrentHoveredIndex = -1;

   this->setMouseTracking(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle mouse move (tool tip related)

   \param[in] orc_GlobalPos Global mouse pos
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTawToolTipBase::m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos)
{
   if (this->ms32_CurrentHoveredIndex >= 0)
   {
      const stw_types::sint32 s32_TabIndex =
         this->tabBar()->tabAt(this->tabBar()->mapFromGlobal(orc_GlobalPos));
      if (s32_TabIndex >= 0)
      {
         if (s32_TabIndex != this->ms32_CurrentHoveredIndex)
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
