//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Tab widget of tool box (header)

   Tab widget of tool box (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETAWTOOLBOX_H
#define C_OGETAWTOOLBOX_H

/* -- Includes ------------------------------------------------------------- */

#include <QTabWidget>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTawToolBox :
   public QTabWidget
{
   Q_OBJECT

public:
   C_OgeTawToolBox(QWidget * const opc_Parent = NULL);
   ~C_OgeTawToolBox(void);

private:
   //Avoid call
   C_OgeTawToolBox(const C_OgeTawToolBox &);
   C_OgeTawToolBox & operator =(const C_OgeTawToolBox &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
