//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI base class for box based data: stores UI information (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUIBSBOXBASE_H
#define C_PUIBSBOXBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPointF>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiBsBox
{
public:
   C_PuiBsBox();
   virtual ~C_PuiBsBox(void);

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   QPointF c_UiPosition;   ///< Position of box based item in relation to scene
   float64_t f64_Width;    ///< Width of box based item in scene
   float64_t f64_Height;   ///< Height of box based item in scene
   float64_t f64_ZetOrder; ///< Z Position of box based item in scene
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
