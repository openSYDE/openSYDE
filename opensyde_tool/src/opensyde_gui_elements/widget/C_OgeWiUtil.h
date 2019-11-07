//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Util class for QWidget (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIUTIL_H
#define C_OGEWIUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QVariant>
#include <QKeyEvent>
#include <QFileDialog>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

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
                                                  const QSize & orc_DefaultSize,
                                                  const bool oq_AddLogEntryForWindowSize = false);
   static stw_types::sintn h_UpdateFontSize(QWidget * const opc_Widget, const QString & orc_Text,
                                            const stw_types::float32 of32_HeightScaling = 1.0F,
                                            const bool oq_IgnoreContentMargins = false,
                                            const QSize * const opc_ImprovedSize = NULL);
   static stw_types::sintn h_GetNextOptimalPointSize(const QFont & orc_Font, const QSize & orc_Size,
                                                     const QString & orc_Text,
                                                     const stw_types::float32 of32_HeightScaling = 1.0F);
   static QWidget * h_GetWidgetUnderNextPopUp(QWidget * const opc_Input);
   static bool h_CheckGlobalKey(const QKeyEvent * const opc_Event);
   static QString h_GetSaveFileName(QWidget * const opc_Parent, const QString & orc_Heading,
                                    const QString & orc_StartingFolder, const QString & orc_Filter,
                                    const QString & orc_DefaultFileName,
                                    const QFileDialog::Options oc_Option = QFileDialog::Options());
   static void h_ShowPathInvalidError(QWidget * const opc_Parent, const QString & orc_InvalidPaths);

private:
   C_OgeWiUtil(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
