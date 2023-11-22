//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class with size functionality (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIBISIZEABLEITEM_HPP
#define C_GIBISIZEABLEITEM_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#include <QObject>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiBiSizeableItem
{
public:
   C_GiBiSizeableItem();
   C_GiBiSizeableItem(const float64_t of64_Width, const float64_t of64_Height);
   virtual ~C_GiBiSizeableItem();

   float64_t f64_Width;
   float64_t f64_Height;

   QRectF GetSizeRect(void) const;

   virtual void Redraw(void) = 0;

private:
   //Avoid call
   C_GiBiSizeableItem(const C_GiBiSizeableItem &);
   C_GiBiSizeableItem & operator =(const C_GiBiSizeableItem &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
