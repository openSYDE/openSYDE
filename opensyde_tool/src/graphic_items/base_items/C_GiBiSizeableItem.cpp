//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Class with size functionality (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.10.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QRectF>

#include "stwtypes.h"
#include "C_GiBiSizeableItem.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     27.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiBiSizeableItem::C_GiBiSizeableItem() :
   f64_Width(0.0),
   f64_Height(0.0)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in] of64_Width  Width
   \param[in] of64_Height Height

   \created     27.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiBiSizeableItem::C_GiBiSizeableItem(const float64 of64_Width, const float64 of64_Height) :
   f64_Width(of64_Width),
   f64_Height(of64_Height)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     27.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiBiSizeableItem::~C_GiBiSizeableItem()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the size as rectangle

   \return  Rectangle with actual size

   \created     27.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QRectF C_GiBiSizeableItem::GetSizeRect(void) const
{
   return QRectF(0.0, 0.0, this->f64_Width, this->f64_Height);
}
