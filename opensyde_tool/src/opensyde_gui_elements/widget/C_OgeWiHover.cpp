//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Hover widget in material design

   The widget offers the possibility to show different widgets with hover widget with
   the same desing and layout.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QScreen>
#include <QApplication>

#include "C_OgeWiHover.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_GtGetText.hpp"
#include "ui_C_OgeWiHover.h"
#include "constants.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
static const int32_t ms32_SHADOW_WIDTH = 8;

static const int32_t ms32_MAXIMIZED_HEIGHT_MINIMUM = 300 + (2 * ms32_SHADOW_WIDTH);
static const int32_t ms32_MINIMIZED_HEIGHT = 31 + (2 * ms32_SHADOW_WIDTH);
static const int32_t ms32_SCROLL_AREA_SIZE = 9;

static const int32_t ms32_WIDTH_MINIMUM = 323 + (2 * ms32_SHADOW_WIDTH);

static const int32_t ms32_TOP_BORDER_AREA = 4;

static const uint32_t mu32_MOUSE_MODE_NO_FUNC = 0U;
static const uint32_t mu32_MOUSE_MODE_RESIZE = 1U;
static const uint32_t mu32_MOUSE_MODE_MOVE = 2U;

static const uint32_t mu32_RESIZE_NONE = 0U;
static const uint32_t mu32_RESIZE_HOR = 1U;
static const uint32_t mu32_RESIZE_VER = 2U;
static const uint32_t mu32_RESIZE_ALL_TOPLEFT = 3U;
static const uint32_t mu32_RESIZE_ALL_TOPRIGHT = 4U;
static const uint32_t mu32_RESIZE_ALL_BOTTOMLEFT = 5U;
static const uint32_t mu32_RESIZE_ALL_BOTTOMRIGHT = 6U;
static const uint32_t mu32_RESIZE_HOR_LEFT = 7U;
static const uint32_t mu32_RESIZE_VER_TOP = 8U;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Widget             Reference to embedded widget
   \param[in]      orc_Title              String with title for showing in the heading
   \param[in]      orc_Icon               Icon of which one
   \param[in]      oq_Search              Flag if search function shall be active or not
   \param[in,out]  opc_ContainerWidget    Optional widget for resize restriction
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiHover::C_OgeWiHover(QWidget & orc_Widget, const QString & orc_Title, const QString & orc_Icon,
                           const bool oq_Search, QWidget * const opc_ContainerWidget, QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_OgeWiHover),
   mpc_Widget(&orc_Widget),
   mpc_ContainerWidget(opc_ContainerWidget),
   mq_Maximized(true),
   mu32_MouseMode(mu32_MOUSE_MODE_NO_FUNC),
   mu32_ResizeMode(mu32_RESIZE_NONE),
   mc_IconPath(orc_Icon),
   mq_SearchActive(oq_Search),
   ms32_OffsetHorizontal(0),
   ms32_OffsetVertical(0),
   ms32_OffsetHorizontalRight(0),
   mq_DarkMode(false)
{
   QImage c_Icon;
   int32_t s32_Index;

   this->mpc_Ui->setupUi(this);

   this->setMinimumSize(ms32_WIDTH_MINIMUM, ms32_MAXIMIZED_HEIGHT_MINIMUM);

   // for changing the mouse cursor
   this->setMouseTracking(true);
   this->mpc_Ui->pc_GroupBox->setMouseTracking(true);

   this->SetMaximizedHeight(this->height());

   // deactivate search elements
   this->mpc_Ui->pc_GroupBoxSearch->setVisible(this->mq_SearchActive);

   // set icons to elements
   c_Icon.load(orc_Icon);
   this->mpc_Ui->pc_LabelIcon->setPixmap(QPixmap::fromImage(c_Icon));
   this->mpc_Ui->pc_LabelSearchImg->SetSvg("://images/IconSearch.svg");
   this->mpc_Ui->pc_BtnCancelSearch->SetCustomIcons(":images/DeleteSearchInput.svg",  // enabled
                                                    ":images/DeleteSearchInput.svg",  // hovered
                                                    ":images/DeleteSearchInput.svg",  // clicked
                                                    ":images/DeleteSearchInput.svg"); // disabled
   this->mpc_Ui->pc_BtnCancelSearch->setVisible(false);

   InitStaticNames();

   // set title
   this->mpc_Ui->pc_LabelTitle->setText(orc_Title);
   this->mpc_Ui->pc_LabelTitle->SetForegroundColor(3);
   this->mpc_Ui->pc_LabelTitle->SetFontPixel(13, false, true);

   // set specific widget
   this->mpc_Widget->setParent(this);
   this->mpc_Ui->pc_VerticalLayout->addWidget(this->mpc_Widget);
   s32_Index = this->mpc_Ui->pc_VerticalLayout->indexOf(this->mpc_Widget);
   this->mpc_Ui->pc_VerticalLayout->setStretch(s32_Index, 1);

   //Deactivate dark mode
   this->ApplyDarkMode(false);

   this->setAttribute(Qt::WA_TranslucentBackground);

   this->mc_TimerAnimation.setInterval(ms32_TIMER_INTERVAL);

   //search connect
   connect(this->mpc_Ui->pc_BtnCancelSearch, &QPushButton::clicked, this, &C_OgeWiHover::m_CancelSearch);
   // connect the signals
   connect(this->mpc_Ui->pc_BtnMin, &QPushButton::clicked, this, &C_OgeWiHover::SigWiHoverMinBtnClicked);
   connect(&this->mc_TimerAnimation, &QTimer::timeout, this, &C_OgeWiHover::m_AnimationTimerEvent);
   connect(this->mpc_Ui->pc_LineEditSearch, &QLineEdit::textChanged, this, &C_OgeWiHover::m_SearchChanged);
   connect(this->mpc_Ui->pc_LabelTitle, &C_OgeLabDoubleClick::SigDoubleClicked,
           this, &C_OgeWiHover::SigWiHoverMinBtnClicked);
   connect(this->mpc_Ui->pc_LabelIcon, &C_OgeLabDoubleClick::SigDoubleClicked,
           this, &C_OgeWiHover::SigWiHoverMinBtnClicked);
} //lint !e429  //no memory leak because of the parent of pc_Shadow and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540} Cleaned up via Qt parent mechanism
C_OgeWiHover::~C_OgeWiHover()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::InitStaticNames() const
{
   this->mpc_Ui->pc_LabelTitle->setText(C_GtGetText::h_GetText("Default Title"));
   this->mpc_Ui->pc_LineEditSearch->setPlaceholderText(C_GtGetText::h_GetText("Search"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns actual state about maximized or minimized

   \return     Actual maximized state of widget
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeWiHover::GetMaximized(void) const
{
   return this->mq_Maximized;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets actual state about maximized or minimized

   \param[in]   oq_Maximized   Actual maximized state of widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::SetMaximized(const bool oq_Maximized)
{
   this->mq_Maximized = oq_Maximized;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns maximized height

   \return     Actual maximized height
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OgeWiHover::GetMaximizedHeight(void) const
{
   return this->ms32_Height;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the maximized height

   \param[in]  os32_Height  New maximized height
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::SetMaximizedHeight(const int32_t os32_Height)
{
   this->ms32_Height = os32_Height;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply current dark mode setting

   \param[in] oq_Active Dark mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::ApplyDarkMode(const bool oq_Active)
{
   QImage c_Icon;
   QImage c_Image;
   QColor c_Color;
   QGraphicsDropShadowEffect * pc_Shadow;

   this->mq_DarkMode = oq_Active;

   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "DarkMode", oq_Active);

   pc_Shadow = new QGraphicsDropShadowEffect(this->mpc_Ui->pc_GroupBox);
   pc_Shadow->setBlurRadius(15.0);
   pc_Shadow->setOffset(0.0);
   if (oq_Active == true)
   {
      c_Color = mc_STYLE_GUIDE_COLOR_32;
      c_Image.load(":images/IconDoubleArrowRightDark.svg");
      if (this->mc_IconPath.contains("IconToolbox") == true)
      {
         this->mc_IconPath = ":images/IconToolboxDark.svg";
      }
      this->mpc_Ui->pc_LabelTitle->SetForegroundColor(32);
   }
   else
   {
      c_Color = mc_STYLE_GUIDE_COLOR_33;
      c_Image.load(":images/IconDoubleArrowRight.svg");
      if (this->mc_IconPath.contains("IconToolbox") == true)
      {
         this->mc_IconPath = ":images/IconToolbox.svg";
      }
      this->mpc_Ui->pc_LabelTitle->SetForegroundColor(3);
   }
   c_Icon.load(this->mc_IconPath);
   this->mpc_Ui->pc_LabelIcon->setPixmap(QPixmap::fromImage(c_Icon));
   this->mpc_Ui->pc_BtnMin->setIcon(QPixmap::fromImage(c_Image));
   c_Color.setAlpha(128);
   pc_Shadow->setColor(c_Color);
   this->mpc_Ui->pc_GroupBox->setGraphicsEffect(pc_Shadow);
} //lint !e429  //no memory leak because of the parent of pc_Shadow and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for animation timer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::m_AnimationTimerEvent()
{
   QSize c_Size;
   int32_t s32_NewHeight;

   m_UpdateParentSize();

   if (this->mq_Maximized == false)
   {
      //hide search items
      this->mpc_Ui->pc_LabelSearchImg->setVisible(false);
      this->mpc_Ui->pc_LineEditSearch->setVisible(false);

      // minimize the widget
      c_Size = this->size();

      if (c_Size.height() == this->ms32_Height)
      {
         // first call
         this->mpc_Widget->setVisible(false);
      }

      if (c_Size.height() > ms32_MINIMIZED_HEIGHT)
      {
         s32_NewHeight = c_Size.height() - this->ms32_StepSize;
         if (s32_NewHeight > ms32_MINIMIZED_HEIGHT)
         {
            this->setMinimumSize(ms32_WIDTH_MINIMUM, s32_NewHeight);
            this->setGeometry(this->x(), this->y(), this->width(), s32_NewHeight);
         }
         else
         {
            this->setMinimumSize(ms32_WIDTH_MINIMUM, ms32_MINIMIZED_HEIGHT);
            this->setGeometry(this->x(), this->y(), this->width(), ms32_MINIMIZED_HEIGHT);
         }
      }
      else
      {
         // finished
         QImage c_Image;
         c_Image.load(":images/IconDoubleArrowRight.svg");
         this->mpc_Ui->pc_BtnMin->setIcon(QPixmap::fromImage(c_Image));
         this->mc_TimerAnimation.stop();
      }
   }
   else
   {
      // maximize the widget
      c_Size = this->size();

      if (c_Size.height() < this->ms32_Height)
      {
         int32_t s32_PosVertical = this->y();
         s32_NewHeight = c_Size.height() + this->ms32_StepSize;

         if (s32_NewHeight >= ms32_MAXIMIZED_HEIGHT_MINIMUM)
         {
            // set the new minimum if the animation is far enough
            this->setMinimumSize(ms32_WIDTH_MINIMUM, ms32_MAXIMIZED_HEIGHT_MINIMUM);
         }

         if (s32_NewHeight < this->ms32_Height)
         {
            // check the bottom border
            if ((s32_PosVertical + s32_NewHeight) > this->mc_ParentWidgetSize.height())
            {
               s32_PosVertical -= this->ms32_StepSize;
            }

            this->setGeometry(this->x(), s32_PosVertical, this->width(), s32_NewHeight);
         }
         else
         {
            // check the bottom border
            if ((s32_PosVertical + this->ms32_Height) > this->mc_ParentWidgetSize.height())
            {
               s32_PosVertical = this->mc_ParentWidgetSize.height() - this->ms32_Height;
            }

            this->setGeometry(this->x(), s32_PosVertical, this->width(), this->ms32_Height);
         }
      }
      else
      {
         // finished
         QImage c_Image;
         c_Image.load(":images/IconDoubleArrowRight.svg");
         this->mpc_Ui->pc_BtnMin->setIcon(QPixmap::fromImage(c_Image));
         this->mpc_Widget->setVisible(true);
         this->mc_TimerAnimation.stop();

         if (this->mq_SearchActive == true)
         {
            //display search items
            this->mpc_Ui->pc_LabelSearchImg->setVisible(true);
            this->mpc_Ui->pc_LineEditSearch->setVisible(true);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::m_SearchChanged(const QString & orc_Text)
{
   if (this->mpc_Ui->pc_LineEditSearch->text() != "")
   {
      this->mpc_Ui->pc_BtnCancelSearch->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_BtnCancelSearch->setVisible(false);
   }
   Q_EMIT this->SigSearchChanged(orc_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update parent size variable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::m_UpdateParentSize(void)
{
   if (this->mpc_ContainerWidget != NULL)
   {
      this->mc_ParentWidgetSize = this->mpc_ContainerWidget->size();
      this->mc_ParentWidgetSize =
         QSize(this->mc_ParentWidgetSize.width() + ms32_SHADOW_WIDTH,
               this->mc_ParentWidgetSize.height() + ms32_SHADOW_WIDTH);
      this->mc_ParentWidgetTopLeft = QPoint(0, 0);

      this->ms32_OffsetHorizontal = ms32_SHADOW_WIDTH;
      this->ms32_OffsetVertical = ms32_TOP_BORDER_AREA;
      this->ms32_OffsetHorizontalRight = 0;
   }
   else
   {
      const QScreen * const pc_Desktop = QApplication::primaryScreen();
      if (pc_Desktop != NULL)
      {
         this->mc_ParentWidgetSize = pc_Desktop->size();
         this->mc_ParentWidgetTopLeft = pc_Desktop->geometry().topLeft();
      }
      this->ms32_OffsetHorizontal = ms32_SHADOW_WIDTH;
      this->ms32_OffsetVertical = ms32_TOP_BORDER_AREA + ms32_SHADOW_WIDTH;
      this->ms32_OffsetHorizontalRight = ms32_SHADOW_WIDTH;
   }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OgeWiHover::mh_GetResizeMode(const QPoint & orc_Pos, const QSize & orc_Size, const bool oq_Maximized)
{
   uint32_t u32_ResizeMode;

   // check the different border areas for the different resize modes
   if (orc_Pos.x() < ms32_SCROLL_AREA_SIZE)
   {
      if ((orc_Pos.y() < ms32_SCROLL_AREA_SIZE) &&
          (oq_Maximized == true))
      {
         u32_ResizeMode = mu32_RESIZE_ALL_TOPLEFT;
      }
      else if ((orc_Pos.y() > (orc_Size.height() - ms32_SCROLL_AREA_SIZE)) &&
               (oq_Maximized == true))
      {
         u32_ResizeMode = mu32_RESIZE_ALL_BOTTOMLEFT;
      }
      else
      {
         u32_ResizeMode = mu32_RESIZE_HOR_LEFT;
      }
   }
   else if (orc_Pos.x() > (orc_Size.width() - ms32_SCROLL_AREA_SIZE))
   {
      if ((orc_Pos.y() < ms32_SCROLL_AREA_SIZE) &&
          (oq_Maximized == true))
      {
         u32_ResizeMode = mu32_RESIZE_ALL_TOPRIGHT;
      }
      else if ((orc_Pos.y() > (orc_Size.height() - ms32_SCROLL_AREA_SIZE)) &&
               (oq_Maximized == true))
      {
         u32_ResizeMode = mu32_RESIZE_ALL_BOTTOMRIGHT;
      }
      else
      {
         u32_ResizeMode = mu32_RESIZE_HOR;
      }
   }
   else if ((orc_Pos.y() < ms32_SCROLL_AREA_SIZE) &&
            (oq_Maximized == true))
   {
      u32_ResizeMode = mu32_RESIZE_VER_TOP;
   }
   else if ((orc_Pos.y() > (orc_Size.height() - ms32_SCROLL_AREA_SIZE)) &&
            (oq_Maximized == true))
   {
      u32_ResizeMode = mu32_RESIZE_VER;
   }
   else
   {
      u32_ResizeMode = mu32_RESIZE_NONE;
   }

   return u32_ResizeMode;
}

//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::mh_AdaptMouseRangePos(QPoint & orc_Pos, const QSize & orc_Range, const QPoint & orc_TopLeft,
                                         const int32_t os32_OffsetHorizontal, const int32_t os32_OffsetVertical)
{
   int32_t s32_Temp = orc_TopLeft.x() - os32_OffsetHorizontal;

   if (orc_Pos.x() < s32_Temp)
   {
      orc_Pos.setX(s32_Temp);
   }

   s32_Temp = orc_TopLeft.y() - (os32_OffsetVertical);
   if (orc_Pos.y() < s32_Temp)
   {
      orc_Pos.setY(s32_Temp);
   }

   s32_Temp = orc_Range.width() + orc_TopLeft.x();
   if (orc_Pos.x() > s32_Temp)
   {
      orc_Pos.setX(s32_Temp);
   }

   s32_Temp = orc_Range.height() + orc_TopLeft.y();
   if (orc_Pos.y() > s32_Temp)
   {
      orc_Pos.setY(s32_Temp);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle cursor in default state

   \param[in] orc_Pos Current cursor position (local)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::m_HandleBasicCursorState(const QPoint & orc_Pos)
{
   const uint32_t u32_ResizeMode = mh_GetResizeMode(orc_Pos, this->size(), this->mq_Maximized);

   // adapt cursor
   switch (u32_ResizeMode)
   {
   case mu32_RESIZE_ALL_TOPLEFT:
   case mu32_RESIZE_ALL_BOTTOMRIGHT:
      this->setCursor(Qt::SizeFDiagCursor);
      break;
   case mu32_RESIZE_ALL_BOTTOMLEFT:
   case mu32_RESIZE_ALL_TOPRIGHT:
      this->setCursor(Qt::SizeBDiagCursor);
      break;
   case mu32_RESIZE_HOR_LEFT:
   case mu32_RESIZE_HOR:
      this->setCursor(Qt::SizeHorCursor);
      break;
   case mu32_RESIZE_VER_TOP:
   case mu32_RESIZE_VER:
      this->setCursor(Qt::SizeVerCursor);
      break;
   default:
      this->setCursor(Qt::ArrowCursor);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::m_CancelSearch(void) const
{
   //cancel search
   this->mpc_Ui->pc_LineEditSearch->setText("");
   this->mpc_Ui->pc_BtnCancelSearch->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   For moving or resizing.

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::mousePressEvent(QMouseEvent * const opc_Event)
{
   this->mc_OldPosition = this->mapToParent(opc_Event->pos());

   m_UpdateParentSize();

   C_OgeWiHover::mh_AdaptMouseRangePos(this->mc_OldPosition, this->mc_ParentWidgetSize, this->mc_ParentWidgetTopLeft,
                                       this->ms32_OffsetHorizontal, this->ms32_OffsetVertical);

   // check what the intention of the mouse click was
   if (opc_Event->button() == Qt::LeftButton)
   {
      this->mu32_ResizeMode = mh_GetResizeMode(opc_Event->pos(), this->size(), this->mq_Maximized);

      if (this->mu32_ResizeMode == mu32_RESIZE_NONE)
      {
         // moving only if the top of the hover widget was clicked
         if (opc_Event->pos().y() < ms32_MINIMIZED_HEIGHT)
         {
            this->mu32_MouseMode = mu32_MOUSE_MODE_MOVE;
         }
      }
      else
      {
         this->mu32_MouseMode = mu32_MOUSE_MODE_RESIZE;
      }
   }

   this->setFocus();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   For moving or resizing.

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   if (this->mu32_MouseMode == mu32_MOUSE_MODE_MOVE)
   {
      this->m_MoveWidget(opc_Event);
   }
   else if (this->mu32_MouseMode == mu32_MOUSE_MODE_RESIZE)
   {
      this->m_ResizeWidget(opc_Event);
   }
   else
   {
      m_HandleBasicCursorState(opc_Event->pos());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse release event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::mouseReleaseEvent(QMouseEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   this->mu32_MouseMode = mu32_MOUSE_MODE_NO_FUNC;

   m_HandleBasicCursorState(opc_Event->pos());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided resize event

   \param[in,out] opc_Event  Pointer to resize event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::resizeEvent(QResizeEvent * const opc_Event)
{
   // for maximizing animation
   if ((this->mq_Maximized == true) &&
       (this->mc_TimerAnimation.isActive() == false))
   {
      this->SetMaximizedHeight(opc_Event->size().height());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Moves the widget

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::m_MoveWidget(const QMouseEvent * const opc_Event)
{
   QPoint c_MousePos = this->mapToParent(opc_Event->pos());
   QPoint c_Delta;
   QPoint c_NewPoint;
   int32_t s32_TempMin;
   int32_t s32_TempMax;

   m_UpdateParentSize();

   C_OgeWiHover::mh_AdaptMouseRangePos(c_MousePos, this->mc_ParentWidgetSize, this->mc_ParentWidgetTopLeft,
                                       this->ms32_OffsetHorizontal, this->ms32_OffsetVertical);
   c_Delta = c_MousePos - this->mc_OldPosition;
   c_NewPoint = this->pos() + c_Delta;

   // check the borders
   s32_TempMin = this->mc_ParentWidgetTopLeft.x() - this->ms32_OffsetHorizontal;
   if (c_NewPoint.x() < s32_TempMin)
   {
      c_NewPoint.setX(s32_TempMin);
   }
   else
   {
      s32_TempMax =
         ((this->mc_ParentWidgetTopLeft.x() + this->mc_ParentWidgetSize.width()) - this->width()) +
         this->ms32_OffsetHorizontalRight;
      if (c_NewPoint.x() > s32_TempMax)
      {
         c_NewPoint.setX(s32_TempMax);
      }
   }

   s32_TempMin = this->mc_ParentWidgetTopLeft.y() - this->ms32_OffsetVertical;
   if (c_NewPoint.y() < s32_TempMin)
   {
      c_NewPoint.setY(s32_TempMin);
   }
   else
   {
      s32_TempMax = ((this->mc_ParentWidgetTopLeft.y() + this->mc_ParentWidgetSize.height()) - this->height()) +
                    this->ms32_OffsetVertical;
      if (c_NewPoint.y() > s32_TempMax)
      {
         c_NewPoint.setY(s32_TempMax);
      }
   }

   this->move(c_NewPoint);
   this->mc_OldPosition = c_MousePos;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resizes the widget

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiHover::m_ResizeWidget(const QMouseEvent * const opc_Event)
{
   QPoint c_MousePos = this->mapToParent(opc_Event->pos());
   QPoint c_Delta;
   QSize c_Size = this->size();
   QPoint c_Pos = this->pos();
   int32_t s32_Temp;

   m_UpdateParentSize();

   C_OgeWiHover::mh_AdaptMouseRangePos(c_MousePos, this->mc_ParentWidgetSize, this->mc_ParentWidgetTopLeft,
                                       this->ms32_OffsetHorizontal, this->ms32_OffsetVertical);
   c_Delta = c_MousePos - this->mc_OldPosition;

   if (this->mq_Maximized == false)
   {
      // if minimized, only the width of the widget might be changed
      c_Delta.setY(0);
   }

   // adapt size and position
   switch (this->mu32_ResizeMode)
   {
   case mu32_RESIZE_ALL_TOPLEFT:
      c_Size.setWidth(c_Size.width() - c_Delta.x());
      c_Pos.setX(c_Pos.x() + c_Delta.x());

      c_Size.setHeight(c_Size.height() - c_Delta.y());
      c_Pos.setY(c_Pos.y() + c_Delta.y());
      break;

   case mu32_RESIZE_ALL_TOPRIGHT:
      c_Size.setWidth(c_Size.width() + c_Delta.x());

      c_Size.setHeight(c_Size.height() - c_Delta.y());
      c_Pos.setY(c_Pos.y() + c_Delta.y());
      break;

   case mu32_RESIZE_ALL_BOTTOMLEFT:
      c_Size.setWidth(c_Size.width() - c_Delta.x());
      c_Pos.setX(c_Pos.x() + c_Delta.x());

      c_Size.setHeight(c_Size.height() + c_Delta.y());
      break;

   case mu32_RESIZE_ALL_BOTTOMRIGHT:
      c_Size.setHeight(c_Size.height() + c_Delta.y());

      c_Size.setWidth(c_Size.width() + c_Delta.x());
      break;

   case mu32_RESIZE_HOR:
      c_Size.setWidth(c_Size.width() + c_Delta.x());
      break;

   case mu32_RESIZE_HOR_LEFT:
      c_Size.setWidth(c_Size.width() - c_Delta.x());
      c_Pos.setX(c_Pos.x() + c_Delta.x());
      break;

   case mu32_RESIZE_VER:
      c_Size.setHeight(c_Size.height() + c_Delta.y());
      break;

   case mu32_RESIZE_VER_TOP:
      c_Size.setHeight(c_Size.height() - c_Delta.y());
      c_Pos.setY(c_Pos.y() + c_Delta.y());
      break;

   default:
      break;
   }

   // check new geometry
   s32_Temp = this->mc_ParentWidgetTopLeft.x() - this->ms32_OffsetHorizontal;
   if (c_Pos.x() < s32_Temp)
   {
      c_Pos.setX(s32_Temp);
      c_Size.setWidth(this->width());

      // don't change the 'next old position'
      c_MousePos.setX(s32_Temp);
   }
   s32_Temp = this->mc_ParentWidgetTopLeft.y() - this->ms32_OffsetVertical;
   if (c_Pos.y() < s32_Temp)
   {
      c_Pos.setY(s32_Temp);
      c_Size.setHeight(this->height());

      // don't change the 'next old position'
      c_MousePos.setY(s32_Temp);
   }
   if ((((c_Size.width() + c_Pos.x()) + this->mc_ParentWidgetTopLeft.x()) > this->mc_ParentWidgetSize.width()) ||
       (c_Size.width() <= this->minimumWidth()))
   {
      c_Pos.setX(this->x());
      c_Size.setWidth(this->width());

      // don't change the 'next old position'
      c_MousePos.setX(this->mc_OldPosition.x());
   }
   if ((((c_Size.height() + c_Pos.y()) + this->mc_ParentWidgetTopLeft.x()) > this->mc_ParentWidgetSize.height()) ||
       (c_Size.height() <= this->minimumHeight()))
   {
      c_Pos.setY(this->y());
      c_Size.setHeight(this->height());

      // don't change the 'next old position'
      c_MousePos.setY(this->mc_OldPosition.y());
   }

   // set new geometry
   this->setGeometry(QRect(c_Pos, c_Size));

   // for maximizing and minimizing animation
   if (this->mq_Maximized == true)
   {
      this->SetMaximizedHeight(c_Size.height());
   }

   this->mc_OldPosition = c_MousePos;
}
