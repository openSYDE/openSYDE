//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget base for dashboard widgets (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDBASEWIDGET_H
#define C_SYVDADASHBOARDBASEWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_SyvDaItDashboardBaseWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_SyvDaItDashboardBaseWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItDashboardBaseWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvDaItDashboardBaseWidget(void);

   void SetWidget(QWidget * const opc_Content);

protected:
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   Ui::C_SyvDaItDashboardBaseWidget * mpc_Ui;
   QWidget * mpc_Widget;

   //Avoid call
   C_SyvDaItDashboardBaseWidget(const C_SyvDaItDashboardBaseWidget &);
   C_SyvDaItDashboardBaseWidget & operator =(const C_SyvDaItDashboardBaseWidget &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
