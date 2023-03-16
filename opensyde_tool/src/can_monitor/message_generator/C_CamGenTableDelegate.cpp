//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate component for message generator table (implementation)

   Delegate component for message generator table

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_Uti.hpp"
#include "constants.hpp"
#include "C_TblDelegateUtil.hpp"
#include "C_CamGenTableDelegate.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QColor C_CamGenTableDelegate::mhc_DEFAULT_BORDER_COLOR = Qt::transparent;
const QColor C_CamGenTableDelegate::mhc_DEFAULT_BACKGROUND_COLOR = Qt::transparent;
const QFont C_CamGenTableDelegate::mhc_DEFAULT_FONT = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_12);
const QColor C_CamGenTableDelegate::mhc_HIGHLIGHT_BACKGROUND_COLOR = Qt::transparent;
const QColor C_CamGenTableDelegate::mhc_HIGHLIGHT_FOREGROUND_COLOR = mc_STYLE_GUIDE_COLOR_0;
const QColor C_CamGenTableDelegate::mhc_HIGHLIGHT_BORDER_COLOR = Qt::transparent;
const QFont C_CamGenTableDelegate::mhc_HIGHLIGHT_FONT = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_12);

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamGenTableDelegate::C_CamGenTableDelegate(QObject * const opc_Parent) :
   C_CamTblDelegate(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Paint item

   Here: paint data manually (for alignment)

   \param[in,out] opc_Painter Painter
   \param[in]     orc_Option  Option
   \param[in]     orc_Index   Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenTableDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                  const QModelIndex & orc_Index) const
{
   const QRect c_PaddedCellRect = orc_Option.rect.adjusted(3, 1, -3, -2);
   const bool q_Selected = orc_Option.state.testFlag(QStyle::State_Selected);

   //Always draw cell (should only be background if any additional paint operations are done)
   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   if (C_TblDelegateUtil::h_PaintMarkedCell(opc_Painter, c_PaddedCellRect, orc_Index, q_Selected,
                                            C_CamGenTableDelegate::mhc_DEFAULT_BACKGROUND_COLOR,
                                            C_CamGenTableDelegate::mhc_DEFAULT_BORDER_COLOR,
                                            C_CamGenTableDelegate::mhc_DEFAULT_FONT,
                                            C_CamGenTableDelegate::mhc_HIGHLIGHT_FOREGROUND_COLOR,
                                            C_CamGenTableDelegate::mhc_HIGHLIGHT_BACKGROUND_COLOR,
                                            C_CamGenTableDelegate::mhc_HIGHLIGHT_BORDER_COLOR,
                                            C_CamGenTableDelegate::mhc_HIGHLIGHT_FONT, 19, 25) == true)
   {
      // to make sure text is not painted twice
      tgl_assert(orc_Index.data(static_cast<int32_t>(Qt::DisplayRole)).toString().isEmpty());
   }
}
