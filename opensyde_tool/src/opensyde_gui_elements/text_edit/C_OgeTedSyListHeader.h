//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit for system view list header (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.11.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETEDSYLISTHEADER_H
#define C_OGETEDSYLISTHEADER_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeTedListHeaderBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTedSyListHeader :
   public C_OgeTedListHeaderBase
{
   Q_OBJECT

public:
   C_OgeTedSyListHeader(QWidget * const opc_Parent = NULL);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
