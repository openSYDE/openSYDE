//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for table/tree delegates (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLDELEGATEUTIL_HPP
#define C_TBLDELEGATEUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QRect>
#include <QPainter>
#include <QModelIndex>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblDelegateUtil
{
public:
   static bool h_PaintMarkedCell(QPainter * const opc_Painter, const QRect & orc_CellRect,
                                 const QModelIndex & orc_Index, const bool oq_Selected,
                                 const QColor & orc_DefaultBackgroundColor, const QColor & orc_DefaultBorderColor,
                                 const QFont & orc_DefaultFont, const QColor & orc_HighlightForegroundColor,
                                 const QColor & orc_HighlightBackgroundColor, const QColor & orc_HighlightBorderColor,
                                 const QFont & orc_HighlightFont, const int32_t os32_HexSpacing,
                                 const int32_t os32_DecimalSpacing);
   static bool h_PaintStringWithRestriction(QPainter * const opc_Painter, QPoint & orc_TopLeft,
                                            const QRect & orc_CompleteCell, const QString & orc_Text,
                                            const QColor & orc_Background, const QColor & orc_Border,
                                            const QColor & orc_Foreground, const QFont & orc_Font,
                                            const QFlags<Qt::AlignmentFlag> & orc_Alignment,
                                            const int32_t os32_CellWidth = -1, const int32_t os32_MarginFront = 0,
                                            const int32_t os32_MarginBack = 0);

private:
   C_TblDelegateUtil(void);
   static int32_t mh_GetSegmentWidth(const QPoint & orc_TopLeft, const QRect & orc_CompleteCell,
                                     QString & orc_DrawnText, const QFont & orc_Font, const int32_t os32_CellWidth,
                                     const int32_t os32_MarginFront, const int32_t os32_MarginBack, bool & orq_Changed);
   static QStringList mh_CreateAndStandardizeStringList(const QString & orc_Input, bool & orq_IsHex);
   static QColor mh_GetColorTransparent(const QColor & orc_ColorBase, const char_t ocn_Transparency);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
