//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table drawing delegate for dashboard table widget (implementation)

   Table drawing delegate for dashboard table widget

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QSvgRenderer>

#include "stwtypes.hpp"
#include "constants.hpp"
#include "C_SyvDaItTaModel.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_SyvDaItTaDelegate.hpp"
#include "C_TblTreDelegateUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

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
C_SyvDaItTaDelegate::C_SyvDaItTaDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in] oe_Style    New style type
   \param[in] oq_DarkMode Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaDelegate::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   switch (oe_Style)
   {
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paint item

   Here: special handling for bar column

   \param[in,out] opc_Painter Painter
   \param[in]     orc_Option  Option
   \param[in]     orc_Index   Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItTaDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                const QModelIndex & orc_Index) const
{
   //Draw background (used to fill in transparent grid)
   C_SdNdeDpUtil::h_DrawTableBackgroundGeneric(opc_Painter, orc_Option, this->mc_BackgroundColorDefault,
                                               this->mc_BackgroundColorAlternating,
                                               this->mc_BackgroundColorSelection);
   //Draw element
   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   // progress bar
   if (orc_Index.column() == C_SyvDaItTaModel::h_EnumToColumn(C_SyvDaItTaModel::eBAR))
   {
      //Padding
      const int32_t s32_OFFSET_X = 7;
      const int32_t s32_OFFSET_Y = 1;
      //Expected progress [0;1]
      const float32_t f32_Progress = orc_Index.data(static_cast<int32_t>(Qt::EditRole)).toFloat();
      //Complete rectangle with padding
      const QRect c_RectBarBase = orc_Option.rect.adjusted(s32_OFFSET_X, s32_OFFSET_Y, -s32_OFFSET_X, -s32_OFFSET_Y);
      const float32_t f32_RectBarBaseWidth = static_cast<float32_t>(c_RectBarBase.width());
      const float32_t f32_RectBarWidth = f32_Progress * f32_RectBarBaseWidth;
      //Progress rectangle
      const QRect c_RectBar(c_RectBarBase.topLeft(),
                            QSize(static_cast<int32_t>(f32_RectBarWidth), c_RectBarBase.height()));
      opc_Painter->save();
      opc_Painter->setPen(Qt::NoPen);
      opc_Painter->setBrush(static_cast<QBrush>(this->mc_BarColor));
      opc_Painter->drawRect(c_RectBar);
      opc_Painter->restore();
   }

   // icon
   C_TblTreDelegateUtil::h_PaintIcon(opc_Painter, orc_Option, orc_Index);

   //TODO draw hover border
}
