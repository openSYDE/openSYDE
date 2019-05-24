//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all tree tool tips

   Base class for all tree tool tips

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QMouseEvent>

#include "stwtypes.h"
#include "constants.h"
#include "C_OgeTreeToolTipBase.h"

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
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTreeToolTipBase::C_OgeTreeToolTipBase(void) :
   ms32_HoveredNumberOfParents(-1),
   ms32_HoveredRow(-1),
   ms32_HoveredCol(-1),
   ms32_HoveredHorzHeader(-1)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call after call of base "mouseMove" (in override of mouseMove)

   \param[in] opc_Event "mouseMove" parameter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeToolTipBase::CallAfterMouseMove(const QMouseEvent * const opc_Event)
{
   this->m_HandleMouseMoveToolTip(opc_Event->globalPos());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call instead of "event" (in override of event)

   \param[in,out] opc_Event "event" parameter
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeTreeToolTipBase::CallForEvent(QEvent * const opc_Event)
{
   bool q_Return;

   if (opc_Event->type() == QEvent::ToolTip)
   {
      //show tooltip
      if (this->m_GetToolTip()->isVisible() == false)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         QHelpEvent * const pc_HelpEvent = dynamic_cast<QHelpEvent * const>(opc_Event);

         //Trigger tooltip update
         m_LastMinuteToolTipUpdate();

         if (pc_HelpEvent != NULL)
         {
            //Never use global pos directly as this seems to ignore transformations
            const QPoint c_GlobalPos = this->m_MapToGlobal(pc_HelpEvent->pos());
            //Check header
            if ((this->m_Header()->isVisible() == true) &&
                (this->m_Header()->geometry().contains(this->m_Header()->mapFromGlobal(c_GlobalPos)) == true))
            {
               const sint32 s32_LogicalIndex =
                  this->m_Header()->logicalIndexAt(this->m_Header()->mapFromGlobal(c_GlobalPos));
               //It seems the logical index is better than the visual index as it also handles moved columns
               if (s32_LogicalIndex >= 0)
               {
                  const QString c_Heading = this->m_Model()->headerData(s32_LogicalIndex, Qt::Horizontal,
                                                                        msn_USER_ROLE_TOOL_TIP_HEADING).value<QString>();
                  const QString c_Content = this->m_Model()->headerData(s32_LogicalIndex, Qt::Horizontal,
                                                                        msn_USER_ROLE_TOOL_TIP_CONTENT).value<QString>();
                  const C_NagToolTip::E_Type e_Type =
                     static_cast<C_NagToolTip::E_Type>(this->m_Model()->headerData(s32_LogicalIndex, Qt::Horizontal,
                                                                                   msn_USER_ROLE_TOOL_TIP_TYPE).toInt());

                  if ((c_Heading.compare("") != 0) || (c_Content.compare("") != 0))
                  {
                     this->ms32_HoveredHorzHeader = s32_LogicalIndex;
                     //Update text
                     this->m_SetMouseTracking(true);
                     this->m_GetToolTip()->SetHeading(c_Heading);
                     this->m_GetToolTip()->SetContent(c_Content);
                     this->m_GetToolTip()->SetType(e_Type);
                     this->m_GetToolTip()->show();
                     //Use global pos for move because that's the actual target position
                     this->m_GetToolTip()->DoMove(pc_HelpEvent->globalPos());
                  }
               }
            }
            else
            {
               //Check table last!
               const QModelIndex c_Index = m_IndexAtGlobalPos(c_GlobalPos);
               if (c_Index.isValid() == true)
               {
                  const sint32 s32_ToolTipRow = c_Index.row();
                  const sint32 s32_ToolTipCol = c_Index.column();
                  if ((s32_ToolTipRow >= 0) && (s32_ToolTipCol >= 0))
                  {
                     const QString c_Heading = c_Index.data(msn_USER_ROLE_TOOL_TIP_HEADING).value<QString>();
                     const QString c_Content = c_Index.data(msn_USER_ROLE_TOOL_TIP_CONTENT).value<QString>();
                     const C_NagToolTip::E_Type e_Type =
                        static_cast<C_NagToolTip::E_Type>(c_Index.data(msn_USER_ROLE_TOOL_TIP_TYPE).toInt());
                     if ((c_Heading.compare("") != 0) || (c_Content.compare("") != 0))
                     {
                        this->ms32_HoveredRow = s32_ToolTipRow;
                        this->ms32_HoveredCol = s32_ToolTipCol;
                        this->ms32_HoveredNumberOfParents = mh_CountParents(c_Index);
                        //Update text
                        this->m_SetMouseTracking(true);
                        this->m_GetToolTip()->SetHeading(c_Heading);
                        this->m_GetToolTip()->SetContent(c_Content);
                        this->m_GetToolTip()->SetType(e_Type);
                        this->m_GetToolTip()->show();
                        //Use global pos for move because that's the actual target position
                        this->m_GetToolTip()->DoMove(pc_HelpEvent->globalPos());
                     }
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

      opc_Event->accept();
      q_Return = true;
   }
   //Necessary to detect mouse move events over integrated widgets
   else if (opc_Event->type() == QEvent::HoverMove)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QHoverEvent * const pc_HoverEvent = dynamic_cast<QHoverEvent * const>(opc_Event);

      if (pc_HoverEvent != NULL)
      {
         m_HandleMouseMoveToolTip(this->m_MapToGlobal(pc_HoverEvent->pos()));
      }
      q_Return = m_BaseEvent(opc_Event);
   }
   else
   {
      q_Return = m_BaseEvent(opc_Event);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call before "eventFilter" (in override of eventFilter)

   \param[in,out] opc_Event "eventFilter" parameter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeToolTipBase::CallBeforeEventFilter(const QEvent * const opc_Event)
{
   if (opc_Event->type() == QEvent::HoverMove)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const QHoverEvent * const pc_HoverEvent = dynamic_cast<const QHoverEvent * const>(opc_Event);

      if (pc_HoverEvent != NULL)
      {
         m_HandleMouseMoveToolTip(this->m_MapToGlobal(pc_HoverEvent->pos()));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide tooltip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeToolTipBase::m_HideToolTip(void)
{
   C_OgeToolTipBase::m_HideToolTip();
   this->ms32_HoveredNumberOfParents = -1;
   this->ms32_HoveredRow = -1;
   this->ms32_HoveredCol = -1;
   this->ms32_HoveredHorzHeader = -1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Do any potential last minute tooltip update actions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeToolTipBase::m_LastMinuteToolTipUpdate(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle mouse move (tool tip related)

   \param[in] orc_GlobalPos Global mouse pos
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeToolTipBase::m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos)
{
   if (((this->ms32_HoveredRow >= 0) && (this->ms32_HoveredCol >= 0)) && (this->ms32_HoveredNumberOfParents >= 0))
   {
      const QModelIndex c_Index = m_IndexAtGlobalPos(orc_GlobalPos);
      if (c_Index.isValid() == true)
      {
         if (((this->ms32_HoveredRow != c_Index.row()) || (this->ms32_HoveredCol != c_Index.column())) ||
             (this->ms32_HoveredNumberOfParents != mh_CountParents(c_Index)))
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
   if ((this->m_Header()->isVisible() == true) && (this->ms32_HoveredHorzHeader >= 0))
   {
      const sint32 s32_LogicalIndex =
         this->m_Header()->logicalIndexAt(this->m_Header()->mapFromGlobal(orc_GlobalPos));
      //It seems the logical index is better than the visual index as it also handles moved columns
      if (s32_LogicalIndex != this->ms32_HoveredHorzHeader)
      {
         //Hide Tooltip
         m_HideToolTip();
      }
      else
      {
         if (this->m_Header()->geometry().contains(this->m_Header()->mapFromGlobal(orc_GlobalPos)) == false)
         {
            //Hide Tooltip
            m_HideToolTip();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Count number of parents

   \param[in] orc_Index Index to count parents from

   \return
   Number of parents
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OgeTreeToolTipBase::mh_CountParents(const QModelIndex & orc_Index)
{
   sint32 s32_Retval = 0;
   QModelIndex c_Tmp = orc_Index;

   while (c_Tmp.parent().isValid() == true)
   {
      ++s32_Retval;
      //Next one
      c_Tmp = c_Tmp.parent();
   }
   return s32_Retval;
}
