//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing the edit background (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEWIEDITBACKGROUND_H
#define C_OGEWIEDITBACKGROUND_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeWiEditBackground :
   public QWidget
{
   Q_OBJECT

public:
   C_OgeWiEditBackground(QWidget * const opc_Parent = NULL);

   void SetEditBackground(const bool oq_Active);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
