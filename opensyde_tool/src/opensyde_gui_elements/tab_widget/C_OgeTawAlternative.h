//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Alternative tab widget (header)

   Alternative tab widget (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETAWALTERNATIVE_H
#define C_OGETAWALTERNATIVE_H

/* -- Includes ------------------------------------------------------------- */

#include <QTabWidget>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTawAlternative :
   public QTabWidget
{
   Q_OBJECT

public:
   C_OgeTawAlternative(QWidget * const opc_Parent = NULL);
   ~C_OgeTawAlternative(void);

private:
   //Avoid call
   C_OgeTawAlternative(const C_OgeTawAlternative &);
   C_OgeTawAlternative & operator =(const C_OgeTawAlternative &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
