//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Implementation for drawing element boundary (implementation)

   detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.10.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPen>
#include <QGraphicsView>

#include "stwtypes.h"
#include "C_GiBiBoundary.h"
#include "C_OgePopUpDialog.h"
#include "C_GiSyBaseWidget.h"
#include "C_GiSyBoundaryWidget.h"
#include "C_PuiSdDataElement.h"
#include "gitypes.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */
const float64 mf64_ActionPointOffsetBoundary = 8.0;

const float64 C_GiBiBoundary::mhf64_MinWidthBoundary = 50.0;
const float64 C_GiBiBoundary::mhf64_MinHeightBoundary = 50.0;

const QString mc_NAME_BOUNDARY = "BOUNDARY";

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     oru64_ID     Unique ID
   \param[in]     orf64_Width  Width of node
   \param[in]     orf64_Height Height of node
   \param[in,out] opc_Parent   Optional pointer to parent

   \created     27.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiBiBoundary::C_GiBiBoundary(const uint64 & oru64_ID, const float64 of64_Width, const float64 of64_Height,
                               QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiBiRectBaseGroup(oru64_ID,
                       mhf64_MinWidthBoundary, mhf64_MinHeightBoundary, mf64_ActionPointOffsetBoundary, false,
                       opc_Parent)
{
   //lint -e{1938}  static const is guaranteed preinitialized before main
   QRectF c_Rect = QRectF(0.0, 0.0,
                          std::max(mhf64_MinWidthBoundary, of64_Width),
                          std::max(mhf64_MinHeightBoundary, of64_Height));
   QPen c_Pen;
   QBrush c_Brush;

   //set default colors
   // border
   this->mpc_Rectangle = new C_GiRect(c_Rect);
   c_Pen = this->mpc_Rectangle->pen();
   c_Pen.setColor(QColor(39, 170, 225));
   c_Pen.setJoinStyle(Qt::MiterJoin);
   this->mpc_Rectangle->setPen(c_Pen);

   // background
   c_Brush = this->mpc_Rectangle->brush();
   c_Brush.setColor(QColor(250, 250, 250));
   c_Brush.setStyle(Qt::SolidPattern);
   this->mpc_Rectangle->setBrush(c_Brush);

   // Notify the base class about the boundary as biggest item as orientation. Very important!
   this->m_SetBiggestItem(*this->mpc_Rectangle);

   this->addToGroup(this->mpc_Rectangle);

   // Init z order
   this->setZValue(mf64_ZORDER_INIT_BOUNDARY);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     25.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiBiBoundary::~C_GiBiBoundary()
{
   //Deleted via Qt parent mechanism
   this->mpc_Rectangle = NULL;
   //lint -e{1740}  no memory leak because of the parent of mpc_Rectangle and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the type of this itme

   \return  ID

   \created     01.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sintn C_GiBiBoundary::type(void) const
{
   return msn_GRAPHICS_ITEM_BOUNDARY;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Open style dialog

   \param[in] oq_DarkMode Optional dark mode flag

   \created     23.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_GiBiBoundary::OpenStyleDialog(const bool oq_DarkMode)
{
   bool q_Retval;
   QGraphicsView * const pc_View = this->scene()->views().at(0);

   QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
   C_GiSyBaseWidget * pc_Dialog = new C_GiSyBaseWidget(*c_New, mc_NAME_BOUNDARY, oq_DarkMode);
   C_GiSyBoundaryWidget * pc_SettingsWidget = new C_GiSyBoundaryWidget(*pc_Dialog);

   pc_SettingsWidget->SetBorderColor(this->GetBorderColor());
   pc_SettingsWidget->SetBackgroundColor(this->GetBackgroundColor());
   pc_SettingsWidget->SetBorderWidth(this->GetBorderWidth());

   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      this->SetBorderColor(pc_SettingsWidget->GetBorderColor());
      this->SetBackgroundColor(pc_SettingsWidget->GetBackgroundColor());
      this->SetBorderWidth(pc_SettingsWidget->GetBorderWidth());
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
   //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set border width

   \param[in] ors32_Width New line width

   \created     28.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiBiBoundary::SetBorderWidth(const stw_types::sint32 & ors32_Width)
{
   QRectF c_Rect = this->mpc_Rectangle->rect();
   float64 f64_WidthDiff;
   QPen c_Pen = this->mpc_Rectangle->pen();

   f64_WidthDiff = static_cast<float64>(ors32_Width) - static_cast<float64>(c_Pen.width());
   f64_WidthDiff = f64_WidthDiff * -1.0;
   c_Pen.setWidth(ors32_Width);

   this->mpc_Rectangle->setPen(c_Pen);

   //Resize
   this->mpc_Rectangle->setPos(this->mpc_Rectangle->pos() - QPointF(f64_WidthDiff / 2.0, f64_WidthDiff / 2.0));
   c_Rect.setWidth(c_Rect.size().width() + f64_WidthDiff);
   c_Rect.setHeight(c_Rect.size().height() + f64_WidthDiff);
   this->mpc_Rectangle->setRect(c_Rect);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set border color

   \param[in] orc_Color New color

   \created     28.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiBiBoundary::SetBorderColor(const QColor & orc_Color)
{
   QPen c_Pen = this->mpc_Rectangle->pen();

   c_Pen.setColor(orc_Color);

   this->mpc_Rectangle->setPen(c_Pen);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set background color

   \param[in] orc_Color New color

   \created     28.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiBiBoundary::SetBackgroundColor(const QColor & orc_Color)
{
   QBrush c_Brush = this->mpc_Rectangle->brush();

   c_Brush.setColor(orc_Color);

   this->mpc_Rectangle->setBrush(c_Brush);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get width

   \created     28.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_GiBiBoundary::GetBorderWidth() const
{
   return this->mpc_Rectangle->pen().width();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get color

   \created     28.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QColor C_GiBiBoundary::GetBorderColor() const
{
   return this->mpc_Rectangle->pen().color();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get color

   \created     28.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QColor C_GiBiBoundary::GetBackgroundColor() const
{
   return this->mpc_Rectangle->brush().color();
}

//-----------------------------------------------------------------------------
void C_GiBiBoundary::m_ResizeUpdateItems(const float64 of64_DiffWidth, const float64 of64_DiffHeight)
{
   Q_UNUSED(of64_DiffWidth)
   Q_UNUSED(of64_DiffHeight)

   this->mpc_Rectangle->update();
}

//-----------------------------------------------------------------------------
/*!
   \brief   load from internal data

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiBiBoundary::m_LoadFromData(const C_PuiBsBoundary & orc_Data, const bool oq_DarkMode)
{
   this->LoadBasicData(orc_Data);
   if (oq_DarkMode == true)
   {
      this->SetBackgroundColor(orc_Data.c_UIBackgroundColorDark);
      this->SetBorderColor(orc_Data.c_UIBorderColorDark);
   }
   else
   {
      this->SetBackgroundColor(orc_Data.c_UIBackgroundColorBright);
      this->SetBorderColor(orc_Data.c_UIBorderColorBright);
   }
   this->SetBorderWidth(orc_Data.s32_UIBorderWidth);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update internal data

   \created     21.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiBiBoundary::m_UpdateData(C_PuiBsBoundary & orc_Data, const bool oq_DarkMode) const
{
   this->UpdateBasicData(orc_Data);

   if (oq_DarkMode == true)
   {
      orc_Data.c_UIBorderColorDark = this->GetBorderColor();
      orc_Data.c_UIBackgroundColorDark = this->GetBackgroundColor();
   }
   else
   {
      orc_Data.c_UIBorderColorBright = this->GetBorderColor();
      orc_Data.c_UIBackgroundColorBright = this->GetBackgroundColor();
   }
   orc_Data.s32_UIBorderWidth = this->GetBorderWidth();
}
