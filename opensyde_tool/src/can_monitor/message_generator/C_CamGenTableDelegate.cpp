//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Delegate component for message generator table (implementation)

   Delegate component for message generator table

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.02.2019  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_Uti.h"
#include "constants.h"
#include "C_TblDelegateUtil.h"
#include "C_CamGenTableDelegate.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */
const QColor C_CamGenTableDelegate::mhc_DefaultBorderColor = Qt::transparent;
const QColor C_CamGenTableDelegate::mhc_DefaultBackgroundColor = Qt::transparent;
const QFont C_CamGenTableDelegate::mhc_DefaultFont = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_12);
const QColor C_CamGenTableDelegate::mhc_HighlightBackgroundColor = Qt::transparent;
const QColor C_CamGenTableDelegate::mhc_HighlightForegroundColor = mc_STYLE_GUIDE_COLOR_0;
const QColor C_CamGenTableDelegate::mhc_HighlightBorderColor = Qt::transparent;
const QFont C_CamGenTableDelegate::mhc_HighlightFont = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_12);

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent

   \created     06.02.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_CamGenTableDelegate::C_CamGenTableDelegate(QObject * const opc_Parent) :
   C_TblDelegate(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paint item

   Here: paint data manually (for alignment)

   \param[in,out] opc_Painter Painter
   \param[in]     orc_Option  Option
   \param[in]     orc_Index   Index

   \created     06.02.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamGenTableDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                  const QModelIndex & orc_Index) const
{
   const QRect c_PaddedCellRect = orc_Option.rect.adjusted(3, 1, -3, -2);
   const bool q_Selected = orc_Option.state.testFlag(QStyle::State_Selected);

   //Always draw cell (should only be background if any additional paint operations are done)
   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   if (C_TblDelegateUtil::h_PaintMarkedCell(opc_Painter, c_PaddedCellRect, orc_Index, q_Selected,
                                            C_CamGenTableDelegate::mhc_DefaultBackgroundColor,
                                            C_CamGenTableDelegate::mhc_DefaultBorderColor,
                                            C_CamGenTableDelegate::mhc_DefaultFont,
                                            C_CamGenTableDelegate::mhc_HighlightForegroundColor,
                                            C_CamGenTableDelegate::mhc_HighlightBackgroundColor,
                                            C_CamGenTableDelegate::mhc_HighlightBorderColor,
                                            C_CamGenTableDelegate::mhc_HighlightFont, 19, 25) == true)
   {
   }
}
