//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Table drawing delegate for dashboard table widget (implementation)

   Table drawing delegate for dashboard table widget

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "constants.h"
#include "C_SdNdeDataPoolUtil.h"
#include "C_SyvDaItTaDelegate.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

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

   \created     29.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaItTaDelegate::C_SyvDaItTaDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply style

   \param[in] oe_Style    New style type
   \param[in] oq_DarkMode Flag if dark mode is active

   \created     29.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItTaDelegate::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   switch (oe_Style)
   {
   //TBD by Karsten: done
   case C_PuiSvDbWidgetBase::eOPENSYDE:
      if (oq_DarkMode == true)
      {
         this->mc_BarColor = mc_STYLE_GUIDE_COLOR_24;
         this->mc_BackgroundColorDefault = mc_STYLE_GUIDE_COLOR_36;
         this->mc_BackgroundColorAlternating = mc_STYLE_GUIDE_COLOR_8;
         this->mc_BackgroundColorSelection = mc_STYLE_GUIDE_COLOR_2_50P;
         this->mc_BorderColorHover = mc_STYLE_GUIDE_COLOR_2;
      }
      else
      {
         this->mc_BarColor = mc_STYLE_GUIDE_COLOR_23;
         this->mc_BackgroundColorDefault = mc_STYLE_GUIDE_COLOR_12;
         this->mc_BackgroundColorAlternating = mc_STYLE_GUIDE_COLOR_11;
         this->mc_BackgroundColorSelection = mc_STYLE_GUIDE_COLOR_10;
         this->mc_BorderColorHover = mc_STYLE_GUIDE_COLOR_7;
      }
      break;

   case C_PuiSvDbWidgetBase::eFLAT:
      if (oq_DarkMode == true)
      {
         this->mc_BarColor = mc_STYLE_GUIDE_COLOR_23;
         this->mc_BackgroundColorDefault = mc_STYLE_GUIDE_COLOR_39;
         this->mc_BackgroundColorAlternating = mc_STYLE_GUIDE_COLOR_34;
         this->mc_BackgroundColorSelection = mc_STYLE_GUIDE_COLOR_9;
         this->mc_BorderColorHover = mc_STYLE_GUIDE_COLOR_9;
      }
      else
      {
         this->mc_BarColor = mc_STYLE_GUIDE_COLOR_24;
         this->mc_BackgroundColorDefault = mc_STYLE_GUIDE_COLOR_11;
         this->mc_BackgroundColorAlternating = mc_STYLE_GUIDE_COLOR_38;
         this->mc_BackgroundColorSelection = mc_STYLE_GUIDE_COLOR_26_40P;
         this->mc_BorderColorHover = mc_STYLE_GUIDE_COLOR_26_40P;
      }
      break;
   case C_PuiSvDbWidgetBase::eSKEUOMORPH:
      if (oq_DarkMode == true)
      {
         this->mc_BarColor = mc_STYLE_GUIDE_COLOR_23;
         this->mc_BackgroundColorDefault = mc_STYLE_GUIDE_COLOR_34;
         this->mc_BackgroundColorAlternating = mc_STYLE_GUIDE_COLOR_33;
         this->mc_BackgroundColorSelection = mc_STYLE_GUIDE_COLOR_9;
         this->mc_BorderColorHover = mc_STYLE_GUIDE_COLOR_9;
      }
      else
      {
         this->mc_BarColor = mc_STYLE_GUIDE_COLOR_23;
         this->mc_BackgroundColorDefault = mc_STYLE_GUIDE_COLOR_11;
         this->mc_BackgroundColorAlternating = mc_STYLE_GUIDE_COLOR_37;
         this->mc_BackgroundColorSelection = mc_STYLE_GUIDE_COLOR_35;
         this->mc_BorderColorHover = mc_STYLE_GUIDE_COLOR_35;
      }
      break;
   case C_PuiSvDbWidgetBase::eOPENSYDE_2:
      if (oq_DarkMode == true)
      {
         this->mc_BarColor = mc_STYLE_GUIDE_COLOR_23;
         this->mc_BackgroundColorDefault = Qt::transparent;     //mc_STYLE_GUIDE_COLOR_34;//50;
         this->mc_BackgroundColorAlternating = Qt::transparent; //mc_STYLE_GUIDE_COLOR_34;//50;
         this->mc_BackgroundColorSelection = mc_STYLE_GUIDE_COLOR_9;
         this->mc_BorderColorHover = mc_STYLE_GUIDE_COLOR_9;
      }
      else
      {
         this->mc_BarColor = mc_STYLE_GUIDE_COLOR_23;
         this->mc_BackgroundColorDefault = Qt::transparent;     //mc_STYLE_GUIDE_COLOR_38;//45;
         this->mc_BackgroundColorAlternating = Qt::transparent; //mc_STYLE_GUIDE_COLOR_38;//45;
         this->mc_BackgroundColorSelection = mc_STYLE_GUIDE_COLOR_25;
         this->mc_BorderColorHover = mc_STYLE_GUIDE_COLOR_25;
      }
      break;

   default:
      break;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paint item

   Here: special handling for bar column

   \param[in,out] opc_Painter Painter
   \param[in]     orc_Option  Option
   \param[in]     orc_Index   Index

   \created     29.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItTaDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                const QModelIndex & orc_Index) const
{
   //Draw background (used to fill in transparent grid)
   C_SdNdeDataPoolUtil::h_DrawTableBackgroundGeneric(opc_Painter, orc_Option, this->mc_BackgroundColorDefault,
                                                     this->mc_BackgroundColorAlternating,
                                                     this->mc_BackgroundColorSelection);
   //Draw element
   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   if (orc_Index.column() == 3)
   {
      //Padding
      const sintn sn_OffsetX = 7;
      const sintn sn_OffsetY = 1;
      //Expected progress [0;1]
      const float32 f32_Progress = orc_Index.data(static_cast<sintn>(Qt::EditRole)).toFloat();
      //Complete rectangle with padding
      const QRect c_RectBarBase = orc_Option.rect.adjusted(sn_OffsetX, sn_OffsetY, -sn_OffsetX, -sn_OffsetY);
      const float32 f32_RectBarBaseWidth = static_cast<float32>(c_RectBarBase.width());
      const float32 f32_RectBarWidth = f32_Progress * f32_RectBarBaseWidth;
      //Progress rectangle
      const QRect c_RectBar(c_RectBarBase.topLeft(),
                            QSize(static_cast<sintn>(f32_RectBarWidth), c_RectBarBase.height()));
      opc_Painter->save();
      opc_Painter->setPen(Qt::NoPen);
      opc_Painter->setBrush(QBrush(this->mc_BarColor));
      opc_Painter->drawRect(c_RectBar);
      opc_Painter->restore();
   }
   //TODO draw hover border
}
