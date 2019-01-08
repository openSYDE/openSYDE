//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing system view update overview

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVUPOVERVIEWWIDGET_H
#define C_SYVUPOVERVIEWWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SyvUpOverviewWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvUpOverviewWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpOverviewWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SyvUpOverviewWidget();

private:
   //Avoid call
   C_SyvUpOverviewWidget(const C_SyvUpOverviewWidget &);
   C_SyvUpOverviewWidget & operator =(const C_SyvUpOverviewWidget &);

   Ui::C_SyvUpOverviewWidget * mpc_Ui;
};
}

#endif
