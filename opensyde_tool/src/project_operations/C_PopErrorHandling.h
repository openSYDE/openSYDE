//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Central handling for errors (header)

   Central handling for errors (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_POPERRORHANDLING_H
#define C_POPERRORHANDLING_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QString>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PopErrorHandling
{
public:
   static void mh_ProjectLoadErr(const stw_types::sint32 & ors32_Err, const QString & orc_Path,
                                 QWidget * const opc_Parent, const stw_types::uint16 ou16_SystemDefinitionVersion);
   static void mh_ProjectSaveErr(const stw_types::sint32 & ors32_Err, QWidget * const opc_Parent);
   static void mh_GetTextInitializeErr(const stw_types::sint32 & ors32_Err);

private:
   C_PopErrorHandling();
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
