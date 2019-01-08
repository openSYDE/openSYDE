//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label to display SVGs (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABSVGONLY_H
#define C_OGELABSVGONLY_H

/* -- Includes ------------------------------------------------------------- */
#include <QSvgRenderer>
#include "C_OgeLabToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabSvgOnly :
   public C_OgeLabToolTipBase
{
public:
   C_OgeLabSvgOnly(QWidget * const opc_Parent = NULL);
   ~C_OgeLabSvgOnly(void);

   void SetSvg(const QString & orc_SvgPath);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;

private:
   QSvgRenderer * mpc_Renderer;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
