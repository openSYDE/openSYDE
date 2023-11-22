//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Central handling for errors (header)

   Central handling for errors (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_POPERRORHANDLING_HPP
#define C_POPERRORHANDLING_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QString>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PopErrorHandling
{
public:
   static void h_ProjectLoadErr(const int32_t & ors32_Err, const QString & orc_Path, QWidget * const opc_Parent,
                                const uint16_t ou16_SystemDefinitionVersion);
   static void h_ProjectSaveErr(const int32_t & ors32_Err, QWidget * const opc_Parent);
   static void h_ServiceProjectSaveErr(const int32_t & ors32_Err, QWidget * const opc_Parent);
   static void h_GetTextInitializeErr(const int32_t & ors32_Err);

private:
   C_PopErrorHandling();
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
