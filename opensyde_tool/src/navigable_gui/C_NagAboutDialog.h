//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for "About openSYDE" popup dialog. (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.07.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_NAGABOUTDIALOG_H
#define C_NAGABOUTDIALOG_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include "C_OgePopUpDialog.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_NagAboutDialog;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_NagAboutDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagAboutDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_NagAboutDialog(void);

   void InitStaticNames(void) const;
   void InitDynamicNames() const;
   void InitLogo() const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_NagAboutDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   void m_OkClicked(void);

   //Avoid call
   C_NagAboutDialog(const C_NagAboutDialog &);
   C_NagAboutDialog & operator =(const C_NagAboutDialog &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
