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

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsLineBase
{
public:
   C_PuiBsLineBase(void);
   virtual ~C_PuiBsLineBase(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   QColor c_UIColor;                           ///< color of line based item
   stw_types::sint32 s32_UIWidthPixels;        ///< width of line based item in pixels
   stw_types::float64 f64_ZOrder;              ///< Z Position of line based item in scene
   std::vector<QPointF> c_UIInteractionPoints; ///< interaction points of line based item in relation to scene
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
