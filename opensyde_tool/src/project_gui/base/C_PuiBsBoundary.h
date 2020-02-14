//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI boundary data: stores UI information (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUIBSBOUNDARY_H
#define C_PUIBSBOUNDARY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QColor>

#include "stwtypes.h"
#include "C_PuiBsBox.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsBoundary :
   public C_PuiBsBox
{
public:
   C_PuiBsBoundary();

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const override;

   QColor c_UIBorderColorBright;        ///< color of border line in bright mode
   QColor c_UIBorderColorDark;          ///< color of border line in dark mode
   QColor c_UIBackgroundColorBright;    ///< color of background in bright mode
   QColor c_UIBackgroundColorDark;      ///< color of background in dark mode
   stw_types::sint32 s32_UIBorderWidth; ///< width of border line
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
