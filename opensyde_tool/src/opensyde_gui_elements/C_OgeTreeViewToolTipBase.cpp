//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Tree view with tool tip (implementation)

   Tree view with tool tip

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.06.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QEvent>
#include <QHelpEvent>
#include <QHeaderView>
#include "constants.h"
#include "C_OgeTreeViewToolTipBase.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
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

   \created     08.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeTreeViewToolTipBase::C_OgeTreeViewToolTipBase(QWidget * const opc_Parent) :
   QTreeView(opc_Parent),
   ms32_HoveredNumberOfParents(-1),
   ms32_HoveredRow(-1),
   ms32_HoveredCol(-1),
   ms32_HoveredHorzHeader(-1)
{
   this->setMouseTracking(true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse move event slot

   Here: Handle tooltip if necessary

   \param[in,out] opc_Event Event identification and information

   \created     08.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeTreeViewToolTipBase::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   QTreeView::mouseMoveEvent(opc_Event);
   m_HandleMouseMoveToolTip(opc_Event->globalPos());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored

   \created     08.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OgeTreeViewToolTipBase::event(QEvent * const opc_Event)
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
            //Never use global pos directly as this seems to ignore transformations
            const QPoint c_GlobalPos = this->mapToGlobal(pc_HelpEvent->pos());
            //Check header
            if ((this->header()->isVisible() == true) &&
                (this->header()->geometry().contains(this->header()->mapFromGlobal(c_GlobalPos)) == true))
            {
               const sint32 s32_LogicalIndex =
                  this->header()->logicalIndexAt(this->header()->mapFromGlobal(c_GlobalPos));
               const sint32 s32_VisualIndex = this->header()->visualIndex(s32_LogicalIndex);
               if (s32_VisualIndex >= 0)
               {
                  const QString c_Heading = this->model()->headerData(s32_VisualIndex, Qt::Horizontal,
                                                                      msn_USER_ROLE_TOOL_TIP_HEADING).value<QString>();
                  const QString c_Content = this->model()->headerData(s32_VisualIndex, Qt::Horizontal,
                                                                      msn_USER_ROLE_TOOL_TIP_CONTENT).value<QString>();
                  const C_NagToolTip::E_Type e_Type =
                     static_cast<C_NagToolTip::E_Type>(this->model()->headerData(s32_VisualIndex, Qt::Horizontal,
                                                                                 msn_USER_ROLE_TOOL_TIP_TYPE).toInt());

                  if ((c_Heading.compare("") != 0) || (c_Content.compare("") != 0))
                  {
                     this->ms32_HoveredHorzHeader = s32_VisualIndex;
                     //Update text
                     this->setMouseTracking(true);
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
               const QModelIndex c_Index = this->indexAt(this->viewport()->mapFromGlobal(c_GlobalPos));
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
                        this->setMouseTracking(true);
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
      m_HideTooltip();

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
         m_HandleMouseMoveToolTip(this->mapToGlobal(pc_HoverEvent->pos()));
      }
      q_Return = QTreeView::event(opc_Event);
   }
   else
   {
      q_Return = QTreeView::event(opc_Event);
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten event filter slot

   Here: Handle tool tip move in special occasion

   \param[in,out] opc_Object Event sender object
   \param[in,out] opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored

   \created     09.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OgeTreeViewToolTipBase::eventFilter(QObject * const opc_Object, QEvent * const opc_Event)
{
   if (opc_Event->type() == QEvent::HoverMove)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QHoverEvent * const pc_HoverEvent = dynamic_cast<QHoverEvent * const>(opc_Event);

      if (pc_HoverEvent != NULL)
      {
         m_HandleMouseMoveToolTip(this->mapToGlobal(pc_HoverEvent->pos()));
      }
   }
   return QTreeView::eventFilter(opc_Object, opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Hide tooltip

   \created     08.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeTreeViewToolTipBase::m_HideTooltip(void)
{
   this->m_HideToolTip();
   this->ms32_HoveredNumberOfParents = -1;
   this->ms32_HoveredRow = -1;
   this->ms32_HoveredCol = -1;
   this->ms32_HoveredHorzHeader = -1;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle mouse move (tool tip related)

   \param[in] orc_GlobalPos Global mouse pos

   \created     08.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeTreeViewToolTipBase::m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos)
{
   if (((this->ms32_HoveredRow >= 0) && (this->ms32_HoveredCol >= 0)) && (this->ms32_HoveredNumberOfParents >= 0))
   {
      const QModelIndex c_Index = this->indexAt(this->viewport()->mapFromGlobal(orc_GlobalPos));
      if (c_Index.isValid() == true)
      {
         if (((this->ms32_HoveredRow != c_Index.row()) || (this->ms32_HoveredCol != c_Index.column())) ||
             (this->ms32_HoveredNumberOfParents != mh_CountParents(c_Index)))
         {
            //Hide Tooltip
            m_HideTooltip();
         }
      }
      else
      {
         //Hide Tooltip
         m_HideTooltip();
      }
   }
   if ((this->header()->isVisible() == true) && (this->ms32_HoveredHorzHeader >= 0))
   {
      const sint32 s32_LogicalIndex =
         this->header()->logicalIndexAt(this->header()->mapFromGlobal(orc_GlobalPos));
      const sint32 s32_VisualIndex = this->header()->visualIndex(s32_LogicalIndex);
      if (s32_VisualIndex != this->ms32_HoveredHorzHeader)
      {
         //Hide Tooltip
         m_HideTooltip();
      }
      else
      {
         if (this->header()->geometry().contains(this->header()->mapFromGlobal(orc_GlobalPos)) == false)
         {
            //Hide Tooltip
            m_HideTooltip();
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Count number of parents

   \param[in] orc_Index Index to count parents from

   \return
   Number of parents

   \created     08.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OgeTreeViewToolTipBase::mh_CountParents(const QModelIndex & orc_Index)
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
