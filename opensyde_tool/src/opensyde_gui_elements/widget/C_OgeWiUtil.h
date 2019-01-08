//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Util class for QWidget (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.10.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEWIUTIL_H
#define C_OGEWIUTIL_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>
#include <QVariant>
#include <QKeyEvent>

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeWiUtil
{
public:
   static void h_DrawBackground(QWidget * const opc_Widget, QPainter * const opc_Painter = NULL);
   static void h_ApplyStylesheetProperty(QWidget * const opc_Widget, const stw_types::charn * const opcn_Name,
                                         const QVariant & orc_Value);
   static void h_ApplyStylesheetPropertyToItselfAndAllChildren(QWidget * const opc_Widget,
                                                               const stw_types::charn * const opcn_Name,
                                                               const QVariant & orc_Value);
   static void h_SetWindowIcon(QWidget * const opc_Widget);
   static void h_CheckAndFixDialogPositionAndSize(QPoint & orc_GlobalPosition, QSize & orc_Size,
                                                  const QSize & orc_DefaultSize);
   static stw_types::sintn h_UpdateFontSize(QWidget * const opc_Widget, const QString & orc_Text,
                                            const stw_types::float32 of32_HeightScaling = 1.0F,
                                            const bool oq_IgnoreContentMargins = false,
                                            const QSize * const opc_ImprovedSize = NULL);
   static stw_types::sintn h_GetNextOptimalPointSize(const QFont & orc_Font, const QSize & orc_Size,
                                                     const QString & orc_Text,
                                                     const stw_types::float32 of32_HeightScaling = 1.0F);
   static QWidget * h_GetWidgetUnderNextPopUp(QWidget * const opc_Input);
   static bool h_CheckGlobalKey(const QKeyEvent * const opc_Event);

private:
   C_OgeWiUtil(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
