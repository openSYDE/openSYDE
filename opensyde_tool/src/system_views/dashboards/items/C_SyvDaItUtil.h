//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for dashboard widget items (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITUTIL_H
#define C_SYVDAITUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItUtil
{
public:
   static void h_InitFontFamily(QLabel * const opc_Label);
   static void h_CopyFontSize(const QLabel * const opc_SourceLabel, QLabel * const opc_TargetLabel,
                              const stw_types::float32 of32_Factor = 1.0F);
   static void h_SyncFontSize(QLabel * const opc_Label1, QLabel * const opc_Label2, QLabel * const opc_Label3 = NULL,
                              QLabel * const opc_Label4 = NULL);
   static QString mh_GetHtmlIndentStyle(const stw_types::uint32 ou32_NumberOfTimes);

private:
   C_SyvDaItUtil(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
