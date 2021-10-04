//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate for HALC channel tree.

   This class handles style of HALC channel tree items.
   Not using standard base class C_TblDelegate as we don't need editors.

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>

#include "constants.h"
#include "C_SdNdeHalcChannelTreeDelegate.h"
#include "C_Uti.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor/destructor

   \param[in,out]  opc_Parent    Parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcChannelTreeDelegate::C_SdNdeHalcChannelTreeDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent),
   msn_CurrentIndexParentRow(-1)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden paint event

   Here: Draw special background of channel ID and paint parent selected state.

   \param[in,out]  opc_Painter   Painter
   \param[in]      orc_Option    Option
   \param[in]      orc_Index     Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                           const QModelIndex & orc_Index) const
{
   if (orc_Index.parent().isValid() == true)
   {
      // channel case: draw special background of channel ID
      QStyleOptionViewItem c_Option = orc_Option;
      this->initStyleOption(&c_Option, orc_Index);

      // calculate width of special background box via text size
      QStringList c_Texts = c_Option.text.split(' ');

      const QFontMetrics c_FontMetrics = QFontMetrics(C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_13));
      const QRect c_Rect = QRect(orc_Option.rect.x() + 23 /*16px icon + 7px padding*/, orc_Option.rect.y() + 4,
                                 c_FontMetrics.boundingRect(c_Texts[0]).width() + 5, 18);

      opc_Painter->save();

      opc_Painter->setBrush(mc_STYLE_GUIDE_COLOR_11);
      opc_Painter->setPen(Qt::transparent);
      opc_Painter->drawRoundedRect(c_Rect, 2.0, 2.0);

      opc_Painter->restore();

      // draw original
      QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   }
   else
   {
      // paint parent of current item in selected style
      QStyleOptionViewItem c_Option = orc_Option;

      if (orc_Index.row() == this->msn_CurrentIndexParentRow)
      {
         c_Option.state.setFlag(QStyle::State_Selected, true);
         c_Option.state.setFlag(QStyle::State_Enabled, true);
      }

      // draw original
      QStyledItemDelegate::paint(opc_Painter, c_Option, orc_Index);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remember parent row of current index.

   \param[in]  osn_ParentRow  Parent row
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeDelegate::SetCurrentIndexParentRow(const stw_types::sintn osn_ParentRow)
{
   this->msn_CurrentIndexParentRow = osn_ParentRow;
}
