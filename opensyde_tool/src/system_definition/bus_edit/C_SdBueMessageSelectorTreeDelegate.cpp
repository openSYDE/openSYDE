//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate for message/signal selection tree.

   This class handles style of message/signal selection tree items.
   Not using standard base class C_TblDelegate as we don't need editors.

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>

#include "constants.hpp"
#include "C_SdBueMessageSelectorTreeDelegate.hpp"
#include "C_Uti.hpp"

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
/*! \brief  Default constructor/destructor

   \param[in,out]  opc_Parent    Parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageSelectorTreeDelegate::C_SdBueMessageSelectorTreeDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden paint event

   Here: Draw special background of PGN and SPN in case of J1939 protocol.

   \param[in,out]  opc_Painter   Painter
   \param[in]      orc_Option    Option
   \param[in]      orc_Index     Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                               const QModelIndex & orc_Index) const
{
   QStyleOptionViewItem c_Option = orc_Option;

   this->initStyleOption(&c_Option, orc_Index);

   QStringList c_Texts = c_Option.text.split(' ');

   if ((c_Texts.empty() == false) && ((c_Texts[0] == "PGN") || (c_Texts[0] == "SPN")))
   {
      // Highlighted text is "PGN 123" or "SPN 123"
      const QString c_SpecialText = c_Texts[0] + " " + c_Texts[1];

      // calculate width of special background box via text size
      const QFontMetrics c_FontMetrics = QFontMetrics(C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_13));
      const QRect c_Rect = QRect(orc_Option.rect.x() + 23 /*16px icon + 7px padding*/, orc_Option.rect.y() + 4,
                                 c_FontMetrics.boundingRect(c_SpecialText).width() + 5, 18);

      opc_Painter->save();

      opc_Painter->setBrush(mc_STYLE_GUIDE_COLOR_11);
      opc_Painter->setPen(Qt::transparent);
      opc_Painter->drawRoundedRect(c_Rect, 2.0, 2.0);

      opc_Painter->restore();
   }

   // draw original
   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
}
