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

#include "stwtypes.hpp"
#include "C_PuiBsBox.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsBoundary :
   public C_PuiBsBox
{
public:
   C_PuiBsBoundary();

   void CalcHash(uint32_t & oru32_HashValue) const override;

   QColor c_UiBorderColorBright;     ///< color of border line in bright mode
   QColor c_UiBorderColorDark;       ///< color of border line in dark mode
   QColor c_UiBackgroundColorBright; ///< color of background in bright mode
   QColor c_UiBackgroundColorDark;   ///< color of background in dark mode
   int32_t s32_UiBorderWidth;        ///< width of border line
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
