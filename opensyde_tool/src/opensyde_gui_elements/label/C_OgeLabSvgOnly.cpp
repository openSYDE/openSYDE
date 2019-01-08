//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Label to display SVGs (implementation)

   Label to display SVGs

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPainter>
#include "C_OgeLabSvgOnly.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_elements;

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

   \param[in,out] opc_Parent Optional pointer to parent

   \created     25.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeLabSvgOnly::C_OgeLabSvgOnly(QWidget * const opc_Parent) :
   C_OgeLabToolTipBase(opc_Parent),
   mpc_Renderer(NULL)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     25.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeLabSvgOnly::~C_OgeLabSvgOnly(void)
{
   delete (mpc_Renderer);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set SVG path

   \param[in] orc_SvgPath SVG path

   \created     25.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabSvgOnly::SetSvg(const QString & orc_SvgPath)
{
   if (this->mpc_Renderer != NULL)
   {
      delete (this->mpc_Renderer);
   }
   if (orc_SvgPath.compare("") == 0)
   {
      this->mpc_Renderer = NULL;
   }
   else
   {
      this->mpc_Renderer = new QSvgRenderer(orc_SvgPath);
   }
   this->update();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten paint event slot

   Here: Draw SVG

   \param[in,out] opc_Event Event identification and information

   \created     25.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLabSvgOnly::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)
   if (this->mpc_Renderer != NULL)
   {
      QPainter c_Painter(this);
      this->mpc_Renderer->render(&c_Painter, this->rect());
   }
}
