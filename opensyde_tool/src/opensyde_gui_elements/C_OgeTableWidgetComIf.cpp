//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table Widget for Node COM Interface Settings  (implementation)

   Table Widget for Node COM Interface Settings.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QEvent>
#include <QScrollBar>
#include <QHelpEvent>
#include <QHoverEvent>
#include <QHeaderView>
#include <QLabel>
#include "constants.hpp"
#include "C_OgeTableWidgetComIf.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

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
C_OgeTableWidgetComIf::C_OgeTableWidgetComIf(QWidget * const opc_Parent) :
   QTableWidget(opc_Parent),
   mpc_ToolTip(NULL)
{
   //UI Settings
   this->setCornerButtonEnabled(false);
   this->setSortingEnabled(false);

   this->setShowGrid(true);
   this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
   this->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
   this->setAlternatingRowColors(true);
   this->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
   this->setDefaultDropAction(Qt::DropAction::MoveAction);
   this->setDragEnabled(false);
   this->setLineWidth(0);
   this->setFrameShadow(QAbstractItemView::Shadow::Plain);
   this->setFrameShape(QAbstractItemView::Shape::NoFrame);
   this->setEditTriggers(EditTrigger::DoubleClicked | EditTrigger::CurrentChanged | EditTrigger::SelectedClicked);

   this->mc_HoveredRow.s32_Row = -1;
   this->mc_HoveredRow.s32_Col = -1;

   // configure the scrollbar to stop resizing the widget when showing or hiding the scrollbar
   this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->verticalScrollBar()->hide();

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   //Connects
   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged, this,
           &C_OgeTableWidgetComIf::m_ScrollBarRangeChangedVer);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTableWidgetComIf::~C_OgeTableWidgetComIf(void)
{
   delete mpc_ToolTip;
   mpc_ToolTip = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set tool tip at position

   \param[in]  ou32_Row       Row (ID)
   \param[in]  ou32_Col       Column (ID)
   \param[in]  orc_Heading    Tool tip heading
   \param[in]  orc_Content    Tool tip content
   \param[in]  oe_Type        Tool tip error type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTableWidgetComIf::SetToolTipAt(const uint32_t ou32_Row, const uint32_t ou32_Col, const QString & orc_Heading,
                                         const QString & orc_Content, const C_NagToolTip::E_Type oe_Type)
{
   C_ItemId c_Id;

   c_Id.s32_Row = static_cast<int32_t>(ou32_Row);
   c_Id.s32_Col = static_cast<int32_t>(ou32_Col);
   if ((orc_Heading.compare("") == 0) && (orc_Content.compare("") == 0))
   {
      const std::map<C_ItemId, C_ToolTipContent>::iterator c_ItPos = this->mc_ToolTips.find(c_Id);
      if (c_ItPos != this->mc_ToolTips.end())
      {
         this->mc_ToolTips.erase(c_ItPos);
      }
   }
   else
   {
      C_ToolTipContent c_Content;
      c_Content.c_Heading = orc_Heading;
      c_Content.c_Content = orc_Content;
      c_Content.e_Type = oe_Type;

      this->mc_ToolTips[c_Id] = c_Content;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set header tool tip at position

   \param[in]  ou32_Section      Header section (ID)
   \param[in]  oe_Orientation    Header orientation (ID)
   \param[in]  orc_Heading       Tool tip heading
   \param[in]  orc_Content       Tool tip content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTableWidgetComIf::SetToolTipHeadingAt(const uint32_t ou32_Section, const Qt::Orientation oe_Orientation,
                                                const QString & orc_Heading, const QString & orc_Content) const
{
   this->model()->setHeaderData(static_cast<int32_t>(ou32_Section), oe_Orientation, orc_Heading,
                                ms32_USER_ROLE_TOOL_TIP_HEADING);
   this->model()->setHeaderData(static_cast<int32_t>(ou32_Section), oe_Orientation, orc_Content,
                                ms32_USER_ROLE_TOOL_TIP_CONTENT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Handle tooltip if necessary

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTableWidgetComIf::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   QTableWidget::mouseMoveEvent(opc_Event);
   m_HandleMouseMoveToolTip(opc_Event->globalPos());
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
bool C_OgeTableWidgetComIf::event(QEvent * const opc_Event)
{
   bool q_Return;

   if (opc_Event->type() == QEvent::ToolTip)
   {
      //show tool-tip
      if (mpc_ToolTip == NULL)
      {
         mpc_ToolTip = new stw::opensyde_gui::C_NagToolTip();
      }

      if (this->mpc_ToolTip->isVisible() == false)
      {
         QHelpEvent * const pc_HelpEvent = dynamic_cast<QHelpEvent * const>(opc_Event);

         if (pc_HelpEvent != NULL)
         {
            //Check first header
            if ((this->verticalHeader()->isVisible() == true) &&
                (this->verticalHeader()->geometry().contains(
                    this->verticalHeader()->mapFromGlobal(pc_HelpEvent->globalPos())) == true))
            {
               const int32_t s32_LogicalIndex =
                  this->verticalHeader()->logicalIndexAt(this->verticalHeader()->mapFromGlobal(
                                                            pc_HelpEvent->globalPos()));
               const int32_t s32_VisualIndex = this->verticalHeader()->visualIndex(s32_LogicalIndex);
               if (s32_VisualIndex >= 0)
               {
                  const QString c_Heading = this->model()->headerData(s32_VisualIndex, Qt::Vertical,
                                                                      ms32_USER_ROLE_TOOL_TIP_HEADING).value<QString>();
                  const QString c_Content = this->model()->headerData(s32_VisualIndex, Qt::Vertical,
                                                                      ms32_USER_ROLE_TOOL_TIP_CONTENT).value<QString>();

                  if ((c_Heading.compare("") != 0) && (c_Content.compare("") != 0))
                  {
                     this->ms32_HoveredVertHeader = s32_VisualIndex;
                     //Update text
                     this->setMouseTracking(true);
                     this->mpc_ToolTip->SetHeading(c_Heading);
                     this->mpc_ToolTip->SetContent(c_Content);
                     this->mpc_ToolTip->SetType(C_NagToolTip::eDEFAULT);
                     this->mpc_ToolTip->show();
                     this->mpc_ToolTip->DoMove(pc_HelpEvent->globalPos());
                  }
               }
            }
            //Check second header
            else if ((this->horizontalHeader()->isVisible() == true) &&
                     (this->horizontalHeader()->geometry().contains(
                         this->horizontalHeader()->mapFromGlobal(pc_HelpEvent->globalPos())) == true))
            {
               const int32_t s32_LogicalIndex =
                  this->horizontalHeader()->logicalIndexAt(this->horizontalHeader()->mapFromGlobal(
                                                              pc_HelpEvent->globalPos()));
               const int32_t s32_VisualIndex = this->horizontalHeader()->visualIndex(s32_LogicalIndex);
               if (s32_VisualIndex >= 0)
               {
                  const QString c_Heading = this->model()->headerData(s32_VisualIndex, Qt::Horizontal,
                                                                      ms32_USER_ROLE_TOOL_TIP_HEADING).value<QString>();
                  const QString c_Content = this->model()->headerData(s32_VisualIndex, Qt::Horizontal,
                                                                      ms32_USER_ROLE_TOOL_TIP_CONTENT).value<QString>();

                  if ((c_Heading.compare("") != 0) && (c_Content.compare("") != 0))
                  {
                     this->ms32_HoveredHorzHeader = s32_VisualIndex;
                     //Update text
                     this->setMouseTracking(true);
                     this->mpc_ToolTip->SetHeading(c_Heading);
                     this->mpc_ToolTip->SetContent(c_Content);
                     this->mpc_ToolTip->SetType(C_NagToolTip::eDEFAULT);
                     this->mpc_ToolTip->show();
                     this->mpc_ToolTip->DoMove(pc_HelpEvent->globalPos());
                  }
               }
            }
            else
            {
               //Check table last!
               const QModelIndex c_Index = this->indexAt(this->viewport()->mapFromGlobal(pc_HelpEvent->globalPos()));
               if (c_Index.isValid() == true)
               {
                  const int32_t s32_ToolTipRow = c_Index.row();
                  const int32_t s32_ToolTipCol = c_Index.column();
                  if ((s32_ToolTipRow >= 0) && (s32_ToolTipCol >= 0))
                  {
                     std::map<C_ItemId, C_ToolTipContent>::const_iterator c_ItToolTip;
                     C_ItemId c_Id;
                     c_Id.s32_Row = s32_ToolTipRow;
                     c_Id.s32_Col = s32_ToolTipCol;
                     c_ItToolTip = this->mc_ToolTips.find(c_Id);
                     if (c_ItToolTip != this->mc_ToolTips.end())
                     {
                        this->mc_HoveredRow = c_Id;
                        //Update text
                        this->setMouseTracking(true);
                        this->mpc_ToolTip->SetHeading(c_ItToolTip->second.c_Heading);
                        this->mpc_ToolTip->SetContent(c_ItToolTip->second.c_Content);
                        this->mpc_ToolTip->SetType(c_ItToolTip->second.e_Type);
                        this->mpc_ToolTip->show();
                        this->mpc_ToolTip->DoMove(pc_HelpEvent->globalPos());
                     }
                  }
               }
            }
         }
      }

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
      QHoverEvent * const pc_HoverEvent = dynamic_cast<QHoverEvent * const>(opc_Event);
      bool q_IpAddressHovered = false;  // for pointing-hand-cursor on link to IP address settings
      bool q_BusBitrateHovered = false; // for pointing-hand-cursor on link to bus property screen
      if (pc_HoverEvent != NULL)
      {
         m_HandleMouseMoveToolTip(this->mapToGlobal(pc_HoverEvent->pos()));

         // check if an IP address cell was hovered
         const QModelIndex c_Index =
            this->indexAt(this->viewport()->mapFromGlobal(this->mapToGlobal(pc_HoverEvent->pos())));
         if (c_Index.isValid())
         {
            QLabel * const pc_Label = dynamic_cast<QLabel * const>(this->cellWidget(c_Index.row(), c_Index.column()));
            if ((pc_Label != NULL) && ((pc_Label->text().split(".")).size() == 4))
            {
               q_IpAddressHovered = true;
            }
            else if ((pc_Label != NULL) &&
                     ((pc_Label->text().contains("kbit/s")) || (pc_Label->text().contains("ETH"))))
            {
               q_BusBitrateHovered = true;
            }
            else
            {
               //nothing to do here
            }
         }
      }

      // change the cursor if a link was hovered
      if (q_IpAddressHovered == true)
      {
         this->setCursor(Qt::PointingHandCursor);
      }
      else if (q_BusBitrateHovered == true)
      {
         this->setCursor(Qt::PointingHandCursor);
      }
      else
      {
         this->setCursor(Qt::ArrowCursor);
      }

      q_Return = QTableWidget::event(opc_Event);
   }
   else
   {
      q_Return = QTableWidget::event(opc_Event);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compare this smaller than other item

   \param[in]  orc_Item    Other item

   \return
   True  This is smaller
   False This is bigger or equal
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeTableWidgetComIf::C_ItemId::operator <(const C_OgeTableWidgetComIf::C_ItemId & orc_Item) const
{
   return (this->s32_Row < orc_Item.s32_Row) ||
          ((this->s32_Row == orc_Item.s32_Row) && (this->s32_Col < orc_Item.s32_Col));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide tool-tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTableWidgetComIf::m_HideToolTip(void)
{
   if (this->mpc_ToolTip != NULL)
   {
      this->mpc_ToolTip->hide();
   }
   this->mc_HoveredRow.s32_Row = -1;
   this->mc_HoveredRow.s32_Col = -1;
   this->ms32_HoveredVertHeader = -1;
   this->ms32_HoveredHorzHeader = -1;

   this->setMouseTracking(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle mouse move (tool tip related)

   \param[in]  orc_GlobalPos  Global mouse position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTableWidgetComIf::m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos)
{
   if ((this->mc_HoveredRow.s32_Row >= 0) && (this->mc_HoveredRow.s32_Col >= 0))
   {
      const QModelIndex c_Index = this->indexAt(this->viewport()->mapFromGlobal(orc_GlobalPos));
      if (c_Index.isValid() == true)
      {
         if ((this->mc_HoveredRow.s32_Row != c_Index.row()) || (this->mc_HoveredRow.s32_Col != c_Index.column()))
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
   if ((this->verticalHeader()->isVisible() == true) && (this->ms32_HoveredVertHeader >= 0))
   {
      const int32_t s32_LogicalIndex =
         this->verticalHeader()->logicalIndexAt(this->verticalHeader()->mapFromGlobal(orc_GlobalPos));
      const int32_t s32_VisualIndex = this->verticalHeader()->visualIndex(s32_LogicalIndex);
      if (s32_VisualIndex != this->ms32_HoveredVertHeader)
      {
         //Hide Tooltip
         m_HideToolTip();
      }
      else
      {
         if (this->verticalHeader()->geometry().contains(
                this->verticalHeader()->mapFromGlobal(orc_GlobalPos)) == false)
         {
            //Hide Tooltip
            m_HideToolTip();
         }
      }
   }
   if ((this->horizontalHeader()->isVisible() == true) && (this->ms32_HoveredHorzHeader >= 0))
   {
      const int32_t s32_LogicalIndex =
         this->horizontalHeader()->logicalIndexAt(this->horizontalHeader()->mapFromGlobal(orc_GlobalPos));
      const int32_t s32_VisualIndex = this->horizontalHeader()->visualIndex(s32_LogicalIndex);
      if (s32_VisualIndex != this->ms32_HoveredHorzHeader)
      {
         //Hide Tooltip
         m_HideToolTip();
      }
      else
      {
         if (this->horizontalHeader()->geometry().contains(
                this->horizontalHeader()->mapFromGlobal(orc_GlobalPos)) == false)
         {
            //Hide Tooltip
            m_HideToolTip();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_OgeTableWidgetComIf::m_ScrollBarRangeChangedVer(const int32_t os32_Min, const int32_t os32_Max) const
{
   // manual showing and hiding of the scrollbar to stop resizing the parent widget when showing or hiding the scrollbar
   if ((os32_Min == 0) && (os32_Max == 0))
   {
      this->verticalScrollBar()->hide();
   }
   else
   {
      this->verticalScrollBar()->show();
   }
}
