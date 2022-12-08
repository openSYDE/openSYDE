//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI base class for segmented line based data: stores UI information (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUIBSLINEBASE_H
#define C_PUIBSLINEBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QColor>
#include <QPointF>
#include <vector>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsLineBase
{
public:
   C_PuiBsLineBase(void);
   virtual ~C_PuiBsLineBase(void);

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   QColor c_UiColor;                           ///< color of line based item
   int32_t s32_UiWidthPixels;                  ///< width of line based item in pixels
   float64_t f64_ZetOrder;                     ///< Z Position of line based item in scene
   std::vector<QPointF> c_UiInteractionPoints; ///< interaction points of line based item in relation to scene
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
