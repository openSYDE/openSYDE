//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class with size functionality (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIBISIZEABLEITEM_H
#define C_GIBISIZEABLEITEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include <QObject>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiBiSizeableItem
{
public:
   C_GiBiSizeableItem();
   C_GiBiSizeableItem(const stw_types::float64 of64_Width, const stw_types::float64 of64_Height);
   virtual ~C_GiBiSizeableItem();

   stw_types::float64 f64_Width;
   stw_types::float64 f64_Height;

   QRectF GetSizeRect(void) const;

   virtual void Redraw(void) = 0;

private:
   //Avoid call
   C_GiBiSizeableItem(const C_GiBiSizeableItem &);
   C_GiBiSizeableItem & operator =(const C_GiBiSizeableItem &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
