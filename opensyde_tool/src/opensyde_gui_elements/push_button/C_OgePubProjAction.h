//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Push button for main page project operations (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.07.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBPROJACTION_H
#define C_OGEPUBPROJACTION_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgePubToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubProjAction :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgePubProjAction(QWidget * const opc_Parent = NULL);

   void SetCustomIcon(QString oc_IconPathEnabled, QString oc_IconPathDisabled);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

private:
   QIcon mc_IconEnabled;
   QIcon mc_IconDisabled;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
