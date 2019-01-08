//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Button with  text and SVG item, drawn manually (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.10.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBSVGICONWITHTEXT_H
#define C_OGEPUBSVGICONWITHTEXT_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgePubSvgIconOnly.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubSvgIconWithText :
   public C_OgePubSvgIconOnly
{
public:
   C_OgePubSvgIconWithText(QWidget * const opc_Parent = NULL);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
