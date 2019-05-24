//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for table/tree delegates (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLDELEGATEUTIL_H
#define C_TBLDELEGATEUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QRect>
#include <QPainter>
#include <QModelIndex>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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
                                 const QFont & orc_HighlightFont, const stw_types::sintn osn_HexSpacing,
                                 const stw_types::sintn osn_DecimalSpacing);
   static bool h_PaintStringWithRestriction(QPainter * const opc_Painter, QPoint & orc_TopLeft,
                                            const QRect & orc_CompleteCell, const QString & orc_Text,
                                            const QColor & orc_Background, const QColor & orc_Border,
                                            const QColor & orc_Foreground, const QFont & orc_Font,
                                            const QFlags<Qt::AlignmentFlag> & orc_Alignment,
                                            const stw_types::sint32 os32_CellWidth = -1,
                                            const stw_types::sint32 os32_MarginFront = 0,
                                            const stw_types::sint32 os32_MarginBack = 0);

private:
   C_TblDelegateUtil(void);
   static QStringList mhc_CreateAndStandardizeStringList(const QString & orc_Input, bool & orq_IsHex);
   static QColor mh_GetColorTransparent(const QColor & orc_ColorBase, const stw_types::charn ocn_Transparency);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
