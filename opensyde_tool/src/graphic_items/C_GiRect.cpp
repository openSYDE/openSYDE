//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       A sizeable variant of QGraphicsRectItem (implementation)

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

#include "C_GiRect.h"

/* -- Used Namespaces ------------------------------------------------------ */
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

   Set up GUI with all elements.

   \param[in]     orc_Rect     Initial size of the item
   \param[in,out] opc_Parent   Optional pointer to parent

   \created     27.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiRect::C_GiRect(const QRectF & orc_Rect, QGraphicsItem * const opc_Parent) :
   QGraphicsRectItem(orc_Rect, opc_Parent),
   C_GiBiSizeableItem(orc_Rect.width(), orc_Rect.height())
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     25.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiRect::~C_GiRect()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the drawing of the rectangle

   \created     27.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiRect::Redraw(void)
{
   this->setRect(this->GetSizeRect());
}
