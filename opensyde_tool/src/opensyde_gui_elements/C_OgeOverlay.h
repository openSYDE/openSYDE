//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Overlay for background to concentrate focus on widget in foreground (header)

   Overlay for background to concentrate focus on widget in foreground (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEOVERLAY_H
#define C_OGEOVERLAY_H

/* -- Includes ------------------------------------------------------------- */

#include <QDialog>
#include <QPaintEvent>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeOverlay :
   public QDialog
{
   Q_OBJECT

public:
   C_OgeOverlay(QWidget * const opc_Parent = NULL, QWidget * const opc_EffectTargetChild = NULL);
   static void h_SetTopWidget(QWidget * const opc_TopWidget);
   static const QWidget * h_GetTopWidget(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event);
   //lint -restore

private:
   static QWidget * mhpc_TopWidget; ///< Most top widget to gray out this most top widgets and all sub widgets when this
                                    // overlay is activated
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
