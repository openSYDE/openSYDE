//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Tab close push button (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBTABCLOSE_H
#define C_OGEPUBTABCLOSE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgePubToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubTabClose :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   C_OgePubTabClose(QWidget * const opc_Parent = NULL);
   void SetDarkMode(const bool oq_Active);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool event(QEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   virtual void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   //lint -restore

private:
   QIcon mc_Default;
   QIcon mc_Hovered;
   QIcon mc_Clicked;
   QIcon mc_DefaultDark;
   QIcon mc_HoveredDark;
   QIcon mc_ClickedDark;
   bool mq_DarkMode;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
